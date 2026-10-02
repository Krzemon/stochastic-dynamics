#include "functions.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Brak trybu pracy.\n\n" << "Dostepne tryby:\n"
            << "  obliczenie\n" << "  odczyt\n"; 
        return 1;
    }

    try {
        if (std::strcmp(argv[1], "obliczenie") == 0)
            doubleVer::runCalculation(argc, argv);
        // else if (std::strcmp(argv[1], "odczyt") == 0)
        //     doubleVer::runRead(argc, argv);
        else
            throw std::invalid_argument("Nieznany tryb: " +std::string(argv[1]));
    } catch (const std::exception& e) {
        std::cerr << "Blad: " << e.what() << '\n';
        return 1;
    }

    return 0;
}