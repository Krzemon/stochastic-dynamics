#pragma once
#include <filesystem>
#include <fstream>
// #include <iostream>
#include <iomanip>
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

struct FileHandle {
    fs::path path;
    std::ofstream out;
};

/**
 * @brief Przygotowuje plik do zapisu danych w katalogu data projektu
 */
inline FileHandle prepareDataFile(const std::string& filename = "out.dat",
                                  const std::string& data_dir_name = "data_out",
                                  const std::string& plot_dir_name = "plots",
                                  bool clear_data_dir = false,
                                  bool clear_plot_dir = false
) { 
    fs::path projectRoot = PROJECT_ROOT_DIR; // makro przekazane z CMake 
    fs::path dataDir = projectRoot / data_dir_name; 
    fs::path plotsDir = projectRoot / plot_dir_name; 
    fs::create_directories(dataDir); 
    fs::create_directories(plotsDir); 

    if (clear_data_dir)
        for (const auto& entry : fs::directory_iterator(dataDir)) 
            fs::remove_all(entry);

    if (clear_plot_dir)
        for (const auto& entry : fs::directory_iterator(plotsDir))
            fs::remove_all(entry);

    FileHandle file;
    file.path = dataDir / filename;
    file.out.open(file.path);

    if (!file.out.is_open())
        throw std::runtime_error("Nie można otworzyć pliku: " + file.path.string());

    return file;
}
