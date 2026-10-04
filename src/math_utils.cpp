#include "math_utils.hpp"

#include <cmath>
#include <algorithm>
#include <stdexcept>


namespace math_utils 
{

    std::vector<double> linspace(double start, double end, size_t size)
    {
        std::vector<double> result(size);
        double step = (end - start) / (size - 1); // size > 1

        for (size_t i=0; i<size; ++i)
            result[i] = start + i*step;

        return result;
    }


    double calc_pearson(const std::vector<double>& x, const std::vector<double>& y) 
    {
        if (x.size() != y.size())
            throw std::invalid_argument("Wektory muszą mieć tę samą długość.");

        size_t n = x.size();
        double sum_x = 0.0, sum_y = 0.0;
        double sum_xy = 0.0, sum_x2 = 0.0, sum_y2 = 0.0;

        for (size_t i=0; i<n; ++i) 
        {
            sum_x += x[i];
            sum_y += y[i];
            sum_xy += x[i] * y[i];
            sum_x2 += x[i] * x[i];
            sum_y2 += y[i] * y[i];
        }

        double numerator = n*sum_xy - sum_x*sum_y;
        double denominator = std::sqrt((n*sum_x2 - sum_x*sum_x) * (n*sum_y2 - sum_y*sum_y));

        if (std::abs(denominator) < 1e-12)
            throw std::runtime_error("Dzielenie przez zero.");

        return numerator / denominator;
    }


    double median(std::vector<double> v)
    {
        std::sort(v.begin(), v.end());
        size_t n = v.size();

        if (n % 2 == 1)
            return v[n / 2];
        else
            return (v[n / 2 - 1] + v[n / 2]) / 2.0;
    }

    double angle(double y, double x) 
    {
        return std::atan2(y, x);
    }

}
