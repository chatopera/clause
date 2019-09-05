/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /home/hain/git/chatopera.bs/NLP/Distance/wordoverlap.cc
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-03-19_18:49:46
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_DISTANCE_OVERLAP__
#define __CHATOPERA_BOT_DISTANCE_OVERLAP__

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

#include "StringUtils.hpp"

using namespace std;

namespace chatopera {
namespace bot {
namespace distance {

class WordOverlap {
 public: // constructors
  WordOverlap();
  ~WordOverlap();

 public: // functions
  static double score(const vector<string> & lhs, const vector<string>& rhs);
  static double score(const set<string> & lhs, const set<string>& rhs);
};

} // namespace distance
} // namespace bot
} // namespace chatopera













#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
