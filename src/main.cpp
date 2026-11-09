#include "double_ver.hpp"
#include "phase_space_analysis.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    #ifdef _OPENMP
    std::cout << "[INFO] OpenMP: WŁĄCZONY\n";
    #else
    std::cout << "[INFO] OpenMP: WYŁĄCZONY\n";
    #endif
    
    prepare_directories(false,false);
    std::string mode = "PQ_DTheta_K";

    try {
        if (mode == "obliczenie") {
            double_ver::run_calculation(argc, argv);
        }
        else if (mode == "K_vs_c") {
            std::vector<std::string> filenames = {"4750/y_266_11_od_0_4.75000.out"};
            
            phase_space_analysis::run_K_vs_c(
                filenames, DEFAULT_C_MIN, DEFAULT_C_MAX, 100000,
                phase_space_analysis::calc_K_corr,
                false, true, 42, 1.25, 2.0
            );

            phase_space_analysis::run_K_vs_c(
                filenames, DEFAULT_C_MIN, DEFAULT_C_MAX, 100000,
                phase_space_analysis::calc_K_reg,
                false, true, 42, 1.25, 2.0
            );
        }
        else if (mode == "K_vs_Nc") {
            std::vector<std::string> filenames = {"4750/y_266_11_od_0_4.75000.out"};
            std::vector<size_t> custom_nc_list = {70, 80, 90, 100, 110, 120, 130};

            phase_space_analysis::run_K_vs_Nc(
                filenames, DEFAULT_C_MIN, DEFAULT_C_MAX, custom_nc_list, 100000,
                phase_space_analysis::calc_K_corr, false, true, 42, 1.25, 2.0
            );

            phase_space_analysis::run_K_vs_Nc(
                filenames, DEFAULT_C_MIN, DEFAULT_C_MAX, custom_nc_list, 100000,
                phase_space_analysis::calc_K_reg, false, true, 42, 1.25, 2.0
            );
        }
        else if (mode == "PQ_DTheta_K") {
            std::vector<std::string> filenames = {"4750/y_266_11_od_0_4.75000.out"};

            phase_space_analysis::run_PQ_DTheta_analysis(
                filenames, DEFAULT_C_MIN, DEFAULT_C_MAX, 100000, 7000, 0, false, 1.25, 2.0, 42
            );
        }
        else {
            std::cerr << "Nieznany tryb: " << mode << "\n\n"
                      << "Dostepne tryby:\n"
                      << "  obliczenie\n"
                      << "  K_vs_c\n"
                      << "  K_vs_Nc\n"
                      << "  PQ_DTheta_K\n";
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
