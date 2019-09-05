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
namespace clause {

BrokerSubscriber::BrokerSubscriber() :
  destination(NULL),
  consumer(NULL) {
  _conn = BrokerConnection::getInstance();
};

BrokerSubscriber::~BrokerSubscriber() {
  this->cleanup();
};

bool BrokerSubscriber::init() {
  try {

    // 该Redis在handler.cpp中完成初始化
    _redis = Redis::getInstance();

    // 该MySQL在handler.cpp中完成初始化
    _mysql = MySQL::getInstance();

    // Create the destination Queue
    destination = _conn->session->createQueue(FLAGS_activemq_queue_to_clause);

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

// 发布新测试版本
inline bool publishNewDevVersion(const boost::scoped_ptr<sql::Statement>& stmt,
                                 const string& chatbotID,
                                 const chatopera::bot::intent::Profile& profile) {
  VLOG(3) << __func__ << " chatbotID: " << chatbotID << ", devver: " << FromProtobufToUtf8DebugString(profile.devver());

  // create new dev version into database
  if(profile.has_devver()) {
    try {
      string devver_id = newDevver(stmt, chatbotID,
                                   profile.devver().version(),
                                   profile.devver().published());
      VLOG(3) << __func__ << " new devver_id: " << devver_id;
    } catch (sql::SQLException &e) {
      VLOG(2) << "# ERR: SQLException in " << __FILE__;
      /* Use what() (derived from std::runtime_error) to fetch the error message */
      VLOG(2) << "# ERR: " << e.what();
      VLOG(2) << " (MySQL error code: " << e.getErrorCode();
      VLOG(2) << ", SQLState: " << e.getSQLState() << " )";
      return false;
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      return false;
    }

    return true;
  }

  return false;
};

// Called from the consumer since this class is a registered MessageListener.
void BrokerSubscriber::onMessage(const Message* message) {
  static int count = 0;

  try {
    count++;
    const TextMessage* textMessage = dynamic_cast<const TextMessage*>(message);
    string text;
    string action;
    string chatbotID;

    if (textMessage != NULL) {
      text = textMessage->getText();
      // NOTE: 通过action判断分发任务处理线程
      action = textMessage->getStringProperty("action");
      chatbotID = textMessage->getStringProperty("chatbotID");
    } else {
      text = "NOT A TEXTMESSAGE!";
    }

    if (FLAGS_activemq_client_ack) {
      message->acknowledge();
    }

    VLOG(3) << "[onMessage] chatbotID: " << chatbotID << ", action: " << action;

    if(text.empty()
        || action.empty()) {
      VLOG(3) << "[onMessage] not a desired action, fast return.";
      return;
    }

    // 使用protobuf反序列化
    bool dispatched = false;

    if(action == "train/finish") {
      chatopera::bot::intent::Profile profile;
      profile.ParseFromString(text);
      VLOG(3) << "[onMessage] profile\n" << FromProtobufToUtf8DebugString(profile);
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      if(publishNewDevVersion(stmt, chatbotID, profile)) {
        // update build status in Redis, update dev version number
        rdone_chatbot_build_and_devver(*_redis, chatbotID, profile.devver().version());

        // delete all sessions for this bot in dev branch
        rdel_chatbot_dev_sessions(*_redis, chatbotID);
      }

      dispatched = true;
    } else {
      VLOG(3) << "[onMessage] unknown action.";
    }

    VLOG(3) << "[onMessage] job dispatched " << dispatched;
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