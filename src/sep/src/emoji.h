/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/sep/src/emoji.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-04_10:47:32
 * @brief
 *
 **/


#ifndef __CHATOPERA_CLAUSE_SEP_EMOJI_H__
#define __CHATOPERA_CLAUSE_SEP_EMOJI_H__

#include <set>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace chatopera {
namespace bot {
namespace sep {

class Emojis {
 public:
  Emojis();
  ~Emojis();
  bool init(const string& config);
  bool contains(const string& word);

 private:
  set<string>* _dict;
};

} // namespace sep
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
