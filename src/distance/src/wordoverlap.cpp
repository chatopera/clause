/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /home/hain/git/chatopera.bs/NLP/Distance/wordoverlap.cc
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-03-19_18:50:00
 * @brief
 *
 **/


#include "wordoverlap.h"

namespace chatopera {
namespace bot {
namespace distance {

WordOverlap::WordOverlap() {
};

WordOverlap::~WordOverlap() {
};

double WordOverlap::score(const vector<string>& lhs, const vector<string>& rhs) {
  unsigned int size_lhs = lhs.size();
  unsigned int size_rhs = rhs.size();

  if (size_lhs == 0 || size_rhs == 0) {
    return 0.0;
  }

  vector<string> intersection;
  std::set_intersection(lhs.begin(), lhs.end(),
                        rhs.begin(), rhs.end(),
                        std::back_inserter(intersection));
  return size_lhs >= size_rhs ? ((double)intersection.size() / (double) size_lhs) : ((double) intersection.size() / (double) size_rhs);
};

double WordOverlap::score(const set<string>& lhs, const set<string>& rhs) {
  unsigned int size_lhs = lhs.size();
  unsigned int size_rhs = rhs.size();

  if (size_lhs == 0 || size_rhs == 0) {
    return 0.0;
  }

  unsigned int size_min = size_lhs >= size_rhs ? size_rhs : size_lhs;
  unsigned int size_max = size_lhs >= size_rhs ? size_lhs : size_rhs;

  set<string> intersection;
  std::set_intersection(lhs.begin(), lhs.end(),
                        rhs.begin(), rhs.end(),
                        std::inserter(intersection, intersection.begin()));

  return ((double) intersection.size() / (double) size_min) * std::max(((double) size_min / (double) size_max), 0.8);
};

} // namespace distance
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
