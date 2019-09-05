/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /home/hain/git/chatopera.bs/NLP/Distance/levenshteinDistance.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-02-20_18:50:42
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_DISTANCE_LEVENSHTEIN__
#define __CHATOPERA_BOT_DISTANCE_LEVENSHTEIN__

#include <string>
#include <vector>

using namespace std;

namespace chatopera {
namespace bot {
namespace distance {

class LevenshteinDistance {
 public: // methods
  LevenshteinDistance();
  static double score(const vector<string>& lhs, const vector<string>& rhs);
};

} // namespace distance
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
