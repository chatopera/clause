/**
 * Math Utilities
 */
/***************************************************************************
*
* Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
*
**************************************************************************/

#ifndef __CHATOPERA_UTILS_MATH_H__
#define __CHATOPERA_UTILS_MATH_H__

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

namespace chatopera {
namespace utils {

/**
 * 笛卡尔积
 * cartesian product in C++
 * https://gist.github.com/thirdwing/953b146ba39c5f5ff562
 */
template <typename T>
inline vector<vector<T> > cart_product (const vector<vector<T>>& v) {
  vector<vector<T>> s = {{}};

  for (auto& u : v) {
    vector<vector<T>> r;

    for (auto& x : s) {
      for (auto y : u) {
        r.push_back(x);
        r.back().push_back(y);
      }
    }

    s.swap(r);
  }

  return s;
};
} // namespace utils
} // namespace chatopera

#endif