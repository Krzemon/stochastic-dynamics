#include "double_ver.hpp"
#include "translation_variables.hpp"

// #include <cstring>
// #include <iostream>
// #include <stdexcept>

#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::string mode = "K_vs_c";
    // std::string mode = "PQ_DTheta_K";

    try {
        if (mode == "obliczenie") {
            double_ver::run_calculation(argc, argv);
        }
        else if (mode == "K_vs_c") {
            translation_variables::run_calculation(argc, argv);
        }
        else if (mode == "PQ_DTheta_K") {
            // y0 = kolumna 0, wynik trafia do data/7889/
            translation_variables::write_trajectory_data("7889/y_266_11_od_0_7.88900.out", 0);
        }
        else {
            std::cerr << "Nieznany tryb: " << mode << "\n\n" << "Dostepne tryby:\n"
                << "  obliczenie\n" 
                << "  analiza\n" 
                << "  PQ_DTheta_K\n"
                << "  K_vs_c (zapis wykresu K od c)\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }






    // std::string mode = "PQ_DTheta_K";  // Domyślny tryb

    // std::vector<double> phi = translation_variables::read_phi_from_file();

    // usize N_burn = 100000;
    // std::vector<double> phi0;
    // for (usize i=N_burn; i<phi.size(); ++i)
    //     phi0.push_back(phi[i]);

    // translation_variables::calc_K_with_c_range(phi0, 0.01, 3.1415, 50);

    // try {
    //     if (mode == "obliczenie")
    //         double_ver::run_calculation(argc, argv);
    //     else if (mode == "analiza")
    //         translation_variables::run_calculation(argc, argv);
    //     else if (mode == "PQ_DTheta_K")
    //         translation_variables::write_trajectory_data("data_out/C_good_data/y_266_11_od_0_78.89000.out");
    //     else {
    //         std::cerr << "Nieznany tryb: " << mode << "\n\n" << "Dostepne tryby:\n"
    //             << "  obliczenie\n" << "  analiza\n" << "  PQ_DTheta_K (domyślny)\n";
    //         return 1;
    //     }
    // } catch (const std::exception& e) {
    //     std::cerr << "Blad: " << e.what() << '\n';
    //     return 1;
    // }




    return 0;
}
