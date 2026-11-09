#include "phase_space_analysis.hpp"
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
#include <vector>
#include <functional>
#include <random>
#include <chrono>

namespace phase_space_analysis 
{
    size_t get_n_lag(size_t N)
    {
        return std::max(size_t(1), std::min(N / 10, N - 1));
    }

    double calc_slope(const std::vector<double>& x, const std::vector<double>& y)
    {
        double n = static_cast<double>(x.size());
        double mx = 0.0, my = 0.0;
        for (size_t i = 0; i < x.size(); ++i) { mx += x[i]; my += y[i]; }
        mx /= n;
        my /= n;

        double sxy = 0.0, sxx = 0.0;
        for (size_t i = 0; i < x.size(); ++i)
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
        for (size_t k = 0; k < cols.size(); ++k)
            file << " " << col_prefix << "y" << k;
        file << "\n";

        for (size_t i = 0; i < x.size(); ++i)
        {
            file << std::setprecision(10) << std::fixed << x[i];
            for (const auto& col : cols)
                file << " " << col[i];
            file << "\n";
        }

        std::cout << "Zapisano tabelę: data/" << out_name << std::endl;
    }

    std::vector<double> generate_c_list(double c_min, double c_max, size_t n_c,
                                        bool use_random, unsigned int seed,
                                        double exclude_min, double exclude_max)
    {
        if (!use_random)
        {
            return math_utils::linspace(c_min, c_max, n_c);
        }

        std::vector<double> c_list;
        c_list.reserve(n_c);

        std::mt19937 rng(seed);
        std::uniform_real_distribution<double> dist(c_min, c_max);

        while (c_list.size() < n_c)
        {
            double val = dist(rng);
            if (exclude_min < exclude_max && val >= exclude_min && val <= exclude_max)
            {
                continue;
            }
            c_list.push_back(val);
        }

        std::sort(c_list.begin(), c_list.end());
        return c_list;
    }

    // ---------- p, q, kąt, odległość ----------

    void make_PQ(const std::vector<double>& phi,
                 std::vector<double>& p,
                 std::vector<double>& q,
                 double c, size_t N)
    {
        double sp = 0.0, sq = 0.0;
        for (size_t n = 0; n < N; ++n)
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
        size_t N = phi.size();
        std::vector<double> p(N, 0.0);
        std::vector<double> q(N, 0.0);

        std::vector<double> theta(N - 1);
        std::vector<double> d(N - 1);

        make_PQ(phi, p, q, c, N);

        for (size_t i = 0; i + 1 < N; ++i)
        {
            double dp = p[i + 1] - p[i];
            double dq = q[i + 1] - q[i];

            theta[i] = math_utils::angle_rad(dq, dp); // Użycie angle_rad w radianach
            d[i] = std::sqrt(dp * dp + dq * dq);
        }

        return {theta, d};
    }

    std::vector<double> read_phi_from_file(const std::string& filename, 
                                           size_t skip_rows, size_t col, bool from_binary)
    {
        std::vector<double> phi;
        
        if (from_binary)
        {
            std::string bin_filename = filename + std::to_string(col) + ".bin";
            phi = read_binary_vector(bin_filename);
        }
        else
        {
            auto cols = read_text_columns(filename, skip_rows);
            if (col >= cols.size())
                throw std::runtime_error("Kolumna " + std::to_string(col) + " nie istnieje w pliku");
            phi = std::move(cols[col]);
        }
        
        // double mean = 0.0;
        // for (size_t j = 0; j < phi.size(); ++j) {
        //     mean += phi[j];
        // }
        // mean /= static_cast<double>(phi.size());
        
        // for (size_t j = 0; j < phi.size(); ++j) {
        //     phi[j] -= mean;
        // }
        
        return phi;
    }

    void write_trajectory_data(const std::string& rel_path, size_t col,
                               size_t n_last, double c, size_t skip_rows)
    {
        std::vector<double> phi = read_phi_from_file("data_out/" + rel_path, skip_rows, col);

        auto [theta, d] = make_angle_distance(phi, c);

        size_t N = phi.size();
        std::vector<double> p(N), q(N);
        make_PQ(phi, p, q, c, N);

        size_t total = theta.size();
        size_t start_idx = (n_last > 0 && total > n_last) ? (total - n_last) : 0;

        std::string out_name = make_out_name(rel_path,
            "p_q_theta_d_y" + std::to_string(col) + "_last_" + std::to_string(n_last));

        auto file = prepare_data_file(out_name);
        file << "# p q theta d\n";

        for (size_t i = start_idx; i < total; ++i)
        {
            double angle = theta[i];
            double dist = d[i];

            if (std::abs(angle) < 1e-10 && std::abs(dist) < 1e-10)
                continue;

            file << std::setprecision(10) << std::fixed
                << p[i] << " " << q[i] << " " << angle << " " << dist << "\n";
        }

        std::cout << "Zapisano trajektorię: data/" << out_name << std::endl;
    }

    // ---------- Obliczanie MSD: Mc oraz Dc ----------

    void compute_M_values(const std::vector<double>& phi, double c, size_t n_lag,
                          std::vector<double>& M_values,
                          std::vector<double>& n_values)
    {
        size_t N = phi.size();
        std::vector<double> p(N), q(N);
        make_PQ(phi, p, q, c, N);

        M_values.clear();
        n_values.clear();
        M_values.reserve(n_lag);
        n_values.reserve(n_lag);

        for (size_t n = 1; n <= n_lag && n < N; ++n)
        {
            double M_sum = 0.0;
            for (size_t j = 0; j + n < N; ++j)
            {
                double dp = p[j + n] - p[j];
                double dq = q[j + n] - q[j];
                M_sum += dp * dp + dq * dq;
            }

            M_values.push_back(M_sum / static_cast<double>(N - n));
            n_values.push_back(static_cast<double>(n));
        }
    }

    void compute_Dc_values(const std::vector<double>& phi, double c, size_t n_lag,
                           std::vector<double>& D_values,
                           std::vector<double>& n_values)
    {
        size_t N = phi.size();
        if (N == 0) return;

        // Średnia czasowa obserwowalnej E_phi
        double sum_phi = 0.0;
        for (size_t j = 0; j < N; ++j) {
            sum_phi += phi[j];
        }
        double E_phi = sum_phi / static_cast<double>(N);
        double E_phi_sq = E_phi * E_phi;

        double denom = 1.0 - std::cos(c);
        if (std::abs(denom) < 1e-12) {
            denom = 1e-12;
        }

        std::vector<double> p(N), q(N);
        make_PQ(phi, p, q, c, N);

        D_values.clear();
        n_values.clear();
        D_values.reserve(n_lag);
        n_values.reserve(n_lag);

        for (size_t n = 1; n <= n_lag && n < N; ++n)
        {
            double M_sum = 0.0;
            for (size_t j = 0; j + n < N; ++j)
            {
                double dp = p[j + n] - p[j];
                double dq = q[j + n] - q[j];
                M_sum += dp * dp + dq * dq;
            }
            double M_n = M_sum / static_cast<double>(N - n);

            // Korekta oscylacyjna V_osc(c, n) = (E_phi)^2 * (1 - cos(n*c)) / (1 - cos(c))
            double V_osc = E_phi_sq * (1.0 - std::cos(static_cast<double>(n) * c)) / denom;

            // D_c(n) = M_c(n) - V_osc(c, n)
            double D_n = M_n - V_osc;

            D_values.push_back(D_n);
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

        for (size_t i = 0; i < n_values.size(); ++i)
        {
            log_n[i] = std::log(n_values[i]);
            log_M[i] = std::log(M_values[i]);
        }

        return calc_slope(log_n, log_M);
    }

    void calc_K_over_c_generic(const std::vector<double>& phi,
                               const std::vector<double>& c_list,
                               std::vector<double>& K_values,
                               KMethodFunc k_method,
                               bool use_corrected_msd)
    {
        size_t n_lag = get_n_lag(phi.size());
        long n_c = static_cast<long>(c_list.size());

        K_values.assign(c_list.size(), 0.0);

        #ifdef _OPENMP
        #pragma omp parallel for schedule(dynamic)
        #endif
        for (long i = 0; i < n_c; ++i)
        {
            std::vector<double> values, n_values;
            if (use_corrected_msd) {
                compute_Dc_values(phi, c_list[i], n_lag, values, n_values);
            } else {
                compute_M_values(phi, c_list[i], n_lag, values, n_values);
            }
            K_values[i] = k_method(values, n_values);
        }
    }

    // ==========================================
    // 1) RUN K_vs_c (z n_c = N / 10)
    // ==========================================
    void run_K_vs_c(const std::vector<std::string>& rel_paths,
                    double c_min, double c_max,
                    size_t skip_rows,
                    KMethodFunc k_method,
                    bool use_corrected_msd,
                    bool use_random,
                    unsigned int seed,
                    double exclude_min,
                    double exclude_max)
    {
        for (const std::string& rel_path : rel_paths)
        {
            std::cout << "[RUN K_vs_c] Plik: " << rel_path << std::endl;
            auto series = read_text_columns("data_out/" + rel_path, skip_rows);

            size_t n_c = 100;

            std::vector<double> c_list = generate_c_list(c_min, c_max, n_c, use_random, seed, exclude_min, exclude_max);

            std::vector<std::vector<double>> cols_K(series.size());
            for (size_t k = 0; k < series.size(); ++k)
            {
                calc_K_over_c_generic(series[k], c_list, cols_K[k], k_method, use_corrected_msd);
            }

            std::string suffix = use_corrected_msd ? "K_vs_c_corrected" : "K_vs_c";
            write_table(make_out_name(rel_path, suffix), "c", "K_c_", c_list, cols_K);
        }
    }

    // ==========================================
    // 2) RUN K_vs_Nc (dla list okien wokół 100: 70, 80, 90, 100, 110, 120, 130)
    // ==========================================
    void run_K_vs_Nc(const std::vector<std::string>& rel_paths,
                     double c_min, double c_max,
                     const std::vector<size_t>& n_c_list,
                     size_t skip_rows,
                     KMethodFunc k_method,
                     bool use_corrected_msd,
                     bool use_random,
                     unsigned int seed,
                     double exclude_min,
                     double exclude_max)
    {
        for (const std::string& rel_path : rel_paths)
        {
            std::cout << "[RUN K_vs_Nc] Plik: " << rel_path << std::endl;
            auto series = read_text_columns("data_out/" + rel_path, skip_rows);

            std::vector<double> x;
            std::vector<std::vector<double>> cols_K(series.size());

            for (size_t n_c : n_c_list)
            {
                x.push_back(static_cast<double>(n_c));
                std::vector<double> c_list = generate_c_list(c_min, c_max, n_c, use_random, seed, exclude_min, exclude_max);

                for (size_t k = 0; k < series.size(); ++k)
                {
                    std::vector<double> K_temp;
                    calc_K_over_c_generic(series[k], c_list, K_temp, k_method, use_corrected_msd);
                    cols_K[k].push_back(math_utils::median(K_temp));
                }
                std::cout << "  N_c = " << n_c << " gotowe" << std::endl;
            }

            std::string suffix = use_corrected_msd ? "K_vs_Nc_corrected" : "K_vs_Nc";
            write_table(make_out_name(rel_path, suffix), "N_c", "K_", x, cols_K);
        }
    }

    // ==========================================
    // 3) RUN PQ_DTheta & series_info.txt (z n_c = N / 10 i c odpowiadającym medianie)
    // ==========================================
    void run_PQ_DTheta_analysis(const std::vector<std::string>& rel_paths,
                                double c_min, double c_max,
                                size_t skip_rows, size_t n_last, size_t col,
                                bool use_corrected_msd,
                                double exclude_min, double exclude_max,
                                unsigned int seed)
    {
        for (const std::string& rel_path : rel_paths)
        {
            std::cout << "[RUN PQ_DTheta] Plik: " << rel_path << std::endl;
            
            auto start_time = std::chrono::high_resolution_clock::now();

            std::vector<double> phi = read_phi_from_file("data_out/" + rel_path, skip_rows, col);

            // Dynamiczne wyznaczenie n_c = N / 10
            // size_t N = phi.size();
            // size_t dynamic_n_c = N / 10;
            // size_t dynamic_n_c = 100;

            // Generowanie listy c
            // std::vector<double> c_list = generate_c_list(c_min, c_max, dynamic_n_c, true, seed, exclude_min, exclude_max);
            std::vector<double> c_list{1.0};
            // Obliczenie K dla każdego c z listy
            std::vector<double> K_values;
            calc_K_over_c_generic(phi, c_list, K_values, calc_K_corr, use_corrected_msd);

            // Ostateczny wynik testu K to mediana z K(c)
            double final_K = math_utils::median(K_values);

            // Wybór c, którego wynik K jest najbliższy medianie
            size_t best_idx = 0;
            double min_diff = std::abs(K_values[0] - final_K);
            for (size_t i = 1; i < K_values.size(); ++i) {
                double diff = std::abs(K_values[i] - final_K);
                if (diff < min_diff) {
                    min_diff = diff;
                    best_idx = i;
                }
            }
            // double median_c = c_list[best_idx];

            // 1. Zapis pliku trajektorii PQ dla wybranego median_c
            write_trajectory_data(rel_path, col, n_last, 1.0, skip_rows);

            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = end_time - start_time;

            // 2. Zapis pliku series_info.txt
            fs::path p(rel_path);
            std::string info_filename = (p.parent_path() / "series_info.txt").generic_string();
            auto info_file = prepare_data_file(info_filename);

            info_file << "# Series analysis information for Python import (Gottwald-Melbourne 0-1 test)\n";
            info_file << std::setprecision(10) << std::fixed;
            info_file << "file_source: " << rel_path << "\n";
            info_file << "final_K: " << final_K << "\n";
            // info_file << "median_c_for_trajectory: " << median_c << "\n";
            info_file << "c_min: " << c_min << "\n";
            info_file << "c_max: " << c_max << "\n";
            // info_file << "n_c: " << dynamic_n_c << "\n";
            info_file << "use_corrected_msd: " << (use_corrected_msd ? "true" : "false") << "\n";
            info_file << "excluded_range: [" << exclude_min << ", " << exclude_max << "]\n";
            info_file << "execution_time_seconds: " << elapsed.count() << "\n";

            std::cout << "Zapisano info: data/" << info_filename << " (Finalny K = " << final_K << ", Czas: " << elapsed.count() << "s)" << std::endl;
        }
    }
}
