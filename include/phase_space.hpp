#pragma once

#include "config.hpp"

#include <cmath>
#include <functional>
#include <optional>
#include <vector>

constexpr double C_MIN = M_PI / 5.0;
constexpr double C_MAX = 4.0*M_PI / 5.0;
constexpr size_t NC = 100;        // losować parametru c w [C_MIN, C_MAX]
constexpr size_t N_SKIP = 100151; // zostaje 150 000 danych szeregu

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

    // void calc_pq_old(
    //     const vec& phi, 
    //     double c, 
    //     vec& p, 
    //     vec& q
    // );

    /**
     * @brief Oblicza p i q dla szeregu
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
     * @brief Oblicza K dla wektora wartości c
     */
    vec calc_k_values(
        const vec& phi,
        const vec& c_values,
        const KMethod& k_method
    );

    /**
     * @brief Zapisuje kolumne wartosci c oraz obliczone dla nich K
     */
    void save_k_col(
        const vec& c_values, 
        const vec& k_values,
        const std::string& out_dir, 
        size_t col
    );

    /**
     * @brief Oblicza kąty theta(n) i odległości d(n) między kolejnymi krokami p,q
     */
    std::pair<vec, vec> make_angle_distance(
        const vec& phi,
        const vec& p,
        const vec& q
    );

    /**
     * @brief Zapisuje trajektorię p,q oraz kąty dtheta dla wybranego c
     */
    void save_pq_dtheta(
        const vec& phi,
        const vec& p,
        const vec& q,
        double c_best,
        double k_best,
        double median_k,
        const std::string& out_dir,
        size_t col
    );

    /**
     * @brief Zapisuje trajektorię p,q dla c najbliższego medianie K
     */
    // void save_pq(
    //     const vec& phi,
    //     const vec& c_values,
    //     const vec& k_values,
    //     double median_k,
    //     const std::string& out_dir,
    //     size_t col
    // );

    /**
     * @brief Oblicza K, zapisuje phase_space_K_col, pq i angle_distance dla jednej kolumny
     */
    void run_pq_dtheta(
        const vec& phi,
        const KMethod& k_method,
        bool center = false,
        const std::string& out_dir = "",
        size_t col = 0,
        std::optional<std::vector<double>> c_fixed = std::nullopt
    );

    /**
     * @brief Oblicza K i zapisuje phase_space_K_col oraz pq dla jednej kolumny
     */
    // void run_pq(
    //     const vec& phi,
    //     const KMethod& k_method,
    //     bool center = false,
    //     const std::string& out_dir = "",
    //     size_t col = 0,
    //     std::optional<std::vector<double>> c_fixed = std::nullopt
    // );

    /**
     * @brief Oblicza K vs c oraz K vs Nc dla różnych liczb próbek c
     * Zapisuje szczegółowe dane K vs c dla każdego Nc oraz podsumowanie median K vs Nc
     */
    void run_k_vs_nc(
        const vec& phi,
        const KMethod& k_method,
        bool center = false,
        const std::string& out_dir = "",
        size_t col = 0,
        const std::vector<size_t>& nc_list = {50, 75, 100, 125, 150},
        std::optional<unsigned int> seed = std::nullopt
    );

}