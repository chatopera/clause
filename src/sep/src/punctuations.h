/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/sep/src/punctuations.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-04_10:47:51
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_SEP_PUNT_H__
#define __CHATOPERA_BOT_SEP_PUNT_H__

#include <string>
#include <set>
#include <vector>
#include <fstream>

using namespace std;

namespace chatopera {
namespace bot {
namespace sep {

class Punctuations {
 public:
  Punctuations();
  ~Punctuations();
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
