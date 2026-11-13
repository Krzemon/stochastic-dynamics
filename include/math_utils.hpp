#pragma once

#include <cmath>
#include <vector>
#include <random>

namespace math_utils 
{

    /**
     * @brief Zwraca wektor liczb ze stałym krokiem <step> od <start> do <end> (wyłączając go)
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
     * @brief Zwraca wektor <size> liczb od <start> do <end> włącznie
     */
    std::vector<double> linspace(double start, double end, size_t size);

    /**
     * @brief Oblicza współczynnik korelacji Pearsona dwóch wektorów
     */
    double correlation(const std::vector<double>& x, const std::vector<double>& y);

    /**
     * @brief Oblicza współczynnik kierunkowy prostej regresji liniowej MNK
     */
    double regression(const std::vector<double>& x, const std::vector<double>& y);
    
    /**
     * @brief Oblicza medianę wektora
     */
    double median(std::vector<double> v);
    
    /**
     * @brief Oblicza kąt w radianach z zakresu [0, 2pi)
     */
    double angle_rad(double y, double x);

    /**
     * @brief Oblicza kąt w stopniach z zakresu [0, 360)
     */
    double angle_deg(double y, double x);

    /**
     * @brief Generuje liczby z rozkładu jednostajnego
     */
    std::vector<double> uniform(size_t n, double min, double max);

}
