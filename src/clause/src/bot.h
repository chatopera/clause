/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/session.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-24_19:02:47
 * @brief
 * Make a conversation.
 **/


#ifndef __CHATOPERA_BOT_CLAUSE_SESSION_H__
#define __CHATOPERA_BOT_CLAUSE_SESSION_H__

#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm>
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <xapian.h>
#include <tsl/htrie_map.h>
#include "leveldb/db.h"

#include <boost/filesystem/string_file.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "basictypes.h"
#include "redis.h"
#include "mysql.h"
#include "marcos.h"
#include "raf.hpp"
#include "maf.hpp"
#include "intent.pb.h"
#include "cppjieba/Jieba.hpp"
#include "similarity.h"
#include "sysdicts/serving/server_types.h"
#include "pattern.h"

using namespace std;
using namespace chatopera::redis;
using namespace chatopera::mysql;
namespace fs = boost::filesystem;

DECLARE_string(data);                      // 配置数据文件
DECLARE_string(workarea);                  // 工作空间
DECLARE_double(intent_classify_threshold);

namespace chatopera {
namespace bot {

// forward definition
namespace crfsuite {
class Tagger;
}

namespace clause {

class ServingHandler;

class Bot {
 public: // constructor
  Bot();
  ~Bot();

 public: // function
  bool init(const string& chatbotID,
            const string& branch,
            const string& buildver);
  void tokenize(const string& query, std::vector<pair<string, string> >& tokens); // 分词
  // 获得意图后，将槽位信息加入到session中
  bool setSessionEntitiesByIntentName(const string& intentName,
                                      intent::TChatSession& session);
  bool session(ChatSession& session);                            // 创建session
  bool classify(const std::vector<pair<string, string> >& query,
                const string& intentName);                      // 意图识别
  bool chat(const ChatMessage& payload,
            const string& query, /* 改写后的query */
            const vector<sysdicts::Entity>& builtins, /* 系统词典识别到的命名实体 */
            const std::vector<PatternDictMatch>& patternDictMatches, /* 正则表达式词典识别到的命名实体 */
            intent::TChatSession& session,
            ChatMessage& reply);
  string getBuildver();                                          // 获得构建版本
  vector<string> getReferredSysdicts();                          // 获得引用的系统词典列表
  bool hasReferredSysdict(const string& dictname);               // 是否引用了某系统词典
  bool patchSysdictsRequestEntities(sysdicts::Data& request);    // 请求系统词典前增加被引用的列表信息
  std::vector<pair<string, intent::TDict> >* getPatternDicts() const; // 获得正则表达式词典列表
  bool hasRelatedPatternDict(const string& dictname, const string& intentName);

 private: // member
  MySQL* _mysql;
  Redis* _redis;
  string _branch;                                      // 分支
  string _chatbotID;
  string _buildver;                                    // 构建版本

  cppjieba::Jieba* _tokenizer;
  chatopera::bot::crfsuite::Tagger* _tagger;           // 命名实体标识
  Xapian::Database* _recall;                           // BoW检索
  chatopera::bot::intent::Profile* _profile;           // 意图描述文件
  chatopera::bot::distance::Similarity* _similarity;   // 相似度比较
  tsl::htrie_map<char, set<string> >* _dictwords_triedb;     // 自定义词典词条的前缀树
  leveldb::DB* _dictwords_leveldb;                     // 自定义词典词条的leveldb
  std::vector<string>* _referred_sysdicts;             // 引用的系统词典
  std::vector<pair<string, intent::TDict> >*  _pattern_dicts; // 正则表达式词典
};


} // namespace clause
} // namespace bot
} // namespace chatopera


#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
