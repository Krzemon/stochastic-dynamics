#include "double_ver.hpp"
#include "config.hpp"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace double_ver 
{
    void read_input_file(const char* filename,double& a1, double& a3, double s[N][N]) 
    {
        std::ifstream input(filename);

        if (!input)
            throw std::runtime_error("Nie można otworzyć pliku wejściowego: " + std::string(filename));

        std::string line;
        std::getline(input, line);
        input >> a1 >> a3;

        for (int i = 0; i < N; ++i) 
            for (int j = 0; j < N; ++j) 
                input >> s[i][j];

        if (!input)
            throw std::runtime_error("Błąd podczas odczytu danych z pliku: " + std::string(filename));
    }

    std::vector<double> precompute_weights(int omega, double ni) 
    {
        std::vector<double> weights(omega + 1);
        weights[0] = 1.0;

        for (int k = 0; k < omega; ++k) 
            weights[k + 1] = weights[k] * (static_cast<double>(k) + ni) / static_cast<double>(k + 1);

        return weights;
    }

    void calculate(
        const double s[N][N],
        double a1, double a2, double a3,
        const std::vector<double>& weights,
        std::vector<double>& y0,
        std::vector<double>& y1,
        std::vector<double>& y2,
        std::vector<double>& y3,
        std::vector<double>& y4
    ) {
        const int size = OMEGA + 1;

        y0.resize(size);
        y1.resize(size);
        y2.resize(size);
        y3.resize(size);
        y4.resize(size);

        y0[0] = 0.8;
        y1[0] = 0.3;
        y2[0] = 0.4;
        y3[0] = 0.6;
        y4[0] = 0.7;

        for (int om = 1; om <= OMEGA; ++om) {

            double sum0 = 0.0;
            double sum1 = 0.0;
            double sum2 = 0.0;
            double sum3 = 0.0;
            double sum4 = 0.0;

            for (int r = 1; r <= om; ++r) 
            {
                const int k = om - r;
                const double bg = weights[k];

                const double v0 = y0[r - 1];
                const double v1 = y1[r - 1];
                const double v2 = y2[r - 1];
                const double v3 = y3[r - 1];
                const double v4 = y4[r - 1];

                const double tanh0 = std::tanh(v0);
                const double tanh1 = std::tanh(v1);
                const double tanh2 = std::tanh(v2);
                const double tanh3 = std::tanh(v3);
                const double tanh4 = std::tanh(v4);

                const double sin0 = std::sin(v0);
                const double sin2 = std::sin(v2);
                const double sin3 = std::sin(v3);
                const double sin4 = std::sin(v4);

                const double G1_v2 = 1.0 - a1 * tanh2;
                const double G2_v4 = a2 - a3 * sin4;

                sum0 += bg * ( -v0
                    + s[0][1] * tanh1
                    + s[0][2] * sin2
                    + s[0][3] * sin3
                );
                sum1 += bg * ( -v1 + F1
                    + s[1][0] * sin0
                    + s[1][2] * sin2
                    + s[1][4] * sin4
                );
                sum2 += bg * ( -v2
                    + s[2][0] * tanh0
                    + s[2][1] * tanh1
                    + s[2][2] * sin2
                );
                sum3 += bg * ( -v3 + F3
                    + s[3][0] * tanh0
                    + G2_v4 * tanh3
                );
                sum4 += bg * ( -v4
                    + s[4][1] * tanh1
                    + G1_v2 * tanh4
                );
            }

            y0[om] = y0[0] + sum0;
            y1[om] = y1[0] + sum1;
            y2[om] = y2[0] + sum2;
            y3[om] = y3[0] + sum3;
            y4[om] = y4[0] + sum4;
        }
    }

    void write_results(
        const char* caseName,
        const char* powtAlfa,
        double a2,
        const std::vector<double>& y0,
        const std::vector<double>& y1,
        const std::vector<double>& y2,
        const std::vector<double>& y3,
        const std::vector<double>& y4,
        const std::string& type = ".dat" 
    ) {
        std::ostringstream filename;
        filename << "y_" << caseName << "_" << powtAlfa << "_od_" << TS << "_" 
                    << std::fixed<< std::setprecision(5) << a2 << type;

        auto file = prepareDataFile(filename.str());
        file.out << std::fixed << std::setprecision(10);

        for (int i=TS; i<=OMEGA; ++i) 
            file.out << y0[i] << ' ' << y1[i] << ' ' << y2[i] << ' ' << y3[i] << ' ' << y4[i] << '\n';
    }

    void read_results(
        const char* filename,
        std::vector<double>& y0,
        std::vector<double>& y1,
        std::vector<double>& y2,
        std::vector<double>& y3,
        std::vector<double>& y4
    ) {
        std::ifstream input(filename);

        if (!input)
            throw std::runtime_error("Nie można otworzyć pliku wynikowego: " + std::string(filename));

        y0.clear(); y1.clear(); y2.clear(); y3.clear(); y4.clear();
        
        double v0,v1,v2,v3,v4;
        while (input >> v0 >> v1 >> v2 >> v3 >> v4) 
        {
            y0.push_back(v0);
            y1.push_back(v1);
            y2.push_back(v2);
            y3.push_back(v3);
            y4.push_back(v4);
        }

        if (!input.eof())
            throw std::runtime_error("Błąd podczas odczytu pliku wynikowego: " + std::string(filename));
    }

    void write_binary(
        const std::string& filename,
        const std::vector<double>& data)
    {
        std::ofstream file(filename, std::ios::binary);

        if (!file)
            throw std::runtime_error("Cannot open file for writing: " + filename);

        std::size_t size = data.size();

        file.write(
            reinterpret_cast<const char*>(&size),
            sizeof(size)
        );

        file.write(
            reinterpret_cast<const char*>(data.data()),
            size * sizeof(double)
        );
    }

    void run_calculation(int argc, char* argv[]) 
    {
        if (argc < 6)
            throw std::invalid_argument("Użycie:\n   program \"obliczenie\" <case> <powt_alfa> <a2> <input_file>");

        double a1, a3;
        double s[N][N]{};
        std::vector<double> y0, y1, y2, y3, y4;

        read_input_file(argv[5],a1,a3,s);

        const int a2i = std::atoi(argv[4]);
        const double a2 = static_cast<double>(a2i) / 100.0;

        const std::vector<double> weights = precompute_weights(OMEGA, NI);

        calculate(s,a1,a2,a3,weights,y0,y1,y2,y3,y4);
        write_results(argv[2],argv[3],a2,y0,y1,y2,y3,y4);
    }

} 

