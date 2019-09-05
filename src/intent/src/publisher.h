/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/intent/src/publisher.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-21_11:05:17
 * @brief
 *
 **/
#ifndef __CHATOPERA_BOT_INTENT_PUBLISHER__
#define __CHATOPERA_BOT_INTENT_PUBLISHER__

#include <string>
#include "gflags/gflags.h"
#include "connection.h"

// activemq
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/ExceptionListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <utility>

using namespace std;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;

namespace chatopera {
namespace bot {
namespace intent {

class BrokerConnection;

/**
 * 发布事件到ActiveMQ
 */
class BrokerPublisher {
 private:
  BrokerPublisher();
  ~BrokerPublisher();

 private:
  BrokerConnection* _conn;
  MessageProducer* producer;
  Destination* destination;

 public:
  static BrokerPublisher* getInstance();
  bool init();  // 初始化
  bool publish(const string& chatbotID,
               const string& queue,
               const string& payload);

 private:
  void cleanup();
  static BrokerPublisher* _instance;
};

} // namespace intent
} // namespace bot
} // namespace chatopera

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
