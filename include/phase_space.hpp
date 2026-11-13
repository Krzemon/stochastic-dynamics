#pragma once

#include "config.hpp"

#include <cmath>
#include <functional>
#include <optional>
#include <vector>

constexpr double C_MIN = M_PI / 5.0;
constexpr double C_MAX = 4.0*M_PI / 5.0;
constexpr size_t NC = 100;
constexpr size_t N_SKIP = 100151; // zostaje 150 000

using KMethod = std::function<double(const vec&, const vec&)>;
// KMethod k_method = math_utils::correlation;
// KMethod k_method = math_utils::regression;

namespace phase_space {

    /**
     * @brief Centruje dane poprzez odjęcie średniej
     */
    vec center_data(
        const vec& data
    );

    /**
     * @brief Oblicza p i q na podstawie szeregu
     */
    void calc_pq(
        const vec& phi,
        double c,
        vec& p,
        vec& q
    );

    /**
     * @brief Oblicza średni kwadrat przesunięcia M(n)
     */
    vec calc_Mn( 
        const vec& phi,
        const vec& p,
        const vec& q,
        double c,
        size_t n_cut
    );

    /**
     * @brief Oblicza czynnik oscylacyjny V_osc(c,n)
     */
    vec calc_Vosc( 
        const vec& phi,
        const vec& p,
        const vec& q,
        double c,
        size_t n_cut
    );

    /**
     * @brief Oblicza wartości D(n) = M(n) - V_osc(c,n)
     */
    vec calc_Dn( 
        const vec& phi,
        const vec& p,
        const vec& q,
        double c,
        size_t n_cut
    );

    /**
     * @brief Oblicza K dla pojedynczego c
     */
    double calc_k(
        const vec& phi,
        double c,
        size_t n_cut,
        const KMethod& k_method
    );

    /**
     * @brief Oblicza K dla wszystkich wartości c
     */
    vec calc_k_values(
        const vec& phi,
        const vec& c_values,
        const KMethod& k_method
    );

    /**
     * @brief Zapisuje trajektorię p,q dla c najbliższego medianie K
     */
    void save_pq(
        const vec& phi,
        const vec& c_values,
        const vec& k_values,
        double median_k,
        const std::string& out_dir,
        size_t col
    );

    /**
     * @brief Główna funkcja analizy
     * @param c_fixed opcjonalna lista wartości c; jeśli pusta, losuje z rozkładu
     */
    void run(
        const mat& data,
        const KMethod& k_method,
        bool center = false,
        const std::string& out_dir = "",
        std::optional<std::vector<double>> c_fixed = std::nullopt
    );

}