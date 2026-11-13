#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstddef>

using vec = std::vector<double>;
using mat = std::vector<vec>;

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
 * @brief Przygotowuje plik tekstowy do zapisu, filename może zawierać podkatalogi.
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
 * @brief Wczytuje wielokolumnowy plik tekstowy
 */
inline std::vector<std::vector<double>> 
read_text_columns(const std::string& filename, size_t skip_rows=0)
{
    std::ifstream f(filename);
    if (!f)
        throw std::runtime_error("Nie można otworzyć pliku: " + filename);

    std::vector<std::vector<double>> cols;
    std::string line;
    size_t row = 0;

    while (std::getline(f, line))
    {
        if (row++ < skip_rows) continue;

        std::istringstream iss(line);
        double v;
        std::vector<double> vals;

        while (iss >> v) vals.push_back(v);

        if (vals.empty()) continue;

        if (cols.empty()) cols.resize(vals.size());

        for (size_t i = 0; i < vals.size(); ++i)
            cols[i].push_back(vals[i]);
    }

    return cols;
}

namespace binary_file 
{
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

    /**
     * @brief Konwertuje plik tekstowy .out na wiele plików binarnych (1 kolumna = 1 plik)
     * 
     * @param text_filepath Ścieżka do pliku tekstowego (np. "data_out/result.out")
     * @param output_prefix Prefix dla plików wyjściowych (np. "data/phi_y")
     *                      Wynik: phi_y0.bin, phi_y1.bin, phi_y2.bin, ...
     * @param skip_rows Liczba wierszy nagłówka do pominięcia
     */
    inline void convert_text_to_binary_columns(
        const std::string& text_filepath,
        const std::string& output_prefix,
        size_t skip_rows = 0)
    {
        auto cols = read_text_columns(text_filepath, skip_rows);

        if (cols.empty())
            throw std::runtime_error("Brak danych w pliku: " + text_filepath);

        for (size_t i = 0; i < cols.size(); ++i)
        {
            std::string bin_filename = output_prefix + std::to_string(i) + ".bin";
            write_binary_vector(bin_filename, cols[i]);
        }

        std::cout << "Konwersja: " << text_filepath << " -> " 
                << cols.size() << " plików binarnych (" << output_prefix << "*.bin)\n";
    }


    /**
     * @brief Konwertuje pliki binarne z powrotem na plik tekstowy .dat
     * 
     * @param bin_filepath_pattern Prefix plików binarnych (np. "data/phi_y")
     * @param text_filepath Ścieżka do pliku wyjściowego (np. "data/output.dat")
     * @param n_cols Liczba kolumn (plików binarnych)
     */
    inline void convert_binary_to_text(
        const std::string& bin_filepath_pattern,
        const std::string& text_filepath,
        size_t n_cols)
    {
        std::vector<std::vector<double>> cols(n_cols);

        for (size_t i = 0; i < n_cols; ++i)
        {
            std::string bin_filename = bin_filepath_pattern + std::to_string(i) + ".bin";
            cols[i] = read_binary_vector(bin_filename);
        }

        if (cols.empty() || cols[0].empty())
            throw std::runtime_error("Brak danych do konwersji");

        const size_t n_rows = cols[0].size();
        for (size_t i = 1; i < n_cols; ++i)
        {
            if (cols[i].size() != n_rows)
                throw std::runtime_error("Kolumny mają różne rozmiary");
        }

        const fs::path filepath = project_root / text_filepath;
        fs::create_directories(filepath.parent_path());

        std::ofstream file(filepath);
        if (!file)
            throw std::runtime_error("Nie można otworzyć pliku: " + filepath.string());

        for (size_t row = 0; row < n_rows; ++row)
        {
            for (size_t col = 0; col < n_cols; ++col)
            {
                if (col > 0) file << " ";
                file << cols[col][row];
            }
            file << "\n";
        }

        std::cout << "Konwersja: " << n_cols << " plików binarnych (" 
                << bin_filepath_pattern << "*.bin) -> " << text_filepath << "\n";
    }
}

