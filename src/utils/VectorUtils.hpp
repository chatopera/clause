/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/utils/VectorUtils.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-17_10:16:47
 * @brief
 *
 **/
#ifndef CHATOPERA_UTILS_VEC_FUNCTS_H
#define CHATOPERA_UTILS_VEC_FUNCTS_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

namespace chatopera {
namespace utils {

/**
 * 打印矩阵
 */
template<class T>
inline string get2DStringVectorMatrixDebuggingStr(const vector<vector<T> >& matrix) {
  stringstream ss;

  for ( int i = 0; i < matrix.size(); i++ ) {
    for ( int j = 0; j < matrix[i].size(); j++ ) {
      ss << matrix[i][j] << '\t';
    }

    ss << '\n';
  }

  return ss.str();
}

/**
 * 打印数组
 */
template<class T>
inline string getStringVectorDebuggingStr(const vector<T >& array) {
  stringstream ss;

  for ( int i = 0; i < array.size(); i++ ) {
    ss << array[i] << '\t';
  }

  return ss.str();
}



} // namespace utils
} // namespace chatopera

#endif