/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

#include <gflags/gflags.h>
#include <glog/logging.h>
#include "thrift/concurrency/ThreadManager.h"
#include "thrift/concurrency/PlatformThreadFactory.h"
#include "thrift/concurrency/Thread.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/server/TNonblockingServer.h"
#include "thrift/transport/TServerSocket.h"
#include "thrift/transport/TNonblockingServerSocket.h"
#include "thrift/transport/TNonblockingServerTransport.h"
#include "thrift/transport/TBufferTransports.h"

#include "serving/Serving.h"
#include "src/handler.h"

DEFINE_int32(server_port, 6600, "Server's port to process requests.");
DEFINE_int32(server_threads, 24, "serving threads");

// mysql
DEFINE_string(mysql_uri, "tcp://host:port/db", "MySQL URI");
DEFINE_string(mysql_user, "root", "MySQL Username");
DEFINE_string(mysql_pass, "123456", "MySQL User Password");

// activemq
DEFINE_bool(activemq_client_ack, false, "ActiveMQ open client ack mode or not");
DEFINE_string(activemq_broker_uri, "failover:(tcp://activemq:61616)", "ActiveMQ Service URL");
DEFINE_string(activemq_queue_to_intent, "chatopera/to/intent", "Messaging routes to Chatopera Intent Service");
DEFINE_string(activemq_queue_to_clause, "chatopera/to/clause", "Messaging routes to Chatopera Clause Service");

// redis
DEFINE_string(redis_host, "", "Redis IP Host");
DEFINE_int32(redis_port, 6379, "Redis port");
DEFINE_int32(redis_db, 5, "Redis Database number, [0-15]");
DEFINE_string(redis_pass, "", "Redis Auth pass.");

// sysdicts
DEFINE_string(sysdicts_host, "sysdicts", "Chatopera Sysdicts Service Host");
DEFINE_int32(sysdicts_port, 8066, "Chatopera Sysdicts Service Port");

// miscs
DEFINE_string(workarea, "../../../../var/trainer/workarea", "Generated and captured models, bot dicts, indexes.");
DEFINE_string(data, "../../../../var/trainer/data", "Prebuilt data, templates, dicts etc.");
DEFINE_double(intent_classify_threshold, 0.9, "Threshold for classify intent.");


using namespace std;
using namespace ::chatopera::bot::clause;
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

int main(int argc, char *argv[]) {
  // 解析命令行参数
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  // 初始化日志库
  google::InitGoogleLogging(argv[0]);
  versionPrint("Chatopera Language Understanding Service");

  stdcxx::shared_ptr<ServingHandler> handler(new ServingHandler());

  CHECK(handler->init()) << "[Fatal Error] Serving Handler fails to init.";

  stdcxx::shared_ptr<TProcessor> processor(new ServingProcessor(handler));
  stdcxx::shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  stdcxx::shared_ptr<TNonblockingServerTransport> serverTransport(new TNonblockingServerSocket(FLAGS_server_port));
  stdcxx::shared_ptr<PlatformThreadFactory> threadFactory = std::shared_ptr<PlatformThreadFactory>(new PlatformThreadFactory());
  stdcxx::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(FLAGS_server_threads);

  threadManager->threadFactory(threadFactory);
  threadManager->start();

  stdcxx::shared_ptr<TNonblockingServer> server(new TNonblockingServer(processor, protocolFactory, serverTransport, threadManager));
  VLOG(2) << "serving on port " << FLAGS_server_port;
  server->serve();
  return 0;
}
