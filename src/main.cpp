#include "config.hpp"
#include "math_utils.hpp"
#include "phase_space.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>


// POLECONE
const std::vector<std::string> files = {
    "4750/y_266_11_od_0_4.75000.out", // punkt staly
    "6000/y_266_11_od_0_6.00000.out", // quasiperiodyczne
    "7500/y_266_11_od_0_7.50000.out", // periodyczne
    "8390/y_266_11_od_0_8.39000.out", // chaos
};

int main(int argc, char* argv[])
{
    prepare_directories(false,false);
    #ifdef _OPENMP
        std::cout << "[INFO] OpenMP: ON\n";
    #else
        std::cout << "[INFO] OpenMP: OFF\n";
    #endif


    // const std::string mode = "PQ_K";
    // const std::string mode = "PQ_DTheta";
    const std::string mode = "PQ_DTheta_fixed_c";
    
    // const std::string mode = "K_vs_Nc";
    // const std::string mode = "K_reg";
    // const std::string mode = "none";


    std::cout << "mode: " << mode << '\n';
    try {
        if (mode == "PQ_K") 
        {
            ;
            // for (const auto& file : files) {   
            //     std::cout << "Przetwarzam: " << file << '\n';
            //     const auto data = read_text_columns((data_in_dir / file).string(), N_SKIP);
            //     const auto out_dir = fs::path(file).parent_path().string();
            //     for (size_t col = 0; col < data.size(); ++col) {
            //         std::cout << col + 1 << " ";
            //         phase_space::run_pq(data[col], math_utils::correlation, true /*center*/, 
            //                             out_dir, col, std::vector<double>{1.0} /*c value*/);
            //     }
            //     std::cout << std::endl;
            // }
        }
        else if (mode == "PQ_DTheta")
        {
            for (const auto& file : files) {
                std::cout << "Przetwarzam: " << file << '\n';
                const auto data = read_text_columns((data_in_dir / file).string(), N_SKIP);
                const auto out_dir = fs::path(file).parent_path().string();
                for (size_t col = 0; col < data.size(); ++col) {
                    std::cout << col + 1 << " ";
                    phase_space::run_pq_dtheta(data[col], math_utils::correlation, true /*center*/,
                                                out_dir, col, std::vector<double>{1.0} /*c value*/);
                }
                std::cout << std::endl;
            }
        }
        else if (mode == "K_vs_Nc")
        {
            const std::vector<size_t> nc_list = {50, 75, 100, 125, 150};
            // for (const auto& file : files) {
            for (const auto& file : std::vector<std::string>{files[0], files[1]}) {
                std::cout << "Przetwarzam: " << file << '\n';
                const auto data = read_text_columns((data_in_dir / file).string(), N_SKIP);
                const auto out_dir = fs::path(file).parent_path().string();
                for (size_t col = 0; col < data.size(); ++col) {
                    std::cout << "Kolumna " << col + 1 << ":\n";
                    phase_space::run_k_vs_nc(data[col], math_utils::correlation, true, out_dir, col, nc_list, 42);
                }
                std::cout << std::endl;
            }
        }
        else if (mode == "K_reg")
        {
            const auto& file = files[0];
            std::cout << "Przetwarzam: " << file << '\n';
            const auto data = read_text_columns((data_in_dir / file).string(), N_SKIP);
            const auto out_dir = fs::path(file).parent_path().string();
            const vec c_values = [&]{
                vec c = math_utils::uniform(NC, C_MIN, C_MAX);
                std::sort(c.begin(), c.end());
                return c;
            }();
            for (size_t col = 0; col < data.size(); ++col) {
                std::cout << col + 1 << " ";
                const vec phi = phase_space::center_data(data[col]);
                const size_t n_cut = phi.size() / 10;
                vec k_values(c_values.size());
                for (size_t i = 0; i < c_values.size(); ++i)
                    k_values[i] = phase_space::calc_k(phi, c_values[i], n_cut, math_utils::regression);
                auto f = prepare_data_file(out_dir + "/K_reg/K_reg_col_" + std::to_string(col + 1) + ".dat");
                f << "# c\tK\n";
                for (size_t i = 0; i < c_values.size(); ++i)
                    f << c_values[i] << '\t' << k_values[i] << '\n';
            }
            std::cout << std::endl;
        }
        else if (mode == "PQ_DTheta_fixed_c")
        {
            std::ifstream fc(data_in_dir / "c_values.dat");
            if (!fc) 
                throw std::runtime_error("Nie mozna otworzyc data/c_values.dat");
            std::vector<double> c_fixed;
            double val;
            while (fc >> val) 
                c_fixed.push_back(val);

            // ---------------------------------
            for (const auto& file : files) {
                std::cout << "Przetwarzam: " << file << '\n';
                const auto data = read_text_columns((data_in_dir / file).string(), N_SKIP);
                const auto out_dir = fs::path(file).parent_path().string();
                for (size_t col = 0; col < data.size(); ++col) {
                    std::cout << col + 1 << " ";
                    phase_space::run_pq_dtheta(data[col], math_utils::correlation, true, out_dir, col, c_fixed);
                }
                std::cout << std::endl;
            }

        }
        else {;} 

    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << '\n';
        return 1;
    }

    return 0;
}




// const std::string article_path = "0=local/article_data/";
// const std::vector<std::string> article = {
//     "7889/y_266_11_od_0_7.88900.out",
// };

