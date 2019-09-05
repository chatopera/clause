/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * cartesian product in C++
 * https://gist.github.com/thirdwing/953b146ba39c5f5ff562
 */

#include "gtest/gtest.h"
#include "glog/logging.h"

#include "MathUtils.hpp"

using namespace std;
using namespace chatopera::utils;

TEST(IntentTest, CART) {
  vector<vector<string> > test{{"car"}, {"book", "horse", "lions"}, {"Shanghai", "HK"}};
  vector<vector<string> > res = cart_product(test);

  for(size_t i = 0; i < res.size(); i++) {
    for (size_t j = 0; j < res[i].size(); j++) {
      LOG(INFO) << i << j << ":" << res[i][j];
    }
  }
}

TEST(IntentTest, CART_SING) {
  vector<vector<string> > test{{"car"}};
  vector<vector<string> > res = cart_product(test);

  for(size_t i = 0; i < res.size(); i++) {
    for (size_t j = 0; j < res[i].size(); j++) {
      LOG(INFO) << i << j << ":" << res[i][j];
    }
  }
}