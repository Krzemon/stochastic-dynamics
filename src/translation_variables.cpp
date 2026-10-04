#include "translation_variables.hpp"
#include "math_utils.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace translation_variables 
{
    // void analyze(
    //     const std::vector<double>& y0,
    //     const std::vector<double>& y1,
    //     const std::vector<double>& y2,
    //     const std::vector<double>& y3,
    //     const std::vector<double>& y4) 
    // {
    //     (void)y0;
    //     (void)y1;
    //     (void)y2;
    //     (void)y3;
    //     (void)y4;
    // }



    double calc_K(const std::vector<double>& phi)
    {
        const size_t N = phi.size();

        std::vector<double> K_values;

        // element 0 jest dla uwydatnienia tworzenia wektora sum
        std::vector<double> p(N+1, .0); // p[0]=0
        std::vector<double> q(N+1, .0); // q[0]=0

        // c przemiatane w zakresie (0, 2pi)
        std::vector<double> frequencies;
        for (double c = 0.01; c < 2*M_PI; c += 0.02)
            frequencies.push_back(c);

        for (const double c : frequencies)
        {
            construct_PQ(phi, p, q, c, N);

            std::vector<double> M_values;
            std::vector<double> n_values = math_utils::arange<double>(1, N);

            // pętla po opóźnieniach n
            for (size_t n = 1; n < N; ++n)
            {
                double M_sum = .0;
                for (size_t j = 1; j+n <= N; ++j) // j od 1, bo to początek szeregu phi
                {
                    double dp = p[j + n] - p[j];
                    double dq = q[j + n] - q[j];
                    M_sum += dp*dp + dq*dq;
                }

                // liczba użytych składników (żeby estymator był nieobciążony, ale nie wiem czy to ma sens w tym przypadku)
                size_t count = N - n + 1; // bo we wzorze N -> inf
                double Mn = M_sum / count;

                M_values.push_back(Mn);
            }

            // K(c) = corr(M(n), n)
            double Kc = math_utils::calc_pearson(M_values, n_values);

            K_values.push_back(Kc);
        }

            return math_utils::median(K_values);
        }


    void construct_PQ(const std::vector<double>& phi,
                            std::vector<double>& p,
                            std::vector<double>& q,
                      const double c, const size_t N)
    {
        p[0] = .0; q[0] = .0;

        for (size_t n = 1; n <= N; ++n)
        {
            p[n] = p[n - 1] + phi[n - 1] * std::cos(c * n);
            q[n] = q[n - 1] + phi[n - 1] * std::sin(c * n);
        }
    }
    



}