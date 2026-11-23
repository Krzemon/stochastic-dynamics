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

    // void calc_pq_old(const vec& phi, double c, vec& p, vec& q)
    // {
    //     size_t n = phi.size();
    //     p.resize(n);
    //     q.resize(n);

    //     double sp = 0.0, sq = 0.0;
    //     for (size_t j = 0; j < n; ++j)
    //     {
    //         double angle = (j + 1) * c;
    //         sp += phi[j] * std::cos(angle);
    //         sq += phi[j] * std::sin(angle);
    //         p[j] = sp;
    //         q[j] = sq;
    //     }
    // }

    void calc_pq(const vec& phi, double c, vec& p, vec& q)
    {
        size_t n = phi.size();
        p.resize(n);
        q.resize(n);
        p[0] = phi[0] * std::cos(c);
        q[0] = phi[0] * std::sin(c);
        for (size_t i = 1; i < n; ++i)
        {
            p[i] = p[i-1] + phi[i-1] * std::cos(c * i);
            q[i] = q[i-1] + phi[i-1] * std::sin(c * i);
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

    void save_k_vs_c_col(const vec& c_values, const vec& k_values,
                         const std::string& out_dir, size_t col,
                         const std::string& subdir = "",
                         const std::string& suffix = "")
    {
        std::string full_dir = subdir.empty() ? out_dir : (out_dir + "/" + subdir);
        std::string filename = suffix.empty() 
            ? ("K_vs_c_col_" + std::to_string(col + 1) + ".dat")
            : ("K_vs_c_col_" + std::to_string(col + 1) + suffix + ".dat");
        
        auto f = prepare_data_file(full_dir + "/" + filename, "data");
        f << "# c\tK\n";
        for (size_t i = 0; i < c_values.size(); ++i)
            f << c_values[i] << '\t' << k_values[i] << '\n';
    }

    std::pair<vec, vec> make_angle_distance(const vec& phi, const vec& p, const vec& q)
    {
        size_t n = phi.size();
        vec theta(n - 1), d(n - 1);
        for (size_t i = 0; i + 1 < n; ++i)
        {
            double dp = p[i + 1] - p[i];
            double dq = q[i + 1] - q[i];
            theta[i] = math_utils::angle_rad(dq, dp);
            d[i] = std::sqrt(dp*dp + dq*dq);
        }
        return {theta, d};
    }

    void save_pq_dtheta(const vec& phi, const vec& p, const vec& q,
                        double c_best, double k_best, double median_k,
                        const std::string& out_dir, size_t col,
                        const std::string& subdir = "")
    {
        std::string full_dir = subdir.empty() ? out_dir : (out_dir + "/" + subdir);
        {
            auto f = prepare_data_file(full_dir + "/pq_col_" + std::to_string(col+1) + ".dat");
            f << std::fixed << std::setprecision(10);
            f << "# c=" << c_best << " K_c=" << k_best << " K=" << median_k << '\n';
            f << "# p(n) q(n)\n";
            for (size_t j = 0; j < phi.size(); ++j)
                f << p[j] << '\t' << q[j] << '\n';
        }
        {
            auto [theta, d] = make_angle_distance(phi, p, q);
            auto f = prepare_data_file(
                full_dir + "/angle_distance_col_" + std::to_string(col + 1) + ".dat");
            f << std::fixed << std::setprecision(10);
            f << "# c=" << c_best << " K_c=" << k_best << " K=" << median_k << '\n';
            f << "# theta(n) d(n)\n";
            for (size_t j = 0; j < theta.size(); ++j)
                f << theta[j] << '\t' << d[j] << '\n';
        }
    }

    // void save_pq(const vec& phi, const vec& c_values,
    //              const vec& k_values, double median_k,
    //              const std::string& out_dir, size_t col,
    //              const std::string& subdir = "")
    // {
    //     size_t best = 0;
    //     for (size_t i = 1; i < c_values.size(); ++i)
    //         if (std::abs(k_values[i] - median_k) < std::abs(k_values[best] - median_k))
    //             best = i;

    //     vec p, q;
    //     calc_pq(phi, c_values[best], p, q);

    //     std::string full_dir = subdir.empty() ? out_dir : (out_dir + "/" + subdir);
    //     auto f = prepare_data_file(
    //         full_dir + "/pq_col_" + std::to_string(col + 1) + ".dat");

    //     f << std::fixed << std::setprecision(10);
    //     f << "# c=" << c_values[best] << " K_c=" << k_values[best] << " K=" << median_k << '\n';
    //     f << "# p(n) q(n)\n";

    //     for (size_t j = 0; j < phi.size(); ++j)
    //         f << p[j] << '\t' << q[j] << '\n';
    // }

    void run_pq_dtheta(const vec& phi, const KMethod& k_method, bool center,
                       const std::string& out_dir, size_t col,
                       std::optional<std::vector<double>> c_fixed)
    {
        vec c_values = c_fixed.has_value()
            ? vec(c_fixed->begin(), c_fixed->end())
            : math_utils::uniform(NC, C_MIN, C_MAX);
        std::sort(c_values.begin(), c_values.end());

        vec data = center ? center_data(phi) : phi;

        vec k_values(c_values.size());
        size_t n_cut = data.size() / 10;

        #ifdef _OPENMP
            #pragma omp parallel for schedule(dynamic)
        #endif
        for (int i = 0; i < static_cast<int>(c_values.size()); ++i)
            k_values[i] = calc_k(data, c_values[i], n_cut, k_method);
        double median_k = math_utils::median(k_values);

        std::string subdir = "PQ_DTheta" + std::string(center ? "_center" : "");
        save_k_vs_c_col(c_values, k_values, out_dir, col, subdir, "_Nc" + std::to_string(NC));

        size_t best = 0;
        for (size_t i = 1; i < c_values.size(); ++i)
            if (std::abs(k_values[i] - median_k) < std::abs(k_values[best] - median_k))
                best = i;

        vec p, q;
        calc_pq(data, c_values[best], p, q);
        save_pq_dtheta(data, p, q, c_values[best], k_values[best], median_k, out_dir, col, subdir);
        std::cout << "  Zapisano do: data/" << out_dir << "/" << subdir << "/\n";
    }

    // void run_pq(const vec& phi, const KMethod& k_method, bool center,
    //             const std::string& out_dir, size_t col,
    //             std::optional<std::vector<double>> c_fixed)
    // {
    //     vec c_values = c_fixed.has_value()
    //         ? vec(c_fixed->begin(), c_fixed->end())
    //         : math_utils::uniform(NC, C_MIN, C_MAX);

    //     std::sort(c_values.begin(), c_values.end());

    //     vec data = center ? center_data(phi) : phi;
    //     vec k_values = calc_k_values(data, c_values, k_method);
    //     double median_k = math_utils::median(k_values);

    //     // Zapisujemy do podkatalogu PQ_K
    //     std::string subdir = "PQ_K" + std::string(center ? "_center" : "");
    //     // save_k_vs_c_col(c_values, k_values, out_dir, col, subdir, "_Nc" + std::to_string(NC));
    //     save_pq(data, c_values, k_values, median_k, out_dir, col, subdir);
        
    //     std::cout << "  Zapisano do: data/" << out_dir << "/" << subdir << "/\n";
    // }

    void run_k_vs_nc(const vec& phi, const KMethod& k_method, bool center,
                     const std::string& out_dir, size_t col,
                     const std::vector<size_t>& nc_list, std::optional<unsigned int> seed)
    {
        vec data = center ? center_data(phi) : phi;
        std::string subdir = "K_vs_Nc";

        // Dla każdej wartości Nc obliczamy K i zapisujemy szczegółowe dane
        vec nc_values;
        vec median_k_values;

        for (size_t nc : nc_list)
        {
            // Generujemy nc losowych wartości c
            vec c_values = math_utils::uniform(nc, C_MIN, C_MAX, seed);
            std::sort(c_values.begin(), c_values.end());

            // Obliczamy K dla każdego c
            vec k_values = calc_k_values(data, c_values, k_method);
            
            // Zapisujemy szczegółowe dane K vs c dla tego Nc
        std::string suffix = "_Nc" + std::to_string(nc) + (center ? "_center" : "");
            save_k_vs_c_col(c_values, k_values, out_dir, col, subdir, suffix);
            
            // Mediana K dla tego nc
            double median_k = math_utils::median(k_values);
            nc_values.push_back(static_cast<double>(nc));
            median_k_values.push_back(median_k);

            std::cout << "  Nc = " << nc << " gotowe (mediana K = " << median_k << ")\n";
        }

        // Zapisujemy podsumowanie: Nc vs mediana K (jeśli więcej niż 1 Nc)
        if (nc_list.size() > 1) {
            std::string full_dir = out_dir + "/" + subdir;
            auto f = prepare_data_file(
                full_dir + "/K_vs_Nc_col_" + std::to_string(col + 1) + ".dat");
            f << "# Nc\tK_median\n";
            for (size_t i = 0; i < nc_values.size(); ++i)
                f << static_cast<size_t>(nc_values[i]) << '\t' << median_k_values[i] << '\n';
            
            std::cout << "  Zapisano: data/" << full_dir 
                      << "/K_vs_Nc_col_" << col + 1 << ".dat (podsumowanie)\n";
        }
        std::cout << "  Zapisano: szczegółowe pliki K_vs_c_col_" << col + 1 
                  << "_Nc*.dat dla każdego Nc\n";
    }

}