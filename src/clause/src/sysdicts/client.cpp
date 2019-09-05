/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/sysdicts/client.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-09-02_22:29:22
 * @brief
 *
 **/

#include "client.h"

DECLARE_string(sysdicts_host);
DECLARE_int32(sysdicts_port);

using namespace std;

namespace chatopera {
namespace bot {

namespace sysdicts {



Client::Client() {
};

Client::~Client() {
  _transport->close();
  delete c;
};

bool Client::init() {
  try {
    std::shared_ptr<TTransport> socket(new TSocket(FLAGS_sysdicts_host, FLAGS_sysdicts_port));
    _transport.reset(new TFramedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(_transport));
    _transport->open();
    c = new ServingClient(protocol);
    VLOG(2) << "sysdicts " << __func__ << " connection successfully.";
    return true;
  } catch(std::exception& exception) {
    VLOG(2) << "sysdicts " << __func__ << " exception: " << exception.what();
    return false;
  }
};

void Client::label(sysdicts::Data& _return, sysdicts::Data& request) {
  c->label(_return, request);
};
} // namespace sysdicts
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
