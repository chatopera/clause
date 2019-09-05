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

#ifndef __CHATOPERA_BOT_INTENT_HANLDER
#define __CHATOPERA_BOT_INTENT_HANLDER

#include <map>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <boost/scoped_ptr.hpp>

#include "glog/logging.h"
#include "serving/Serving.h"
#include "serving/server_types.h"
#include "mysql/jdbc.h"
#include "src/connection.h"
#include "src/subscriber.h"
#include "src/publisher.h"

#define CL_SYSDICT_CHATBOT_ID "@BUILTIN"

using namespace std;

namespace chatopera {
namespace bot {
namespace intent {

class ServingHandler : virtual public ServingIf {
 public:
  ServingHandler();
  ~ServingHandler();
  bool init();
  void ping();

 protected:
 private:
  BrokerConnection* _brokerconn;                 // activemq connection
  BrokerSubscriber* _brokersub;                  // activemq subscriber
  BrokerPublisher*  _brokerpub;                  // activemq publisher
};

} // namespace clause
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */