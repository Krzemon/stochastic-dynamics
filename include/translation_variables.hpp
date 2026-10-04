#pragma once

#include <vector>

namespace translation_variables 
{
    // void analyze(
    //     const std::vector<double>& y0,
    //     const std::vector<double>& y1,
    //     const std::vector<double>& y2,
    //     const std::vector<double>& y3,
    //     const std::vector<double>& y4
    // );


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


}