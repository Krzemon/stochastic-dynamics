#pragma once

#include "config.hpp"
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <random>
#include <fstream>
#include <cstddef>
#include <chrono>
#include <cmath>

constexpr double DEFAULT_C_MIN = M_PI / 5.0;
constexpr double DEFAULT_C_MAX = 4.0 * M_PI / 5.0;

namespace phase_space_analysis 
{
    using KMethodFunc = std::function<double(const std::vector<double>&, const std::vector<double>&)>;

    // ---------- funkcje pomocnicze i kąty ----------

    size_t get_n_lag(size_t N);

    double calc_slope(const std::vector<double>& x, const std::vector<double>& y);

    std::string make_out_name(const std::string& rel_path, const std::string& prefix);

    inline std::ofstream prepare_data_file(
        const std::string& filename,
        const std::string& data_dir_name = "data")
    {
        const fs::path filepath = project_root / data_dir_name / filename;
        fs::create_directories(filepath.parent_path());

        std::ofstream file(filepath);
        if (!file)
            throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

        return file;
    }

    void write_table(const std::string& out_name,
                     const std::string& x_name, const std::string& col_prefix,
                     const std::vector<double>& x,
                     const std::vector<std::vector<double>>& cols);

    std::vector<double> generate_c_list(double c_min = DEFAULT_C_MIN, double c_max = DEFAULT_C_MAX, size_t n_c = 100,
                                        bool use_random = true,
                                        unsigned int seed = 42,
                                        double exclude_min = -1.0,
                                        double exclude_max = -1.0);

    // Kąty w radianach [0, 2*pi) oraz stopniach [0, 360)
    // double angle_rad(double y, double x);
    // double angle_deg(double y, double x);

    // ---------- p, q, kąt, odległość ----------

    void make_PQ(const std::vector<double>& phi, 
                 std::vector<double>& p,
                 std::vector<double>& q,    
                 double c, size_t N);

    auto make_angle_distance(const std::vector<double>& phi, double c = 1.0) 
        -> std::pair<std::vector<double>, std::vector<double>>;

    // ---------- wczytywanie ----------



    std::vector<double> read_phi_from_file(const std::string& filename,
                                           size_t skip_rows = 100000,
                                           size_t col = 0,
                                           bool from_binary = false);
                            
    void write_trajectory_data(const std::string& rel_path, size_t col = 0,
                               size_t n_last = 7000, double c = 1.0,
                               size_t skip_rows = 100000);

    // ---------- wyznaczanie K oraz wersje MSD (Mc oraz Dc) ----------

    // Wersja 1: Standardowe M(n)
    void compute_M_values(const std::vector<double>& phi, double c, size_t n_lag,
                          std::vector<double>& M_values,
                          std::vector<double>& n_values);

    // Wersja 2: Zmodyfikowane D(n) = M(n) - V_osc(c, n) z lepszą zbieżnością
    void compute_Dc_values(const std::vector<double>& phi, double c, size_t n_lag,
                           std::vector<double>& D_values,
                           std::vector<double>& n_values);

    double calc_K_corr(const std::vector<double>& M_values,
                       const std::vector<double>& n_values);

    double calc_K_reg(const std::vector<double>& M_values,
                      const std::vector<double>& n_values);

    void calc_K_over_c_generic(const std::vector<double>& phi,
                               const std::vector<double>& c_list,
                               std::vector<double>& K_values,
                               KMethodFunc k_method,
                               bool use_corrected_msd = false);

    // ---------- GŁÓWNE FUNKCJE URUCHOMIENIOWE (RUNY) ----------

    void run_K_vs_c(const std::vector<std::string>& rel_paths,
                    double c_min = DEFAULT_C_MIN, double c_max = DEFAULT_C_MAX,
                    size_t skip_rows = 100000,
                    KMethodFunc k_method = calc_K_corr,
                    bool use_corrected_msd = false,
                    bool use_random = true,
                    unsigned int seed = 42,
                    double exclude_min = -1.0,
                    double exclude_max = -1.0);

    void run_K_vs_Nc(const std::vector<std::string>& rel_paths,
                     double c_min = DEFAULT_C_MIN, double c_max = DEFAULT_C_MAX,
                     const std::vector<size_t>& n_c_list = {70, 80, 90, 100, 110, 120, 130},
                     size_t skip_rows = 100000,
                     KMethodFunc k_method = calc_K_corr,
                     bool use_corrected_msd = false,
                     bool use_random = true,
                     unsigned int seed = 42,
                     double exclude_min = -1.0,
                     double exclude_max = -1.0);

    void run_PQ_DTheta_analysis(const std::vector<std::string>& rel_paths,
                                double c_min = DEFAULT_C_MIN, double c_max = DEFAULT_C_MAX,
                                size_t skip_rows = 100000, size_t n_last = 7000, size_t col = 0,
                                bool use_corrected_msd = false,
                                double exclude_min = 1.25,
                                double exclude_max = 2.0,
                                unsigned int seed = 42);
}