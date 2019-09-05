/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/sysdicts/client.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-02_22:29:06
 * @brief
 *
 **/

#ifndef __CHATOPERA_BOT_SYSDICTS_H__
#define __CHATOPERA_BOT_SYSDICTS_H__

#include "thrift/protocol/TBinaryProtocol.h"
#include <thrift/transport/TSocket.h>
#include "thrift/transport/TBufferTransports.h"
#include <thrift/transport/TTransportUtils.h>
#include "serving/Serving.h"
#include "serving/server_constants.h"
#include "serving/server_types.h"
#include "glog/logging.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace chatopera {
namespace bot {

namespace clause {
class ServingHandler;
}

namespace sysdicts {

class Client {

  friend class clause::ServingHandler;

 public:
  Client();
  ~Client();

 public:
  bool init();
  void label(sysdicts::Data& _return, sysdicts::Data& request);

 private:
  std::shared_ptr<TTransport> _transport;
  ServingClient* c;

};


}
}
}


#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
