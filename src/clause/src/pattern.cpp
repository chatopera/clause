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
 * 根据pattern搜索
 */
Json::Value PatternRegex::search(const DictPattern& pattern, DictPatternCheck& check) {
  Json::Value root;
  Json::Value data(Json::arrayValue);

  if((!pattern.__isset.patterns) || (pattern.patterns.size() == 0)) {
    root["rc"] = 1;
    root["error"] = "Error, none Patterns.";
  } else if((!check.__isset.input) || check.input.empty()) {
    root["rc"] = 2;
    root["error"] = "Error, empty input.";
  } else {
    const char *s =  check.input.c_str();

    for(vector<string>::const_iterator it = pattern.patterns.begin(); it != pattern.patterns.end(); it++) {
      VLOG(3) << __func__ << " pattern:" << *it;
      boost::regex expr{*it};
      boost::cmatch results;
      bool r = boost::regex_search(s, results, expr);

      if(r) {
        //如果匹配成功
        VLOG(3) << __func__ << " search [" << *it  << "], matched: " << results[0];

        for(boost::cmatch::iterator it2 = results.begin(); it2 != results.end(); ++it2) {
          Json::Value matched;
          //       指向子串对应首位置        指向子串对应尾位置          子串内容
          matched["val"] = it2->str();
          matched["begin"] = (int)(it2->first - s);
          matched["end"] = (int)(it2->second - s);
          matched["regex"] = *it;
          data.append(matched);
          break;
        }

        // 只获取一个结果，如果一个regex命中，则不继续查找
        break;
      }
    }

    root["rc"] = 0;
    root["data"] = data;
  }

  Json::FastWriter fastWriter;
  check.output = fastWriter.write(root);
  check.__isset.output = true;
  return root;
};

} // namespace clause
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
