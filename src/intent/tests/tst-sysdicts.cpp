/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/intent/tests/tst-sysdicts.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-03_10:16:57
 * @brief
 * 支持使用系统词典进行训练
 **/

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

/**
 * 一个简单的Profile
 */
inline void build_sysdicts_profile(Profile& profile) {
  std::string chatbotID("mychatbotID3");
  std::string id("myid");
  profile.set_id(id);
  profile.set_chatbotid(chatbotID);

  /**
   * 创建自定义词典
   */
  // 创建dict1
  TDict* customdict =  profile.add_dicts();
  customdict->set_id("46A3434DE394FAA40B1E1F7130ADA902");
  customdict->set_name("taste");
  customdict->set_chatbotid(chatbotID);
  customdict->set_builtin(false);
  customdict->set_active(false);
  customdict->set_description("dict1 description");
  customdict->set_samples("word1,word2");
  customdict->set_createdate("2019-08-10 12:00:00");
  customdict->set_updatedate("2019-08-10 12:00:00");

  // 创建词条
  TDictWord* dictword1 = customdict->add_dictwords();
  dictword1->set_word("榴莲");
  dictword1->set_dict_id("46A3434DE394FAA40B1E1F7130ADA902");

  TDictWord* dictword2 = customdict->add_dictwords();
  dictword2->set_word("芒果");
  dictword2->set_dict_id("46A3434DE394FAA40B1E1F7130ADA902");

  /**
   * 创建意图
   */
  TIntent* intent1 = profile.add_intents();
  intent1->set_id("F57AC77EC99E4C3E7D38E410407D7A03");
  intent1->set_name("order_pizza");

  // 增加槽位
  TIntentSlot* slot1 = intent1->add_slots();
  slot1->set_name("type");
  slot1->set_requires(true);
  slot1->set_question("您希望要什么口味的");
  slot1->set_dictname("taste");

  TIntentSlot* slot2 = intent1->add_slots();
  slot2->set_name("date");
  slot2->set_requires(true);
  slot2->set_question("你希望什么时候送到");
  slot2->set_dictname("@TIME");

  // 增加说法
  TIntentUtter* utter1 = intent1->add_utters();
  utter1->set_intent_id("F57AC77EC99E4C3E7D38E410407D7A03");
  utter1->set_utterance("帮我送一个pizza");

  TIntentUtter* utter2 = intent1->add_utters();
  utter2->set_intent_id("F57AC77EC99E4C3E7D38E410407D7A03");
  utter2->set_utterance("{date}帮我送一个{type}pizza");

  TIntentUtter* utter3 = intent1->add_utters();
  utter3->set_intent_id("F57AC77EC99E4C3E7D38E410407D7A03");
  utter3->set_utterance("帮我送个{type}味的piaaz");

  TIntentUtter* utter4 = intent1->add_utters();
  utter4->set_intent_id("F57AC77EC99E4C3E7D38E410407D7A03");
  utter4->set_utterance("帮我送一个{type}的pizza");
};

TEST(IntentTest, TRAIN_SYSDICTS) {
  LOG(INFO) << "TRAIN_SYSDICTS";
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
    build_sysdicts_profile(profile);

    string serialized;
    profile.SerializeToString(&serialized);
    message.reset(session->createTextMessage(serialized));
    message->setStringProperty("action", "train");
    message->setStringProperty("chatbotID", "mychatbotID3");
    producer->send(message.get());

    connection->close();
  } catch (CMSException& e) {
    VLOG(1) << "[consumer] error";
    e.printStackTrace();
    return false;
  }

  activemq::library::ActiveMQCPP::shutdownLibrary();
}



/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
