#include "translation_variables.hpp"
#include "math_utils.hpp"

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdexcept>

namespace translation_variables 
{
    double calc_K(const std::vector<double>& phi)
    {
        const size_t N = phi.size();
        std::vector<double> K_values;

        // element 0 jest dla uwydatnienia tworzenia wektora sum
        std::vector<double> p(N+1);
        std::vector<double> q(N+1);
        p[0] = .0; q[0] = .0;

        // c przemiatane w zakresie (0, 2pi)
        std::vector<double> frequencies;
        for (double c = 0.01; c < 2*M_PI; c += 0.02)
            frequencies.push_back(c);

        for (const double c : frequencies)
        {
            make_PQ(phi, p, q, c, N);

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


    void make_PQ(const std::vector<double>& phi,
                       std::vector<double>& p,
                       std::vector<double>& q,
                 const double c, const size_t N)
    {
        for (size_t n = 1; n <= N; ++n)
        {
            p[n] = p[n - 1] + phi[n - 1] * std::cos(c * n);
            q[n] = q[n - 1] + phi[n - 1] * std::sin(c * n);
        }
    }
    
    auto make_angle_distance(const std::vector<double>& phi) -> std::pair<std::vector<double>, std::vector<double>>
    {
        const size_t N = phi.size();
        // element 0 jest dla uwydatnienia tworzenia wektora sum
        std::vector<double> p(N+1);
        std::vector<double> q(N+1);
        p[0] = .0; q[0] = .0;

        std::vector<double> theta(N); // theta[0] powstaje z [0] i [1] 
        std::vector<double> d(N);     // d[0] powstaje z [0] i [1] 

        double c = M_PI/4;
        make_PQ(phi, p, q, c, N);

        for (int i = 0; i < N; ++i)
        {
            double dp = p[i+1] - p[i];
            double dq = q[i+1] - q[i];

            theta[i] = math_utils::angle(dq, dp);
            d[i] = std::sqrt(dp*dp + dq*dq);
        }

        return {theta, d};
    }

    std::vector<double> read_phi_from_file(const std::string& filename)
    {
        std::ifstream input_file(filename);

        if (!input_file.is_open())
            throw std::runtime_error("Nie można otworzyć pliku: " + filename);

        std::vector<double> phi;
        std::string line;

        while (std::getline(input_file, line))
        {
            std::istringstream iss(line);
            double value;
                
            if (iss >> value)
                phi.push_back(value);
        }
        return phi;
    }


    void run_calculation(int argc, char* argv[])
    {
        // if (argc < 4)
        //     throw std::invalid_argument("Użycie:\n   program \"analiza\" <plik_wejściowy> <plik_wyjściowy_Mn>");
        
        const std::vector<double> phi = read_phi_from_file();

        double K_val = calc_K(phi);

        std::cout << "Wartosc K dla chaosu wynosi: " << K_val << std::endl;

    }
}