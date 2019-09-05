/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * Send activemq message with protobuf
 */

#include "gtest/gtest.h"
#include "glog/logging.h"

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

#include "intent.pb.h"

using namespace activemq;
using namespace activemq::core;
using namespace activemq::transport;
using namespace decaf::lang;
using namespace decaf::util;
using namespace decaf::util::concurrent;
using namespace cms;
using namespace std;
using namespace chatopera::bot::intent;

TEST(IntentTest, INIT) {
  LOG(INFO) << "INIT";
  EXPECT_TRUE(true) << "Not done.";
}

TEST(IntentTest, ACTIVEMQ) {
  LOG(INFO) << "ACTIVEMQ";
  Connection* connection;
  Session* session;
  Destination* destination;
  MessageConsumer* consumer;
  bool useTopic;
  std::string brokerURI("failover:(tcp://192.168.2.219:8058)");
  std::string queueName("chatopera/to/intent");
  bool clientAck(false);

  try {
    activemq::library::ActiveMQCPP::initializeLibrary();
    // Create a ConnectionFactory
    ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);

    // Create a Connection
    connection = connectionFactory->createConnection();
    delete connectionFactory;

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection);
    connection->start();
    LOG(INFO) << "connection started";

    std::auto_ptr<Session> session(connection->createSession());
    std::auto_ptr<Destination> dest(session->createQueue(queueName));
    std::auto_ptr<MessageProducer> producer(session->createProducer(dest.get()));

    producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

    std::auto_ptr<TextMessage> message;

    // send bot profile
    Profile profile;
    std::string chatbotID("mychatbotID2");
    profile.set_id("myid2");
    profile.set_chatbotid(chatbotID);

    string serialized;
    profile.SerializeToString(&serialized);
    message.reset(session->createTextMessage(serialized));
    message->setStringProperty("action", "train");
    message->setStringProperty("chatbotID", chatbotID);
    producer->send(message.get());

    connection->close();
  } catch (CMSException& e) {
    VLOG(1) << "[consumer] error";
    e.printStackTrace();
    return false;
  }

  activemq::library::ActiveMQCPP::shutdownLibrary();
}