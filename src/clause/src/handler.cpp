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
#include "TimeUtils.hpp"
#include "StringUtils.hpp"
#include "intent.pb.h"
#include "maf.hpp"
#include "raf.hpp"

using namespace std;
using namespace chatopera::utils;
using namespace chatopera::bot::clause;

DECLARE_string(activemq_broker_uri);
DECLARE_string(data);
DECLARE_string(workarea);

namespace chatopera {
namespace bot {
namespace clause {

ServingHandler::ServingHandler() {
  _emojis = new sep::Emojis();
  _punts = new sep::Punctuations();
  _stopwords = new sep::Stopwords();
};

ServingHandler::~ServingHandler() {
  delete _brokersub;
  delete _emojis;
  delete _punts;
  delete _stopwords;
};

bool ServingHandler::init() {
  VLOG(3) << __func__ << " current path: " << fs::current_path().string()
          << "\n data dir: " << FLAGS_data << "\n workarea " << FLAGS_workarea;

  try {
    // 检查配置目录是否存在
    if(!fs::exists(FLAGS_data)) {
      VLOG(2) << __func__ << " data dir not exist. " << FLAGS_data;
      return false;
    }

    if(!fs::exists(FLAGS_workarea)) {
      VLOG(2) << __func__ << " workarea dir not exist. " << FLAGS_workarea;
      return false;
    }

    // Wordfilers
    if(!_emojis->init(FLAGS_data + "/sep/emoji.utf8")) {
      VLOG(2) << __func__ << " Emojis fails";
      return false;
    }

    if(!_punts->init(FLAGS_data + "/sep/punctuations.utf8")) {
      VLOG(2) << __func__ << " Punctuations fails";
      return false;
    }

    if(!_stopwords->init(FLAGS_data + "/sep/stopwords.utf8")) {
      VLOG(2) << __func__ << " Stopwords fails";
      return false;
    }

    // Redis, 先初始化Redis, 在MQ等服务中需要依赖Redis实例
    _redis = Redis::getInstance();

    if(!_redis->init(FLAGS_redis_host,
                     FLAGS_redis_port,
                     FLAGS_redis_db,
                     FLAGS_redis_pass)) {
      VLOG(2) << "Init redis fails.";
      return false;
    }

    // mysql database
    _mysql = chatopera::mysql::MySQL::getInstance();

    /**
     * activemq connection
     */
    _brokerconn = BrokerConnection::getInstance();
    _brokerconn->init(FLAGS_activemq_broker_uri);

    // 事件监听者
    _brokersub = new BrokerSubscriber();

    if(!_brokersub->init()) {
      VLOG(2) << "Init fails, exception in BrokerSubscriber";
      return false;
    }

    // 事件发布
    _brokerpub = BrokerPublisher::getInstance();

    if(!_brokerpub->init()) {
      VLOG(2) << "Init fails, exception in BrokerPublisher";
      return false;
    }

    // 系统词典
    _sysdicts = new sysdicts::Client();

    if(!_sysdicts->init()) {
      VLOG(2) << "Init fails, exception in Sysdicts.";
      return false;
    }
  } catch (sql::SQLException &e) {
    /*
      The JDBC API throws three different exceptions:

    - sql::MethodNotImplementedException (derived from sql::SQLException)
    - sql::InvalidArgumentException (derived from sql::SQLException)
    - sql::SQLException (derived from std::runtime_error)
    */
    VLOG(2) << "# ERR: SQLException in " << __FILE__;
    VLOG(2) << "(" << "init" << ") on line " << __LINE__ << endl;

    /* Use what() (derived from std::runtime_error) to fetch the error message */

    VLOG(2) << "# ERR: " << e.what();
    VLOG(2) << " (MySQL error code: " << e.getErrorCode();
    VLOG(2) << ", SQLState: " << e.getSQLState() << " )" << endl;
    return false;
  }

  return true;
};

/**
 * 设置异常返回
 */
inline void rc_and_error(Data& data, const int rc, const std::string& error) {
  CHECK(rc != 0) << "[Error] error response code should not be 0, because 0 is for success.";
  data.rc = rc;
  data.error = error;
  data.__isset.rc = true;
  data.__isset.error = true;
}

/**
 * 创建自定义字典
 */
void ServingHandler::postCustomDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.customdict &&
      request.customdict.__isset.name &&
      request.customdict.__isset.chatbotID) {
    try {
      VLOG(3) << __func__ << " customdict create name " << request.customdict.name;
      CustomDict customdict;
      string id = generate_uuid();
      customdict.createdate = GetCurrentTimestampFormatted();
      customdict.updatedate = customdict.createdate;
      customdict.chatbotID = request.customdict.chatbotID;
      customdict.name = request.customdict.name;
      customdict.__isset.createdate = true;
      customdict.__isset.updatedate = true;
      customdict.__isset.chatbotID = true;
      customdict.__isset.name = true;

      if(request.customdict.__isset.description) {
        customdict.description = request.customdict.description;
        customdict.__isset.description = true;
      }

      if(request.customdict.__isset.samples) {
        customdict.samples = request.customdict.samples;
        customdict.__isset.samples = true;
      }

      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
      sql.str("");
      sql << "INSERT INTO cl_dicts(id, name, chatbotID, createdate, updatedate";

      if(request.customdict.__isset.description) {
        sql << ", description";
      }

      if(request.customdict.__isset.samples) {
        sql << ", samples";
      }

      sql << ") VALUES ('";
      sql << id << "','" << request.customdict.name << "','";
      sql << request.customdict.chatbotID << "', '" << customdict.createdate;
      sql << "', '" << customdict.updatedate;

      if(request.customdict.__isset.description) {
        sql << "', '" << request.customdict.description;
      }

      if(request.customdict.__isset.samples) {
        sql << "', '" << request.customdict.samples;
      }

      sql << "')";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      VLOG(3) << __func__ << " create new record id " << id;
      _return.rc = 0;
      _return.customdict = customdict;
      _return.__isset.rc = true;
      _return.__isset.customdict = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, customdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};


/**
 * 更新自定义词典
 */
void ServingHandler::putCustomDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.customdict.__isset.chatbotID &&
      request.customdict.__isset.name &&
      request.customdict.__isset.description ) {
    try {
      CustomDict customdict;

      if(getDictDetailByChatbotIDAndName(customdict, stmt,
                                         request.customdict.chatbotID,
                                         request.customdict.name
                                        )) {
        VLOG(3) << "[putCustomDict] customdict update description: "
                << request.customdict.description;

        std::string updatedate = GetCurrentTimestampFormatted();

        sql.str("");
        sql << "UPDATE cl_dicts SET description = '";
        sql << request.customdict.description << "', updatedate = '";
        sql << updatedate << "' WHERE chatbotID = '";
        sql << request.customdict.chatbotID << "' and name = '";
        sql << request.customdict.name << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->executeUpdate(sql.str());
        customdict.updatedate = updatedate;

        _return.rc = 0;
        _return.msg = "success";
        _return.customdict = customdict;
        _return.__isset.rc = true;
        _return.__isset.msg = true;
        _return.__isset.customdict = true;
      } else {
        rc_and_error(_return, 4, "Record not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, customdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 *  获取自定义词典列表
 */
void ServingHandler::getCustomDicts(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.chatbotID) {
    try {
      int pagesize = request.__isset.pagesize ? request.pagesize : 20;
      // page begins with 1 and default is 1.
      int page = request.__isset.page ? request.page : 1;

      if(page <= 0) // redefine any invalid value as 1.
        page = 1;

      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
      VLOG(3) << __func__ << " pagesize " << pagesize << ", page " << page;
      getDictsWithPagination(_return, stmt, false,
                             request.chatbotID, pagesize, page);
      _return.rc = 0;
      _return.__isset.rc = true;

      stmt.reset(NULL);
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, customdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得自定义字典详情
 * #TODO 暂时用不上
 */
void ServingHandler::getCustomDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
};

/**
 *  删除自定义词典
 */
void ServingHandler::delCustomDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.customdict.__isset.chatbotID && request.customdict.__isset.name) { // 验证是否是更新
    CustomDict dict;

    try {
      // 获得词典详情
      if(getDictDetailByChatbotIDAndName(dict, stmt,
                                         request.customdict.chatbotID,
                                         request.customdict.name)) {

        // check if this customdict is used in slot
        sql.str("");
        sql << "SELECT COUNT(*) FROM cl_intent_slots WHERE dict_id = '" << dict.id << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));
        cset->next();

        if(cset->getInt(1) > 0) {
          // 存在槽位对词典的引用，无法删除
          rc_and_error(_return, 12, "Referenced by slot(s), deletion not permitted.");
          return;
        }

        // delete all dictwords
        VLOG(3) << __func__ << " delete dictwords in customdict.";
        sql.str("");
        sql << "DELETE FROM cl_dict_words where dict_id = '";
        sql << dict.id << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->execute(sql.str());

        // finally, delete customdict
        sql.str("");
        sql << "DELETE FROM cl_dicts where chatbotID = '";
        sql << request.customdict.chatbotID << "' and name = '";
        sql << request.customdict.name << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->execute(sql.str());

        _return.rc = 0;
        _return.msg = "success";
        _return.__isset.rc = true;
        _return.__isset.msg = true;
      } else {
        rc_and_error(_return, 11, "Customdict not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 5, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 10, "Invalid params, customdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};


/**
 * 创建系统词典
 */
void ServingHandler::postSysDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.sysdict.__isset.name &&
      request.sysdict.__isset.description &&
      request.sysdict.__isset.samples) {
    try {
      string id = generate_uuid();
      SysDict sysdict;
      sysdict.createdate = GetCurrentTimestampFormatted();
      sysdict.updatedate = sysdict.createdate;
      sysdict.id = id;
      sysdict.description = request.sysdict.description;
      sysdict.samples = request.sysdict.samples;
      sysdict.builtin = true;
      sysdict.active = false;
      sysdict.name = request.sysdict.name;
      sysdict.__isset.createdate = true;
      sysdict.__isset.updatedate = true;
      sysdict.__isset.id = true;
      sysdict.__isset.description = true;
      sysdict.__isset.samples = true;
      sysdict.__isset.builtin = true;
      sysdict.__isset.active = true;
      sysdict.__isset.name = true;
      sql.str("");
      sql << "INSERT INTO cl_dicts(id, name, chatbotID, description, createdate,";
      sql << "updatedate, samples, builtin, active) VALUES('";
      sql << id << "','" << request.sysdict.name << "','" << CL_SYSDICT_CHATBOT_ID << "','";
      sql << request.sysdict.description << "','" << sysdict.createdate << "','";
      sql << sysdict.updatedate << "','" << request.sysdict.samples << "'," << 1 << "," << 0 << ")";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());
      VLOG(3) << __func__ << " create new record id " << id;
      _return.sysdict = sysdict;
      VLOG(3) << __func__ << " sysdict: " << FromThriftToUtf8DebugString(&sysdict);

      _return.__isset.sysdict = true;

      _return.rc = 0;
      _return.__isset.rc = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, sysdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
}

/**
 * 更新系统词典(samples,description,active)
 */
void ServingHandler::putSysDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

  if( request.__isset.sysdict && request.sysdict.__isset.name) {
    try {
      SysDict sysdict;

      if(getSysDictDetail(sysdict, stmt, "",
                          request.sysdict.name)) {

        std::string updatedate = GetCurrentTimestampFormatted();

        if(!updateSysDict(sysdict,
                          stmt,
                          request,
                          updatedate)) {
          rc_and_error(_return, 1, "Invalid params, sysdict obj is required.");
          return;
        }

        sysdict.updatedate = updatedate;
        sysdict.builtin = true;
        sysdict.__isset.builtin = true;

        _return.rc = 0;
        _return.msg = "success";
        _return.sysdict = sysdict;
        VLOG(3) << __func__ << " sysdict: " << FromThriftToUtf8DebugString(&sysdict);

        _return.__isset.rc = true;
        _return.__isset.msg = true;
        _return.__isset.sysdict = true;
      } else {
        rc_and_error(_return, 4, "Record not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, sysdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
}

/**
 * 获取所有系统字典列表
 */
void ServingHandler::getSysDicts(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  int pagesize = request.__isset.pagesize ? request.pagesize : 20;
  // page begins with 1 and default is 1.
  int page = request.__isset.page ? request.page : 1;

  if(page <= 0) // redefine any invalid value as 1.
    page = 1;

  VLOG(3) << __func__ << " pagesize " << pagesize << ", page " << page;

  try {
    if(request.__isset.chatbotID) {
      if(getDictsWithPagination(_return, stmt, true, request.chatbotID, pagesize, page)) {
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "query error in db.");
      };

    } else {
      if(getDictsWithPagination(_return, stmt, true, "", pagesize, page)) {
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "query error in db.");
      };
    }

  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

void ServingHandler::getSysDict(Data& _return, const Data& request) {

};

/**
 * 引用系统字典
 */
void ServingHandler::refSysDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.sysdict &&
      request.__isset.chatbotID &&
      request.sysdict.__isset.name) {
    try {
      std::string createdate = GetCurrentTimestampFormatted();
      string id = generate_uuid();

      sql.str("");
      sql << "INSERT INTO cl_bot_sysdict(id, chatbotID, dict_id, createdate) VALUES('"
          << id << "','" << request.chatbotID << "', (SELECT id FROM cl_dicts WHERE name = '"
          << request.sysdict.name << "' AND chatbotID = '" << CL_SYSDICT_CHATBOT_ID << "'), '";
      sql << createdate << "')";

      VLOG(3) << __func__ << "  execute SQL: " << sql.str();
      stmt->execute(sql.str());

      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, sysdict obj and chatbotID is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 取消引用系统字典
 */
void ServingHandler::unrefSysDict(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.sysdict.__isset.name &&
      request.__isset.chatbotID) {
    try {
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
      stringstream sql;
      sql.str("");
      sql << "DELETE FROM cl_bot_sysdict where chatbotID = '"
          << request.chatbotID << "' and dict_id = ("
          << "SELECT id FROM cl_dicts WHERE name = '"
          <<  request.sysdict.name << "' AND chatbotID ='"
          << CL_SYSDICT_CHATBOT_ID << "')";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, sysdict obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 创建或更新词条
 * 包括更新系统词典和自定义词典
 */
void ServingHandler::putDictWord(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.dictword &&
      request.dictword.__isset.word &&
      (request.__isset.customdict || request.__isset.sysdict)) {
    try {

      // 查询dict_id
      string dict_id;
      sql.str("");
      sql << "SELECT id FROM cl_dicts WHERE name = '";

      if(request.__isset.customdict &&
          request.customdict.__isset.name) {
        if(request.__isset.chatbotID) {
          sql << request.customdict.name << "' AND chatbotID ='";
          sql << request.chatbotID << "'";
        } else {
          rc_and_error(_return, 10, "Invalid dict data, chatbot ID is required for customdict.");
          return;
        }
      } else if(request.__isset.sysdict &&
                request.sysdict.__isset.name) {
        sql << request.sysdict.name << "' AND chatbotID ='";
        sql << CL_SYSDICT_CHATBOT_ID << "'";
      } else {
        rc_and_error(_return, 10, "Invalid dict data.");
        return;
      }

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

      if(rset->rowsCount() == 1) {
        rset->next();
        dict_id = rset->getString("id");
      } else {
        rc_and_error(_return, 12, "Invalid dict data.");
        return;
      }

      // 查询该词条是否已经存在
      sql.str("");
      sql << "SELECT COUNT(*) FROM cl_dict_words WHERE word = '";
      sql << request.dictword.word << "' AND dict_id = '";
      sql << dict_id << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      rset.reset(stmt->executeQuery(sql.str()));
      rset->next();

      if(rset->getInt(1) == 1) {
        // 执行更新操作
        sql.str("");
        sql << "UPDATE cl_dict_words SET synonyms = '"
            << (request.dictword.__isset.synonyms ? request.dictword.synonyms : "")
            << "' where dict_id = '" << dict_id << "' and word = '"
            << request.dictword.word << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->execute(sql.str());

        _return.rc = 0;
        _return.msg = "success";
        _return.__isset.rc = true;
        _return.__isset.msg = true;

      } else if(rset->getInt(1) == 0) {
        // 执行新建操作
        sql.str("");
        sql << "INSERT INTO cl_dict_words(word, dict_id, synonyms) VALUES('";
        sql << request.dictword.word << "', '" << dict_id << "', '";
        sql << (request.dictword.__isset.synonyms ? request.dictword.synonyms : "");
        sql << "')";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->execute(sql.str());

        _return.rc = 0;
        _return.msg = "success";
        _return.__isset.rc = true;
        _return.__isset.msg = true;
      } else {
        if(rset->getInt(1) > 1)
          VLOG(3) << __func__ << " Should not happens, this word has more then one record in db.";

        _return.rc = 1;
        _return.msg = "请检查参数是否正确！";
        _return.__isset.rc = true;
        _return.__isset.msg = true;
      }

      rset.reset(NULL);
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, dictword and (sysdict|customdict) is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得词条列表
 */
void ServingHandler::getDictWords(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;
  int pagesize = request.__isset.pagesize ? request.pagesize : 20;
  // page begins with 1 and default is 1.
  int page = request.__isset.page ? request.page : 1;
  string query = request.__isset.query ? request.query : "";

  if(page <= 0) // redefine any invalid value as 1.
    page = 1;

  try {
    Data message = request;

    if(getWordsWithPagination(_return,
                              stmt,
                              message,
                              pagesize,
                              page,
                              query)) {
      VLOG(3) << "[getDictWords] _return: " << FromThriftToUtf8DebugString(&_return);
      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;
    } else {
      _return.rc = 1;
      _return.error = "请检查chatbotID或dictionary name是否正确";
      _return.__isset.rc = true;
      _return.__isset.error = true;
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 检查词条是否在现有词典中
 */
void ServingHandler::hasDictWord(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if( request.__isset.chatbotID &&
      request.__isset.customdict &&
      request.customdict.__isset.name &&
      request.__isset.dictword &&
      request.dictword.__isset.word) {
    try {

      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
      CustomDict customdict;

      if(getDictDetailByChatbotIDAndName(customdict,
                                         stmt,
                                         request.chatbotID,
                                         request.customdict.name,
                                         false)) {
        if(isExistDictwordByCustomdictIdAndWord(stmt,
                                                customdict.id,
                                                request.dictword.word)) {
          _return.rc = 0;
          _return.__isset.rc = true;
          _return.msg = "Word exist.";
          _return.__isset.msg = true;
        } else {
          _return.rc = 1;
          _return.__isset.rc = true;
          _return.msg = "Word not exist.";
          _return.__isset.msg = true;
        }
      } else {
        _return.rc = 12;
        _return.__isset.rc = true;
        _return.msg = "Customdict not exist.";
        _return.__isset.msg = true;
      }

      stmt.reset(NULL);
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 11, "Invalid params. ChatbotID, customdict name and dictword are required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 删除词条
 */
void ServingHandler::delDictWord(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.dictword &&
      request.dictword.__isset.word) {

    try {
      sql.str("");
      sql << "DELETE FROM cl_dict_words WHERE word = '";
      sql << request.dictword.word << "' and dict_id = (";
      sql << "SELECT id FROM cl_dicts WHERE name = '";

      if(request.__isset.customdict &&
          request.customdict.__isset.name) {
        if(request.__isset.chatbotID) {
          sql << request.customdict.name << "' AND chatbotID ='";
          sql << request.chatbotID << "')";
        } else {
          rc_and_error(_return, 10, "Invalid dict data, chatbot ID is required for customdict.");
          return;
        }
      } else if(request.__isset.sysdict &&
                request.sysdict.__isset.name) {
        sql << request.sysdict.name << "' AND chatbotID ='";
        sql << CL_SYSDICT_CHATBOT_ID << "')";
      } else {
        rc_and_error(_return, 10, "Invalid dict data.");
        return;
      }

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }

  } else {
    rc_and_error(_return, 1, "Invalid params, sysdict or customdict obj is required.");
  }
};

/**
 * 通过chatbotID获得所有被引用的系统词典的列表
 */
void ServingHandler::mySysdicts(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.chatbotID) {
    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
    stringstream sql;

    try {

      // 查询系统词典
      sql.str("");
      sql << "SELECT D.name as dname, D.description as"
          << " ddescription, D.createdate as dcreatedate, "
          << " D.updatedate as dupdatedate, D.samples as dsamples  FROM cl_dicts D"
          << " LEFT OUTER JOIN cl_bot_sysdict B ON B.dict_id = D.id "
          << " WHERE B.chatbotID = '" << request.chatbotID << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

      while(rset->next()) {
        SysDict sysdict;
        sysdict.name = rset->getString("dname");
        sysdict.description = rset->getString("ddescription");
        sysdict.samples = rset->getString("dsamples");
        sysdict.createdate = rset->getString("dcreatedate");
        sysdict.updatedate = rset->getString("dupdatedate");
        sysdict.referred = true;
        sysdict.builtin = true;
        sysdict.__isset.name = true;
        sysdict.__isset.description = true;
        sysdict.__isset.samples = true;
        sysdict.__isset.createdate = true;
        sysdict.__isset.updatedate = true;
        sysdict.__isset.referred = true;
        sysdict.__isset.builtin = true;
        _return.sysdicts.push_back(sysdict);
        _return.__isset.sysdicts = true;
      }

      _return.rc = 0;
      _return.__isset.rc = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }


  } else {
    rc_and_error(_return, 1, "Invalid params, chatbotID is required.");
  }


  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 *  通过chatbotID获得所有自定义词典列表和被引用的系统列表
 */
void ServingHandler::myDicts(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.chatbotID) {
    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
    stringstream sql;

    try {
      // 查询自定义词典
      sql.str("");
      sql << "SELECT name FROM cl_dicts where chatbotID = '";
      sql << request.chatbotID << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

      while(rset->next()) {
        CustomDict customdict;
        customdict.name = rset->getString("name");
        customdict.__isset.name = true;
        _return.customdicts.push_back(customdict);
      }

      VLOG(3) << "dicts count " << rset->rowsCount();

      if(rset->rowsCount() > 0) {
        _return.__isset.customdicts = true;
      } else {
        _return.rc = 0;
        _return.msg = "没有自定义词典";
        _return.__isset.rc = true;
        _return.__isset.msg = true;
      }

      // 查询系统词典
      sql.str("");
      sql << "SELECT D.name as dname FROM cl_dicts D";
      sql << " LEFT OUTER JOIN cl_bot_sysdict B ON B.dict_id = D.id ";
      sql << " WHERE B.chatbotID = '" << request.chatbotID << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      boost::scoped_ptr< sql::ResultSet > s_rset(stmt->executeQuery(sql.str()));

      while(s_rset->next()) {
        SysDict sysdict;
        sysdict.name = s_rset->getString("dname");
        sysdict.__isset.name = true;
        _return.sysdicts.push_back(sysdict);
      }

      if(_return.sysdicts.size() > 0) {
        _return.__isset.sysdicts = true;
        _return.rc = 0;

        if(_return.msg ==  "没有自定义词典") {
          _return.msg = "没有自定义词典";
        } else {
          _return.msg = "success";
        }

        _return.__isset.rc = true;
        _return.__isset.msg = true;
      } else {
        _return.rc = 0;

        if(_return.msg ==  "没有自定义词典") {
          _return.msg = "没有引用系统词典;没有自定义词典";
        } else {
          _return.msg = "没有引用系统词典";
        }

        _return.__isset.rc = true;
        _return.__isset.msg = true;
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }

    stmt.reset(NULL);
  } else {
    rc_and_error(_return, 1, "Invalid params, chatbotID is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
}

/**
 * 创建意图
 */
void ServingHandler::postIntent(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.intent
      && request.intent.__isset.chatbotID
      && request.intent.__isset.name) {
    VLOG(3) << __func__ << " intent create new obj";

    // 创建新记录
    try {
      Intent intent;
      string id = generate_uuid();
      intent.createdate = GetCurrentTimestampFormatted();
      intent.updatedate = intent.createdate;
      intent.chatbotID = request.intent.chatbotID;
      intent.name = request.intent.name;
      intent.__isset.createdate = true;
      intent.__isset.updatedate = true;
      intent.__isset.chatbotID = true;
      intent.__isset.name = true;

      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
      sql.str("");
      sql << "INSERT INTO cl_intents(id, chatbotID, name, createdate, updatedate) VALUES ('";
      sql << id << "','";
      sql << request.intent.chatbotID << "', '" << request.intent.name ;
      sql << "', '" << intent.createdate << "', '" << intent.updatedate;
      sql << "')";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      VLOG(3) << __func__ << " create new record id " << id;
      _return.rc = 0;
      _return.intent = intent;
      _return.__isset.rc = true;
      _return.__isset.intent = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, intent obj is required.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};


/**
 * 更新意图
 */
void ServingHandler::putIntent(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  if(request.__isset.intent
      && request.intent.__isset.chatbotID
      && request.intent.__isset.name) { // 验证数据是否存在
    try {
      Intent intent;

      if(getIntentDetailByChatbotIDAndName(intent, stmt,
                                           request.intent.chatbotID,
                                           request.intent.name)) {
        VLOG(3) << __func__ << " intent update id:" << intent.id;
        // 在记录存在的情况下，只进行更新
        sql.str("");
        std::string updatedate = GetCurrentTimestampFormatted();
        sql << "UPDATE cl_intents SET description = '";
        sql << request.intent.description << "', updatedate = '";
        sql << updatedate << "' where chatbotID = '";
        sql << request.intent.chatbotID << "' AND name = '";
        sql << request.intent.name << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->executeUpdate(sql.str());

        intent.description = request.intent.description;
        intent.updatedate = updatedate;

        _return.rc = 0;
        _return.msg = "success";
        _return.intent = intent;
        _return.__isset.rc = true;
        _return.__isset.msg = true;
        _return.__isset.intent = true;
      } else {
        rc_and_error(_return, 4, "Record does not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, intent obj is required.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得意图识别列表
 */
void ServingHandler::getIntents(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  // pagination info
  int pagesize = request.__isset.pagesize ? request.pagesize : 20;
  // page begins with 1 and default is 1.
  int page = request.__isset.page ? request.page : 1;

  if(page <= 0) // redefine any invalid value as 1.
    page = 1;

  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  VLOG(3) << __func__ << " pagesize " << pagesize << ", page " << page;

  try {
    if(request.__isset.chatbotID) {
      if(getIntentsWithPagination(_return, stmt, request.chatbotID, pagesize, page)) {
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "Invalid response from database.");
      };
    } else {
      if(getIntentsWithPagination(_return, stmt, "", pagesize, page)) {
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "Invalid response from database.");
      }
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得意图详情
 * 可以通过传入id或intent(name, chatbotID)的方法获得详情
 */
void ServingHandler::getIntent(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

  try {
    if(request.__isset.id) {
      if(getIntentDetailById(_return.intent, stmt, request.id)) {
        _return.__isset.intent = true;
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "Record does not exist.");
      }
    } else if(request.__isset.intent && request.intent.__isset.chatbotID
              && request.intent.__isset.name) {
      if(getIntentDetailByChatbotIDAndName(_return.intent, stmt,
                                           request.intent.chatbotID,
                                           request.intent.name)) {
        _return.__isset.intent = true;
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "Record does not exist.");
      }
    } else {
      _return.rc = 1;
      _return.error = "Invalid params, intent obj is required.";
      _return.__isset.rc = true;
      _return.__isset.error = true;
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 删除意图，同时删除其关联的槽位和说法
 */
void ServingHandler::delIntent(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  Intent intent; // 意图

  try {
    // 通过id删除
    if(request.__isset.id) {

      if(!getIntentDetailById(intent, stmt, request.id)) {
        rc_and_error(_return, 2, "Record do not exist.");
        return;
      }
    } else if(request.__isset.intent && request.intent.__isset.chatbotID
              && request.intent.__isset.name) {
      /**
      * 查找目标意图
      */
      if(!getIntentDetailByChatbotIDAndName(intent, stmt,
                                            request.intent.chatbotID,
                                            request.intent.name)) {
        rc_and_error(_return, 2, "Record do not exist.");
        return;
      }
    } else {
      rc_and_error(_return, 1, "Invalid params, intent obj is required.");
      return;
    }

    /**
     * 执行删除任务
     */
    VLOG(3) << __func__ << " intent: " << FromThriftToUtf8DebugString(&intent);
    // 删除相关槽位
    delSlotsByIntentId(stmt, intent.id);

    // 删除相关说法
    delUttersByIntentId(stmt, intent.id);

    // 删除意图
    delIntentById(stmt, intent.id);
    _return.rc = 0;
    _return.msg = "done.";
    _return.__isset.rc = true;
    _return.__isset.msg = true;
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

void ServingHandler::postUtter(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.intent &&
      request.intent.__isset.chatbotID &&
      request.intent.__isset.name &&
      request.__isset.utter &&
      request.utter.__isset.utterance) {
    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

    Intent intent; // 意图

    try {
      if(getIntentDetailByChatbotIDAndName(intent, stmt,
                                           request.intent.chatbotID,
                                           request.intent.name)) {
        VLOG(3) << __func__ << " intent " << FromThriftToUtf8DebugString(&intent);
        IntentUtter utter;
        utter.id = generate_uuid();
        utter.createdate = GetCurrentTimestampFormatted();
        utter.updatedate = utter.createdate;
        utter.utterance = request.utter.utterance;
        utter.__isset.id = true;
        utter.__isset.updatedate =  true;
        utter.__isset.createdate = true;
        utter.__isset.utterance = true;

        stringstream sql;
        sql << "INSERT INTO cl_intent_utters(id, intent_id, utterance,";
        sql << " createdate, updatedate) VALUES ('";
        sql << utter.id << "', '" << intent.id << "', '";
        sql << utter.utterance << "', '";
        sql << utter.createdate << "', '";
        sql << utter.updatedate << "')";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->execute(sql.str());

        _return.rc = 0;
        _return.utter = utter;
        _return.__isset.rc = true;
        _return.__isset.utter = true;
      } else {
        rc_and_error(_return, 5, "Intent record not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }

    stmt.reset(NULL);
  } else {
    rc_and_error(_return, 1, "Invalid params, intent obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};


/**
 * 更新意图说法
 */
void ServingHandler::putUtter(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.utter &&
      request.utter.__isset.id &&
      request.utter.__isset.utterance) {
    try {
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
      IntentUtter utter;

      if(getIntentUtterDetailById(utter, stmt,
                                  request.utter.id)) {
        stringstream sql;
        utter.updatedate = GetCurrentTimestampFormatted();
        sql << "UPDATE cl_intent_utters SET utterance = '";
        sql << request.utter.utterance << "', updatedate = '";
        sql << utter.updatedate << "' WHERE id = '" << request.utter.id << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->execute(sql.str());

        utter.utterance = request.utter.utterance;
        _return.rc = 0;
        _return.utter = utter;
        _return.__isset.rc = true;
        _return.__isset.utter = true;
      } else {
        rc_and_error(_return, 6, "Record not exist.");
      }

      stmt.reset(NULL);
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, intent obj is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得说法列表
 */
void ServingHandler::getUtters(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  // pagination info
  int pagesize = request.__isset.pagesize ? request.pagesize : 20;
  // page begins with 1 and default is 1.
  int page = request.__isset.page ? request.page : 1;

  if(page <= 0) // redefine any invalid value as 1.
    page = 1;

  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  VLOG(3) << __func__ << " pagesize " << pagesize << ", page " << page;

  try {
    if(request.__isset.intent &&
        request.intent.__isset.chatbotID &&
        request.intent.__isset.name ) {
      Intent intent;

      if(getIntentDetailByChatbotIDAndName(intent, stmt,
                                           request.intent.chatbotID,
                                           request.intent.name)) {
        if(getUttersWithPagination(_return, stmt, intent.id, pagesize, page)) {
          _return.rc = 0;
          _return.__isset.rc = true;
        } else {
          _return.rc = 7;
          _return.error = "Records not exist.";
          _return.__isset.rc = true;
          _return.__isset.error = true;
        }
      } else {
        rc_and_error(_return, 6, "Intent not exist.");
      }

    } else {
      if(getUttersWithPagination(_return, stmt, "", pagesize, page)) {
        _return.rc = 0;
        _return.__isset.rc = true;
      };
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  stmt.reset(NULL);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得说法详情
 */
void ServingHandler::getUtter(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.utter &&
      request.utter.__isset.id) {

    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

    try {
      if(getIntentUtterDetailById(_return.utter, stmt, request.utter.id)) {
        _return.rc = 0;
        _return.__isset.rc = true;
        _return.__isset.utter = true;
      } else {
        rc_and_error(_return, 6, "Record not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }

    stmt.reset(NULL);
  } else {
    rc_and_error(_return, 1, "Invalid params, utter.id is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 删除说法
 */
void ServingHandler::delUtter(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.utter &&
      request.utter.__isset.id) {
    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

    try {
      stringstream sql;
      sql << "DELETE FROM cl_intent_utters where id = '" << request.utter.id;
      sql << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }

    stmt.reset(NULL);
  } else {
    rc_and_error(_return, 1, "Invalid params, utter.id is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 创建槽位
 */
void ServingHandler::postSlot(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.intent &&
      request.__isset.slot && (
        request.__isset.customdict ||
        request.__isset.sysdict
      ) &&
      request.intent.__isset.chatbotID &&
      request.intent.__isset.name &&
      request.slot.__isset.name
    ) {
    try {
      stringstream sql;
      bool isCustomdict = true;
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      // 获取意图
      Intent intent;

      if(!getIntentDetailByChatbotIDAndName(intent, stmt,
                                            request.intent.chatbotID,
                                            request.intent.name)) {
        // 未能正确获得意图信息
        rc_and_error(_return, 8, "intent set in params does not exist in database.");
        return;
      }

      if(request.__isset.customdict && request.__isset.sysdict) {
        rc_and_error(_return, 6, "Set either customdict or sysdict, but not both.");
        return;
      } else if(request.__isset.customdict &&
                !(request.customdict.__isset.chatbotID &&
                  request.customdict.__isset.name)
               ) {
        // invalid value for customdict
        rc_and_error(_return, 5, "Set chatbotID and name for customdict.");
        return;
      } else if(request.__isset.sysdict && request.sysdict.__isset.name) {
        isCustomdict = false;
      } else if(request.__isset.sysdict && !request.sysdict.__isset.name) {
        rc_and_error(_return, 7, "Set name for sysdict.");
        return;
      }

      // 获取词典
      Dict dict;

      if(!getDictDetailByChatbotIDAndName(dict, stmt,
                                          isCustomdict ? request.customdict.chatbotID : CL_SYSDICT_CHATBOT_ID,
                                          isCustomdict ? request.customdict.name : request.sysdict.name,
                                          !isCustomdict)) {
        // 未能得到该自定义词典信息
        rc_and_error(_return, 8, "dict set in params does not exist in database.");
        return;
      }

      // 创建槽位
      IntentSlot slot;
      slot.id = generate_uuid();
      slot.createdate = GetCurrentTimestampFormatted();
      slot.updatedate = slot.createdate;
      slot.name = request.slot.name;

      slot.__isset.id = true;
      slot.__isset.createdate = true;
      slot.__isset.updatedate = true;
      slot.__isset.name = true;

      // 是否必填
      if(!request.slot.__isset.requires) {
        slot.requires = false;
      } else {
        slot.requires = request.slot.requires;
      }

      slot.__isset.requires = true;

      // 追问
      if(request.slot.__isset.question) {
        slot.question = request.slot.question;
        slot.__isset.question = true;
      }

      sql << "INSERT INTO cl_intent_slots(id, intent_id, name,";
      sql << " createdate, updatedate, dict_id, requires, question";
      sql << ") VALUES ('" << slot.id << "', '" << intent.id << "', '";
      sql << slot.name << "', '" << slot.createdate;
      sql << "', '" << slot.updatedate << "', '" << dict.id;
      sql << "', " << (slot.requires ? 1 : 0) << ", '";
      sql << (slot.__isset.question ? slot.question : "" ) << "')";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      slot.dict = dict;
      slot.__isset.dict = true;

      _return.rc = 0;
      _return.slot = slot;
      _return.__isset.rc = true;
      _return.__isset.slot = true;
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, intent, dict(customdict or sysdict), slot are required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 更新槽位
 */
void ServingHandler::putSlot(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.slot &&
      request.slot.__isset.id) {
    try {

      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      // 获得Slot
      IntentSlot slot;

      if(getSlotDetailById(slot, stmt, request.slot.id)) {
        // 准备更新SQL语句
        stringstream sql;
        bool appendcomma = false;
        sql << "UPDATE cl_intent_slots SET";

        // 更新 requires
        if(request.slot.__isset.requires) {
          sql << " requires = " << (request.slot.requires ? 1 : 0);
          slot.requires = request.slot.requires;
          slot.__isset.requires = true;
          appendcomma = true;
        }

        // 更新 追问
        if(request.slot.__isset.question) {
          if(appendcomma)
            sql << ", ";

          sql << " question = '" << request.slot.question << "'";
          slot.question = request.slot.question;
          slot.__isset.question = true;
          appendcomma = true;
        }

        // 更新 name
        if(request.slot.__isset.name) {
          if(appendcomma)
            sql << ", ";

          sql << " name = '" << request.slot.name << "'";
          slot.name = request.slot.name;
          slot.__isset.name = true;
          appendcomma = true;
        }

        // 开始设置词典
        if(request.__isset.customdict && request.__isset.sysdict) {
          rc_and_error(_return, 9, "Can not set both customdict and sysdict in the meanwhile.");
          return;
        } else if(request.__isset.customdict) {
          if(request.customdict.__isset.name &&
              request.customdict.__isset.chatbotID) {
            // 更新词典：自定义词典
            // 获得自定义词典
            CustomDict customdict;

            if(getDictDetailByChatbotIDAndName(customdict,
                                               stmt,
                                               request.customdict.chatbotID,
                                               request.customdict.name)) {
              if(appendcomma)
                sql << ", ";

              sql << " dict_id ='" << customdict.id << "'";
              slot.dict_id = customdict.id;
              slot.__isset.dict_id = true;
              _return.customdict = customdict;
              _return.__isset.customdict = true;
              appendcomma = true;

              // 更新 slot.dict 值
              slot.dict = customdict;
              slot.__isset.dict = true;
            } else {
              rc_and_error(_return, 8, "Record not exist for customdict");
              return;
            }
          } else {
            rc_and_error(_return, 7, "Invalid value for customdict");
            return;
          }
        } else if(request.__isset.sysdict) {
          // 设置系统词典
          if(request.sysdict.__isset.name) {
            SysDict sysdict;

            //  此处系统字典也要是active的，此处不做active的检查
            //  #TODO 在getSysDicts要默认返回active的
            if(getDictDetailByChatbotIDAndName(sysdict, stmt,
                                               CL_SYSDICT_CHATBOT_ID,
                                               request.sysdict.name,
                                               true)) {
              if(appendcomma)
                sql << ", ";

              sql << " dict_id = '" << sysdict.id << "' ";
              slot.dict_id = sysdict.id;
              slot.__isset.dict_id = true;
              _return.sysdict = sysdict;
              _return.__isset.sysdict = true;
              appendcomma = true;

              // 更新 slot.dict 值
              slot.dict = sysdict;
              slot.__isset.dict = true;
            } else {
              rc_and_error(_return, 12, "Record not exist for sysdict");
              return;
            }
          } else {
            rc_and_error(_return, 11, "Invalid value for sysdict");
            return;
          }
        }  // 结束设置词典

        // 设置 updatedate
        slot.updatedate = GetCurrentTimestampFormatted();

        if(appendcomma)
          sql << ", ";

        sql << " updatedate ='" << slot.updatedate << "'";
        appendcomma = true;

        // 增加 WHERE
        sql << " WHERE id = '" << slot.id << "'";

        VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
        stmt->executeUpdate(sql.str());

        _return.rc = 0;
        _return.slot = slot;
        _return.__isset.rc = true;
        _return.__isset.slot = true;
      } else {
        rc_and_error(_return, 6, "Record not exist");
        return;
      }

      stmt.reset(NULL);
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, slot.id is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得槽位列表
 */
void ServingHandler::getSlots(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  try {
    if(request.__isset.intent  &&
        request.intent.__isset.chatbotID &&
        request.intent.__isset.name) {
      // pagination info
      int pagesize = request.__isset.pagesize ? request.pagesize : 20;
      // page begins with 1 and default is 1.
      int page = request.__isset.page ? request.page : 1;

      if(page <= 0) // redefine any invalid value as 1.
        page = 1;

      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      //  获得意图
      Intent intent;

      if(!getIntentDetailByChatbotIDAndName(intent, stmt,
                                            request.intent.chatbotID,
                                            request.intent.name)) {
        rc_and_error(_return, 5, "Intent not exist");
        return;
      }

      if(getSlotsWithPagination(_return, stmt, intent.id, pagesize, page)) {
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        rc_and_error(_return, 6, "Error when finding slots.");
        return;
      }
    } else {
      rc_and_error(_return, 1, "Invalid params, intent is required.");
      return;
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
    return;
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 获得槽位详情
 */
void ServingHandler::getSlot(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  try {
    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

    if(request.__isset.slot &&
        request.slot.__isset.id) {
      IntentSlot slot;

      if(getSlotDetailById(slot, stmt, request.slot.id)) {
        _return.rc = 0;
        _return.slot = slot;
        _return.__isset.rc = true;
        _return.__isset.slot = true;
        return true;
      } else {
        _return.rc = 4;
        _return.error = "Record not exist";
        _return.__isset.rc = true;
        _return.__isset.error = true;
      }
    } else if(request.__isset.intent &&
              request.__isset.slot &&
              request.intent.__isset.chatbotID &&
              request.intent.__isset.name &&
              request.slot.__isset.name) {
      // 获得Intent
      Intent intent;

      if(!getIntentDetailByChatbotIDAndName(intent,
                                            stmt,
                                            request.intent.chatbotID,
                                            request.intent.name)) {
        _return.rc = 5;
        _return.error = "Intent not exist";
        _return.__isset.rc = true;
        _return.__isset.error = true;
        return;
      }

      // 槽位
      IntentSlot slot;

      if(getIntentSlotDetailByNameAndIntentId(slot,
                                              stmt,
                                              request.slot.name,
                                              intent.id)) {
        _return.rc = 0;
        _return.slot = slot;
        _return.__isset.rc = true;
        _return.__isset.slot = true;
      } else {
        _return.rc = 6;
        _return.error = "Can not find record.";
        _return.__isset.rc = true;
        _return.__isset.error = true;
      }

      stmt.reset(NULL);
    } else {
      rc_and_error(_return, 1, "Invalid params, intent, dict(customdict or sysdict), slot are required.");
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 删除槽位
 */
void ServingHandler::delSlot(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
  stringstream sql;

  try {
    if(request.__isset.slot &&
        request.slot.__isset.id) {
      // 使用ID删除
      sql.str("");
      sql << "DELETE from cl_intent_slots WHERE id ='";
      sql << request.slot.id << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;

    } else if(request.__isset.intent &&
              request.__isset.slot &&
              request.intent.__isset.chatbotID &&
              request.intent.__isset.name &&
              request.slot.__isset.name) {

      // 获取意图
      Intent intent;

      if(!getIntentDetailByChatbotIDAndName(intent,
                                            stmt,
                                            request.intent.chatbotID,
                                            request.intent.name)) {
        _return.rc = 5;
        _return.error = "Invalid intent.";
        _return.__isset.rc = true;
        _return.__isset.error = true;
        return;
      }

      sql.str("");
      sql << "DELETE from cl_intent_slots WHERE name ='";
      sql << request.slot.name << "' AND intent_id = '";
      sql << intent.id << "'";

      VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
      stmt->execute(sql.str());

      _return.rc = 0;
      _return.msg = "success";
      _return.__isset.rc = true;
      _return.__isset.msg = true;
    } else {
      rc_and_error(_return, 1, "Invalid params.");
    }
  } catch (sql::SQLException &e) {
    mysql_error(_return, e);
  } catch (std::runtime_error &e) {
    VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
    VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
    rc_and_error(_return, 3, "ERR: Duplicate entry.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 查看语言模型训练状态
 */
void ServingHandler::status(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.chatbotID) {
    string status = _redis->get(rkey_chatbot_build(request.chatbotID));

    if(status.empty()) {
      // 为获得状态
      // 从磁盘获得前面训练成功的版本信息
      fs::path developdir(FLAGS_workarea + "/" + request.chatbotID + "/develop");
      VLOG(3) << __func__ << " not found status for " << request.chatbotID << " in Redis, "
              << ", check status in disk " << developdir.string();

      if(fs::exists(developdir)) {
        fs::path symlink = fs::read_symlink(developdir);
        VLOG(3) << __func__ << " develop version in disk: " << symlink.string();
        rdone_chatbot_build_and_devver(*_redis, request.chatbotID, symlink.string());
        _return.rc = 0;
        _return.__isset.rc = true;
        _return.msg = CL_CHATBOT_BUILD_DONE;
        _return.__isset.msg = true;
      } else {
        // never trained before
        _redis->set(rkey_chatbot_build(request.chatbotID), CL_CHATBOT_BUILD_EMTPY);
        rc_and_error(_return, 4, CL_CHATBOT_BUILD_EMTPY);
      }
    } else if(status == CL_CHATBOT_BUILD_TRAINING) {
      rc_and_error(_return, 2, CL_CHATBOT_BUILD_TRAINING);
    } else if(status == CL_CHATBOT_BUILD_DONE) {
      _return.rc = 0;
      _return.__isset.rc = true;
      _return.msg = CL_CHATBOT_BUILD_DONE;
      _return.__isset.msg = true;
    } else if(status == CL_CHATBOT_BUILD_FAIL) {
      rc_and_error(_return, 3, CL_CHATBOT_BUILD_FAIL);
    } else if(status == CL_CHATBOT_BUILD_EMTPY) {
      rc_and_error(_return, 4, CL_CHATBOT_BUILD_EMTPY);
    } else if(status == CL_CHATBOT_BUILD_CANCAL) {
      rc_and_error(_return, 5, CL_CHATBOT_BUILD_CANCAL);
    } else {
      VLOG(2) << __func__ << " unknown status " << status << " for chatbot " << request.chatbotID;
      rc_and_error(_return, 6, "Unknown status.");
    }
  } else {
    rc_and_error(_return, 1, "Invalid params, chatbotID is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 训练机器人
 */
void ServingHandler::train(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.chatbotID) {
    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

    // build profile
    chatopera::bot::intent::Profile profile;
    profile.set_chatbotid(request.chatbotID);

    size_t intents_size = 0;     // 意图总个数
    size_t slots_size = 0;       // 槽位总个数
    size_t utters_size = 0;      // 说法总个数
    size_t customdicts_size = 0; // 自定义词典总个数
    size_t dictwords_size = 0;   // 自定义词条总个数

    Data context; // 从数据库检索数据

    try {
      /**
       * 获得所有自定义词典及词条
       */
      if(getDictsWithPagination(context, stmt, false, request.chatbotID,
                                CL_CUSTOMDICT_MAX_NUMBER, 1) &&
          context.__isset.customdicts &&
          context.customdicts.size() > 0) {
        VLOG(3) << "getDictsWithPagination data: " << FromThriftToUtf8DebugString(&context);

        for(const CustomDict& customdict : context.customdicts) {
          // 创建词典
          chatopera::bot::intent::TDict* tdict = profile.add_dicts();
          tdict->set_id(customdict.id);
          tdict->set_name(customdict.name);
          tdict->set_chatbotid(customdict.chatbotID);
          tdict->set_builtin(false);
          customdicts_size++;

          Data params;
          params.chatbotID = customdict.chatbotID;
          params.__isset.chatbotID = true;
          params.customdict.name = customdict.name;
          params.customdict.__isset.name = true;
          size_t customdict_words_count = 0;

          // 获得每个词典的词条
          if(getWordsWithPagination(context,
                                    stmt,
                                    params,
                                    CL_CUSTOMWORD_MAX_NUMBER,
                                    1,
                                    "")) {
            if(context.__isset.dictwords) {
              for(const DictWord& dictword : context.dictwords) {
                // 创建词典词条
                chatopera::bot::intent::TDictWord* tdictword = tdict->add_dictwords();
                tdictword->set_word(dictword.word);
                tdictword->set_dict_id(customdict.id);
                tdictword->set_synonyms(dictword.synonyms);
                dictwords_size++;
                customdict_words_count++;
              }
            }
          }

          // 自定义词典的词条为0
          if(customdict_words_count == 0) {
            rc_and_error(_return, 22, "无法开始训练，包含有词典数为0的自定义词典。");
            return;
          }
        }
      }

      /**
       * 获得所有意图
       */
      if(getIntentsWithPagination(context, stmt,
                                  request.chatbotID,
                                  CL_INTENTS_MAX_NUMBER_PERBOT, 1) &&
          context.__isset.intents &&
          context.intents.size() > 0) {
        for(const Intent& intent : context.intents) {
          chatopera::bot::intent::TIntent* tintent = profile.add_intents();
          tintent->set_id(intent.id);
          tintent->set_chatbotid(intent.chatbotID);
          tintent->set_name(intent.name);
          intents_size++;

          // 获得意图槽位
          if(getSlotsWithPagination(context, stmt, intent.id,
                                    CL_SLOTS_MAX_NUMBER_PERINTENT, 1) &&
              context.__isset.slots &&
              context.slots.size() > 0) {
            for(const IntentSlot& slot : context.slots) {
              chatopera::bot::intent::TIntentSlot* tslot = tintent->add_slots();
              tslot->set_name(slot.name);
              tslot->set_dictname(slot.dict.name);
              tslot->set_requires(slot.requires);
              tslot->set_question(slot.question);
              slots_size++;
            }
          }

          // 获得意图说法
          if(getUttersWithPagination(context, stmt, intent.id,
                                     CL_UTTER_MAX_NUMBER_PERINTENT, 1) &&
              context.__isset.utters &&
              context.utters.size() > 0) {
            for(const IntentUtter& utter : context.utters) {
              chatopera::bot::intent::TIntentUtter* tutter = tintent->add_utters();
              tutter->set_id(utter.id);
              tutter->set_intent_id(utter.intent_id);
              tutter->set_utterance(utter.utterance);
              utters_size++;
            }
          }
        }

        if(utters_size == 0) { // 没有说法，无法训练模型
          VLOG(3) << __func__ << " no utterance detected in chatbot " << request.chatbotID;
          rc_and_error(_return, 21, "无法开始训练，确定该机器人的意图说法数量大于1");
          return;
        }

        // 发送训练任务
        VLOG(3) << __func__ << " chatbotID intents: " << intents_size << ", slots: " << slots_size
                << ", utters: " << utters_size << ", dictwords: " << dictwords_size << ", customdicts: " << customdicts_size;

        string serialized;
        profile.SerializeToString(&serialized);

        if(_brokerpub->publish(request.chatbotID, "train", serialized)) {
          _return.rc = 0;
          _return.__isset.rc = true;
          _return.msg = "train job is dispatched.";
          _return.__isset.msg = true;

          // 设置BUILD状态
          _redis->set(rkey_chatbot_build(request.chatbotID), CL_CHATBOT_BUILD_TRAINING);
        }
      } else {
        // 没有意图，不进行训练，更新BOT状态，指定未进行训练的原因
        VLOG(3) << __func__ << " no intent detected in chatbot " << request.chatbotID;
        rc_and_error(_return, 21, "无法开始训练，确定该机器人的意图数量大于1");
        // #TODO 设置BUILD状态：失败 （目前忽略失败后更新Build状态）
        // _redis->set(rkey_chatbot_build(request.chatbotID), CL_CHATBOT_BUILD_FAIL);
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(2) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(2) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }

    VLOG(3) << __func__ << " profile data \n" << FromProtobufToUtf8DebugString(profile);
  } else {
    rc_and_error(_return, 1, "Invalid params, chatbotID is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};


/**
 * 创建新的会话周期
 * 每次对话要
 */
void ServingHandler::putSession(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.session &&
      request.session.__isset.chatbotID &&
      request.session.__isset.uid &&
      request.session.__isset.channel &&
      request.session.__isset.branch) {

    if(request.session.branch != CL_BOT_BRANCH_DEV &&
        request.session.branch != CL_BOT_BRANCH_PRO) {
      rc_and_error(_return, 11, "Invalid branch in request session, set 'dev' or 'pro'.");
      return;
    }

    boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());
    string session_id(generate_uuid());

    try {
      ChatSession session;

      if(createNewSession(session,
                          stmt,
                          request.session.chatbotID,
                          request.session.uid,
                          request.session.channel,
                          request.session.branch)) {
        // 生成Session到Redis缓存
        intent::TChatSession tsession;
        tsession.set_id(session.id);
        tsession.set_channel(session.channel);
        tsession.set_uid(session.uid);
        tsession.set_branch(session.branch);
        tsession.set_createdate(session.createdate);
        tsession.set_updatedate(session.updatedate);
        tsession.set_chatbotid(session.chatbotID);

        string serialized;
        string sessionKey = rkey_chatbot_session(session.id);
        tsession.SerializeToString(&serialized);

        // 设定过期时间
        _redis->set(sessionKey, serialized);
        _redis->expire(sessionKey, CL_CHATSESSION_MAX_IDLE_PERIOD);

        append_sessionid_to_chatbot_branch(*_redis,
                                           request.session.chatbotID,
                                           request.session.branch,
                                           session.id);

        _return.rc = 0;
        _return.session = session;
        _return.__isset.rc = true;
        _return.__isset.session = true;
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 10, "Invalid params, chatbotID, uid, channel, branch is required in session obj.");
    return;
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * ChatSession的数据格式转化
 * 从Proto到Thrift
 */
inline void convert_session_proto2thrift(const intent::TChatSession& session,
    ChatSession& thr) {
  thr.id = session.id();
  thr.__isset.id = true;
  thr.intent_name = session.intent_name();
  thr.__isset.intent_name = true;
  thr.chatbotID = session.chatbotid();
  thr.__isset.chatbotID = true;
  thr.uid = session.uid();
  thr.__isset.uid = true;
  thr.channel = session.channel();
  thr.__isset.channel = true;
  thr.resolved = session.resolved();
  thr.__isset.resolved = true;

  for(const intent::TChatSession::Entity& entity : session.entities()) {
    Entity e;
    e.name = entity.name();
    e.val = entity.val();
    e.requires = entity.requires();
    e.dictname = entity.dictname();
    e.__isset.name = true;
    e.__isset.val = true;
    e.__isset.requires = true;
    e.__isset.dictname = true;
    thr.entities.push_back(e);
  }

  if(thr.entities.size() > 0) {
    thr.__isset.entities = true;
  }

  thr.branch = session.branch();
  thr.__isset.branch = true;
  thr.createdate = session.createdate();
  thr.__isset.createdate = true;
  thr.updatedate = session.updatedate();
  thr.__isset.updatedate = true;
}

/**
 * 获得会话信息
 */
void ServingHandler::getSession(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.session &&
      request.session.__isset.id) {

    intent::TChatSession tsession;

    if(getSessionFromRedisById(*_redis, request.session.id, tsession)) {
      convert_session_proto2thrift(tsession, _return.session);
      _return.__isset.session = true;
      _return.rc = 0;
      _return.__isset.rc = true;
    } else {
      rc_and_error(_return, 11, "Invalid sessionId, session not exist or expired.");
    }
  } else {
    rc_and_error(_return, 10, "Invalid params.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 验证BOT实例
 * 如果该实例已经不是最新的，从新加载BOT
 */
int ServingHandler::resolveBotByChatbotIDAndBranch(const Redis& redis,
    map<string, Bot* >& bots,
    const intent::TChatSession& session) {
  VLOG(3) << __func__ << " resolve chatbotID: " << session.chatbotid() << ", branch: " << session.branch();
  int result = 1;

  // 获得版本信息
  string version;

  if(session.branch() == CL_BOT_BRANCH_DEV) { // 开发分支
    version = redis.get(rkey_chatbot_devver(session.chatbotid()));
  } else if(session.branch() == CL_BOT_BRANCH_PRO) { // 生产环境
    version = redis.get(rkey_chatbot_prover(session.chatbotid()));
  } else {
    VLOG(3) << __func__ << " Error: invalid branch";
    throw std::runtime_error("Error: invalid branch, should not happen." );
  }

  if(!version.empty()) {  // 取得最新版本
    VLOG(3) << __func__ << " get version in redis: " << version;
    bool reload = false;  // 是否重新加载BOT
    bool freepre = false; // 是否释放前版本

    if(bots.find(session.chatbotid()) != bots.end()) { // 当前服务包含该bot实例
      if(bots[session.chatbotid()]->getBuildver() != version) {
        reload = true;
        freepre = true;
      }
    } else { // 重新加载BOT
      reload = true;
    }

    VLOG(3) << __func__ << " reload: " << reload << ", free pre: " << freepre;

    // 重新加载
    if(reload) {
      {
        std::lock_guard<std::mutex> guard(_bot_lock);

        if(freepre) { // 释放前版本
          delete bots[session.chatbotid()];
          bots.erase(session.chatbotid());
        }

        bots[session.chatbotid()] = new Bot();

        if(bots[session.chatbotid()]->init(session.chatbotid(),
                                           session.branch(),
                                           version)) {
          return true;
        } else {
          throw std::runtime_error("Can not reload chatbot " + session.chatbotid() + " version " + version );
        }
      }
    }

  } else {
    VLOG(3) << __func__ << " Error: bot version not available.";
    throw std::runtime_error("CL:INVALID_BOT_VERSION_INFO" );
  }

  return result;
}


/**
 * 聊天
 */
void ServingHandler::chat(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.message &&
      request.message.__isset.textMessage &&
      request.__isset.session &&
      request.session.__isset.id) {
    try {
      /**
       * 获得session信息
       */
      intent::TChatSession session;

      if(getSessionFromRedisById(*_redis, request.session.id, session)) {
        VLOG(3) << __func__ << " restore session \n" << FromProtobufToUtf8DebugString(session);

        bool isDevBranch = true;

        if(session.branch() == CL_BOT_BRANCH_DEV) { // 调试版本
          // keep isDevBranch as it is true
        } else if(session.branch() == CL_BOT_BRANCH_PRO) { // 生产环境
          isDevBranch = false;
        } else {
          rc_and_error(_return, 12, "Invalid chat branch.");
          return;
        }

        if(resolveBotByChatbotIDAndBranch(*_redis, isDevBranch ? _bots_dev : _bots_pro, session)) {
          if(session.resolved()) {
            VLOG(3) << " intent is resolved. \n"
                    << FromProtobufToUtf8DebugString(session);
            // 检查是否已经完成意图识别
            // 设置 session 返回值, 返回结果
            convert_session_proto2thrift(session, _return.session);
            _return.__isset.session = true;
            _return.rc = 0;
            _return.__isset.rc = true;
            _return.msg = "Done, create new session as next context.";
            _return.__isset.msg = true;
            return;
          }

          // 赋值BOT
          const Bot& bot =  *((isDevBranch ? _bots_dev : _bots_pro)[session.chatbotid()]);
          sysdicts::Data syswords; // 系统词典的词条信息

          /**
           * 获得系统词典分析
           */
          if(bot.getReferredSysdicts().size() > 0) { // BOT有引用系统词典
            sysdicts::Data sysdict_request;

            // 分析Session查看是否需要请求，减少不必要的网络请求
            for(const intent::TChatSession::Entity& entity : session.entities()) {
              // session中已经有意图
              if(entity.builtin() && entity.val().empty()) {
                sysdicts::Entity e;
                e.slotname = entity.name();
                e.dictname = entity.dictname();
                e.__isset.slotname = true;
                e.__isset.dictname = true;
                sysdict_request.entities.push_back(e);
                sysdict_request.__isset.entities = true;
              }
            }

            // 发送的条件:
            // 1. 还没有识别意图，query改写依赖于系统词典
            // 2. 已经识别到意图，需要进一步识别槽位信息
            if(session.intent_name().empty() || ((!session.intent_name().empty()) && sysdict_request.__isset.entities)) {
              bot.patchSysdictsRequestEntities(sysdict_request);
              sysdict_request.query = request.message.textMessage;
              sysdict_request.__isset.query = true;
              // 发送分析
              _sysdicts->label(syswords, sysdict_request);
            }
          }

          // 使用系统词典进行 query 改写
          string query(request.message.textMessage);

          for(const sysdicts::Entity entity : syswords.entities) {
            if(bot.hasReferredSysdict(entity.dictname)) {
              VLOG(3) << __func__ << " detect referred sysdict: " << entity.dictname << ", value: " << entity.val << ", slotname: " << entity.slotname;
              replace_first(query, entity.val, entity.dictname);
            } else {
              VLOG(3) << __func__ << " discard unreferred sysdict word: " << entity.dictname << ", value: " << entity.val;
            }
          }

          VLOG(3) << __func__ << " post query rewrite: " << query;

          ChatMessage reply; // 回复
          reply.receiver = session.uid();
          reply.__isset.receiver = true;

          std::vector<pair<string, string> > tokens;
          bot.tokenize(query, tokens);

          if(request.message.__isset.terms && request.message.terms.size() > 0) {
            request.message.terms.clear();
          }

          if(request.message.__isset.tags && request.message.tags.size() > 0) {
            request.message.tags.clear();
          }

          for(const pair<string, string>& token : tokens) {
            request.message.terms.push_back(token.first);
            request.message.tags.push_back(token.second);
          }

          // 检查是否有意图
          if(session.intent_name().empty()) {
            // 未检查出意图，首先识别意图
            if(!(bot.classify(tokens, *session.mutable_intent_name()))) {
              // 未识别到意图，返回 fallback
              VLOG(3) << __func__ << " can not find intent";
              reply.is_fallback = true;
              reply.__isset.is_fallback = true;
              session.set_is_fallback(true);
              _return.message = reply;
              _return.__isset.message = true;

              // 保存 session 到 Redis
              put_session_into_redis_by_id(*_redis, session);

              // 设置 session 返回值
              convert_session_proto2thrift(session, _return.session);
              _return.__isset.session = true;

              _return.rc = 0;
              _return.__isset.rc = true;
              _return.msg = "No intent is detected.";
              _return.__isset.msg = true;
              return;
            } else {
              // 初次识别到意图，添加 session的entities信息
              bot.setSessionEntitiesByIntentName(session.intent_name(), session);
              VLOG(3) << __func__ << " first resolve intent: " << FromProtobufToUtf8DebugString(session);
            }
          } else {
            VLOG(3) << __func__ << " already got intent " << session.intent_name();
          }

          // 已经识别意图，继续识别 entities
          VLOG(3) << __func__ << " find entities.";

          if(bot.chat(request.message,
                      query,
                      syswords.entities,
                      session,
                      reply)) {
            // 查看最新session信息
            VLOG(3) << __func__ << " latest session info: \n" << FromProtobufToUtf8DebugString(session);

            // 保存 session 到 Redis
            put_session_into_redis_by_id(*_redis, session);

            // 设置session返回值
            convert_session_proto2thrift(session, _return.session);
            _return.__isset.session = true;

            _return.message = reply;
            _return.__isset.message = true;
            _return.rc = 0;
            _return.__isset.rc = true;
          } else {
            VLOG(3) << __func__ << "Unexpected return from bot::chat";
            rc_and_error(_return, 10, "Internal error.");
            return;
          }
        } else {
          rc_and_error(_return, 13, "Bot in dev branch is not available.");
          return;
        }
      } else {
        rc_and_error(_return, 11, "Invalid session info, expired or not exist.");
        return;
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      string reason(e.what());
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << reason << endl;

      if(!reason.empty()) {
        if(reason == "CL:INVALID_BOT_VERSION_INFO") {
          rc_and_error(_return, 25, "Invalid version info, maybe bot is not trained.");
        } else {
          rc_and_error(_return, 26, reason);
        }
      } else {
        rc_and_error(_return, 3, "Internal Error.");
      }
    }
  } else {
    rc_and_error(_return, 10, "Invalid params, message and session info are required.");
    return;
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 查看调试及生产版本信息
 */
void ServingHandler::version(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  devver(_return, request);
  prover(_return, request);
  _return.rc = 0;
  _return.__isset.rc = true;
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 查看调试版本信息
 */
void ServingHandler::devver(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.chatbotID) {
    try {
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      if(getDevverByChatbotID(_return.devver, stmt, request.chatbotID)) {
        _return.__isset.devver = true;
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        // error
        rc_and_error(_return, 11, "Can not find data for devver in db, maybe not trained yet.");
        return;
      };

    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 10, "Invalid params, chatbotID is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * 查看生产版本信息
 */
void ServingHandler::prover(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.chatbotID) {
    try {
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      if(getProverByChatbotID(_return.prover, stmt, request.chatbotID)) {
        _return.__isset.prover = true;
        _return.rc = 0;
        _return.__isset.rc = true;
      } else {
        // error
        rc_and_error(_return, 11, "Can not find data for prover in db, maybe not trained yet.");
        return;
      };

    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 10, "Invalid params, chatbotID is required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};


/**
 * 上线生产环境
 * 将测试版本升级为生产环境版本
 */
void ServingHandler::online(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.chatbotID &&
      request.__isset.prover &&
      request.prover.__isset.version) {
    try {
      boost::scoped_ptr<sql::Statement> stmt(_mysql->conn->createStatement());

      // 首先确认该版本存在
      const string botdir(FLAGS_workarea + "/" + request.chatbotID);
      fs::path verdir(botdir + "/" + request.prover.version);
      VLOG(3) << __func__ << " version dir: " << verdir.string();

      if(fs::exists(verdir)) {
        fs::path link(botdir + "/production"); // 创建生产分支的软连接

        if(fs::exists(link)) {
          fs::remove(link);
        }

        // 创建软链接
        fs::create_symlink(request.prover.version, link);

        // 创建MySQL版本
        newProver(stmt, request.chatbotID, request.prover.version, request.prover.notes);

        // 设置 Redis 信息
        rupdate_chatbot_prover(*_redis, request.chatbotID, request.prover.version);
        rupdate_chatbot_prover_online(*_redis, request.chatbotID);

        _return.rc = 0;
        _return.__isset.rc = true;
        _return.msg = "success";
        _return.__isset.msg = true;
      } else {
        rc_and_error(_return, 11, "Error, version not exist.");
      }
    } catch (sql::SQLException &e) {
      mysql_error(_return, e);
    } catch (std::runtime_error &e) {
      VLOG(3) << __func__ << " # ERR: runtime_error in " << __FILE__;
      VLOG(3) << __func__ << " # ERR: " << e.what() << endl;
      rc_and_error(_return, 3, "ERR: Duplicate entry.");
    }
  } else {
    rc_and_error(_return, 10, "Invalid params, chatbotID and prover are required.");
  }

  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

void ServingHandler::offline(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request: " << FromThriftToUtf8DebugString(&request);
  VLOG(3) << __func__ << " response: " << FromThriftToUtf8DebugString(&_return);
};

/**
 * Error Handler
 * https://gitlab.chatopera.com/chatopera/chatopera.bot/issues/212#note_2269
 */
bool ServingHandler::mysql_error(Data& _return, const sql::SQLException &e) {
  VLOG(2) << "# ERR: SQLException in " << __FILE__;
  /* Use what() (derived from std::runtime_error) to fetch the error message */
  VLOG(2) << "# ERR: " << e.what();
  VLOG(2) << " (MySQL error code: " << e.getErrorCode();
  VLOG(2) << ", SQLState: " << e.getSQLState() << " )";

  if(e.getSQLState() == "23000" && e.getErrorCode() == 1062) {
    rc_and_error(_return, 2, "ERR: Duplicate entry.");
    return true;
  } else if(e.getSQLState() == "HY000" && e.getErrorCode() == 2006) {
    rc_and_error(_return, 3, "MySQL server connection is broken.");
    return true;
  } else if(e.getSQLState() == "23000" && e.getErrorCode() == 1048) {
    rc_and_error(_return, 4, "Invalid params.");
    return true;
  }

  return false;
};
} // namespace clause
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
