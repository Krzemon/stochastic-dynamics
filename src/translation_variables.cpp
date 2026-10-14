#include "translation_variables.hpp"
#include "math_utils.hpp"
#include "config.hpp"

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <sstream>

namespace translation_variables 
{
    double calc_K(const std::vector<double>& phi)
    {
        usize N = phi.size();
        std::vector<double> K_values;

        std::vector<double> p(N+1);
        std::vector<double> q(N+1);
        p[0] = 0.0; q[0] = 0.0;

        std::vector<double> frequencies;
        for (double c = 0.01; c < 2*M_PI; c += 0.02)
            frequencies.push_back(c);

        for (double c : frequencies)
        {
            make_PQ(phi, p, q, c, N);

            std::vector<double> M_values;
            std::vector<double> n_values = math_utils::arange<double>(1, N);

            for (usize n = 1; n < N; ++n)
            {
                double M_sum = 0.0;
                for (usize j = 1; j+n <= N; ++j)
                {
                    double dp = p[j + n] - p[j];
                    double dq = q[j + n] - q[j];
                    M_sum += dp*dp + dq*dq;
                }

                usize count = N - n + 1;
                double Mn = M_sum / count;

                M_values.push_back(Mn);
            }

            double Kc = math_utils::calc_pearson(M_values, n_values);
            K_values.push_back(Kc);
        }

        return math_utils::median(K_values);
    }


    void make_PQ(const std::vector<double>& phi,
                 std::vector<double>& p,
                 std::vector<double>& q,
                 double c, usize N)
    {
        for (usize n = 1; n <= N; ++n)
        {
            p[n] = p[n - 1] + phi[n - 1] * std::cos(c * n);
            q[n] = q[n - 1] + phi[n - 1] * std::sin(c * n);
        }
    }

    
    auto make_angle_distance(const std::vector<double>& phi, double c) 
        -> std::pair<std::vector<double>, std::vector<double>>
    {
        usize N = phi.size();
        std::vector<double> p(N+1);
        std::vector<double> q(N+1);
        p[0] = 0.0; q[0] = 0.0;

        std::vector<double> theta(N);
        std::vector<double> d(N);

        make_PQ(phi, p, q, c, N);

        for (usize i = 0; i < N; ++i)
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

        std::vector<double> y0;
        std::string line;

        while (std::getline(input_file, line))
        {
            std::istringstream iss(line);
            double v0, v1, v2, v3, v4;
                
            if (iss >> v0 >> v1 >> v2 >> v3 >> v4)
                y0.push_back(v0);
        }
        
        return y0;
    }


    void write_trajectory_data(const std::string& input_filepath, usize n_last, double c)
    {
        prepare_directories(false, false);

        std::vector<double> phi = read_phi_from_file(input_filepath);

        std::string filename = input_filepath.substr(input_filepath.find_last_of("/\\") + 1);
        
        std::size_t pos = filename.find("_od_0_");
        if (pos == std::string::npos)
            throw std::runtime_error("Nie można wyciągnąć wartości s z nazwy pliku");
        
        std::string s_str = filename.substr(pos + 6, 5);
        double s_value = std::stod(s_str);
        
        auto [theta, d] = make_angle_distance(phi, c);

        usize N = phi.size();
        std::vector<double> p(N + 1);
        std::vector<double> q(N + 1);
        p[0] = 0.0; q[0] = 0.0;
        
        make_PQ(phi, p, q, c, N);

        usize total = theta.size();
        usize start_idx = (n_last > 0 && total > n_last) ? (total - n_last) : 0;


        int s_int = static_cast<int>(s_value * 100);
        
        std::ostringstream filename_stream;
        filename_stream << s_int << "/alpha_seq_s_" << std::fixed << std::setprecision(2) << s_value << ".dat";
        
        auto file = prepare_data_file(filename_stream.str());

        file << "# p q theta d\n";
        
        for (usize i = start_idx; i < total; ++i)
        {
            double angle = theta[i];
            double dist = d[i];
            
            if (std::abs(angle) < 1e-10 && std::abs(dist) < 1e-10)
                continue;
                
            file << std::setprecision(10) << std::fixed
                 << p[i] << " "
                 << q[i] << " "
                 << angle << " "
                 << dist << "\n";
        }

        std::cout << "Zapisano: data/" << filename_stream.str() << std::endl;
    }


    void run_calculation(int argc, char* argv[])
    {
        std::vector<double> phi = read_phi_from_file();

        double K_val = calc_K(phi);

        std::cout << "Wartosc K wynosi: " << K_val << std::endl;
    }
}
