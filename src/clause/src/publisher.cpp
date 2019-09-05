/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/intent/src/publisher.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-21_11:05:35
 * @brief
 *
 **/


#include "publisher.h"

using namespace std;

namespace chatopera {
namespace bot {
namespace clause {

BrokerPublisher* BrokerPublisher::_instance = NULL;

BrokerPublisher::BrokerPublisher() :
  destination(NULL) {
};

BrokerPublisher::~BrokerPublisher() {
  this->cleanup();
};

// 初始化
bool BrokerPublisher::init() {
  destination = _conn->session->createQueue(FLAGS_activemq_queue_to_intent);
  producer = _conn->session->createProducer(destination);
  producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);
  VLOG(3) << "[publisher] init done.";

  return true;
}

BrokerPublisher* BrokerPublisher::getInstance() {
  if(_instance == NULL) {
    _instance = new BrokerPublisher();
    (*_instance)._conn = BrokerConnection::getInstance();
  }

  return _instance;
};


/**
 * Publish event with payload
 */
bool BrokerPublisher::publish(const string& chatbotID,
                              const string& action,
                              const string& payload) {
  VLOG(3) << "[publish] chatbotID: " << chatbotID << ", action " << action;
  std::auto_ptr<TextMessage> message;
  message.reset(_conn->session->createTextMessage(payload));
  message->setStringProperty("action", action);
  message->setStringProperty("chatbotID", chatbotID);
  producer->send(message.get());
  return true;
};

void BrokerPublisher::cleanup() {
  //*************************************************
  // Always close destination, consumers and producers before
  // you destroy their sessions and connection.
  //*************************************************

  // Destroy resources.
  try {
    if( destination != NULL ) delete destination;
  } catch (CMSException& e) {}

  destination = NULL;
  _instance = NULL;
};

} // namespace intent
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
