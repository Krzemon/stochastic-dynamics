#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstddef>
#include <cstring>

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


inline std::vector<std::vector<double>> 
read_text_columns(const std::string& filename, size_t skip_rows = 0)
{
    // Otwieramy plik w trybie binarnym od razu na końcu (std::ios::ate), aby poznać jego dokładny rozmiar
    std::ifstream f(filename, std::ios::binary | std::ios::ate);
    if (!f)
        throw std::runtime_error("Nie można otworzyć pliku: " + filename);

    const std::streamsize size = f.tellg();
    if (size <= 0)
        return {};

    // Wczytujemy cały plik jednym strzałem do RAM-u
    std::string buffer(static_cast<size_t>(size), '\0');
    f.seekg(0, std::ios::beg);
    if (!f.read(buffer.data(), size))
        throw std::runtime_error("Błąd odczytu pliku: " + filename);
    f.close(); // Zamykamy plik, bo wszystko mamy już w pamięci

    // Szybko zliczamy wiersze w pamięci RAM i wyliczamy rozmiar do rezerwacji
    const size_t total_rows = std::count(buffer.begin(), buffer.end(), '\n') + 1;
    const size_t expected_rows = (total_rows > skip_rows) ? (total_rows - skip_rows) : 0;

    std::vector<std::vector<double>> cols;
    const char* ptr = buffer.data();
    const char* const end = ptr + buffer.size();
    size_t row = 0;

    while (ptr < end)
    {
        // Znajdujemy koniec bieżącej linii
        const char* line_end = static_cast<const char*>(std::memchr(ptr, '\n', end - ptr));
        if (!line_end) 
            line_end = end;

        if (row++ >= skip_rows)
        {
            const char* line_ptr = ptr;
            size_t col_idx = 0;
            const bool is_first_data_row = cols.empty();

            while (line_ptr < line_end)
            {
                // Pomijamy białe znaki w obrębie linii
                while (line_ptr < line_end && (*line_ptr == ' ' || *line_ptr == '\t' || *line_ptr == '\r'))
                    ++line_ptr;

                if (line_ptr == line_end) 
                    break;

                double v;
                auto [next_ptr, ec] = std::from_chars(line_ptr, line_end, v);

                if (ec != std::errc()) 
                    break;

                if (is_first_data_row)
                {
                    cols.emplace_back();
                    cols.back().reserve(expected_rows);
                }
                else if (col_idx >= cols.size())
                {
                    break; // Ignorujemy nadmiarowe kolumny
                }

                cols[col_idx++].push_back(v);
                line_ptr = next_ptr;
            }
        }

        // Przesuwamy wskaźnik na początek kolejnej linii
        ptr = (line_end < end) ? (line_end + 1) : end;
    }

    return cols; 
}

// time check
// #include <chrono>
// // 1. Zapisujemy czas przed wywołaniem funkcji
// auto start = std::chrono::high_resolution_clock::now();

// // Wywołanie Twojej funkcji
// auto dane = read_text_columns("twoj_plik.txt", 1);

// // 2. Zapisujemy czas tuż po zakończeniu funkcji
// auto stop = std::chrono::high_resolution_clock::now();

// // 3. Obliczamy różnicę (np. w milisekundach z ułamkiem)
// std::chrono::duration<double, std::milli> czas_ms = stop - start;

// std::cout << "Czas wczytywania: " << czas_ms.count() << " ms\n";




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
