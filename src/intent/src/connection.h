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

#ifndef __CHATOPERA_BOT_INTENT_BROKER_CONNECTION__
#define __CHATOPERA_BOT_INTENT_BROKER_CONNECTION__

#include "trainer.h"
#include "gflags/gflags.h"

// activemq
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
#include <activemq/transport/DefaultTransportListener.h>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Integer.h>
#include <activemq/util/Config.h>
#include <decaf/util/Date.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <utility>

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;
using namespace chatopera::utils;

DECLARE_string(activemq_queue_to_intent);
DECLARE_string(activemq_queue_to_clause);

namespace chatopera {
namespace bot {
namespace intent {

class BrokerSubscriber;
class BrokerPublisher;

class BrokerConnection : public ExceptionListener,
  public DefaultTransportListener {
  friend class BrokerSubscriber;
  friend class BrokerPublisher;

 private:
  static BrokerConnection* _instance;
  Connection* connection;
  Session* session;
  std::string brokerURI;

 private:
  BrokerConnection(const BrokerConnection&);
  BrokerConnection& operator=(const BrokerConnection&);
  BrokerConnection() {};
  virtual ~BrokerConnection();

 public:
  static BrokerConnection* getInstance();
  bool init(const std::string& brokerURI);
  void close();
  virtual void onException(const CMSException& ex AMQCPP_UNUSED);
  virtual void onException(const decaf::lang::Exception& ex);
  virtual void transportInterrupted();
  virtual void transportResumed();

 private:
  void cleanup();
};

} // namespace intent
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */