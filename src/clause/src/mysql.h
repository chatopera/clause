/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/database.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-23_11:42:38
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_CLAUSE_MYSQL_H__
#define __CHATOPERA_BOT_CLAUSE_MYSQL_H__

#include <string>
#include <sstream>
#include "mysql/jdbc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

using namespace std;

DECLARE_string(mysql_uri);
DECLARE_string(mysql_user);
DECLARE_string(mysql_pass);

namespace chatopera {

namespace bot {
namespace clause {
class ServingHandler;
class BrokerSubscriber;
} // namespace clause
} // namespace bot

namespace mysql {

class MySQL {
  friend class chatopera::bot::clause::ServingHandler;
  friend class chatopera::bot::clause::BrokerSubscriber;

 public: // functions
  bool init();
  static MySQL* getInstance();

 private: // constructors
  MySQL();
  ~MySQL();

 private: // members
  static MySQL* _instance;
  boost::scoped_ptr<sql::Connection> conn;  // mysql connection
};

} // namespace mysql
} // namespace chatopera











#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
