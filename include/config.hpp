#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using usize = std::size_t;
namespace fs = std::filesystem;

const fs::path project_root = PROJECT_ROOT_DIR;

/**
 * @brief Tworzy główne katalogi data/ i plots/.
 */
inline void prepare_directories(
    bool clear_data = false,
    bool clear_plots = false,
    const std::string& data_dir_name = "data",
    const std::string& plots_dir_name = "plots")
{
    const fs::path data_dir = project_root / data_dir_name;
    const fs::path plots_dir = project_root / plots_dir_name;

    fs::create_directories(data_dir);
    fs::create_directories(plots_dir);

    if (clear_data) {
        for (const auto& entry : fs::directory_iterator(data_dir))
            fs::remove_all(entry);
    }

    if (clear_plots) {
        for (const auto& entry : fs::directory_iterator(plots_dir))
            fs::remove_all(entry);
    }
}


/**
 * @brief Przygotowuje plik tekstowy do zapisu.
 * filename może zawierać podkatalogi, np.:
 *
 *   "wyniki.dat"
 *   "zad_1/wyniki.dat"
 *   "data/zad_1/part_A/wyniki.dat"
 */
inline std::ofstream prepare_data_file(
    const std::string& filename,
    const std::string& data_dir_name = "data")
{
    const fs::path filepath = project_root / data_dir_name / filename;
    fs::create_directories(filepath.parent_path());

    std::ofstream file(filepath);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

    return file;
}


/**
 * @brief Zapisuje vector<double> do pliku binarnego.
 */
inline void write_binary_vector(
    const std::string& filename,
    const std::vector<double>& data)
{
    const fs::path filepath = project_root / filename;

    fs::create_directories(filepath.parent_path());

    std::ofstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

    const std::uint64_t size = data.size();
    file.write(reinterpret_cast<const char*>(&size), 
               sizeof(size));

    file.write(reinterpret_cast<const char*>(data.data()),
               static_cast<std::streamsize>(data.size() * sizeof(double)));
    if (!file)
        throw std::runtime_error("Błąd zapisu pliku: " + filepath.string());
}


/**
 * @brief Wczytuje vector<double> z pliku binarnego.
 */
inline std::vector<double> read_binary_vector(const std::string& filename)
{
    const fs::path filepath = project_root / filename;
    std::ifstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

    std::uint64_t size;
    file.read(reinterpret_cast<char*>(&size), 
              sizeof(size));
    if (!file)
        throw std::runtime_error("Błąd odczytu rozmiaru pliku: " + filepath.string());

    std::vector<double> data(size);
    file.read(reinterpret_cast<char*>(data.data()),
              static_cast<std::streamsize>(size * sizeof(double)));
    if (!file)
        throw std::runtime_error("Błąd odczytu danych z pliku: " + filepath.string());

    return data;
}


/**
 * @brief Zapisuje macierz (vector<vector<double>>) do pliku binarnego i metadane
 * 
 * Example usage:
 *
 * write_binary("wyniki.bin", {x, y, z}, {"x", "y", "z"});
 */
inline void write_binary(
    const std::string& filename,
    const std::vector<std::vector<double>>& columns,
    const std::vector<std::string>& names)
{
    if (columns.empty())
        throw std::runtime_error("Brak danych.");

    if (columns.size() != names.size())
        throw std::runtime_error("Liczba nazw musi odpowiadać liczbie kolumn.");

    const std::uint64_t rows = columns[0].size();
    const std::uint64_t cols = columns.size();

    for (const auto& column : columns) {
        if (column.size() != rows)
            throw std::runtime_error("Wszystkie kolumny muszą mieć taki sam rozmiar.");
    }

    const fs::path filepath = project_root / filename;
    fs::create_directories(filepath.parent_path());

    // Zapis danych binarnych
    std::vector<double> buffer(rows * cols);
    for (std::uint64_t i = 0; i < rows; ++i)
        for (std::uint64_t j = 0; j < cols; ++j)
            buffer[i * cols + j] = columns[j][i];

    std::ofstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

    file.write(
        reinterpret_cast<const char*>(buffer.data()),
        static_cast<std::streamsize>(buffer.size() * sizeof(double)));
    if (!file)
        throw std::runtime_error("Błąd zapisu pliku: " + filepath.string());

    // Zapis metadanych
    fs::path meta_path = filepath;
    meta_path.replace_extension(".meta");
    std::ofstream meta(meta_path);
    if (!meta)
        throw std::runtime_error("Nie można otworzyć pliku: " + meta_path.string());

    meta << "rows=" << rows << '\n';
    meta << "cols=" << cols << '\n';

    for (std::size_t j = 0; j < names.size(); ++j) {
        if (j > 0)
            meta << ' ';
        meta << names[j];
    }
    meta << '\n';
}


/**
 * @brief Wczytuje nazwy kolumn z pliku metadanych (.meta)
 * 
 * Example usage:
 *
 * auto names = read_column_names("wyniki.meta"); // names[0] = x, names[1] = y
 */
inline std::vector<std::string>
read_column_names(const std::string& meta_filename)
{
    std::ifstream meta(project_root / meta_filename);
    if (!meta)
        throw std::runtime_error("Nie można otworzyć pliku: " + meta_filename);

    std::string line;

    std::getline(meta, line); // rows
    std::getline(meta, line); // cols
    std::getline(meta, line); // nazwy kolumn (zmiennych)

    std::istringstream iss(line);

    std::vector<std::string> names;
    std::string name;

    while (iss >> name)
        names.push_back(name);

    return names;
}


/**
 * @brief Wczytuje macierz danych (vector<vector<double>>) z pliku binarnego
 * 
 * Example usage:
 *
 * auto data = read_binary("wyniki.bin");
 */
inline std::vector<std::vector<double>>
read_binary(const std::string& filename)
{
    const fs::path filepath = project_root / filename;
    fs::path meta_path = filepath;
    meta_path.replace_extension(".meta");

    std::ifstream meta(meta_path);
    if (!meta)
        throw std::runtime_error("Nie można otworzyć pliku: " + meta_path.string());

    std::uint64_t rows, cols;
    std::string line;

    std::getline(meta, line);
    rows = std::stoull(line.substr(line.find('=') + 1));
    std::getline(meta, line);
    cols = std::stoull(line.substr(line.find('=') + 1));
    std::getline(meta, line);    // Pomijamy nazwy kolumn

    std::ifstream file(filepath, std::ios::binary);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

    std::vector<double> buffer(rows * cols);
    file.read(
        reinterpret_cast<char*>(buffer.data()),
        static_cast<std::streamsize>(buffer.size() * sizeof(double)));

    if (!file)
        throw std::runtime_error("Błąd odczytu pliku: " + filepath.string());

    std::vector<std::vector<double>> columns(cols, std::vector<double>(rows));

    for (std::uint64_t i = 0; i < rows; ++i)
        for (std::uint64_t j = 0; j < cols; ++j)
            columns[j][i] = buffer[i * cols + j];

    return columns;
}