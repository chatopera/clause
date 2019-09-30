/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/database.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-23_11:42:53
 * @brief
 *
 **/


#include "mysql.h"
#include <unistd.h>

namespace chatopera {
namespace mysql {

MySQL* MySQL::_instance = NULL;

MySQL::MySQL() {
};

MySQL::~MySQL() {
  conn.reset(NULL);
  _instance = NULL;
}


bool MySQL::init() {
  /**
   * mysql connection
   */
  sql::Driver * driver = sql::mysql::get_driver_instance();
  sql::ConnectOptionsMap connection_properties;
  // https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-connect-options.html
  connection_properties["hostName"] = FLAGS_mysql_uri; // uri 格式 tcp://host:port/db
  connection_properties["userName"] = FLAGS_mysql_user;
  connection_properties["password"] = FLAGS_mysql_pass;
  connection_properties["OPT_RECONNECT"] = true;
  connection_properties["OPT_CHARSET_NAME"] = "utf8mb4";
  connection_properties["OPT_SET_CHARSET_NAME"] = "utf8mb4";

  // wait for mysql initialization in 30s
  unsigned int conn_attempts = 30;

  do {
    try {
      conn.reset(driver->connect(connection_properties));
      break;
    } catch (sql::SQLException &e) {
      VLOG(conn_attempts > 1 ? 3 : 2) << "# ERR: SQLException in " << __FILE__;
      /* Use what() (derived from std::runtime_error) to fetch the error message */
      VLOG(conn_attempts > 1 ? 3 : 2) << "# ERR: " << e.what();
      VLOG(conn_attempts > 1 ? 3 : 2) << " (MySQL error code: " << e.getErrorCode();
      VLOG(conn_attempts > 1 ? 3 : 2) << ", SQLState: " << e.getSQLState() << " )";

      if(conn_attempts > 1) {
        VLOG(2) << __func__ << " reconnect to MySQL ...";
      } else {
        VLOG(2) << __func__ << " abort connect to MySQL, fail to initialize connection.";
      }

      sleep(1);
    }
  } while ((--conn_attempts) > 0);

  return conn->isValid();
};

MySQL* MySQL::getInstance() {
  if(_instance == NULL) {
    _instance = new MySQL();
    CHECK(_instance->init()) << "Fail to setup MySQL connection.";
  }

  return _instance;
};


}
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
