#pragma once

#include <cmath>
#include <vector>

namespace math_utils 
{

    /**
     * @brief Generuje wektor ze stałym krokiem w zadanym zakresie
     */
    template<typename T = int>
    std::vector<T> arange(T start, T end, T step=1)
    {
        std::vector<T> result;
        for (T x = start; x < end; x += step)
            result.push_back(x);
        return result;   
    }

    /**
     * @brief Generuje wektor n wymiarowy ze stałą różnicą między elementami
     */
    std::vector<double> linspace(double start, double end, size_t size);

    /**
     * @brief Oblicza współczynnik korelacji Pearsona dla dwóch wektorów
     */
    double calc_pearson(const std::vector<double>& x, const std::vector<double>& y);

    /**
     * @brief Oblicza medianę wektora
     */
    double median(std::vector<double> v);

    /**
     * @brief Oblicza kąt w radianach z dokłądnością do ćwiartki
     */
    double angle(double y, double x);

}
