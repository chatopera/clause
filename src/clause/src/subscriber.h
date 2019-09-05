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

#ifndef __CHATOPERA_BOT_CLAUSE_BROKER_SUBSCRIBER__
#define __CHATOPERA_BOT_CLAUSE_BROKER_SUBSCRIBER__

#include <boost/scoped_ptr.hpp>
#include "activemq.h"
#include "basictypes.h"
#include "gflags/gflags.h"
#include "intent.pb.h"
#include "mysql/jdbc.h"
#include "mysql.h"
#include "redis.h"
#include "raf.hpp"
#include "maf.hpp"

using namespace chatopera::redis;
using namespace chatopera::mysql;

namespace chatopera {
namespace bot {
namespace clause {

class BrokerSubscriber : public MessageListener {

 public:
  BrokerSubscriber();

 private:
  BrokerConnection* _conn;
  Destination* destination;
  MessageConsumer* consumer;
  Redis* _redis;
  MySQL* _mysql;

 public:
  virtual ~BrokerSubscriber();

  void close();
  bool init();
  virtual void onMessage(const Message* message);

 private:
  void cleanup();
};

} // namespace intent
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */