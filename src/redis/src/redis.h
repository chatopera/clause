/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/redis/src/redictools.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-22_12:50:59
 * @brief
 *
 **/


#ifndef __CHATOPERA_REDIS_TOOLS_H__
#define __CHATOPERA_REDIS_TOOLS_H__

#include <string>
#include <iostream>
#include <vector>
#include <hiredis/hiredis.h>

using namespace std;

namespace chatopera {
namespace redis {

class Redis {
 public:
  static Redis* getInstance();

  void expire(const string& key, unsigned int seconds);
  signed int ttl(string key);

  int set(string key, string value);
  string get(string key);

  int setList(const string& key, const vector<string>& value);
  vector<string> list(const string& key);
  bool rpush(const string& key, const string& value);
  string rpop(const string& key);
  void del(const string& key);

  bool init(const string& host,
            const int& port = 6379,
            const int& db = 0,
            const string& pass = "");

 private: // constructor
  Redis();
  ~Redis();

 private:
  string _host;
  int _port;
  int _db;
  string _pass;
  static Redis* _instance;
  redisContext *_c;
};


} // namespace redis
} // namespace chatopera








#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
