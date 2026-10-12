#pragma once

#include <filesystem>
#include <fstream>
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
                                  bool clear_plot_dir = false)
{ 
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

/**
 * @brief Zapisuje wektor do pliku binarnego
 */
inline void write_binary(const std::string& filename, 
                         const std::vector<double>& data)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filename);

    const size_t size = data.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    file.write(reinterpret_cast<const char*>(data.data()), size * sizeof(double));
    if (!file)
        throw std::runtime_error("Błąd zapisu pliku: " + filename);
}  

/**
 * @brief Wczytuje wektor z pliku binarnego
 */
inline std::vector<double> read_binary(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filename);

    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!file)
        throw std::runtime_error("Błąd odczytu pliku: " + filename);

    std::vector<double> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size * sizeof(double));
    if (!file)
        throw std::runtime_error("Błąd odczytu danych z pliku: " + filename);

    return data;
}