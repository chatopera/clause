/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

#include "gtest/gtest.h"
#include "glog/logging.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#include <boost/regex.hpp>

TEST(ClauseTest, INIT) {
  LOG(INFO) << "INIT";
  EXPECT_TRUE(true) << "Not done.";
}

/**
 * 说明文章
 * https://blog.csdn.net/freeking101/article/details/54729274
 * regex_match只验证是否完全匹配
 */
TEST(ClauseTest, RegexMatch) {
  LOG(INFO) << "RegexMatch";

  //( 1 )   ((  3  )  2 )((  5 )4)(    6    )
  //(\w+)://((\w+\.)*\w+)((/\w*)*)(/\w+\.\w+)?
  //^协议://网址(x.x...x)/路径(n个\字串)/网页文件(xxx.xxx)
  const char *szReg = "(\\w+)://((\\w+\\.)*\\w+)((/\\w*)*)(/\\w+\\.\\w+)?";
  const char *szStr = "http://www.cppprog.com/2009/0112/48.html";

  // 字符串匹配
  boost::regex reg(szReg);
  boost::cmatch mat; // 提取字串
  bool r = boost::regex_match(szStr, mat, reg);

  if(r) { //如果匹配成功
    //显示所有子串
    for(boost::cmatch::iterator itr = mat.begin(); itr != mat.end(); ++itr) {
      //       指向子串对应首位置        指向子串对应尾位置          子串内容
      LOG(INFO) << itr->first - szStr << ' ' << itr->second - szStr << ' ' << *itr << endl;
    }
  }

  //也可直接取指定位置信息
  if(mat[4].matched) LOG(INFO) << "Path is" << mat[4] << endl;

  EXPECT_TRUE(true) << "done.";
}


/**
 * 说明文章
 * https://blog.csdn.net/freeking101/article/details/54729274
 * 从一大串字符串里找出匹配的一小段字符串（比如从网页文件里找超链接），这时就要使用regex_search
 */
TEST(ClauseTest, RegexSearch) {
  LOG(INFO) << "RegexSearch";

  //( 1 )   ((  3  )  2 )((  5 )4)(    6    )
  //(\w+)://((\w+\.)*\w+)((/\w*)*)(/\w+\.\w+)?
  //^协议://网址(x.x...x)/路径(n个\字串)/网页文件(xxx.xxx)
  const char *szReg = "(\\w+)://((\\w+\\.)*\\w+)((/\\w*)*)(/\\w+\\.\\w+)?";
  const char *szStr = "http://www.cppprog.com/2009/0112/48.html";

  // 字符串匹配
//   boost::regex reg(szReg);
  boost::regex reg("(\\d+)*?" );    //查找字符串里的数字
  boost::cmatch mat; // 提取字串
  bool r = boost::regex_search(szStr, mat, reg);

  if(r) { //如果匹配成功
    // LOG(INFO) << "searched:" << mat[0] << endl;
    //显示所有子串
    for(boost::cmatch::iterator itr = mat.begin(); itr != mat.end(); ++itr) {
      //       指向子串对应首位置        指向子串对应尾位置          子串内容
      LOG(INFO) << itr->first - szStr << ' ' << itr->second - szStr << ' ' << *itr << endl;
    }
  }

  //也可直接取指定位置信息
  if(mat[4].matched) LOG(INFO) << "Path is" << mat[4] << endl;

  EXPECT_TRUE(true) << "done.";
}

/**
 * https://theboostcpplibraries.com/boost.regex
 */
TEST(ClauseTest, RegexSearch2) {
  LOG(INFO) << "RegexSearch2";

  std::string s = "Boost Libraries";
  boost::regex expr{"(\\w+)\\s(\\w+)"};
  boost::smatch what;

  if (boost::regex_search(s, what, expr)) {
    LOG(INFO) << what[0] << '\n';
    LOG(INFO) << what[1] << "_" << what[2] << '\n';
  }

  EXPECT_TRUE(true) << "done.";
}

/**
 * https://theboostcpplibraries.com/boost.regex
 * Raw String
 */
TEST(ClauseTest, RegexSearch3) {
  LOG(INFO) << "RegexSearch3";
  const char *s = "看到111 222";
  boost::regex expr(R"(\d+)", boost::regex::perl);
  boost::cmatch what;

  bool r = boost::regex_search(s, what, expr);

  if(r) { //如果匹配成功
    // LOG(INFO) << "searched:" << mat[0] << endl;
    //显示所有子串
    for(boost::cmatch::iterator itr = what.begin(); itr != what.end(); ++itr) {
      //       指向子串对应首位置        指向子串对应尾位置          子串内容
      LOG(INFO) << itr->first - s << ' ' << itr->second - s << ' ' << *itr << endl;
    }
  }

  EXPECT_TRUE(true) << "done.";
}

