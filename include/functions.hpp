#pragma once

#include <vector>
#include <string>

namespace doubleVer 
{
    constexpr int N = 5;
    constexpr int OMEGA = 250150;
    constexpr double NI = 0.7;
    constexpr double F1 = 0.0;
    constexpr double F3 = 0.0;
    constexpr int TS = 0;

    /**
     * @brief Odczytuje parametry a1, a3 i macierz s
     */
    void readInputFile(const char* filename, double& a1, double& a3, double s[N][N]);

    /**
     * @brief Przelicza wagi przed ciężką pętlą
     */
    std::vector<double> precomputeWeights(int omega, double ni);

    /**
     * @brief Wykonuje główną pętlę obliczeniową wektorów y
     */
    void calculate(
        const double s[N][N],
        double a1, double a2, double a3,
        const std::vector<double>& weights,
        std::vector<double>& y0,
        std::vector<double>& y1,
        std::vector<double>& y2,
        std::vector<double>& y3,
        std::vector<double>& y4
    );

    /**
     * @brief Zapisuje wektory do pliku tekstowego "y_<caseName>_<powtAlfa>_od_<TS>_<a2/100>
     */
    void writeResults(
        const char* caseName,
        const char* powtAlfa,
        double a2,
        const std::vector<double>& y0,
        const std::vector<double>& y1,
        const std::vector<double>& y2,
        const std::vector<double>& y3,
        const std::vector<double>& y4,
        const std::string& type
    );

    /**
     * @brief Wczytuje zapisane wyniki z pliku tekstowego do wektorów
     */
    void readResults_out(
        const char* filename,
        std::vector<double>& y0,
        std::vector<double>& y1,
        std::vector<double>& y2,
        std::vector<double>& y3,
        std::vector<double>& y4
    );

    /**
     * @brief Wykonuje kod "doubleVer"
     */
    void runCalculation(int argc, char* argv[]);
    
    /**
     * @brief Obsługuje tryb odczytu z pliku
     */
    // void runRead(int argc, char* argv[]);

} 

// namespace analysis 
// {
//     void analyze(
//         const std::vector<double>& y0,
//         const std::vector<double>& y1,
//         const std::vector<double>& y2,
//         const std::vector<double>& y3,
//         const std::vector<double>& y4
//     );
// }