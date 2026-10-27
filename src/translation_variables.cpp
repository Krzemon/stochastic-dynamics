#include "translation_variables.hpp"
#include "math_utils.hpp"
#include "config.hpp"

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace translation_variables 
{
    // ---------- funkcje pomocnicze ----------

    usize get_n_lag(usize N)
    {
        return std::max(usize(1), std::min(N / 10, N - 1));
    }


    double calc_slope(const std::vector<double>& x, const std::vector<double>& y)
    {
        double n = static_cast<double>(x.size());
        double mx = 0.0, my = 0.0;
        for (usize i = 0; i < x.size(); ++i) { mx += x[i]; my += y[i]; }
        mx /= n;
        my /= n;

        double sxy = 0.0, sxx = 0.0;
        for (usize i = 0; i < x.size(); ++i)
        {
            sxy += (x[i] - mx) * (y[i] - my);
            sxx += (x[i] - mx) * (x[i] - mx);
        }
        return sxy / sxx;
    }


    std::string make_out_name(const std::string& rel_path, const std::string& prefix)
    {
        fs::path p(rel_path);
        return (p.parent_path() / (prefix + "_" + p.stem().string() + ".dat")).generic_string();
    }


    void write_table(const std::string& out_name,
                     const std::string& x_name, const std::string& col_prefix,
                     const std::vector<double>& x,
                     const std::vector<std::vector<double>>& cols)
    {
        auto file = prepare_data_file(out_name);

        file << "# " << x_name;
        for (usize k = 0; k < cols.size(); ++k)
            file << " " << col_prefix << "y" << k;
        file << "\n";

        for (usize i = 0; i < x.size(); ++i)
        {
            file << std::setprecision(10) << std::fixed << x[i];
            for (const auto& col : cols)
                file << " " << col[i];
            file << "\n";
        }

        std::cout << "Zapisano: data/" << out_name << std::endl;
    }


    // ---------- p, q, kąt, odległość ----------

    void make_PQ(const std::vector<double>& phi,
                 std::vector<double>& p,
                 std::vector<double>& q,
                 double c, usize N)
    {
        double sp = 0.0, sq = 0.0;
        for (usize n = 0; n < N; ++n)
        {
            sp += phi[n] * std::cos(c * (n + 1));
            sq += phi[n] * std::sin(c * (n + 1));
            p[n] = sp;
            q[n] = sq;
        }
    }

    
    auto make_angle_distance(const std::vector<double>& phi, double c) 
        -> std::pair<std::vector<double>, std::vector<double>>
    {
        usize N = phi.size();
        std::vector<double> p(N, 0.0);
        std::vector<double> q(N, 0.0);

        std::vector<double> theta(N - 1);
        std::vector<double> d(N - 1);

        make_PQ(phi, p, q, c, N);

        for (usize i = 0; i + 1 < N; ++i)
        {
            double dp = p[i + 1] - p[i];
            double dq = q[i + 1] - q[i];

            theta[i] = math_utils::angle(dq, dp);
            d[i] = std::sqrt(dp * dp + dq * dq);
        }

        return {theta, d};
    }


    // ---------- wczytywanie ----------

    std::vector<std::vector<double>> read_columns_from_file(const std::string& filename,
                                                            usize skip_rows, usize n_cols)
    {
        std::ifstream input_file(filename);

        if (!input_file.is_open())
            throw std::runtime_error("Nie można otworzyć pliku: " + filename);

        std::vector<std::vector<double>> columns(n_cols);
        std::vector<double> v(n_cols);
        std::string line;
        usize current_row = 0;

        while (std::getline(input_file, line))
        {
            if (current_row < skip_rows)
            {
                ++current_row;
                continue;
            }
            ++current_row;

            std::istringstream iss(line);
            bool ok = true;
            for (double& x : v)
                if (!(iss >> x)) { ok = false; break; }

            if (ok)
                for (usize i = 0; i < n_cols; ++i)
                    columns[i].push_back(v[i]);
        }

        return columns;
    }


    std::vector<double> read_phi_from_file(const std::string& filename, usize skip_rows)
    {
        return read_columns_from_file(filename, skip_rows, 5)[0];
    }


    void write_trajectory_data(const std::string& rel_path, usize col,
                               usize n_last, double c)
    {
        prepare_directories(false, false);

        auto series = read_columns_from_file("data_out/" + rel_path, 0, 5);
        const std::vector<double>& phi = series[col];

        auto [theta, d] = make_angle_distance(phi, c);

        usize N = phi.size();
        std::vector<double> p(N, 0.0);
        std::vector<double> q(N, 0.0);

        make_PQ(phi, p, q, c, N);

        usize total = theta.size();
        usize start_idx = (n_last > 0 && total > n_last) ? (total - n_last) : 0;

        std::string out_name = make_out_name(rel_path,
            "p_q_theta_d_y" + std::to_string(col) + "_last_" + std::to_string(n_last));

        auto file = prepare_data_file(out_name);

        file << "# p q theta d\n";

        for (usize i = start_idx; i < total; ++i)
        {
            double angle = theta[i];
            double dist = d[i];

            if (std::abs(angle) < 1e-10 && std::abs(dist) < 1e-10)
                continue;

            file << std::setprecision(10) << std::fixed
                 << p[i] << " "
                 << q[i] << " "
                 << angle << " "
                 << dist << "\n";
        }

        std::cout << "Zapisano: data/" << out_name << std::endl;
    }


    // ---------- wyznaczanie K ----------

    void compute_M_values(const std::vector<double>& phi, double c, usize n_lag,
                          std::vector<double>& M_values,
                          std::vector<double>& n_values)
    {
        usize N = phi.size();
        std::vector<double> p(N), q(N);
        make_PQ(phi, p, q, c, N);

        M_values.clear();
        n_values.clear();
        M_values.reserve(n_lag);
        n_values.reserve(n_lag);

        for (usize n = 1; n <= n_lag && n < N; ++n)
        {
            double M_sum = 0.0;
            for (usize j = 0; j + n < N; ++j)
            {
                double dp = p[j + n] - p[j];
                double dq = q[j + n] - q[j];
                M_sum += dp * dp + dq * dq;
            }

            M_values.push_back(M_sum / static_cast<double>(N - n));
            n_values.push_back(static_cast<double>(n));
        }
    }


    double calc_K_corr(const std::vector<double>& M_values,
                       const std::vector<double>& n_values)
    {
        return math_utils::calc_pearson(M_values, n_values);
    }


    double calc_K_reg(const std::vector<double>& M_values,
                      const std::vector<double>& n_values)
    {
        std::vector<double> log_n(n_values.size());
        std::vector<double> log_M(M_values.size());

        for (usize i = 0; i < n_values.size(); ++i)
        {
            log_n[i] = std::log(n_values[i]);
            log_M[i] = std::log(M_values[i]);
        }

        return calc_slope(log_n, log_M);
    }


    void calc_K_over_c(const std::vector<double>& phi,
                       const std::vector<double>& c_list,
                       std::vector<double>& K_corr,
                       std::vector<double>& K_reg)
    {
        usize n_lag = get_n_lag(phi.size());
        long n_c = static_cast<long>(c_list.size());

        K_corr.assign(c_list.size(), 0.0);
        K_reg.assign(c_list.size(), 0.0);

        // każda iteracja zapisuje tylko do własnego indeksu i ma własne p, q, M
        #pragma omp parallel for schedule(dynamic)
        for (long i = 0; i < n_c; ++i)
        {
            std::vector<double> M_values, n_values;
            compute_M_values(phi, c_list[i], n_lag, M_values, n_values);

            K_corr[i] = calc_K_corr(M_values, n_values);
            K_reg[i]  = calc_K_reg(M_values, n_values);
        }
    }


    double calc_K(const std::vector<double>& phi)
    {
        std::vector<double> frequencies;
        for (double c = 0.01; c < 2 * M_PI; c += 0.02)
            frequencies.push_back(c);

        return calc_K_with_c_list(phi, frequencies);
    }


    double calc_K_with_c_list(const std::vector<double>& phi,
                              const std::vector<double>& c_list)
    {
        usize n_c = c_list.size();
        usize n_lag = get_n_lag(phi.size());

        std::vector<double> K_corr, K_reg;
        calc_K_over_c(phi, c_list, K_corr, K_reg);

        double final_K = math_utils::median(K_corr);
        std::cout << "[Lista c] Liczba c (n_c): " << n_c
                  << ", opóźnienie n: " << n_lag
                  << " -> Wartość K wynosi: " << final_K << std::endl;

        return final_K;
    }


    double calc_K_with_c_range(const std::vector<double>& phi,
                               double c_min, double c_max, usize n_c)
    {
        return calc_K_with_c_list(phi, math_utils::linspace(c_min, c_max, n_c));
    }


    void write_K_vs_c(const std::string& rel_path,
                      const std::vector<std::vector<double>>& series,
                      double c_min, double c_max, usize n_c)
    {
        prepare_directories(false, false);

        std::vector<double> c_list = math_utils::linspace(c_min, c_max, n_c);

        std::vector<std::vector<double>> cols_corr(series.size());
        std::vector<std::vector<double>> cols_reg(series.size());

        for (usize k = 0; k < series.size(); ++k)
            calc_K_over_c(series[k], c_list, cols_corr[k], cols_reg[k]);

        write_table(make_out_name(rel_path, "K_vs_c_corr"), "c", "K_c_", c_list, cols_corr);
        write_table(make_out_name(rel_path, "K_vs_c_reg"),  "c", "K_c_", c_list, cols_reg);
    }


    void write_K_vs_Nc(const std::string& rel_path,
                       const std::vector<std::vector<double>>& series,
                       double c_min, double c_max,
                       const std::vector<usize>& n_c_list)
    {
        prepare_directories(false, false);

        std::vector<double> x;
        std::vector<std::vector<double>> cols_corr(series.size());
        std::vector<std::vector<double>> cols_reg(series.size());

        for (usize n_c : n_c_list)
        {
            x.push_back(static_cast<double>(n_c));
            std::vector<double> c_list = math_utils::linspace(c_min, c_max, n_c);

            for (usize k = 0; k < series.size(); ++k)
            {
                std::vector<double> K_corr, K_reg;
                calc_K_over_c(series[k], c_list, K_corr, K_reg);

                cols_corr[k].push_back(math_utils::median(K_corr));
                cols_reg[k].push_back(math_utils::median(K_reg));
            }
            std::cout << "N_c = " << n_c << " gotowe" << std::endl;
        }

        write_table(make_out_name(rel_path, "K_vs_Nc_corr"), "N_c", "K_", x, cols_corr);
        write_table(make_out_name(rel_path, "K_vs_Nc_reg"),  "N_c", "K_", x, cols_reg);
    }


    void run_K_analysis(const std::vector<std::string>& rel_paths,
                        double c_min, double c_max,
                        const std::vector<usize>& n_c_list,
                        usize n_c_fixed, usize skip_rows)
    {
        for (const std::string& rel_path : rel_paths)
        {
            std::cout << "Plik: " << rel_path << std::endl;

            auto series = read_columns_from_file("data_out/" + rel_path, skip_rows, 5);

            write_K_vs_c(rel_path, series, c_min, c_max, n_c_fixed);
            write_K_vs_Nc(rel_path, series, c_min, c_max, n_c_list);
        }
    }


    void run_calculation(int argc, char* argv[])
    {
        // wersja testowa: jeden plik
        std::vector<std::string> filenames = {
            "7889/y_266_11_od_0_7.88900.out",
            // "7910/y_266_11_od_0_7.91000.out",
            // "7945/y_266_11_od_0_7.94500.out",
            // "8639/y_266_11_od_0_8.63900.out",
            // "8660/y_266_11_od_0_8.66000.out"
        };// TODO 8390, 4750, 6000, 7500
            

        // przedział Gottwalda i Melbourne: (pi/5, 4pi/5)
        double c_min = M_PI / 5.0;
        double c_max = 4.0 * M_PI / 5.0;

        // N_c
        std::vector<usize> n_c_list = math_utils::arange<usize>(10, 121, 10);

        run_K_analysis(filenames, c_min, c_max, n_c_list, 100, 100000);
    }

}