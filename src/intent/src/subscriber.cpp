/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

#include "subscriber.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "intent.pb.h"
#include <thread>

DECLARE_bool(activemq_client_ack);

namespace chatopera {
namespace bot {
namespace intent {

BrokerSubscriber::BrokerSubscriber() :
  destination(NULL),
  consumer(NULL) {
  _conn = BrokerConnection::getInstance();
  _trainer = new Trainer();
  CHECK(_trainer->init()) << "Fail to init trainer.";
};

BrokerSubscriber::~BrokerSubscriber() {
  delete _trainer;
  this->cleanup();
};

bool BrokerSubscriber::init() {
  try {
    // Create the destination Queue
    destination = _conn->session->createQueue(FLAGS_activemq_queue_to_intent);

    // Create a MessageConsumer from the Session to the Topic or Queue
    consumer = _conn->session->createConsumer(destination);
    consumer->setMessageListener(this);

    VLOG(3) << "[subscriber] setup listener successfully.";
    return true;
  } catch (CMSException& e) {
    VLOG(1) << "[subscriber] error";
    e.printStackTrace();
    return false;
  }
};

// Called from the consumer since this class is a registered MessageListener.
void BrokerSubscriber::onMessage(const Message* message) {
  static int count = 0;

  try {
    count++;
    const TextMessage* textMessage = dynamic_cast<const TextMessage*>(message);
    string text = "";
    string action = "";

    if (textMessage != NULL) {
      text = textMessage->getText();
      // NOTE: 通过action判断分发任务处理线程
      action = textMessage->getStringProperty("action");
    } else {
      text = "NOT A TEXTMESSAGE!";
    }

    if (FLAGS_activemq_client_ack) {
      message->acknowledge();
    }

    if(text.empty() || action != "train") {
      VLOG(3) << "[onMessage] not a desired action, fast return.";
      return;
    }

    // 使用protobuf反序列化
    if(action == "train") {
      // #TODO cancel previous train job
      std::thread train(&Trainer::train, _trainer, text);
      train.detach();
    } else {
      VLOG(3) << "[onMessage] unknown action.";
    }

    VLOG(3) << "[onMessage] job dispatched.";
  } catch (CMSException& e) {
    e.printStackTrace();
  }
};

void BrokerSubscriber::cleanup() {
  //*************************************************
  // Always close destination, consumers and producers before
  // you destroy their sessions and connection.
  //*************************************************

  // Destroy resources.
  try {
    if( destination != NULL ) delete destination;
  } catch (CMSException& e) {}

  destination = NULL;

  try {
    if( consumer != NULL ) delete consumer;
  } catch (CMSException& e) {}

  consumer = NULL;
};

} // namespace intent
} // namespace bot
} // namespace chatopera