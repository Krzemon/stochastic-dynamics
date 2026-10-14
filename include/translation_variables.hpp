#pragma once

#include "config.hpp"
#include <vector>
#include <string>

namespace translation_variables 
{
    double calc_K(const std::vector<double>& phi);

    void make_PQ(const std::vector<double>& phi, 
                 std::vector<double>& p,
                 std::vector<double>& q,    
                 double c, usize N);

    auto make_angle_distance(const std::vector<double>& phi, double c = 1.0) 
        -> std::pair<std::vector<double>, std::vector<double>>;

    std::vector<double> read_phi_from_file(const std::string& filename 
                               = "data_out/C_good_data/y_266_11_od_0_78.89000.out");

    void write_trajectory_data(const std::string& input_filepath, usize n_last = 7000, double c = 1.0);

    void run_calculation(int argc, char* argv[]);
}
