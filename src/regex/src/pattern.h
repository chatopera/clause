/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/clause/src/clause/src/pattern.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-12-11_14:45:51
 * @brief 正则表达式词典
 *
 **/


#ifndef __CHATOPERA_BOT_CLAUSE_PATTERN_H__
#define __CHATOPERA_BOT_CLAUSE_PATTERN_H__

#include <string>
#include <vector>
#include <boost/regex.hpp>
#include <sstream>

#include "glog/logging.h"
#include "StringUtils.hpp"
#include "json/json.h"

using namespace std;

namespace chatopera {
namespace bot {
namespace clause {

/**
 * 表达式匹配的内容
 */
struct PatternDictMatch {
  string source;   // 原始输入
  string dictname; // 正则表达式词典名字
  string dict_id;  // 正则表达式词典ID
  string val;      // 匹配值
  int    begin;    // 开始
  int    end;      // 结尾
  string regex;    // 匹配的表达式
};

class PatternRegex {
 public: // constructors
  PatternRegex();
  ~PatternRegex();

 public: // functions
  static bool match(const string& pattern, const string& source, PatternDictMatch& pdm);
  static bool checkBoostPcreGrammar(const string& pattern, string& error_msg);
};

} // namespace clause
} // bot
} // chatopera











#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
