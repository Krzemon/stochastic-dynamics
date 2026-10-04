#include "double_ver.hpp"
#include "translation_variables.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Brak trybu pracy.\n\n" << "Dostepne tryby:\n"
            << "  obliczenie\n" << "  analiza\n"; 
        return 1;
    }

    try {
        if (std::strcmp(argv[1], "obliczenie") == 0)
            double_ver::run_calculation(argc, argv);
        else if (std::strcmp(argv[1], "analiza") == 0)
            translation_variables::run_calculation(argc, argv);
        else
            throw std::invalid_argument("Nieznany tryb: " + std::string(argv[1]));
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}