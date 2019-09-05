/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-05-22_14:22:23
 * @brief
 *
 **/

#include "handler.h"
#include "gflags/gflags.h"

using namespace chatopera::utils;
using namespace chatopera::bot::intent;
using namespace std;

DECLARE_string(activemq_broker_uri);
DECLARE_bool(activemq_client_ack);

namespace chatopera {
namespace bot {
namespace intent {

ServingHandler::ServingHandler() {
};

ServingHandler::~ServingHandler() {
  delete _brokersub;
};

bool ServingHandler::init() {
  /**
   * activemq connection
   */
  // 连接
  _brokerconn = BrokerConnection::getInstance();
  _brokerconn->init(FLAGS_activemq_broker_uri);
  // 事件监听者
  _brokersub = new BrokerSubscriber();

  if(!_brokersub->init()) {
    VLOG(2) << "Init fails, exception in BrokerSubscriber";
    return false;
  }

  // 事件发布者
  _brokerpub = BrokerPublisher::getInstance();

  if(!_brokerpub->init()) {
    VLOG(2) << "Init fails, exception in BrokerPublisher.";
    return false;
  }


  return true;
};

void ServingHandler::ping() {
  VLOG(3) << "ping";
}

} // intent
} // bot
} // chatopera