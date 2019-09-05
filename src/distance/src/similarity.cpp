/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/distance/src/similarity.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-26_10:25:21
 * @brief
 *
 **/


#include "similarity.h"
#include <iomanip>

#include "glog/logging.h"

namespace chatopera {
namespace bot {
namespace distance {

Similarity::Similarity() {
}

Similarity::~Similarity() {
}

double Similarity::compare(const vector<string>& lhs, const vector<string>& rhs) {
  VLOG(4) << __func__ << " lhs: " << join(lhs, " ") << " , rhs: " << join(rhs, " ");


  vector<string> x = lhs;
  vector<string> y = rhs;

  // sort string vectors
  std::sort(x.rbegin(), x.rend(), [] (const string & lhs, const string & rhs) {
    return lhs.compare(rhs) >= 0;
  });

  std::sort(y.rbegin(), y.rend(), [] (const string & lhs, const string & rhs) {
    return lhs.compare(rhs) >= 0;
  });

  double overlap = WordOverlap::score(x, y);
  double levenshteinDistance = LevenshteinDistance::score(x, y);
  VLOG(4) << __func__ << " compare overlap: " << overlap << ", levenshteinDistance: " << levenshteinDistance;
  double result = 0.0;

  // fast return for high overlap
  if(overlap > 0.9 && levenshteinDistance > 0.4) {
    result = overlap;
  } else if(overlap > 0.8 && levenshteinDistance > 0.8) {
    result = std::max(0.9, std::max(overlap, levenshteinDistance));
  } else {
    result = (0.5 * overlap) + (0.5 * levenshteinDistance);
  }

  return std::min(result, 1.0);
};

/**
 * 排序接口
 */
bool Similarity::sort(const std::vector<std::string>& post,
                      const vector<pair<string, vector<string> > >& relevants,
                      vector<pair<string, double> >& scores) {
  VLOG(3) << __func__ << " post: " << join(post, "");

  for(const pair<string, vector<string> >& relevant : relevants) {
    double score = compare(post, relevant.second);
    VLOG(3) << __func__ << " relevant: " << join(relevant.second, "")
            << ", score: " << std::fixed << std::setprecision(6) << score ;
    scores.push_back(make_pair(relevant.first, score));
  }

  std::sort(scores.rbegin(), scores.rend(), [](const pair<string, double>& lhs, const pair<string, double>& rhs) {
    return lhs.second < rhs.second;
  });
};

} // namespace distance
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
