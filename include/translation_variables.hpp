#pragma once

#include "config.hpp"
#include <vector>
#include <string>
#include <utility>

using usize = std::size_t;

namespace translation_variables 
{
    // ---------- funkcje pomocnicze ----------

    usize get_n_lag(usize N);

    // nachylenie prostej y = a*x + b (metoda najmniejszych kwadratów)
    double calc_slope(const std::vector<double>& x, const std::vector<double>& y);

    // "7889/y_266_11_od_0_7.88900.out" + "K_vs_c_corr" -> "7889/K_vs_c_corr_y_266_11_od_0_7.88900.dat"
    std::string make_out_name(const std::string& rel_path, const std::string& prefix);

    // zapis tabeli: x | cols[0] ... cols[m-1]
    void write_table(const std::string& out_name,
                     const std::string& x_name, const std::string& col_prefix,
                     const std::vector<double>& x,
                     const std::vector<std::vector<double>>& cols);

    // ---------- p, q, kąt, odległość ----------

    void make_PQ(const std::vector<double>& phi, 
                 std::vector<double>& p,
                 std::vector<double>& q,    
                 double c, usize N);

    auto make_angle_distance(const std::vector<double>& phi, double c = 1.0) 
        -> std::pair<std::vector<double>, std::vector<double>>;

    // ---------- wczytywanie ----------

    std::vector<std::vector<double>> read_columns_from_file(const std::string& filename,
                                                            usize skip_rows = 100000,
                                                            usize n_cols = 5);

    std::vector<double> read_phi_from_file(const std::string& filename 
                                               = "data_out/C_good_data/y_266_11_od_0_78.89000.out",
                                           usize skip_rows = 100000);
                            
    void write_trajectory_data(const std::string& rel_path, usize col = 0,
                               usize n_last = 7000, double c = 1.0);

    // ---------- wyznaczanie K ----------

    // M(n) dla n = 1 ... n_lag (oraz odpowiadające im n jako double)
    void compute_M_values(const std::vector<double>& phi, double c, usize n_lag,
                          std::vector<double>& M_values,
                          std::vector<double>& n_values);

    // metoda 1: korelacja Pearsona M(n) z n
    double calc_K_corr(const std::vector<double>& M_values,
                       const std::vector<double>& n_values);

    // metoda 2: nachylenie regresji log M(n) ~ log n
    double calc_K_reg(const std::vector<double>& M_values,
                      const std::vector<double>& n_values);

    // K_corr[i], K_reg[i] dla każdego c_list[i]
    void calc_K_over_c(const std::vector<double>& phi,
                       const std::vector<double>& c_list,
                       std::vector<double>& K_corr,
                       std::vector<double>& K_reg);

    double calc_K(const std::vector<double>& phi);

    double calc_K_with_c_list(const std::vector<double>& phi,
                              const std::vector<double>& c_list);

    double calc_K_with_c_range(const std::vector<double>& phi,
                               double c_min, double c_max, usize n_c);

    // ---------- zapis zależności ----------

    // K(c) dla n_c punktów; pliki K_vs_c_corr_*.dat i K_vs_c_reg_*.dat
    void write_K_vs_c(const std::string& rel_path,
                      const std::vector<std::vector<double>>& series,
                      double c_min, double c_max, usize n_c = 100);

    // K(N_c); pliki K_vs_Nc_corr_*.dat i K_vs_Nc_reg_*.dat
    void write_K_vs_Nc(const std::string& rel_path,
                       const std::vector<std::vector<double>>& series,
                       double c_min, double c_max,
                       const std::vector<usize>& n_c_list);

    void run_K_analysis(const std::vector<std::string>& rel_paths,
                        double c_min, double c_max,
                        const std::vector<usize>& n_c_list,
                        usize n_c_fixed = 100, usize skip_rows = 100000);

    void run_calculation(int argc, char* argv[]);
}