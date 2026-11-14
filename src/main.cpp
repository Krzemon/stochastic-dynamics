#include "config.hpp"
#include "math_utils.hpp"
#include "phase_space.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

const std::string article_path = "0=local/article_data/";
const std::vector<std::string> article = {
    "7889/y_266_11_od_0_7.88900.out",
};


// POLECONE
const std::vector<std::string> files = {
    "data/4750/y_266_11_od_0_4.75000.out", // punkt staly
    "data/6000/y_266_11_od_0_6.00000.out", // quasiperiodyczne
    "data/7500/y_266_11_od_0_7.50000.out", // periodyczne
    "data/8390/y_266_11_od_0_8.39000.out", // chaos
};

int main(int argc, char* argv[])
{
    #ifdef _OPENMP
        std::cout << "[INFO] OpenMP: ON\n";
    #else
        std::cout << "[INFO] OpenMP: OFF\n";
    #endif

    prepare_directories(false,false);
    const std::string mode = "PQ_DTheta_K";
    // const std::string mode = "PQ_fixed_c";


    std::cout << "mode: " << mode << '\n';
    try {
        if (mode == "PQ_DTheta_K") 
        {
            auto file = "data/4750/y_266_11_od_0_4.75000.out";
            // auto file = article_path + article[0];
            // for (const auto& file : files) {   
                std::cout << "Przetwarzam: " << file << '\n';
                const auto data = read_text_columns(file, N_SKIP);
                phase_space::run(data, math_utils::correlation, false, 
                        fs::path(file).parent_path().filename().string(), //); 
                        std::vector<double>{1.0});
            // }
        }
        else if (mode == "PQ_fixed_c")
        {
            // taki sam seed losowania, ale zapisalem je wcześniej 
            std::ifstream fc("data/c_values.dat");
            if (!fc) 
                throw std::runtime_error("Nie mozna otworzyc data/c_values.dat");
            std::vector<double> c_fixed;
            double val;
            while (fc >> val) c_fixed.push_back(val);

            for (const auto& file : files) {
                std::cout << "Przetwarzam: " << file << '\n';
                const auto data = read_text_columns(file, N_SKIP);
                phase_space::run(data, math_utils::correlation, false,
                        fs::path(file).parent_path().filename().string(),
                        c_fixed);
            }
        }
        else {;} 

    } catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << '\n';
        return 1;
    }

    return 0;
}