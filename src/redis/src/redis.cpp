/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/redis/src/Rediss.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-22_12:58:09
 * @brief
 * hiredis Common Functions
 * http://redisgate.jp/redis/clients/hiredis_common_keys.php
 **/

#include "redis.h"

#include <stdio.h>
#include <string.h>
#include <sstream>
#include <glog/logging.h>
#include <boost/format.hpp>

// REDIS_REPLY 响应的类型 type
// "REDIS_REPLY_STRING 1";
// "REDIS_REPLY_ARRAY 2";
// "REDIS_REPLY_INTEGER 3";
// "REDIS_REPLY_NIL 4";
// "REDIS_REPLY_STATUS 5";
// "REDIS_REPLY_ERROR 6";

namespace chatopera {
namespace redis {

Redis* Redis::_instance = NULL;

Redis::Redis() {
  _c = NULL;
};

Redis::~Redis() {
  if(_c != NULL) {
    redisFree(_c);//析构函数释放资源
    VLOG(2) << "[redis] free redis connection ";;
  }
};

/**
 * 单例获得实例方法
 */
Redis* Redis::getInstance() {
  if(_instance == NULL)
    _instance = new Redis();

  return _instance;
}

/**
 * 初始化Redis连接
 */
bool Redis::init(const string& host,
                 const int& port,
                 const int& db,
                 const string& pass) {
  // 连接信息
  _host = host;
  _port = port;
  _db = db;
  _pass = pass;

  struct timeval timeout = { 1, 500000 }; // 1.5 seconds 设置连接等待时间
  char ip[255];
  strcpy(ip, _host.c_str());
  VLOG(3) << "[init] ip: " << _host << ", port: " << port << ", db: " << db << ", pass: *****";

  _c = redisConnectWithTimeout(ip, _port, timeout);//建立连接

  if (_c->err) {
    VLOG(2) << "Redis : Connection error: " << _c->errstr;
    return false;
  }

  // 认证
  if(!_pass.empty()) {
    redisReply *reply;
    reply = (redisReply *)redisCommand(_c, "AUTH %s", _pass.c_str());

    if (reply->type == REDIS_REPLY_ERROR) {
      VLOG(2) << "[init] auth failure.";
      freeReplyObject(reply);
      return false;
    } else {
      freeReplyObject(reply);
    }
  }

  // 选择 db
  redisReply *reply;

  /* Switch to specific DB */
  reply =  (redisReply *) redisCommand(_c, "SELECT %s", std::to_string(_db).c_str());
  freeReplyObject(reply);

  return true;
};

/**
 * 设置过期
 */
void Redis::expire(const string& key, unsigned int seconds) {
  VLOG(4) << __func__ << " key " << key << " " << seconds;
  redisCommand(_c, "EXPIRE %s %s", key.c_str(), std::to_string(seconds).c_str());
};


//向数据库写入string类型数据
int Redis::set(string key, string value) {
  VLOG(4) << __func__ << key << " = " << value;

  if(_c == NULL || _c->err) { //int err; /* Error flags, 错误标识，0表示无错误 */
    VLOG(2) << "Redis init Error !!!";;
    return -1;
  }

  redisReply *reply;
  reply = (redisReply *)redisCommand(_c, "SET %s %s", key.c_str(), value.c_str()); //执行写入命令
  VLOG(4) << "set string type = " << reply->type;; //获取响应的枚举类型
  int result = 0;

  if(reply == NULL) {
    redisFree(_c);
    _c = NULL;
    result = -1;
    VLOG(4) << "set string fail : reply->str = NULL ";;
    //pthread_spin_unlock(&_c_flock);
    return -1;
  } else if(strcmp(reply->str, "OK") == 0) { //根据不同的响应类型进行判断获取成功与否
    result = 1;
  } else {
    result = -1;
    VLOG(4) << "set string fail :" << reply->str;;
  }

  freeReplyObject(reply);//释放响应信息

  return result;
};

/**
 * 删除KEY
 */
void Redis::del(const string& key) {
  VLOG(4) << __func__ << " key " << key;
  redisCommand(_c, "DEL %s", key.c_str());
}

//从数据库读出string类型数据
string Redis::get(string key) {
  if(_c == NULL || _c->err) {
    VLOG(4) << "Redis init Error !!!";;
    return NULL;
  }

  redisReply *reply;
  reply = (redisReply *)redisCommand(_c, "GET %s", key.c_str());
  VLOG(4) << "get string type = " << reply->type;

  if(reply == NULL) {
    redisFree(_c);
    _c = NULL;
    VLOG(2) << "ERROR getString: reply = NULL!!!!!!!!!!!! maybe redis server is down";;
    return NULL;
  } else if(reply->len <= 0) {
    // key not exist
    freeReplyObject(reply);
    return "";
  } else {
    stringstream ss;
    ss << reply->str;
    freeReplyObject(reply);
    return ss.str();
  }
};


//从数据库读出string类型数据
signed int Redis::ttl(string key) {
  if(_c == NULL || _c->err) {
    VLOG(4) << "Redis init Error !!!";;
    return NULL;
  }

  redisReply *reply;
  reply = (redisReply *)redisCommand(_c, "TTL %s", key.c_str());

  if(reply == NULL) {
    VLOG(2) << __func__ << " reply is NULL";
    throw std::runtime_error("Lost redis connection.");
  } else if (reply->type == REDIS_REPLY_ERROR) {
    VLOG(3) << __func__ << "Error reading key: " << key;
    freeReplyObject(reply);
    return;
  } else {
    return reply->integer;
  }
};


// 向数据库写入vector（list）类型数据
int Redis::setList(const string& key, const vector<string>& value) {
  if(_c == NULL || _c->err) {
    VLOG(4) << "Redis init Error !!!";;
    return -1;
  }

  redisReply *reply;

  int valueSize = value.size();
  int result = 0;

  for(int i = 0; i < valueSize; i++) {
    reply = (redisReply*)redisCommand(_c, "RPUSH %s %s", key.c_str(), value.at(i).c_str());
    VLOG(4) << "set list type = " << reply->type << " ";
    int old = reply->integer;

    if(reply == NULL) {
      redisFree(_c);
      _c = NULL;
      result = -1;
      VLOG(4) << "set list fail : reply->str = NULL ";;
      //pthread_spin_unlock(&_c_flock);
      return -1;
    } else if(reply->integer == old++) {
      result = 1;
      VLOG(4) << "rpush list ok";;
      continue;
    } else {
      result = -1;
      VLOG(4) << "set list fail ,reply->integer = " << reply->integer;;
      return -1;
    }

  }

  freeReplyObject(reply);
  VLOG(4) << "set List  success";;
  return result;
};

//从数据库读出vector（list）类型数据
vector<string> Redis::list(const string& key) {

  if(_c == NULL || _c->err) {
    VLOG(2) << "Redis init Error !!!";;
    return vector<string> {}; //返回空的向量
  }

  redisReply *reply;
  reply = (redisReply*)redisCommand(_c, "LLEN %s", key.c_str());
  int valueSize = reply->integer;
  VLOG(4) << "List size is :" << reply->integer;;

  reply = (redisReply*)redisCommand(_c, "LRANGE %s %d %d", key.c_str(), 0, valueSize - 1);
  VLOG(4) << "get list type = " << reply->type;;
  VLOG(4) << "get list size = " << reply->elements;; //对于数组类型可以用elements元素获取数组长度

  redisReply** replyVector = reply->element;//获取数组指针
  vector<string> result;

  for(int i = 0; i < valueSize; i++) {
    string temp = (*replyVector)->str; //遍历redisReply*数组,存入vector向量
    result.push_back(temp);
    replyVector++;
  }

  VLOG(4) << "result size:" << result.size();;
  return result;
};

bool Redis::rpush(const string& key, const string& value) {
  if(_c == NULL || _c->err) {
    VLOG(2) << "Redis init Error !!!";;
    return false;
  }

  redisReply *reply;
  reply = (redisReply*)redisCommand(_c, "RPUSH %s %s", key.c_str(), value.c_str());

  if (reply == NULL) {
    VLOG(3) << "redisCommand reply is NULL: " << _c->errstr;
    return false;
  } else if(reply->type == REDIS_REPLY_ERROR) {
    VLOG(3) << "Command Error: " << reply->str;
    freeReplyObject(reply);
    return false;
  }

  return true;
};

string Redis::rpop(const string& key) {
  if(_c == NULL || _c->err) {
    VLOG(2) << "Redis init Error !!!";;
    throw std::runtime_error("Null Redis Connection.");
  }

  redisReply *reply;
  reply = (redisReply*)redisCommand(_c, "RPOP %s", key.c_str());

  if (reply == NULL) {
    VLOG(3) << "redisCommand reply is NULL: " << _c->errstr;
    return "";
  } else if(reply->type == REDIS_REPLY_ERROR) {
    VLOG(3) << "Command Error: " << reply->str;
    freeReplyObject(reply);
    return "";
  } else {
    string temp(reply->str);
    return temp;
  }

  return "";
};



} // namespace redis
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
