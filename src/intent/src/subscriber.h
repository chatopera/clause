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

#ifndef __CHATOPERA_BOT_INTENT_BROKER_SUBSCRIBER__
#define __CHATOPERA_BOT_INTENT_BROKER_SUBSCRIBER__

#include "connection.h"
#include "trainer.h"
#include "gflags/gflags.h"

namespace chatopera {
namespace bot {
namespace intent {

class BrokerSubscriber : public MessageListener {

 public:
  BrokerSubscriber();

 private:
  BrokerConnection* _conn;
  Destination* destination;
  MessageConsumer* consumer;
  Trainer* _trainer;

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