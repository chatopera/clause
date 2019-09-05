/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/sep/tests/tst-sep.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-04_11:07:47
 * @brief
 *
 **/

#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>

#include "emoji.h"
#include "punctuations.h"
#include "stopwords.h"

using namespace std;
using namespace chatopera::bot::sep;

TEST(SepTest, EMOJI)  {
  string config("../../../../var/test/sep/emoji.utf8");
  Emojis emoji;
  CHECK(emoji.init(config)) << "fail to init";
  EXPECT_TRUE(emoji.contains("🐱")) << "check fails.";
  EXPECT_FALSE(emoji.contains("1")) << "check fails.";
}

TEST(SepTest, PUNT) {
  string config("../../../../var/test/sep/punctuations.utf8");
  Punctuations punt;
  CHECK(punt.init(config)) << "fail to init";
  EXPECT_TRUE(punt.contains(",")) << "fail to check";
}

TEST(SepTest, STOPWORDS) {
  string config("../../../../var/test/sep/stopwords.utf8");
  Stopwords st;
  CHECK(st.init(config)) << "fail to init";
  EXPECT_TRUE(st.contains("也是")) << "fail to check";
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
