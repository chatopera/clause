/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * Redis Access Funtions
 */

#ifndef __CHATOPERA_BOT_CLAUSE_REDIS_ACCESS_F__
#define __CHATOPERA_BOT_CLAUSE_REDIS_ACCESS_F__

#include <redis.h>

#include "marcos.h"
#include <intent.pb.h>
#include <glog/logging.h>
#include "gflags/gflags.h"
#include <sstream>

using namespace std;
using namespace chatopera::redis;

namespace chatopera {
namespace bot {
namespace clause {

/**
 * Get redis key for chatbot build status
 */
inline string rkey_chatbot_build(const string& chatbotID) {
  stringstream ss;
  ss << "build:";
  ss << chatbotID;
  ss << ":status";
  return ss.str();
};

/**
 * Get redis key for chatbot develop branch version
 */
inline string rkey_chatbot_devver(const string& chatbotID) {
  stringstream ss;
  ss << "dev:";
  ss << chatbotID;
  ss << ":version";
  return ss.str();
};

/**
 * Get redis key for chatbot production branch version
 */
inline string rkey_chatbot_prover(const string& chatbotID) {
  stringstream ss;
  ss << "pro:";
  ss << chatbotID;
  ss << ":version";
  return ss.str();
}

/**
 * 更新机器人BUILD状态和develop version
 */
inline void rdone_chatbot_build_and_devver(const Redis& redis,
    const string& chatbotID,
    const string& version) {
  // update build status in Redis
  redis.set(rkey_chatbot_build(chatbotID), CL_CHATBOT_BUILD_DONE);
  // update dev version number
  redis.set(rkey_chatbot_devver(chatbotID), version);
};

/**
 * 聊天机器人对话session
 */
inline string rkey_chatbot_session(const string& sessionId) {
  stringstream ss;
  ss << "sid:" << sessionId;
  return ss.str();
};

/**
 * Get session info by session id
 */
inline bool getSessionFromRedisById(const Redis& redis,
                                    const string& sessionId,
                                    intent::TChatSession& session) {
  string serialized = redis.get(rkey_chatbot_session(sessionId));

  if(serialized.empty()) {
    return false;
  } else {
    session.ParseFromString(serialized);
    return true;
  }
}

/**
 * Production status
 */
inline string rkey_chatbot_prostatus(const string& chatbotID) {
  // update prover status
  stringstream ss;
  ss << "pro:" << chatbotID << ":status";
  return ss.str();
}

/**
 * Get session info by session id
 */
inline bool put_session_into_redis_by_id(const Redis& redis,
    intent::TChatSession& session) {
  string key = rkey_chatbot_session(session.id());
  string serialized;
  session.SerializeToString(&serialized);
  redis.set(key, serialized);
  redis.expire(key, CL_CHATSESSION_MAX_IDLE_PERIOD);
  return true;
}

/**
 * 更新机器人Prod version
 */
inline void rupdate_chatbot_prover(const Redis& redis,
                                   const string& chatbotID,
                                   const string& version) {
  // update prover version number
  redis.set(rkey_chatbot_prover(chatbotID), version);
};

/**
 * 更新机器人Prod Online 状态
 */
inline void rupdate_chatbot_prover_online(const Redis& redis,
    const string& chatbotID) {
  redis.set(rkey_chatbot_prostatus(chatbotID), CL_BOT_PRO_STATUS_ONLINE);
};

/**
 * 更新机器人Prod Offline 状态
 */
inline void rupdate_chatbot_prover_offline(const Redis& redis,
    const string& chatbotID) {
  redis.set(rkey_chatbot_prostatus(chatbotID), CL_BOT_PRO_STATUS_OFFLINE);
};

inline string rkey_chatbot_branch_session_lis(const string& chatbotID, const string& branch) {
  stringstream ss;
  ss << "sids:" << chatbotID << ":" << branch;
  return ss.str();
}

/**
 * 追加session信息到BOT分支上
 */
inline void append_sessionid_to_chatbot_branch(const Redis& redis,
    const string& chatbotID,
    const string& branch,
    const string& sessionId) {
  redis.rpush(rkey_chatbot_branch_session_lis(chatbotID, branch), sessionId);
}

/**
 * 删除所有和dev分支关联的session
 */
inline void rdel_chatbot_dev_sessions(const Redis& redis, const string& chatbotID) {
  string host = rkey_chatbot_branch_session_lis(chatbotID, CL_BOT_BRANCH_DEV);
  vector<string> keys = redis.list(host);

  for(const string& key : keys) {
    redis.del(rkey_chatbot_session(key));
  }

  redis.del(host);
}

} // namespace clause
} // namespace bot
} // namespace chatopera

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */