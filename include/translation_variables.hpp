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
     * @brief Przekształca szereg czasowy na trajektorię w płaszczyźnie PQ
     */
    void construct_PQ(const std::vector<double>& phi, 
                            std::vector<double>& p,
                            std::vector<double>& q,    
                            double c, size_t N);

    /**
     * @brief Odczytuje phi z pliku
     */
    std::vector<double> read_phi_from_file(const std::string& filename 
                               = "../data_out/y_266_11_od_0_83.90000.dat");
    /**
     * @brief Wykonuje kod analizy
     */
    void run_calculation(int argc, char* argv[]);
}