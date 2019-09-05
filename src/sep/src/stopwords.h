/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/sep/src/stopwords.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-04_10:47:42
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_SEP_STOPWORDS_H__
#define __CHATOPERA_BOT_SEP_STOPWORDS_H__

#include <set>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

namespace chatopera {
namespace bot {
namespace sep {

class Stopwords {
 public:
  Stopwords();
  ~Stopwords();
  bool init(const string& stopwords_dict_path);
  bool contains(const string& word);

 private:
  set<string>* _dict;
};


} // namespace sep
} // namespace bot
} // namespace chatopera




#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
