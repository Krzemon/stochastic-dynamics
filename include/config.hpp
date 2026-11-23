#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstddef>

#include <charconv>  // std::from_chars
#include <algorithm> // std::count


using vec = std::vector<double>;
using mat = std::vector<vec>;

namespace fs = std::filesystem;
const fs::path project_root = PROJECT_ROOT_DIR;
const fs::path data_in_dir = project_root / "0=local" / "data_in";

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
 * @brief Przygotowuje plik tekstowy do zapisu, filename może zawierać podkatalogi.
 *
 *   "wyniki.dat"
 *   "zad_1/wyniki.dat"
 *   "data/zad_1/part_A/wyniki.dat"
 */
inline std::ofstream prepare_data_file(const std::string& filename)
{
    const std::string data_dir_name = "data";
    const fs::path filepath = project_root / data_dir_name / filename;
    fs::create_directories(filepath.parent_path());

    std::ofstream file(filepath);
    if (!file)
        throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

    return file;
}





/**
 * @brief Wczytuje wielokolumnowy plik tekstowy (wolniej)
 */
// inline std::vector<std::vector<double>> 
// read_text_columns_old(const std::string& filename, size_t skip_rows=0)
// {
//     std::ifstream f(filename);
//     if (!f)
//         throw std::runtime_error("Nie można otworzyć pliku: " + filename);

//     std::vector<std::vector<double>> cols;
//     std::string line;
//     size_t row = 0;

//     while (std::getline(f, line))
//     {
//         if (row++ < skip_rows) 
//             continue;

//         std::istringstream iss(line);
//         double v;
//         std::vector<double> vals;
//         while (iss >> v) 
//             vals.push_back(v);
//         if (vals.empty()) 
//             continue;
//         if (cols.empty()) 
//             cols.resize(vals.size());
//         for (size_t i = 0; i < vals.size(); ++i)
//             cols[i].push_back(vals[i]);
//     }

//     return cols;
// }
