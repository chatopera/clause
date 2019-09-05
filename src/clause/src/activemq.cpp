/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

#include "activemq.h"
#include "glog/logging.h"
#include "intent.pb.h"
#include <thread>

DECLARE_bool(activemq_client_ack);

namespace chatopera {
namespace bot {
namespace clause {

// init static member
BrokerConnection* BrokerConnection::_instance = NULL;

BrokerConnection::~BrokerConnection() {
  this->cleanup();
};

BrokerConnection* BrokerConnection::getInstance() {
  if(_instance == NULL)
    _instance = new BrokerConnection();

  return _instance;
};

bool BrokerConnection::init(const std::string& brokerURI) {
  activemq::library::ActiveMQCPP::initializeLibrary();

  VLOG(3) << "Broker URI: " << brokerURI;

  try {
    // Create a ConnectionFactory
    ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);

    // Create a Connection
    connection = connectionFactory->createConnection();
    delete connectionFactory;

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection);

    if (amqConnection != NULL) {
      amqConnection->addTransportListener(this);
    }

    VLOG(3) << "[broker] connecting ...";
    connection->start();
    VLOG(3) << "[broker] connected successfully.";
    connection->setExceptionListener(this);

    // Create a Session
    if (FLAGS_activemq_client_ack) {
      session = connection->createSession(Session::CLIENT_ACKNOWLEDGE);
    } else {
      session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
    }

    return true;
  } catch (CMSException& e) {
    VLOG(1) << "[broker] error";
    e.printStackTrace();
    return false;
  }
};

void BrokerConnection::close() {
  this->cleanup();
};

// If something bad happens you see it here as this class is also been
// registered as an ExceptionListener with the connection.
void BrokerConnection::onException(const CMSException& ex AMQCPP_UNUSED) {
  printf("CMS Exception occurred.  Shutting down client.\n");
  exit(1);
};

void BrokerConnection::onException(const decaf::lang::Exception& ex) {
  printf("Transport Exception occurred: %s \n", ex.getMessage().c_str());
};

void BrokerConnection::transportInterrupted() {
  std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void BrokerConnection::transportResumed() {
  std::cout << "The Connection's Transport has been Restored." << std::endl;
}

void BrokerConnection::cleanup() {
  //*************************************************
  // Always close destination, consumers and producers before
  // you destroy their sessions and connection.
  //*************************************************

  // Close open resources.
  try {
    if( session != NULL ) session->close();

    if( connection != NULL ) connection->close();
  } catch (CMSException& e) {}

  // Now Destroy them
  try {
    if( session != NULL ) delete session;
  } catch (CMSException& e) {}

  session = NULL;

  try {
    if( connection != NULL ) delete connection;
  } catch (CMSException& e) {}

  connection = NULL;

  activemq::library::ActiveMQCPP::shutdownLibrary();

  _instance = NULL;
}
} // namespace intent
} // namespace bot
} // namespace chatopera