//! c/c++ headers
#include <iostream>
#include <memory>
#include <utility>
//! dependency headers
#include <mlpack/core.hpp>
#include "transforms/common/utilities.hpp"
#include "transforms/svd/svd.hpp"
#include "correspondences/common/base.hpp"
#include "correspondences/qap/qap.hpp"  // TODO(jwd): need to figure out a way to add this at compile time via cmake switch
//! project headers
#include "nmsac/helper.hpp"
#include "nmsac/registration.hpp"

namespace cor = correspondences;
namespace xfrm = transforms;

/**
 * @brief Run full point-set registration pipeline: including subsampling, correspondence solution, and homogenous transformation
 * identification.
 *
 * @param [in] src_sub subsampled source points
 * @param [in] tgt_sub subsampled target points
 * @param [in] config NMSAC configuration struct
 * @param [in][out] optimal_rot best rotation between source and target points, identified in translate-then-rotate mapping
 * @param [in][out] optimal_trans best translation between source and target points, identified in translate-then-rotate mapping
 * @param [in][out] src_corr_ids indices of points in source that were matched
 * @param [in][out] tgt_corr_ids indices of points in target that were matched
 * @return true if all algorithm stages were successful, false otherwise
 */
bool nmsac::registration(arma::mat const & src_sub, arma::mat const & tgt_sub,
    nmsac::ConfigNMSAC const & config, arma::mat33 & optimal_rot, arma::vec3 & optimal_trans,
    arma::uvec & src_corr_ids, arma::uvec & tgt_corr_ids) noexcept {
  //! check input validity
  if (src_sub.n_rows != 3) {
    std::cout << static_cast<std::string>(__func__) <<
      ": First argument must be a matrix with 3 rows" << std::endl;
    return false;
  } else if (tgt_sub.n_rows != 3) {
    std::cout << static_cast<std::string>(__func__) <<
      ": Second argument must be a matrix with 3 rows" << std::endl;
    return false;
  }

  /**
   * @todo need a better abstraction for sharing config data
   */
  cor::qap::Config reg_config;
  reg_config.epsilon = config.epsilon;
  reg_config.pairwise_dist_threshold = config.pair_dist_thresh;
  reg_config.n_pair_threshold = config.n_pair_thresh;

  /**
   * core computation is done by external lib function call;
   * see Register package usage in CMakeLists.txt
   */
  cor::correspondences_t corrs;
  std::unique_ptr<cor::CorrespondencesBase> corr_object = std::make_unique<cor::QAP>(
      src_sub, tgt_sub, reg_config);

  /**
   * @todo check for status and exit on failure
   */
  auto const corr_status = corr_object->calc_correspondences(corrs);


  /**
   * @todo check for status and exit on failure
   */
  arma::mat44 H_opt;
  auto const xfrm_status = xfrm::best_fit_transform(src_sub, tgt_sub, corrs, H_opt);

  auto const & m = corrs.size();  //! == config.k
  src_corr_ids.resize(m);
  tgt_corr_ids.resize(m);
  for (auto [i, c] : xfrm::enumerate(corrs)) {
    auto const p = c.first;
    src_corr_ids(i) = p.first;
    tgt_corr_ids(i) = p.second;
  }

  //! convert from homogeneous transform to (R, t)
  from_homog(optimal_rot, optimal_trans, H_opt);
  return true;
}
