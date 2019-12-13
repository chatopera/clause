/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-05-22_14:22:10
 * @brief
 *
 **/

#ifndef __CHATOPERA_BOT_CLAUSE_HANLDER
#define __CHATOPERA_BOT_CLAUSE_HANLDER

#include <map>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <mutex>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "json/json.h"
#include "glog/logging.h"
#include "serving/Serving.h"
#include "serving/server_types.h"
#include "mysql/jdbc.h"
#include "redis.h"
#include "intent.pb.h"
#include "sysdicts/serving/server_types.h"
#include "sysdicts/client.h"
#include "sysdicts/client.h"

#include "marcos.h"
#include "basictypes.h"
#include "activemq.h"       // activemq client
#include "mysql.h"          // mysql client
#include "subscriber.h" // activemq subscription
#include "publisher.h"  // activemq publish
#include "bot.h"        // bot instance
#include "pattern.h"    // regex utils

#include "emoji.h"
#include "punctuations.h"
#include "stopwords.h"

// redis flags
DECLARE_string(redis_host);
DECLARE_string(redis_pass);
DECLARE_int32(redis_port);
DECLARE_int32(redis_db);

DECLARE_string(workarea);
DECLARE_string(data);
DECLARE_double(intent_classify_threshold);

using namespace std;
using namespace boost::algorithm;
using namespace chatopera::utils;
using namespace chatopera::redis;
namespace fs = boost::filesystem;

namespace chatopera {
namespace bot {
namespace clause {

class ServingHandler : virtual public ServingIf {
 public:
  ServingHandler();
  ~ServingHandler();
  bool init();

  /**
   * 词典管理
   */
  // 创建自定义词典
  void postCustomDict(Data& _return, const Data& request);
  // 更新自定义词典
  void putCustomDict(Data& _return, const Data& request);
  // 获得自定义词典列表
  void getCustomDicts(Data& _return, const Data& request);
  // 获得自定义词典详情
  void getCustomDict(Data& _return, const Data& request);
  // 删除自定义词典
  void delCustomDict(Data& _return, const Data& request);
  // 创建系统词典
  void postSysDict(Data& _return, const Data& request);
  // 更新系统词典
  void putSysDict(Data& _return, const Data& request);
  // 获得系统词典列表
  void getSysDicts(Data& _return, const Data& request);
  // 获得系统词典详情
  void getSysDict(Data& _return, const Data& request);
  // 引用系统词典
  void refSysDict(Data& _return, const Data& request);
  // 取消引用系统词典
  void unrefSysDict(Data& _return, const Data& request);
  // 创建或更新词条
  void putDictWord(Data& _return, const Data& request);
  // 获得词条列表
  void getDictWords(Data& _return, const Data& request);
  // 删除词条
  void delDictWord(Data& _return, const Data& request);
  // 通过chatbotID获取所有词典列表
  void myDicts(Data& _return, const Data& request);
  // 通过chatbotID获取所有被引用词典的列表
  void mySysdicts(Data& _return, const Data& request);
  // 标准词是否已经包含在自定义词典中
  void hasDictWord(Data& _return, const Data& request);
  // 正则表达式词典
  void getDictPattern(Data& _return, const Data& request);
  void putDictPattern(Data& _return, const Data& request);
  void checkDictPattern(Data& _return, const Data& request);
  void checkHistoryDictPattern(Data& _return, const Data& request);

  /**
   * 意图
   */
  // 创建意图
  void postIntent(Data& _return, const Data& request);
  // 更新意图
  void putIntent(Data& _return, const Data& request);
  // 获取意图列表
  void getIntents(Data& _return, const Data& request);
  // 获得意图详情
  void getIntent(Data& _return, const Data& request);
  // 删除意图
  void delIntent(Data& _return, const Data& request);
  // 创建说法
  void postUtter(Data& _return, const Data& request);
  // 更新说法
  void putUtter(Data& _return, const Data& request);
  // 获得说法列表
  void getUtters(Data& _return, const Data& request);
  // 获得说法详情
  void getUtter(Data& _return, const Data& request);
  // 删除说法
  void delUtter(Data& _return, const Data& request);
  // 创建槽位
  void postSlot(Data& _return, const Data& request);
  // 更新槽位
  void putSlot(Data& _return, const Data& request);
  // 获取槽位列表
  void getSlots(Data& _return, const Data& request);
  // 获取槽位详情
  void getSlot(Data& _return, const Data& request);
  // 删除槽位
  void delSlot(Data& _return, const Data& request);

  /**
   * 对话管理
   */
  // 查看训练状态
  void status(Data& _return, const Data& request);
  // 训练模型
  void train(Data& _return, const Data& request);
  // 查看调试及生产版本信息
  void version(Data& _return, const Data& request);
  // 查看生产版本信息
  void prover(Data& _return, const Data& request);
  // 查看调试版本信息
  void devver(Data& _return, const Data& request);

  // 聊天
  void chat(Data& _return, const Data& request);
  // 上线
  void online(Data& _return, const Data& request);
  // 下线
  void offline(Data& _return, const Data& request);
  // 创建或更新会话
  void putSession(Data& _return, const Data& request);
  // 获得会话信息
  void getSession(Data& _return, const Data& request);

 protected:
  bool mysql_error(Data& _return, const sql::SQLException &e);

 private: // functions
  int resolveBotByChatbotIDAndBranch(const Redis& redis,
                                     map<string, Bot* >& bots,
                                     const intent::TChatSession& session);

 private:
  chatopera::mysql::MySQL* _mysql;               // mysql connection
  BrokerConnection* _brokerconn;                 // activemq connection
  BrokerSubscriber* _brokersub;                  // activemq connection
  BrokerPublisher*  _brokerpub;                  // activemq connection
  Redis*            _redis;                      // Redis
  map<string, Bot* > _bots_dev;                  // Chat development instances
  map<string, Bot* > _bots_pro;                  // Chat production instances
  mutex              _bot_lock;                  // Lock for bots managing
  sysdicts::Client*  _sysdicts;                  // Client for Sysdicts Service
  sep::Emojis*       _emojis;                    // Emojis Filter
  sep::Punctuations* _punts;                     // Punctuations Filter
  sep::Stopwords*    _stopwords;                 // Stopwords Filter
};

} // namespace clause
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */