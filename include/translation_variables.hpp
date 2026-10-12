#pragma once

#include <vector>
#include <string>

namespace translation_variables 
{
    /**
     * @brief Oblicza parametr K dla szeregu czasowego phi
     */
    double calc_K(const std::vector<double>& phi);

    /**
     * @brief Przekształca szereg czasowy na trajektorię w płaszczyźnie (p,q)
     */
    void make_PQ(const std::vector<double>& phi, 
                       std::vector<double>& p,
                       std::vector<double>& q,    
                 const double c, const size_t N);

    /**
     * @brief Przekształca trajektorię w płaszczyźnie (p,q) na trajektorię w płaszyczyźnie (theta,d)
     */
    auto make_angle_distance(const std::vector<double>& phi) ->std::pair<std::vector<double>, std::vector<double>>;

    /**
     * @brief Odczytuje phi z pliku
     */
    std::vector<double> read_phi_from_file(const std::string& filename 
                               = "../data_out/y_266_11_od_0_83.90000.dat");


    void analyse_along_trajectory();
    void analyse_across_s_values();



    /**
     * @brief Wykonuje kod analizy
     */
    void run_calculation(int argc, char* argv[]);
}