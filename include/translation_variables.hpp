#pragma once

#include "config.hpp"
#include <vector>
#include <string>

namespace translation_variables 
{
    /**
     * @brief Oblicza parametr K dla szeregu czasowego phi
     */
    double calc_K(const std::vector<double>& phi);

    /**
     * @brief Przekształca szereg czasowy na trajektorię w płaszczyźnie (p,q)
     */
    void make_PQ(const std::vector<double>& phi, 
                 std::vector<double>& p,
                 std::vector<double>& q,    
                 double c, usize N);

    /**
     * @brief Przekształca trajektorię w płaszczyźnie (p,q) na (theta,d) z parametrem c
     */
    auto make_angle_distance(const std::vector<double>& phi, double c = 1.0) 
        -> std::pair<std::vector<double>, std::vector<double>>;

    /**
     * @brief Odczytuje phi z pliku (y0)
     */
    std::vector<double> read_phi_from_file(const std::string& filename 
                               = "data_out/C_good_data/y_266_11_od_0_78.89000.out");

    /**
     * @brief Zapisuje dane trajektorii P-Q do pliku
     * @param input_filepath pełna ścieżka do pliku wejściowego
     * @param n_last liczba ostatnich punktów (0 = wszystkie)
     * @param c parametr transformacji
     */
    void write_trajectory_data(const std::string& input_filepath, usize n_last = 7000, double c = 1.0);

    /**
     * @brief Wykonuje kod analizy
     */
    void run_calculation(int argc, char* argv[]);
}
