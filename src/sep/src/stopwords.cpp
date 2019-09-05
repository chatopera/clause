/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/sep/src/stopwords.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-04_10:48:51
 * @brief
 *
 **/


#include "stopwords.h"

#include <boost/algorithm/string.hpp>
#include "glog/logging.h"

using namespace boost::algorithm;

namespace chatopera {
namespace bot {
namespace sep {

Stopwords::Stopwords() {
  _dict = new set<string>();
};

Stopwords::~Stopwords() {
  delete _dict;
};

bool Stopwords::init(const string& config) {
  _dict->clear();
  // load words from file
  ifstream f(config);
  CHECK(f.is_open()) << "Can not open file " << config;

  string line;

  while(getline(f, line)) {
    trim(line);
    _dict->insert(line);
  }

  VLOG(2) << "load stopwords [length " << _dict->size() << "] from " << config << " successfully." ;
  return true;
};

bool Stopwords::contains(const string& word) {
  return _dict->find(word) != _dict->end();
};


} // namespace sep
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
