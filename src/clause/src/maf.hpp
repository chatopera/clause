/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * MySQL Access Functions
 */

#ifndef __CHATOPERA_BOT_CLAUSE_DATABASE_ACCESS_F__
#define __CHATOPERA_BOT_CLAUSE_DATABASE_ACCESS_F__

#include "marcos.h"
#include "glog/logging.h"
#include "mysql/jdbc.h"
#include "basictypes.h"
#include "TimeUtils.hpp"
#include "StringUtils.hpp"
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "serving/server_types.h"
#include "intent.pb.h"

using namespace chatopera::utils;

namespace chatopera {
namespace bot {
namespace clause {

/**
 * 查询BOT是否引用系统词典
 */
inline bool getBotSysdictByDictIdAndChatbotID(BotSysdict& botSysdict,
    const boost::scoped_ptr<sql::Statement>& stmt,
    const string& dictId,
    const string& chatbotID) {
  stringstream sql;
  sql << "SELECT id, createdate from cl_bot_sysdict WHERE dict_id = '";
  sql << dictId << "' and chatbotID ='" << chatbotID << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    botSysdict.id = rset->getString("id");
    botSysdict.createdate = rset->getString("createdate") ;
    botSysdict.__isset.id = true;
    botSysdict.__isset.createdate = true;
    return true;
  } else {
    return false;
  }
}

/**
 * 查询自定义词典对id的信息
 */
template <typename T>
inline bool getDictDetailByChatbotIDAndName(T& dict,
    const boost::scoped_ptr<sql::Statement>& stmt,
    const string& chatbotID,
    const string& name,
    bool isBuiltin = false) {
  VLOG(3) << __func__ << " name: " << name << ", chatbotID: " << chatbotID << ", isBuiltin: " << isBuiltin;
  stringstream sql;

  // 获得详情
  sql << "SELECT id, name, chatbotID, description,"
      << " createdate, updatedate, samples";

  if(isBuiltin) {
    sql << ", builtin, active";
  }

  sql << " from cl_dicts WHERE name ='" << name << "' and chatbotID = '";
  sql << (isBuiltin ? CL_SYSDICT_CHATBOT_ID : chatbotID) << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    dict.name = rset->getString("name");
    dict.chatbotID = rset->getString("chatbotID");
    dict.id = rset->getString("id");
    dict.createdate = rset->getString("createdate");
    dict.updatedate = rset->getString("updatedate");
    dict.description = rset->getString("description");
    dict.__isset.id = true;
    dict.__isset.name = true;
    dict.__isset.description = true;
    dict.__isset.chatbotID = true;
    dict.__isset.createdate = true;
    dict.__isset.updatedate = true;

    if(isBuiltin) {
      // 系统字典
      dict.builtin = rset->getInt("builtin");
      dict.active = rset->getInt("active");
      dict.__isset.builtin = true;
      dict.__isset.active = true;

      // 查看是否被引用
      BotSysdict botSysdict;

      if(getBotSysdictByDictIdAndChatbotID(botSysdict, stmt, dict.id, chatbotID)) {
        dict.referred = true;
      } else {
        dict.referred = false;
      }

      dict.__isset.referred = true;
    }

    rset.reset(NULL);
    return true;
  } else {
    // invalid record
    return false;
  }
};


/**
 * 查询词典
 * isBuiltin: true 查询系统词典; false 查询自定义词典
 */
inline bool getDictsWithPagination(Data& data,
                                   const boost::scoped_ptr<sql::Statement>& stmt,
                                   const bool isBuiltin,
                                   const string chatbotID,
                                   const int pagesize,
                                   const int page) {


  stringstream sql;
  // retrieve pagination info
  sql.str("");
  sql << "SELECT COUNT(*) FROM cl_dicts";

  if(isBuiltin) {

    if(data.__isset.sysdicts && data.sysdicts.size() > 0) {
      data.sysdicts.clear();
    }

    sql << " WHERE builtin = 1"
        << " AND chatbotID = '" << CL_SYSDICT_CHATBOT_ID << "' ";
  } else {
    if(data.__isset.customdicts && data.customdicts.size() > 0) {
      data.customdicts.clear();
    }

    sql << " WHERE builtin = 0";

    if(!chatbotID.empty()) {
      sql << " AND chatbotID = '" << chatbotID << "'";
    }
  }

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));
  cset->next();
  data.totalrows = cset->getInt(1);
  data.currpage = page;
  data.pagesize = pagesize;
  data.totalpage = ceil((double)data.totalrows / pagesize);

  data.__isset.totalrows = true;
  data.__isset.currpage = true;
  data.__isset.pagesize = true;
  data.__isset.totalpage = true;

  // get rows
  sql.str("");
  sql << "SELECT D.id as did, D.name as dname, D.chatbotID as dchatbotid,"
      << " D.description as ddescription, D.active as dactive, D.samples as dsamples, "
      << " D.createdate as dcreatedate, D.updatedate as dupdatedate";

  if((!chatbotID.empty()) && isBuiltin) {
    sql << ", B.id as bid";
  }

  sql << " FROM cl_dicts D";

  if((!chatbotID.empty()) && isBuiltin) {
    sql << " LEFT OUTER JOIN cl_bot_sysdict B ON B.dict_id = D.id ";
    sql << " AND B.chatbotID = '" << chatbotID << "'";
  }

  if(isBuiltin) {
    sql << " WHERE D.builtin = 1"
        << " AND D.chatbotID = '" << CL_SYSDICT_CHATBOT_ID << "' ";
  } else {
    sql << " WHERE D.builtin = 0";

    if(!chatbotID.empty()) {
      sql << " AND D.chatbotID = '" << chatbotID << "'";
    }
  }

  sql << " ORDER BY D.createdate DESC";
  sql << " LIMIT " << pagesize * (page - 1) << ", " << pagesize;

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << __func__ << " row count: " << rset->rowsCount();

  if(isBuiltin) { // 添加到系统词典
    while(rset->next()) {
      SysDict sysdict;
      sysdict.id = rset->getString("did");
      sysdict.name = rset->getString("dname");
      sysdict.description = rset->getString("ddescription");
      sysdict.createdate = rset->getString("dcreatedate");
      sysdict.updatedate = rset->getString("dupdatedate");
      sysdict.active = rset->getBoolean("dactive");
      sysdict.samples = rset->getString("dsamples");

      if(!chatbotID.empty()) {
        std::string referred(rset->getString("bid"));
        sysdict.referred = referred.empty() ? false : true;
        sysdict.__isset.referred = true;
      }

      sysdict.__isset.samples = true;
      sysdict.__isset.active = true;
      sysdict.__isset.id = true;
      sysdict.__isset.name = true;
      sysdict.__isset.description = true;
      sysdict.__isset.createdate = true;
      sysdict.__isset.updatedate = true;
      data.sysdicts.push_back(sysdict);
    }

    if(rset->rowsCount() > 0) {
      data.__isset.sysdicts = true;
    }
  } else { // 添加到自定义词典
    while(rset->next()) {
      CustomDict customdict;
      customdict.id = rset->getString("did");
      customdict.name = rset->getString("dname");
      customdict.chatbotID = rset->getString("dchatbotID");
      customdict.description = rset->getString("ddescription");
      customdict.createdate = rset->getString("dcreatedate");
      customdict.updatedate = rset->getString("dupdatedate");
      customdict.__isset.id = true;
      customdict.__isset.name = true;
      customdict.__isset.chatbotID = true;
      customdict.__isset.description = true;
      customdict.__isset.createdate = true;
      customdict.__isset.updatedate = true;
      data.customdicts.push_back(customdict);
    }

    if(rset->rowsCount() > 0) {
      data.__isset.customdicts = true;
    }
  }

  return true;
};


/**
 * 查询系统词典对应id或name的信息
 */
inline bool getSysDictDetail(SysDict& sysdict,
                             boost::scoped_ptr<sql::Statement>& stmt,
                             const string& id,
                             const string& name) {
  stringstream sql;

  // 获得详情
  if(id.empty()) {
    sql.str("");
    sql << "SELECT id, name, description, createdate, updatedate, samples, active, builtin";
    sql << " from cl_dicts WHERE name ='" << name << "' and chatbotID = '" << CL_SYSDICT_CHATBOT_ID << "'";
  } else {
    sql.str("");
    sql << "SELECT id, name, description, createdate, updatedate, samples, active, builtin";
    sql << " from cl_dicts WHERE id ='" << id << "'";
  }

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    sysdict.name = rset->getString("name");
    sysdict.chatbotID = CL_SYSDICT_CHATBOT_ID;
    sysdict.id = rset->getString("id");;
    sysdict.createdate = rset->getString("createdate");
    sysdict.updatedate = rset->getString("updatedate");
    sysdict.description = rset->getString("description");
    sysdict.samples = rset->getString("samples");
    sysdict.active = rset->getBoolean("active");
    sysdict.__isset.active = true;
    sysdict.__isset.samples = true;
    sysdict.__isset.id = true;
    sysdict.__isset.name = true;
    sysdict.__isset.description = true;
    sysdict.__isset.chatbotID = true;
    sysdict.__isset.createdate = true;
    sysdict.__isset.updatedate = true;
    rset.reset(NULL);
    return true;
  } else {
    // invalid record
    return false;
  }
};


/**
 * 更新系统词典
 */
inline bool updateSysDict(SysDict& sysdict,
                          const boost::scoped_ptr<sql::Statement>& stmt,
                          const Data& data,
                          const std::string& updatedate) {
  VLOG(3) << "updatedate time " << updatedate;

  if(data.sysdict.__isset.active  ||
      data.sysdict.__isset.description ||
      data.sysdict.__isset.samples) {

    stringstream sql;
    sql.str("");

    sql << "UPDATE cl_dicts SET ";

    if(data.sysdict.__isset.description) {
      sql << "description = '" << data.sysdict.description << "',";
      sysdict.description = data.sysdict.description;
    }

    if(data.sysdict.__isset.samples) {
      sql << "samples = '" << data.sysdict.samples << "',";
      sysdict.samples = data.sysdict.samples;
    }

    if(data.sysdict.__isset.active) {
      sql << "active = '" << data.sysdict.active << "',";
      sysdict.active = data.sysdict.active;
    }

    sql << " updatedate = '" << updatedate << "' WHERE chatbotID = '";
    sql << CL_SYSDICT_CHATBOT_ID << "' and name = '";
    sql << data.sysdict.name << "'";

    VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
    stmt->executeUpdate(sql.str());
    return true;
  } else {
    return false;
  }

}

inline bool getWordsWithPagination(Data& data,
                                   const boost::scoped_ptr<sql::Statement>& stmt,
                                   const Data& message,
                                   const int pagesize,
                                   const int page,
                                   const string& query) {

  if(data.__isset.dictwords && data.dictwords.size() > 0) {
    data.dictwords.clear();
  }

  stringstream sql;
  // retrieve pagination info
  sql.str("");
  sql << "SELECT COUNT(*) FROM cl_dict_words ";
  sql << " WHERE dict_id =(SELECT id FROM cl_dicts WHERE name = '";

  if(!message.chatbotID.empty()) {
    sql << message.customdict.name << "' AND chatbotID ='";
    sql << message.chatbotID << "')";
  } else {
    sql << message.sysdict.name << "' AND chatbotID ='";
    sql << CL_SYSDICT_CHATBOT_ID << "')";
  }

  if(!query.empty()) {
    sql << " AND ( word LIKE '%" << query << "%' OR synonyms LIKE '%" << query << "%')";
  }

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));

  cset->next();
  data.totalrows = cset->getInt(1);

  data.currpage = page;
  data.pagesize = pagesize;
  data.totalpage = ceil((double)data.totalrows / pagesize);

  data.__isset.totalrows = true;
  data.__isset.currpage = true;
  data.__isset.pagesize = true;
  data.__isset.totalpage = true;

  // get rows
  sql.str("");
  sql << "SELECT word, synonyms, dict_id, createdate,";
  sql << "updatedate FROM cl_dict_words D WHERE dict_id = (";
  sql << "SELECT id FROM cl_dicts WHERE name = '";

  if(!message.chatbotID.empty()) {
    sql << message.customdict.name << "' AND chatbotID ='";
    sql << message.chatbotID << "')";
  } else {
    sql << message.sysdict.name << "' AND chatbotID ='";
    sql << CL_SYSDICT_CHATBOT_ID << "')";
  }

  if(!query.empty()) {
    sql << " AND ( word LIKE '%" << query << "%' OR synonyms LIKE '%" << query << "%')";
  }

  sql << " ORDER BY D.createdate DESC";
  sql << " LIMIT " << pagesize * (page - 1) << ", " << pagesize;

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << "[getIntentsWithPagination] row count: " << rset->rowsCount();

  while(rset->next()) {
    DictWord word;
    word.word = rset->getString("word");
    word.synonyms = rset->getString("synonyms");
    word.dict_id = rset->getString("dict_id");
    word.createdate = rset->getString("createdate");
    word.updatedate = rset->getString("updatedate");
    word.__isset.updatedate = true;
    word.__isset.createdate = true;
    word.__isset.dict_id = true;
    word.__isset.word = true;
    word.__isset.synonyms = true;
    data.dictwords.push_back(word);
  }

  if(rset->rowsCount() >= 0) {
    data.__isset.dictwords = true;
  }

  return true;
};


/**
 * 获得意图详情
 */
inline bool getIntentDetailById(Intent& intent,
                                boost::scoped_ptr<sql::Statement>& stmt,
                                const string& id) {
  stringstream sql;
  // 获得详情
  sql.str("");
  sql << "SELECT name, chatbotID, description, createdate, updatedate";
  sql << " from cl_intents WHERE id ='" << id << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    intent.name = rset->getString("name");
    intent.chatbotID = rset->getString("chatbotID");
    intent.id = id;
    intent.createdate = rset->getString("createdate");
    intent.updatedate = rset->getString("updatedate");
    intent.description = rset->getString("description");
    intent.__isset.id = true;
    intent.__isset.name = true;
    intent.__isset.description = true;
    intent.__isset.chatbotID = true;
    intent.__isset.createdate = true;
    intent.__isset.updatedate = true;
    rset.reset(NULL);
    return true;
  } else {
    // invalid record
    return false;
  }
};

/**
 * 获得意图详情
 */
inline bool getIntentDetailByChatbotIDAndName(Intent& intent,
    boost::scoped_ptr<sql::Statement>& stmt,
    const string& chatbotID,
    const string& name) {
  stringstream sql;

  // 获得详情
  sql.str("");
  sql << "SELECT id, description, createdate, updatedate";
  sql << " from cl_intents WHERE chatbotID ='" << chatbotID << "' and name = '";
  sql << name << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    intent.name = name;
    intent.chatbotID = chatbotID;
    intent.id = rset->getString("id");
    intent.createdate = rset->getString("createdate");
    intent.updatedate = rset->getString("updatedate");
    intent.description = rset->getString("description");
    intent.__isset.id = true;
    intent.__isset.name = true;
    intent.__isset.description = true;
    intent.__isset.chatbotID = true;
    intent.__isset.createdate = true;
    intent.__isset.updatedate = true;
    rset.reset(NULL);
    return true;
  } else {
    // invalid record
    return false;
  }
};


/**
 * 查询意图列表
 */
inline bool getIntentsWithPagination(Data& data,
                                     const boost::scoped_ptr<sql::Statement>& stmt,
                                     const string chatbotID,
                                     const int pagesize,
                                     const int page) {
  stringstream sql;
  // retrieve pagination info
  sql.str("");
  sql << "SELECT COUNT(*) FROM cl_intents";

  if(!chatbotID.empty()) {
    sql << " WHERE chatbotID = '" << chatbotID << "'";
  }

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));
  cset->next();
  data.totalrows = cset->getInt(1);
  data.currpage = page;
  data.pagesize = pagesize;
  data.totalpage = ceil((double)data.totalrows / pagesize);

  data.__isset.totalrows = true;
  data.__isset.currpage = true;
  data.__isset.pagesize = true;
  data.__isset.totalpage = true;

  // get rows
  sql.str("");
  sql << "SELECT id, name, chatbotID, description, createdate, updatedate FROM cl_intents";

  if (!chatbotID.empty()) {
    sql << " WHERE chatbotID='" << chatbotID << "'";
  }

  sql << " ORDER BY createdate ASC";
  sql << " LIMIT " << pagesize * (page - 1) << ", " << pagesize;

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << "[getIntentsWithPagination] row count: " << rset->rowsCount();

  while(rset->next()) {
    Intent intent;
    intent.id = rset->getString("id");
    intent.name = rset->getString("name");
    intent.chatbotID = rset->getString("chatbotID");
    intent.description = rset->getString("description");
    intent.createdate = rset->getString("createdate");
    intent.updatedate = rset->getString("updatedate");
    intent.__isset.id = true;
    intent.__isset.name = true;
    intent.__isset.chatbotID = true;
    intent.__isset.description = true;
    intent.__isset.createdate = true;
    intent.__isset.updatedate = true;
    data.intents.push_back(intent);
  }

  if(rset->rowsCount() > 0) {
    data.__isset.intents = true;
  }

  return true;
};


/**
 * 通过IntentId删除槽位
 */
inline void delSlotsByIntentId(boost::scoped_ptr<sql::Statement>& stmt, const string& intentId) {
  if(!intentId.empty()) {
    stringstream sql;
    sql << "DELETE FROM cl_intent_slots WHERE intent_id = '" << intentId << "'";

    VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
    stmt->execute(sql.str());
  }
};

/**
 * 通过IntentId删除说法
 */
inline void delUttersByIntentId(boost::scoped_ptr<sql::Statement>& stmt, const string& intentId) {
  if(!intentId.empty()) {
    stringstream sql;
    sql << "DELETE FROM cl_intent_utters WHERE intent_id = '" << intentId << "'";

    VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
    stmt->execute(sql.str());
  }
};

inline void delIntentById(boost::scoped_ptr<sql::Statement>& stmt, const string& id) {
  if(!id.empty()) {
    stringstream sql;
    sql << "DELETE FROM cl_intents WHERE id = '" << id << "'";

    VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
    stmt->execute(sql.str());
  }
};



/**
 * 通过意图说法ID获得详情
 */
inline bool getIntentUtterDetailById(IntentUtter& utter,
                                     boost::scoped_ptr<sql::Statement>& stmt,
                                     const string& utterId) {
  stringstream sql;
  sql << "SELECT id, intent_id, utterance, createdate, updatedate from cl_intent_utters where id = '";
  sql << utterId << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() > 0) {
    rset->next();
    utter.id = rset->getString("id");
    utter.utterance = rset->getString("utterance");
    utter.intent_id = rset->getString("intent_id");
    utter.createdate = rset->getString("createdate");
    utter.updatedate = rset->getString("updatedate");
    utter.__isset.id = true;
    utter.__isset.utterance = true;
    utter.__isset.intent_id = true;
    utter.__isset.createdate = true;
    utter.__isset.updatedate = true;
    return true;
  } else {
    return false;
  }
};


/**
 * 查询说法列表
 */
inline bool getUttersWithPagination(Data& data,
                                    const boost::scoped_ptr<sql::Statement>& stmt,
                                    const string intent_id,
                                    const int pagesize,
                                    const int page) {
  if(data.__isset.utters && data.utters.size() > 0) {
    data.utters.clear();
  }

  stringstream sql;
  // retrieve pagination info
  sql.str("");
  sql << "SELECT COUNT(*) FROM cl_intent_utters";

  if(!intent_id.empty()) {
    sql << " WHERE intent_id = '" << intent_id << "'";
  }

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));
  cset->next();
  data.totalrows = cset->getInt(1);
  data.currpage = page;
  data.pagesize = pagesize;
  data.totalpage = ceil((double)data.totalrows / pagesize);

  data.__isset.totalrows = true;
  data.__isset.currpage = true;
  data.__isset.pagesize = true;
  data.__isset.totalpage = true;

  // get rows
  sql.str("");
  sql << "SELECT id, intent_id, utterance, createdate, updatedate FROM cl_intent_utters";

  if (!intent_id.empty()) {
    sql << " WHERE intent_id='" << intent_id << "'";
  }

  sql << " ORDER BY createdate ASC";
  sql << " LIMIT " << pagesize * (page - 1) << ", " << pagesize;

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << "[getUttersWithPagination] row count: " << rset->rowsCount();

  while(rset->next()) {
    IntentUtter utter;
    utter.id = rset->getString("id");
    utter.utterance = rset->getString("utterance");
    utter.intent_id = rset->getString("intent_id");
    utter.createdate = rset->getString("createdate");
    utter.updatedate = rset->getString("updatedate");
    utter.__isset.id = true;
    utter.__isset.utterance = true;
    utter.__isset.intent_id = true;
    utter.__isset.createdate = true;
    utter.__isset.updatedate = true;
    data.utters.push_back(utter);
  }

  if(rset->rowsCount() > 0) {
    data.__isset.utters = true;
  }

  return true;
};


/**
 * 通过ID获得槽位详情
 */
inline bool getSlotDetailById(IntentSlot& slot,
                              const boost::scoped_ptr<sql::Statement>& stmt,
                              const string& slotId) {
  stringstream sql;
  sql << "SELECT S.id as sid, S.name as sname, S.intent_id as sintent_id,"
      << " S.requires as srequires, S.question as squestion, S.createdate"
      << " as screatedate, S.updatedate as supdatedate, D.id as did, D.name as dname,"
      << " D.chatbotID as dchatbotid, D.description as ddescription, D.createdate as dcreatedate,"
      << " D.updatedate as dupdatedate, D.samples as dsamples, D.builtin as dbuiltin, D.active as dactive"
      << " FROM cl_intent_slots S"
      << " LEFT OUTER JOIN cl_dicts D ON D.id = S.dict_id"
      << " WHERE S.id='" << slotId << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    slot.id = rset->getString("sid");
    slot.name = rset->getString("sname");
    slot.intent_id = rset->getString("sintent_id");
    slot.requires = rset->getInt("srequires");
    slot.question = rset->getString("squestion");
    slot.createdate = rset->getString("screatedate");
    slot.updatedate = rset->getString("supdatedate");
    slot.__isset.id = true;
    slot.__isset.name = true;
    slot.__isset.intent_id = true;
    slot.__isset.requires = true;
    slot.__isset.question = true;
    slot.__isset.createdate = true;
    slot.__isset.updatedate = true;
    // 设置dict
    slot.dict.name = rset->getString("dname");
    slot.dict.builtin = rset->getInt("dbuiltin");
    slot.dict.description = rset->getString("ddescription");
    slot.dict.createdate = rset->getString("dcreatedate");
    slot.dict.updatedate = rset->getString("dupdatedate");
    slot.dict.samples = rset->getString("dsamples");
    slot.dict.__isset.name = true;
    slot.dict.__isset.builtin = true;
    slot.dict.__isset.description = true;
    slot.dict.__isset.createdate = true;
    slot.dict.__isset.updatedate = true;
    slot.dict.__isset.samples = true;

    if(slot.dict.builtin) {
      slot.dict.active = rset->getInt("dactive");
      slot.dict.__isset.active = true;
    }

    slot.__isset.dict = true;
    return true;
  } else {
    return false;
  }
};

/**
 * 查询槽位详情
 */
inline bool getIntentSlotDetailByNameAndIntentId(IntentSlot& slot,
    const boost::scoped_ptr<sql::Statement>& stmt,
    const string& name,
    const string& intentId) {
  stringstream sql;
  sql << "SELECT id, name, intent_id, createdate, updatedate, dict_id, ";
  sql << "requires, question FROM cl_intent_slots";
  sql << " WHERE name = '" << name << "' AND intent_id = '";
  sql << intentId << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  if(rset->rowsCount() == 1) {
    rset->next();
    slot.id = rset->getString("id");
    slot.intent_id = rset->getString("intent_id");
    slot.name = rset->getString("name");
    slot.createdate = rset->getString("createdate");
    slot.updatedate = rset->getString("updatedate");
    slot.dict_id = rset->getString("dict_id");
    slot.requires = rset->getInt("requires");
    slot.question = rset->getString("question");
    slot.__isset.id = true;
    slot.__isset.intent_id = true;
    slot.__isset.name = true;
    slot.__isset.createdate = true;
    slot.__isset.updatedate = true;
    slot.__isset.dict_id = true;
    slot.__isset.requires = true;
    slot.__isset.question = true;
    return true;
  } else {
    return false;
  }
};


/**
 * 查询槽位列表
 */
inline bool getSlotsWithPagination(Data& data,
                                   const boost::scoped_ptr<sql::Statement>& stmt,
                                   const string intentId,
                                   const int pagesize,
                                   const int page) {
  if(data.__isset.slots && data.slots.size() > 0) {
    data.slots.clear();
  }

  stringstream sql;
  // retrieve pagination info
  sql.str("");
  sql << "SELECT COUNT(*) FROM cl_intent_slots";

  if(!intentId.empty()) {
    sql << " WHERE intent_id = '" << intentId << "'";
  }

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));
  cset->next();
  data.totalrows = cset->getInt(1);
  data.currpage = page;
  data.pagesize = pagesize;
  data.totalpage = ceil((double)data.totalrows / pagesize);

  data.__isset.totalrows = true;
  data.__isset.currpage = true;
  data.__isset.pagesize = true;
  data.__isset.totalpage = true;

  // get rows
  sql.str("");
  sql << "SELECT S.id as sid, S.name as sname, S.intent_id as sintent_id,"
      << " S.requires as srequires, S.question as squestion, S.createdate"
      << " as screatedate, S.updatedate as supdatedate, D.id as did, D.name as dname,"
      << " D.chatbotID as dchatbotid, D.description as ddescription, D.createdate as dcreatedate,"
      << " D.updatedate as dupdatedate, D.samples as dsamples, D.builtin as dbuiltin, D.active as dactive"
      << " FROM cl_intent_slots S"
      << " LEFT OUTER JOIN cl_dicts D ON D.id = S.dict_id";

  if (!intentId.empty()) {
    sql << " WHERE intent_id='" << intentId << "'";
  }

  sql << " ORDER BY S.createdate ASC";
  sql << " LIMIT " << pagesize * (page - 1) << ", " << pagesize;

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << "[getSlotsWithPagination] row count: " << rset->rowsCount();

  while(rset->next()) {
    IntentSlot slot;
    slot.id = rset->getString("sid");
    slot.name = rset->getString("sname");
    slot.intent_id = rset->getString("sintent_id");
    slot.requires = rset->getInt("srequires");
    slot.question = rset->getString("squestion");
    slot.createdate = rset->getString("screatedate");
    slot.updatedate = rset->getString("supdatedate");
    slot.__isset.id = true;
    slot.__isset.name = true;
    slot.__isset.intent_id = true;
    slot.__isset.requires = true;
    slot.__isset.question = true;
    slot.__isset.createdate = true;
    slot.__isset.updatedate = true;
    // 设置dict
    slot.dict.name = rset->getString("dname");
    slot.dict.builtin = rset->getInt("dbuiltin");
    slot.dict.description = rset->getString("ddescription");
    slot.dict.createdate = rset->getString("dcreatedate");
    slot.dict.updatedate = rset->getString("dupdatedate");
    slot.dict.samples = rset->getString("dsamples");
    slot.dict.__isset.name = true;
    slot.dict.__isset.builtin = true;
    slot.dict.__isset.description = true;
    slot.dict.__isset.createdate = true;
    slot.dict.__isset.updatedate = true;
    slot.dict.__isset.samples = true;

    if(slot.dict.builtin) {
      slot.dict.active = rset->getInt("dactive");
      slot.dict.__isset.active = true;
    }

    slot.__isset.dict = true;
    data.slots.push_back(slot);
  }

  if(rset->rowsCount() > 0) {
    data.__isset.slots = true;
  }

  return true;
};

inline std::string newDevver(const boost::scoped_ptr<sql::Statement>& stmt,
                             const string& chatbotID,
                             const string& version,
                             bool published = true) {
  VLOG(3) << __func__ << " chatbotID: " << chatbotID << ", devver: " << version;
  std::string uuid = generate_uuid();
  stringstream sql;

  sql << "INSERT INTO cl_dev_vers(id, chatbotID, version, published) VALUES ('";
  sql << uuid << "', '" << chatbotID << "', '" << version << "', " << published << ")";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  stmt->execute(sql.str());

  return uuid;
};

inline bool createNewSession(ChatSession& session,
                             const boost::scoped_ptr<sql::Statement>& stmt,
                             const string& chatbotID,
                             const string& uid,
                             const string& channel,
                             const string& branch) {
  VLOG(3) << __func__ << " chatbotID: " << chatbotID << ", branch " << branch
          << ", uid: " << uid;
  std::string uuid = generate_uuid();
  std::string createdate = GetCurrentTimestampFormatted();

  stringstream sql;

  sql << "INSERT INTO cl_chat_sessions(id, chatbotID, uid, channel, branch, createdate, updatedate) VALUES ('";
  sql << uuid << "', '" << chatbotID << "', '" << uid << "', '" << channel << "', '";
  sql << branch << "', '" << createdate << "', '" << createdate << "')";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  stmt->execute(sql.str());

  session.chatbotID = chatbotID;
  session.channel = channel;
  session.branch = branch;
  session.createdate = createdate;
  session.updatedate = createdate;
  session.id = uuid;
  session.uid = uid;
  session.__isset.chatbotID = true;
  session.__isset.channel = true;
  session.__isset.branch = true;
  session.__isset.createdate = true;
  session.__isset.updatedate = true;
  session.__isset.id = true;
  session.__isset.uid = true;

  return true;
};

/**
 * 获得当前调试版本信息
 */
inline bool getDevverByChatbotID(DevelopVersion& devver,
                                 const boost::scoped_ptr<sql::Statement>& stmt,
                                 const string& chatbotID) {
  stringstream sql;
  sql << "SELECT id, version, createdate, published FROM cl_dev_vers WHERE chatbotID = '";
  sql << chatbotID << "' ORDER BY createdate DESC limit 0,1";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << __func__ << " row count: " << rset->rowsCount();

  if(rset->rowsCount() == 1) {
    rset->next();
    devver.id = rset->getString("id");
    devver.version = rset->getString("version");
    devver.createdate = rset->getString("createdate");
    devver.published = rset->getInt("published");
    devver.__isset.id = true;
    devver.__isset.version = true;
    devver.__isset.createdate = true;
    devver.__isset.published = true;
    return true;
  } else {
    return false;
  }
};

/**
 * 获得当前调试版本信息
 */
inline bool getProverByChatbotID(ProdVersion& prover,
                                 const boost::scoped_ptr<sql::Statement>& stmt,
                                 const string& chatbotID) {
  stringstream sql;
  sql << "SELECT id, version, createdate FROM cl_prod_vers WHERE chatbotID = '";
  sql << chatbotID << "' AND active = 1 ORDER BY createdate DESC limit 0,1";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(sql.str()));

  VLOG(3) << __func__ << " row count: " << rset->rowsCount();

  if(rset->rowsCount() == 1) {
    rset->next();
    prover.id = rset->getString("id");
    prover.version = rset->getString("version");
    prover.createdate = rset->getString("createdate");
    prover.__isset.id = true;
    prover.__isset.createdate = true;
    prover.__isset.version = true;
    return true;
  } else {
    return false;
  }
};

/**
 * 创建新的生产环境
 */
inline void newProver(const boost::scoped_ptr<sql::Statement>& stmt,
                      const string& chatbotID,
                      const string& version,
                      const string& notes,
                      bool active = true) {
  VLOG(3) << __func__ << " chatbotID: " << chatbotID << ", devver: " << version;
  std::string uuid = generate_uuid();
  std::string createdate = GetCurrentTimestampFormatted();
  stringstream sql;
  sql << "INSERT INTO cl_prod_vers(id, chatbotID, version, notes, createdate, active, latest) VALUES ('";
  sql << uuid << "', '" << chatbotID << "', '"
      << version << "', '"
      << ( notes.empty() ? notes : "" )
      << "', '" << createdate << "', " <<  active << "," << true << ")";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  stmt->execute(sql.str());
};

/**
 * 检测词条在词表中的唯一性
 */
inline bool isExistDictwordByCustomdictIdAndWord(const boost::scoped_ptr<sql::Statement>& stmt,
    const string& customdictId,
    const string& word) {
  stringstream sql;
  sql << "SELECT COUNT(*) FROM cl_dict_words WHERE word = '" << word;
  sql << "' AND dict_id = '" << customdictId << "'";

  VLOG(3) << __func__ << " execute SQL: \n---\n" << sql.str() << "\n---";
  boost::scoped_ptr< sql::ResultSet > cset(stmt->executeQuery(sql.str()));
  cset->next();
  return cset->getInt(1) == 1;
}


} // namespace clause
} // namespace bot
} // namespace chatopera


#endif