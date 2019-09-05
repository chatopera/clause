/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /home/hain/git/chatopera.bs/NLP/Distance/levenshteinDistance.cc
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-02-20_18:50:50
 * @brief
 *
 **/


#include "levenshtein.h"

using namespace std;

namespace chatopera {
namespace bot {
namespace distance {

LevenshteinDistance::LevenshteinDistance() {
};

double LevenshteinDistance::score(const vector<string>& lhs, const vector<string>& rhs) {
  const size_t m(lhs.size());
  const size_t n(rhs.size());

  if (m == 0 || n == 0)
    return 0.0;

  size_t *costs = new size_t[n + 1];

  for( size_t k = 0; k <= n; k++ ) costs[k] = k;

  size_t i = 0;

  for (vector<string>::const_iterator it1 = lhs.begin(); it1 != lhs.end(); ++it1, ++i ) {
    costs[0] = i + 1;
    size_t corner = i;

    size_t j = 0;

    for (vector<string>::const_iterator it2 = rhs.begin(); it2 != rhs.end(); ++it2, ++j ) {
      size_t upper = costs[j + 1];

      if( *it1 == *it2 ) {
        costs[j + 1] = corner;
      } else {
        size_t t(upper < corner ? upper : corner);
        costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
      }

      corner = upper;
    }
  }

  size_t result = costs[n];
  delete [] costs;

  return 1.0 - result / (double) max(m, n);
};

} // namespace distance
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
