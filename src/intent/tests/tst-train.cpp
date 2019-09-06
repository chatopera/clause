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

/**
 * 一个简单的Profile
 */
inline void build_simple_profile(Profile& profile) {
  std::string chatbotID("mychatbotID2");
  std::string id("myid");
  profile.set_id(id);
  profile.set_chatbotid(chatbotID);

  /**
   * 创建自定义词典
   */
  // 创建dict1
  TDict* customdict =  profile.add_dicts();
  customdict->set_id("dict1");
  customdict->set_name("dictName1");
  customdict->set_chatbotid(chatbotID);
  customdict->set_builtin(false);
  customdict->set_active(false);
  customdict->set_description("dict1 description");
  customdict->set_samples("word1,word2");
  customdict->set_createdate("2019-08-10 12:00:00");
  customdict->set_updatedate("2019-08-10 12:00:00");

  // 创建词条
  TDictWord* dictword1 = customdict->add_dictwords();
  dictword1->set_word("计程车");
  dictword1->set_synonyms("出租车;taxi");

  // 创建dict2
  TDict* customdict2 =  profile.add_dicts();
  customdict2->set_id("dict2");
  customdict2->set_name("dictName2");
  customdict2->set_chatbotid(chatbotID);
  customdict2->set_builtin(false);
  customdict2->set_active(false);
  customdict2->set_description("dict2 description");
  customdict2->set_samples("word1,word2");
  customdict2->set_createdate("2019-08-10 12:00:00");
  customdict2->set_updatedate("2019-08-10 12:00:00");

  // 创建词条
  TDictWord* dictword2 = customdict2->add_dictwords();
  dictword2->set_word("北京");
  dictword2->set_synonyms("北平;燕京;中国首都");

  // 创建dict3
  TDict* customdict3 =  profile.add_dicts();
  customdict3->set_id("dict3");
  customdict3->set_name("直辖市");
  customdict3->set_chatbotid(chatbotID);
  customdict3->set_builtin(false);
  customdict3->set_active(false);
  customdict3->set_description("dict3 description");
  customdict3->set_samples("word1,word2");
  customdict3->set_createdate("2019-08-10 12:00:00");
  customdict3->set_updatedate("2019-08-10 12:00:00");

  // 创建词条
  TDictWord* dictword3 = customdict3->add_dictwords();
  dictword3->set_word("北京");
  dictword3->set_synonyms("北平;燕京;中国首都");

  //   测试自定义词典
  //   TDictWord* dictword3 = customdict2->add_dictwords();
  //   dictword3->set_word("女干事");

  /**
   * 创建意图
   */
  TIntent* intent1 = profile.add_intents();
  intent1->set_id("intent1");
  intent1->set_name("book_cab");

  // 增加槽位
  TIntentSlot* slot1 = intent1->add_slots();
  slot1->set_id("slot1_id");
  slot1->set_name("cityName");
  slot1->set_requires(true);
  slot1->set_question("你在哪座城市");
  slot1->set_dictname("dictName2");

  TIntentSlot* slot2 = intent1->add_slots();
  slot2->set_id("slot2_id");
  slot2->set_name("taxi");
  slot2->set_requires(true);
  slot2->set_question("你想做什么工具");
  slot2->set_dictname("dictName1");

  // 增加说法
  TIntentUtter* utter1 = intent1->add_utters();
  utter1->set_id("utter1_id");
  utter1->set_utterance("我想 打车");

  TIntentUtter* utter2 = intent1->add_utters();
  utter2->set_id("utter2_id");
  utter2->set_utterance("我想叫计程车");

  TIntentUtter* utter3 = intent1->add_utters();
  utter3->set_id("utter3_id");
  utter3->set_utterance("我想从{cityName}叫{taxi}");
  // 测试分词
  //   TIntentUtter* utter4 = intent1->add_utters();
  //   utter4->set_id("utter4_id");
  //   utter4->set_utterance("女干事每月经过机房时都会检查二十四口交换机。");
};

TEST(IntentTest, TRAIN_SAMPLE) {
  LOG(INFO) << "TRAIN_SAMPLE";
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
    build_simple_profile(profile);

    string serialized;
    profile.SerializeToString(&serialized);
    message.reset(session->createTextMessage(serialized));
    message->setStringProperty("action", "train");
    message->setStringProperty("chatbotID", "mychatbotID2");
    producer->send(message.get());

    connection->close();
  } catch (CMSException& e) {
    VLOG(1) << "[consumer] error";
    e.printStackTrace();
    return false;
  }

  activemq::library::ActiveMQCPP::shutdownLibrary();
}