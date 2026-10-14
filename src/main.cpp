#include "double_ver.hpp"
#include "translation_variables.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    std::string mode = "alpha_seq";  // Domyślny tryb
    
    if (argc >= 2) {
        mode = argv[1];
    } else {
        std::cout << "Brak argumentu - uruchamiam domyślny tryb: " << mode << "\n\n";
    }

    try {
        if (mode == "obliczenie")
            double_ver::run_calculation(argc, argv);
        else if (mode == "analiza")
            translation_variables::run_calculation(argc, argv);
        else if (mode == "alpha_seq")
            translation_variables::write_trajectory_data("data_out/C_good_data/y_266_11_od_0_83.90000.out");
        else {
            std::cerr << "Nieznany tryb: " << mode << "\n\n" << "Dostepne tryby:\n"
                << "  obliczenie\n" << "  analiza\n" << "  alpha_seq (domyślny)\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
