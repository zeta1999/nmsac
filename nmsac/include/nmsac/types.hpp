#pragma once
//! c/c++ headers
#include <iostream>
//! dependency headers
//! project headers

namespace nmsac {
struct ConfigNMSAC {
    ConfigNMSAC() : random_seed(11011),
        print_status(false),
        ps(0.99),
        max_iter(1e4),
        min_iter(5),
        k(4),
        points_per_sample(12),
        epsilon(0.015),
        n_pair_thresh(5),
        pair_dist_thresh(1e-2),
        max_iter_icp(100),
        tol_icp(1e-8),
        outlier_rej_icp(0.2) { }
    uint64_t random_seed;
    bool print_status;
    double ps;
    size_t max_iter;
    size_t min_iter;
    size_t k;
    size_t points_per_sample;
    double epsilon;
    size_t n_pair_thresh;
    double pair_dist_thresh;
    size_t max_iter_icp;
    double tol_icp;
    double outlier_rej_icp;
};
};  // namespace nmsac