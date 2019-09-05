/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/distance/src/similarity.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-26_10:25:33
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_DISTANCE_SIMILARITY_H__
#define __CHATOPERA_BOT_DISTANCE_SIMILARITY_H__


#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "levenshtein.h"
#include "wordoverlap.h"

using namespace std;
using namespace boost::algorithm;

namespace chatopera {
namespace bot {
namespace distance {

class Similarity {
 public: // constructors
  Similarity();
  ~Similarity();

 public: // functions
  double compare(const vector<string>& lhs, const vector<string>& rhs);
  bool   sort(const std::vector<std::string>& post,
              const vector<pair<string, vector<string> > >& relevants,
              vector<pair<string, double> >& scores);
};

} // namespace distance
} // namespace bot
} // namespace chatopera













#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
