#include "phase_space.hpp"
#include "math_utils.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

namespace phase_space {

    vec center_data(const vec& data)
    {
        double mean = std::accumulate(data.begin(), data.end(), 0.0); 
        mean /= data.size();
        vec phi(data.size());
        for (size_t i = 0; i < data.size(); ++i)
            phi[i] = data[i] - mean;
        return phi;
    }

    void calc_pq(const vec& phi, double c, vec& p, vec& q)
    {
        size_t n = phi.size();
        p.resize(n);
        q.resize(n);

        double sp = 0.0, sq = 0.0;
        for (size_t j = 0; j < n; ++j)
        {
            double angle = (j + 1) * c;
            sp += phi[j] * std::cos(angle);
            sq += phi[j] * std::sin(angle);
            p[j] = sp;
            q[j] = sq;
        }
    }

    vec calc_Mn(
        const vec& phi,
        const vec& p,
        const vec& q,
        double c, size_t n_cut)
    {
        vec m_values(n_cut);
        for (size_t i = 1; i <= n_cut; ++i)
        {
            size_t m = phi.size() - i;
            double sum = 0.0;
            for (size_t j = 0; j < m; ++j)
            {
                double dp = p[j + i] - p[j];
                double dq = q[j + i] - q[j];
                sum += dp*dp + dq*dq;
            }
            m_values[i - 1] = sum / m;
        }
        return m_values;
    }

    vec calc_Vosc(
        const vec& phi,
        const vec& p,
        const vec& q,
        double c, size_t n_cut)
    {
        double e_phi = std::accumulate(phi.begin(), phi.end(), 0.0) / phi.size();
        double e_phi2 = e_phi * e_phi;
        vec v_values(n_cut);
        for (size_t i = 1; i <= n_cut; ++i)
            v_values[i - 1] = e_phi2 * (1.0 - std::cos(i * c))
                                       / (1.0 - std::cos(c));
        return v_values;
    }

    vec calc_Dn(
        const vec& phi,
        const vec& p,
        const vec& q,
        double c, size_t n_cut)
    {
        vec m_values = calc_Mn(phi, p, q, c, n_cut);
        vec v_values = calc_Vosc(phi, p, q, c, n_cut);
        vec d_values(n_cut);
        for (size_t i = 0; i < n_cut; ++i)
            d_values[i] = m_values[i] - v_values[i];
        return d_values;
    }

    double calc_k(
        const vec& phi, double c,
        size_t n_cut, const KMethod& k_method)
    {
        vec p, q;
        calc_pq(phi, c, p, q);
        vec d_values = calc_Dn(phi, p, q, c, n_cut);
        vec n_values = math_utils::arange<double>(1, n_cut + 1, 1);
        return k_method(n_values, d_values);
    }

    vec calc_k_values(
        const vec& phi,
        const vec& c_values,
        const KMethod& k_method)
    {
        size_t n_cut = phi.size() / 10;
        vec k_values(c_values.size());

        #ifdef _OPENMP
        #pragma omp parallel for schedule(dynamic)
        #endif
        for (int i = 0; i < static_cast<int>(c_values.size()); ++i)
            k_values[i] = calc_k(phi, c_values[i], n_cut, k_method);

        return k_values;
    }

    void save_pq(const vec& phi, const vec& c_values,
                 const vec& k_values, double median_k,
                 const std::string& out_dir, size_t col)
    {
        size_t best = 0;
        for (size_t i = 1; i < c_values.size(); ++i)
            if (std::abs(k_values[i] - median_k) < std::abs(k_values[best] - median_k))
                best = i;

        vec p, q;
        calc_pq(phi, c_values[best], p, q);

        auto f = prepare_data_file(
            out_dir + "/pq_col_" + std::to_string(col + 1) + ".dat", "data");

        f << std::fixed << std::setprecision(10);
        f << "# c=" << c_values[best] << " K_c=" << k_values[best] << " K=" << median_k << '\n';
        f << "# n p(n) q(n)\n";

        for (size_t j = 0; j < phi.size(); ++j)
            f << j + 1 << '\t' << p[j] << '\t' << q[j] << '\n';
    }

    void run(const mat& data, const KMethod& k_method, bool center,
             const std::string& out_dir,
             std::optional<std::vector<double>> c_fixed)
    {
        vec c_values = c_fixed.has_value()
            ? vec(c_fixed->begin(), c_fixed->end())
            : math_utils::uniform(NC, C_MIN, C_MAX);

        std::sort(c_values.begin(), c_values.end());

        // std::cout << "case: " << out_dir << " | columns: ";
        for (size_t col = 0; col < data.size(); ++col)
        {
            std::cout << col + 1 << " ";
            vec phi = center ? center_data(data[col]) : data[col];
            vec k_values = calc_k_values(phi, c_values, k_method);
            double median_k = math_utils::median(k_values);

            auto col_file = prepare_data_file(
                out_dir + "/phase_space_K_col_" + std::to_string(col + 1) + ".dat", "data");

            col_file << "# c\tK\n";
            for (size_t i = 0; i < c_values.size(); ++i)
                col_file << c_values[i] << '\t' << k_values[i] << '\n';

            save_pq(phi, c_values, k_values, median_k, out_dir, col);
        }
        std::cout << std::endl;
    }

}