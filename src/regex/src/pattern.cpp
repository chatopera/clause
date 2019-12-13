/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/clause/src/clause/src/pattern.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-12-11_14:45:36
 * @brief
 *
 **/

#include "pattern.h"

using namespace chatopera::utils;
using namespace std;

namespace chatopera {
namespace bot {
namespace clause {

PatternRegex::PatternRegex() {
};

PatternRegex::~PatternRegex() {
};

/**
 * 从一个字符串中根据正则表达式匹配出目标值
 */
bool PatternRegex::match(const string& pattern, const string& source, PatternDictMatch& pdm) {
  VLOG(3) << __func__ << " source: " << source << ", pattern: " << pattern;
  pdm.source = source;
  pdm.regex = pattern;

  boost::regex expr(pattern, boost::regex::perl);
  boost::cmatch results;

  const char *raw = source.c_str();
  bool r = boost::regex_search(raw, results, expr);

  if(r) {
    for(boost::cmatch::iterator it = results.begin(); it != results.end(); ++it) {
      //  指向子串对应首位置        指向子串对应尾位置          子串内容
      pdm.val = it->str();
      pdm.begin = (int)(it->first - raw);
      pdm.end = (int)(it->second - raw);
      VLOG(3) << __func__ << " val " << pdm.val  << ", range [" << pdm.begin << "," << pdm.end << "]";
      return true;
    }
  }

  return false;
};

} // namespace clause
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
