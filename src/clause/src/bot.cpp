/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/conv.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-24_19:02:59
 * @brief
 *
 **/

#include "bot.h"
#include "crfsuite.hpp"
#include "tsl/serialize.hpp"

namespace chatopera {
namespace bot {
namespace clause {

Bot::Bot() {
  _similarity = new chatopera::bot::distance::Similarity();
};

Bot::~Bot() {
  delete _referred_sysdicts;
  delete _profile;
  delete _dictwords_triedb;
  delete _dictwords_leveldb;
  delete _similarity;
  delete _pattern_dicts;
  // Jieba分词
  delete _tokenizer;
  // crfsuite tagger
  delete _tagger;
  // 关闭xapian搜索引擎
  _recall->close();
  delete _recall;
};

/**
 * 初始化
 */
bool Bot::init(const string& chatbotID,
               const string& branch,
               const string& buildver) {
  VLOG(3) << __func__ << " chatbotID: " << chatbotID << ", branch: " << branch << ", buildver: " << buildver;
  bool result = true;

  try {
    _mysql = MySQL::getInstance();
    _redis = Redis::getInstance();
    _chatbotID = chatbotID;
    _branch = branch;
    _buildver = buildver;

    // 初始化分词器
    VLOG(3) << __func__ << " tokenizer ...";
    stringstream ss;
    ss << FLAGS_workarea << "/" << chatbotID << "/" << buildver;
    string verdir = ss.str();
    string dictdir = verdir + "/jieba";
    _tokenizer = new cppjieba::Jieba( dictdir + "/jieba.dict.utf8",
                                      dictdir + "/hmm_model.utf8",
                                      dictdir + "/user.dict.utf8",
                                      dictdir + "/idf.utf8",
                                      dictdir + "/stop_words.utf8");

    VLOG(3) << __func__ << " tokenizer successfully.";

    // 初始化xapian搜索引擎
    VLOG(3) << __func__ << " xapian ...";
    _recall = new Xapian::Database(verdir + "/xapian");
    VLOG(3) << __func__ << " xapian successfully.";

    // 初始化crfsuire tagger
    VLOG(3) << __func__ << " tagger ...";
    _tagger = new chatopera::bot::crfsuite::Tagger();

    if(!_tagger->open(verdir + "/crfsuite.ner.model")) {
      // TODO 可能因训练失败而导致没有NER model的情况：原因比如机器人没有一个合理的说法
      VLOG(2) << __func__ << " fail to open crfsuite model for chatbotID: " << chatbotID << ", branch: " << branch << ", version: " << buildver;
      result = false;
    }

    VLOG(3) << __func__ << " tagger successfully.";

    // 初始化 dictwords
    VLOG(3) << __func__ << " dictwords hat-trie ...";
    string dictwordsfile(verdir + "/dictwords.trie.bin");
    _dictwords_triedb = new tsl::htrie_map<char, set<string> >();

    {
      std::ifstream ifs;
      ifs.exceptions(ifs.badbit | ifs.failbit | ifs.eofbit);
      ifs.open(dictwordsfile, std::ios::binary);

      boost::iostreams::filtering_istream fi;
      fi.push(boost::iostreams::zlib_decompressor());
      fi.push(ifs);

      boost::archive::binary_iarchive ia(fi);

      ia >> (*_dictwords_triedb);
    }
    VLOG(3) << __func__ << " dictwords hat-trie successfully.";

    // 初始化自定义词典词条的leveldb
    VLOG(3) << __func__ << " dictwords leveldb ...";
    leveldb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = false;
    leveldb::Status status = leveldb::DB::Open(options, verdir + "/leveldb", &_dictwords_leveldb);

    if(!status.ok()) {
      VLOG(3) << __func__ << " warn: can not load leveldb in " << verdir << "/leveldb";
    }

    VLOG(3) << __func__ << " dictwords leveldb successfully.";

    // 初始化 profile
    VLOG(3) << __func__ << " profile ...";
    _profile = new intent::Profile();
    fs::path profileFile(verdir + "/profile.pbs");
    std::string profileStringify;
    fs::load_string_file(profileFile, profileStringify);
    _profile->ParseFromString(profileStringify);
    VLOG(3) << __func__ << " loaded profile: \n" << FromProtobufToUtf8DebugString(*_profile);
    VLOG(3) << __func__ << " profile successfully.";


    // 初始化正则表达式词典
    _pattern_dicts = new std::vector<pair<string, intent::TDict> >();

    for(const intent::TDict& dict : _profile->dicts()) {
      if(dict.type() != CL_DICT_TYPE_PATTERN)
        continue;

      if(dict.has_dictpattern())
        _pattern_dicts->push_back(std::make_pair(dict.name(), dict));
    }

    VLOG(3) << __func__ << " loaded pattern dict size: " << _pattern_dicts->size();

    // 获得所有引用的系统词典
    VLOG(3) << __func__ << " sysdicts ...";
    _referred_sysdicts = new std::vector<string>();

    for(const intent::TIntent& intent : _profile->intents()) {
      for(const intent::TIntentSlot& slot : intent.slots()) {
        if(boost::starts_with(slot.dictname(), "@")) {
          _referred_sysdicts->push_back(slot.dictname());
        }
      }
    }

    VLOG(3) << __func__ << " sysdicts successfully. dictnames: " << boost::join(*_referred_sysdicts, "\t");

  } catch(std::exception ex) {
    VLOG(2) << __func__ << " bot fails. chatbotID: " << chatbotID << ", branch: " << branch << ", buildver: " << buildver;
    VLOG(2) << __func__ << ex.what();
    result = false;
  }

  return result;
};

/**
 * 会话周期
 */
bool Bot::session(ChatSession& session) {
  bool result = true;

  return result;
};

/**
 * 获得构建版本
 */
string Bot::getBuildver() {
  return _buildver;
};

void Bot::tokenize(const string& query, std::vector<pair<string, string> >& tokens) {
  _tokenizer->Tag(query, tokens);
};

/**
 * 意图识别
 * 从xapian数据库中找回候选集并进行比较，得到最匹配的作为意图
 */
bool Bot::classify(const std::vector<pair<string, string> >& query,
                   const string& intentName) {
  _recall->reopen();
  // Start an enquire session.
  Xapian::Enquire enquire(*_recall);
  vector<Xapian::Query> conditions;

  // 分词
  vector<string> lhs;
  vector<string> lhschs;

  for(const pair<string, string>& token : query) {
    conditions.push_back(Xapian::Query(token.first));
    lhs.push_back(token.first);
    vector<string> chs;
    CharSegment(token.first, chs);

    for(const string& ch : chs) {
      lhschs.push_back(ch);
    }
  }

  // fast query with OP_ELITE_SET
  Xapian::Query q(Xapian::Query::OP_ELITE_SET, conditions.begin(), conditions.end(), 30);
  VLOG(3) << __func__ << " parsed query is:" << q.get_description();

  // Find the top 10 results for the query.
  enquire.set_query(q);
  Xapian::MSet matches = enquire.get_mset(0, 10);

  VLOG(3) << __func__ << " " << matches.get_matches_estimated() << " results found.";
  VLOG(3) << __func__ << " Matches 1-" << matches.size() << ":\n";

  if(matches.size() > 0) {
    vector<pair<string, vector<string> > > relevants;
    vector<pair<string, double> > scores;

    for(Xapian::MSetIterator it = matches.begin(); it != matches.end(); it++) {
      chatopera::bot::intent::Augmented::Sample sample;
      sample.ParseFromString(it.get_document().get_data());
      VLOG(4) << __func__ << " get sample from xapian \n" << FromProtobufToUtf8DebugString(sample);

      std::vector<std::string> ch;
      CharSegment(sample.utterance(), ch);

      relevants.push_back(make_pair(sample.intent_name(), ch));
    }

    // 排序
    _similarity->sort(lhschs, relevants, scores);

    for(const pair<string, double>& score : scores) {
      VLOG(3) << __func__ << " intent: " << score.first << " score: " << score.second;

      if(score.second >= FLAGS_intent_classify_threshold) {
        intentName = score.first;
        break;
      }
    }

    if(!intentName.empty()) {
      return true;
    }

    return false;
  } else {
    // Not found relevant data
    VLOG(3) << __func__ << " No relevant data for utterance: " << query;
    return false;
  }
};

/**
 * 移动特征提取窗口
 * 给定当前curr位置，计算其他位置的号码
 * 不合法的情况下，标记为 小于0
 */
inline bool mv_feature_window(const signed int& length,
                              signed int& curr,
                              signed int& pre2,
                              signed int& pre1,
                              signed int& post1,
                              signed int& post2) {

  if(curr >= 0 && curr <= length) {
    pre2 = curr - 2;
    pre1 = curr - 1;
    post1 = curr + 1;
    post2 = curr + 2;

    if(post1 > length) {
      post1 = -1;
    }

    if(post2 > length) {
      post2 = -1;
    }

    return true;
  } else {
    // invalid curr index, maybe come to the end of loop
    return false;
  }
}

/**
 * 构建Ner的预测输入数据xseq
 */
inline void setupNerItemSequence(const vector<string>& terms,
                                 const vector<string>& poss,
                                 crfsuite::ItemSequence& xseq) {

  if(terms.size() != poss.size()) {
    throw std::runtime_error("setupNerItemSequence: Invaid labeling data.");
  }

  VLOG(3) << __func__ << " terms: " << join(terms, "\t");
  VLOG(3) << __func__ << " poss: " << join(poss, "\t");

  const signed int length = (terms.size() - 1);

  // 标识位
  signed int curr, pre2, pre1, post1, post2;

  vector<string>::const_iterator term = terms.begin();
  vector<string>::const_iterator pos  = poss.begin();

  /**
   * 按如下规则生成特征
   * http://www.chokkan.org/software/crfsuite/tutorial.html
   * http://www.chokkan.org/software/crfsuite/manual.html
   * w[t-2], w[t-1], w[t], w[t+1], w[t+2],
   * w[t-1]|w[t], w[t]|w[t+1],
   * pos[t-2], pos[t-1], pos[t], pos[t+1], pos[t+2],
   * pos[t-2]|pos[t-1], pos[t-1]|pos[t], pos[t]|pos[t+1], pos[t+1]|pos[t+2],
   * pos[t-2]|pos[t-1]|pos[t], pos[t-1]|pos[t]|pos[t+1], pos[t]|pos[t+1]|pos[t+2]
   */

  while(mv_feature_window(length, curr, pre2, pre1, post1, post2)) {
    // VLOG(3) << "[labelNerFeaturesDatas] length: " << length << "curr: " << curr
    //         << ", pre2: " << pre2 << ", pre1: " << pre1
    //         << ", post1: " << post1 << ", post2: " << post2;
    crfsuite::Item item;

    // feature extract 1: w[t-2]
    // feature extract 2: pos[t-2]
    // feature extract 3: pos[t-2]|pos[t-1]
    // feature extract 4: pos[t-2]|pos[t-1]|pos[t]
    if(pre2 >= 0) {
      crfsuite::Attribute attr;

      stringstream ss;
      ss << "w[t-2]=" << *(term + pre2);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t-2]=" << "@" << *(pos + pre2);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t-2]|pos[t-1]="
         << "@" << *(pos + pre2) << "|"
         << "@" << *(pos + pre1);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t-2]|pos[t-1]|pos[t]="
         << "@" << *(pos + pre2) << "|"
         << "@" << *(pos + pre1) << "|"
         << "@" << *(pos + curr);
      attr.set_attr(ss.str());
      item.push_back(attr);
    }

    // feature extract 5: w[t-1]
    // feature extract 6: pos[t-1]
    // feature extract 7: w[t-1]|w[t]
    // feature extract 8: pos[t-1]|pos[t]
    if(pre1 >= 0) {
      crfsuite::Attribute attr;

      stringstream ss;
      ss << "w[t-1]=" << *(term + pre1);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t-1]=" << "@" <<  *(pos + pre1);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "w[t-1]|w[t]=" << *(term + pre1) << "|" << *(term + curr);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t-1]|pos[t]="
         << "@" << *(pos + pre1) << "|"
         << "@" << *(pos + curr);
      attr.set_attr(ss.str());
      item.push_back(attr);
    }

    // feature extract 9: pos[t-1]|pos[t]|pos[t+1]
    if(pre1 >= 0 && post1 > 0) {
      crfsuite::Attribute attr;

      stringstream ss;
      ss << "pos[t-1]|pos[t]|pos[t+1]="
         << "@" << *(pos + pre1) << "|"
         << "@" << *(pos + curr) << "|"
         << "@" << *(pos + post1);
      attr.set_attr(ss.str());
      item.push_back(attr);
    }

    // feature extract 10: w[t]
    // feature extract 11: pos[t]
    if(curr >= 0) {
      crfsuite::Attribute attr;

      stringstream ss;
      ss << "w[t]=" << *(term + curr);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t]=" << "@" << *(pos + curr);
      attr.set_attr(ss.str());
      item.push_back(attr);
    }

    // feature extract 12: w[t+1]
    // feature extract 13: pos[t+1]
    // feature extract 14: w[t]|w[t+1]
    // feature extract 15: pos[t]|pos[t+1]
    if(post1 > 0) {
      crfsuite::Attribute attr;

      stringstream ss;
      ss << "w[t+1]=" << *(term + 1);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t+1]=" << "@" << *(pos + 1);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "w[t]|w[t+1]=" << *(term + curr) << "|" << *(term + post1);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t]|pos[t+1]="
         << "@" << *(pos + curr) << "|"
         << "@" << *(pos + post1);
      attr.set_attr(ss.str());
      item.push_back(attr);
    }

    // feature extract 16: w[t+2]
    // feature extract 17: pos[t+2]
    // feature extract 18: pos[t+1]|pos[t+2]
    // feature extract 19: pos[t]|pos[t+1]|pos[t+2]
    if(post2 > 0) {
      crfsuite::Attribute attr;

      stringstream ss;
      ss << "w[t+2]=" << *(term + post2);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t+2]=" << "@" << *(pos + post2);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t+1]|pos[t+2]="
         << "@" << *(pos + post1) << "|"
         << "@" << *(pos + post2);
      attr.set_attr(ss.str());
      item.push_back(attr);

      ss.str("");
      ss << "pos[t]|pos[t+1]|pos[t+2]="
         << "@" << *(pos + curr) << "|"
         << "@" << *(pos + post1) << "|"
         << "@" << *(pos + post2);
      attr.set_attr(ss.str());
      item.push_back(attr);
    }

    if(curr == 0) {
      crfsuite::Attribute attr("__BOS__");
      item.push_back(attr);
    }

    if( curr == length) {
      crfsuite::Attribute attr("__EOS__");
      item.push_back(attr);
    }

    xseq.push_back(item);
    curr++;
  };
}

/**
 * 从ner的返回结果中获得实体信息
 */
inline bool extract_slot_candidates_with_yseq(const vector<string>& terms,
    const vector<string>& yseq,
    vector<pair<string, string> >& candidates) {
  candidates.clear();

  // 不合法数据
  if(terms.size() == 0 || terms.size() != yseq.size()) {
    return false;
  }

  size_t length = (terms.size() - 1);
  size_t curr = 0;
  vector<string>::const_iterator tbegin = terms.begin();
  vector<string>::const_iterator ybegin = yseq.begin();

  while(curr <= length) {
    if(boost::starts_with(*(ybegin + curr), "B-@")) { // 分词词性标签
      curr++;
      continue;
    } else if(boost::starts_with(*(ybegin + curr), "B-")) { // 实体开始标签
      // 获得当前命名实体名称
      string slotName(*(ybegin + curr));
      boost::replace_first(slotName, "B-", "");

      stringstream ss;
      ss << *(tbegin + curr);
      size_t next = (++curr);

      while(next <= length && ( *(ybegin + next) == ("I-" + slotName))) {
        ss << *(tbegin + next);
        curr++;
        next++;
      }

      VLOG(3) << __func__ << " append " << slotName << " : " << ss.str();
      candidates.push_back(make_pair(slotName, ss.str()));
      continue;
    }

    // 其他情况比如 O 开始的标签直接忽略
    curr++;
  }
}

/**
 * if customized dicts contains word
 */
inline bool lookup_word_by_dictname_in_leveldb(const leveldb::DB& db,
    const string& dictname,
    const string& word) {
  VLOG(3) << __func__ << " dictname: " << dictname << ", word: " << word;

  stringstream ss;
  ss << dictname << '\001' << word;
  string start(ss.str());
  leveldb::Slice key = start;
  std::string value;
  leveldb::Status s = db.Get(leveldb::ReadOptions(), key, &value);

  if(s.ok()) {
    VLOG(3) << __func__ << " db " << dictname << "  contains word " << word;
    return true;
  } else {
    VLOG(3) << __func__ << " do " << dictname << " not contain word: " << word;
    return false;
  }
};


/**
 * 检索执行词典的命名实体
 * 当前指定匹配到词典名称，即便在实体词典中查找到其他选项也忽略
 */
inline bool extract_slotvalue_from_utterence_with_triedata(const tsl::htrie_map<char, set<string> >& dictwords,
    const string& query,
    const string& dictname,
    string& slotvalue) {
  VLOG(3) << __func__ << " query: " << query << ", find dictname " << dictname;

  // 直接匹配
  auto match = dictwords.longest_prefix(query);

  if(match != dictwords.end()) {
    string key = match.key();
    VLOG(3) << __func__ << " word " << match.key() << ", dictname "
            << *match;

    if((*match).find(dictname) != (*match).end() ) {
      slotvalue = key;
      return true;
    }
  }

  vector<string> chs;
  CharSegment(query, chs);

  size_t length = chs.size();

  if(length <= 1) {
    return false;
  }

  vector<string>::iterator cbegin = chs.begin();

  for(size_t curr = 1; curr < length; curr++) {
    stringstream ss;
    size_t incre = 0;

    while ((cbegin + curr + incre) != chs.end()) {
      ss << *(cbegin + curr + incre);
      incre++;
    }

    VLOG(3) << __func__ << " prefix string: " << ss.str();

    // 再次匹配
    match = dictwords.longest_prefix(ss.str());

    if(match != dictwords.end()) {
      string key = match.key();
      VLOG(3) << __func__ << " word " << match.key() << ", dictname "
              << *match;

      if((*match).find(dictname) !=  (*match).end()) {
        slotvalue = key;
        return true;
      }
    }
  }
}


/**
 * 根据intent定义和session信息确定是否被解决
 */
inline bool is_resolved_intent_by_session(intent::TChatSession& session,
    const intent::TIntent& intent,
    ChatMessage& reply) {
  for(const intent::TIntentSlot& slot : intent.slots()) {
    if(!slot.requires()) continue;

    bool settledown = false; // 作为必填的槽位，该槽位的值是否确定

    for(const intent::TChatSession::Entity& entity : session.entities()) {
      if(entity.name() == slot.name() &&
          (!entity.val().empty())) {
        settledown = true;
        break;
      }
    }

    if(!settledown) {
      // 设置session的追问信息
      session.set_resolved(false);
      session.set_is_proactive(true);
      session.set_is_fallback(false);
      session.set_proactive_slotname(slot.name());        // 停止追问
      session.set_proactive_dictname(slot.dictname());        // 停止追问
      session.set_proactive_question(slot.question());        // 停止追问

      // 设置 reply 为追问
      reply.textMessage = slot.question();
      reply.is_proactive = true;
      reply.is_fallback = false;
      reply.receiver = session.uid();
      reply.__isset.textMessage = true;
      reply.__isset.is_proactive = true;
      reply.__isset.is_fallback = true;
      reply.__isset.receiver = true;
      return false;
    }
  }

  return true;
};

/**
 * 获得意图后，将槽位信息添加到session
 */
bool Bot::setSessionEntitiesByIntentName(const string& intentName,
    intent::TChatSession& session) {
  bool result = false;
  session.clear_entities();

  for(const intent::TIntent& i : _profile->intents()) {
    if(i.name() == intentName) {
      for(const intent::TIntentSlot& slot : i.slots()) {
        intent::TChatSession::Entity* entity = session.add_entities();
        entity->set_name(slot.name());
        entity->set_requires(slot.requires());
        entity->set_dictname(slot.dictname());

        if(boost::starts_with(slot.dictname(), "@")) {
          entity->set_builtin(true);
        }
      }

      result = true;
      break;
    }
  }

  return result;
  VLOG(3) << __func__ << " after appending entities in session \n" << FromProtobufToUtf8DebugString(session);
};

/**
 * 在session中，通过entity的name设置value
 */
inline bool setSessionSlotValueByName(const string& slotName,
                                      const string& entityValue,
                                      intent::TChatSession& session) {
  bool result = false;

  for(intent::TChatSession::Entity& entity : *session.mutable_entities()) {
    if(entity.name() == slotName) {
      entity.set_val(entityValue);
      result = true;
      break;
    }
  }

  return true;
};

std::vector<pair<string, intent::TDict> >* Bot::getPatternDicts() const {
  return _pattern_dicts;
}

/**
 * 获得引用的系统词典列表
 */
std::vector<string> Bot::getReferredSysdicts() {
  return *_referred_sysdicts;
};

/**
 * 是否引用了系统词典
 */
bool Bot::hasReferredSysdict(const string& dictname) {
  vector<string>::iterator it = find(_referred_sysdicts->begin(), _referred_sysdicts->end(), dictname);
  return it != _referred_sysdicts->end();
};

/**
 * 是否使用了正则表达式词典
 */
bool Bot::hasRelatedPatternDict(const string& dictname, const string& intentName) {
  VLOG(3) << __func__ << " intentName: " << intentName  << ", dictname: " << dictname;

  if(intentName.empty()) {
    // 还没有确定意图
    for(const intent::TIntent& i : _profile->intents()) {
      for(const intent::TIntentSlot& slot : i.slots()) {
        if(slot.dictname() == dictname) {
          return true;
        }
      }
    }
  } else {
    // 已经确定了意图
    for(const intent::TIntent& i : _profile->intents()) {
      if(i.name() == intentName) {
        for(const intent::TIntentSlot& slot : i.slots()) {
          if(slot.dictname() == dictname) {
            return true;
          }
        }

        break;
      }
    }
  }

  return false;
}

/**
 * 请求系统词典前增加被引用的列表信息
 */
bool Bot::patchSysdictsRequestEntities(sysdicts::Data& request) {
  for(vector<string>::iterator it = _referred_sysdicts->begin(); it != _referred_sysdicts->end(); it++) {
    sysdicts::Entity entity;
    entity.dictname = *it;
    entity.__isset.dictname = true;
    request.entities.push_back(entity);
    request.__isset.entities = true;
  }
};

/**
 * 实体的调试信息
 */
inline string debugstr_for_entities_candidates(const vector<pair<string, string> >& candidates) {
  stringstream ss;

  for(vector<pair<string, string> >::const_iterator it = candidates.begin(); it != candidates.end(); it++ ) {
    ss << '\t' << it->first << '\t' << it->second << '\n';
  }

  return ss.str();
}

/**
 * 通过NER和POS信息确定槽位值
 */
inline bool get_slotvalue_by_ner_and_pos(const ChatMessage& payload,
    const string& nerValue,
    string& slotvalue) {

  if(payload.terms.size() != payload.tags.size())
    return false;

  size_t len = payload.terms.size();
  vector<string>::const_iterator tbegin = payload.terms.begin();
  vector<string>::const_iterator pbegin = payload.tags.begin();

  for(size_t i = 0; i < len ; i++) {
    if(nerValue == (*(tbegin + i)) && (boost::starts_with((*(pbegin + i)), "n") ||
                                       boost::starts_with((*(pbegin + i)), "eng"))) {
      slotvalue = nerValue;
      return true;
    }
  }

  return false;
}

/**
 * 检查指定的字典名是否属于正则表达式词典
 */
inline bool is_dictname_belongto_patterndicts(const vector<pair<string, intent::TDict> >& pattern_dicts, const string& dictname) {
  for(vector<pair<string, intent::TDict> >::const_iterator it = pattern_dicts.begin(); it !=  pattern_dicts.end(); it++) {
    if(it->first == dictname) {
      return true;
    }
  }

  return false;
}



/**
 * 对话接口
 * @return bool 成功设置textMessage或resolve情况下，返回true
 */
bool Bot::chat(const ChatMessage& payload,
               const string& query,
               const vector<sysdicts::Entity>& builtins,
               const std::vector<PatternDictMatch>& patternDictMatches,
               intent::TChatSession& session,
               ChatMessage& reply) {
  VLOG(3) << __func__ << " query: " << query;
  intent::TIntent* intent;

  for(const intent::TIntent& i : _profile->intents()) {
    if(i.name() == session.intent_name()) {
      VLOG(3) << __func__ << " get intent from profile: \n" << FromProtobufToUtf8DebugString(i);
      intent = &i;
      break;
    }
  }

  if(intent != 0) {
    VLOG(3) << __func__ << " query: " << payload.textMessage << ", rewrite query: " << query << "\nintent: \n" << FromProtobufToUtf8DebugString(*intent);

    /**
     * Step 1: 处理槽位
     */
    if(session.is_proactive()) { // 上一轮是否是追问
      // 确定该槽位还不包含在session.entity中
      bool settledown = false;

      // 在追问的情况下，首先从trie中查找
      if(session.proactive_slotname().empty()) {
        // 追问条件不合法
        VLOG(3) << __func__ << " Internal Error. Can not find proactive_slotname in session \n" << FromProtobufToUtf8DebugString(session);
        return false;
      } else {
        string slotvalue;
        // TODO 已经赋过的值过滤
        // 此处有不严谨的地方，因为后面对未得到值的命名实体遍历时跳过这些值
        // 但是偶然情况下，两个命名实体的值可能是一样的
        // 最坏的情况是做多次的追问，体验比较勉强
        std::set<string> bypassValues;

        if(session.proactive_dictname() == CL_SYSDICT_DICT_ANY) {
          // @ANY 匹配任何字符串
          slotvalue = payload.textMessage;
          settledown = true;
        } else if(boost::starts_with(session.proactive_dictname(), "@")) { // 处理系统词典
          for(const sysdicts::Entity& se : builtins) {
            if(se.dictname == session.proactive_dictname()) {
              // 查找到
              settledown = true;
              slotvalue = se.val;
              VLOG(3) << __func__ << " proactive_slotname " << session.proactive_slotname() << " : " << slotvalue;
              bypassValues.insert(se.val);
              break;
            }
          }
        } else if(is_dictname_belongto_patterndicts(*_pattern_dicts, session.proactive_dictname())) {
          // 该词典属于正则表达式词典
          VLOG(3) << __func__ << " proactive_dictname belongto patterndicts: " <<  session.proactive_dictname();

          for(const PatternDictMatch& pdm : patternDictMatches) {
            if(session.proactive_dictname() == pdm.dictname ) {
              slotvalue = pdm.val;
              settledown = true;
              bypassValues.insert(pdm.val);
              break;
            }
          }
        } else if(extract_slotvalue_from_utterence_with_triedata(*_dictwords_triedb,
                  payload.textMessage,
                  session.proactive_dictname(),
                  slotvalue)) {
          // 从用户词表词典中查找到命名实体值
          VLOG(3) << __func__ << " proactive_slotname " << session.proactive_slotname() << " : " << slotvalue;
          settledown = true;
          bypassValues.insert(slotvalue);
        }

        if(settledown) {
          // set session entity
          setSessionSlotValueByName(session.proactive_slotname(), slotvalue, session);
        }

        /**
         * 查找其它槽位信息
         */
        // 在本次对话中，用户也说的了其他的槽位的值，在NER中能识别出来
        // 但是这也加入了判断错误的风险，所以忽略对反问时，其他槽位的识别
        // 查找还没有确定值的命名实体
        if(session.proactive_dictname() != CL_SYSDICT_DICT_ANY) {
          for(const intent::TChatSession::Entity& ie : session.entities()) {
            string extras_slotvalue;

            if(ie.val().empty()) {
              // 该槽位值还没有确定
              if(boost::starts_with(ie.dictname(), "@")) {
                // 系统词典
                for(const sysdicts::Entity& se : builtins) {
                  if((se.dictname == ie.dictname()) && (bypassValues.find(se.val) == bypassValues.end())) {
                    // 查找到
                    VLOG(3) << __func__ << " slotname " << ie.name() << " : " << se.val;
                    setSessionSlotValueByName(ie.name(), se.val, session);
                    bypassValues.insert(se.val);
                    break;
                  }
                }
              } else if(is_dictname_belongto_patterndicts(*_pattern_dicts, ie.dictname())) {
                // 正则表达式词典
                for(const PatternDictMatch& pdm : patternDictMatches) {
                  if((ie.dictname() == pdm.dictname) && (bypassValues.find(pdm.val) == bypassValues.end())) {
                    // 查找到
                    VLOG(3) << __func__ << " slotname " << ie.name() << " : " << pdm.val;
                    setSessionSlotValueByName(ie.name(), pdm.val, session);
                    bypassValues.insert(pdm.val);
                    break;
                  }
                }
              } else if(extract_slotvalue_from_utterence_with_triedata(*_dictwords_triedb,
                        payload.textMessage,
                        ie.dictname(),
                        extras_slotvalue)) {
                // 查询用户词表词典
                if(bypassValues.find(extras_slotvalue) == bypassValues.end()) {
                  setSessionSlotValueByName(ie.name(), extras_slotvalue, session);
                  bypassValues.insert(extras_slotvalue);
                  VLOG(3) << __func__ << " slotname " << ie.name() << " : " << extras_slotvalue;
                }
              }
            }
          }
        }

        // 未查找到，继续追问, session信息不变
        if(settledown) {
          session.set_is_proactive(false);           // 停止追问
          session.clear_proactive_slotname();        // 停止追问
          session.clear_proactive_dictname();        // 停止追问
          session.clear_proactive_question();        // 停止追问
        } else {
          VLOG(3) << __func__ << " not found value for proactive slot " <<  session.proactive_slotname();
          reply.textMessage = session.proactive_question();
          reply.is_proactive = true;
          reply.is_fallback = false;
          reply.receiver = session.uid();
          reply.__isset.textMessage = true;
          reply.__isset.is_proactive = true;
          reply.__isset.is_fallback = true;
          reply.__isset.receiver = true;
          return true;
        }
      }
    } else { // 不是追问，是在意图识别中带有槽位信息
      /**
       * 进行NER识别
       * 未识别到的槽位并且为必填项: 设置回复为追问。
       */
      crfsuite::ItemSequence xseq;
      setupNerItemSequence(payload.terms, payload.tags, xseq);
      VLOG(3) << __func__ << " labeling entities with ner model ...";
      vector<string> labels = _tagger->tag(xseq);

      VLOG(3) << __func__ << " labels: " << join(labels, "\t");
      VLOG(3) << __func__ << " tokens: " << join(payload.terms, "\t");

      vector<pair<string, string> > candidates; // candidates for entities.
      extract_slot_candidates_with_yseq(payload.terms, labels, candidates);
      VLOG(3) << __func__ << " entities candidates: " << debugstr_for_entities_candidates(candidates);

      // 系统词典
      vector<sysdicts::Entity>::const_iterator ise = builtins.begin();
      bool hasSysdicts = (builtins.size() > 0);

      for(vector<pair<string, string> >::iterator it = candidates.begin(); it != candidates.end(); it++) {
        bool settledown = false; // 是否解决了一个槽位的值
        string slotvalue;
        intent::TChatSession::Entity* entity = NULL;

        // 检查该槽位是否有值
        for(const intent::TChatSession::Entity& ie : session.entities()) {
          if(ie.name() == it->first) {
            if(!ie.val().empty()) { // 该槽位值已经确定
              settledown = true;
              break;
            } else {
              entity = &ie;
              break;
            }
          }
        }

        if(settledown)
          continue; // 处理下一个槽位候选

        VLOG(3) << __func__ << " analysis slotname: " << it->first << ", value: " << it->second << ", hasSysdicts: " << hasSysdicts;

        // 该槽位没有值并且查找到关联的实体
        if((!settledown) && (entity != NULL)) {
          if(entity->builtin()) { // 是系统词典

            if(hasSysdicts) {
              VLOG(3) << __func__ << " check against sysdicts with: " << FromThriftToUtf8DebugString(&(*ise));

              if(ise->dictname == it->second) {
                VLOG(3) << __func__ << " resolve slot: " << it->first << " as value: " << ise->val << " successfully.";
                slotvalue = ise->val;
                settledown = true;
                ise++;

                if(ise == builtins.end())
                  hasSysdicts = false;
              }
            } else if(!boost::starts_with(it->second, "@")) { // entity是系统词典，但是NER分析出不是系统词典
              // 当前系统词典分析器没有分析出来相应值
              // 或者系统词典已经遍历完全，这时利用NER的值
              // 查找terms，获得NER值的词性
              if(get_slotvalue_by_ner_and_pos(payload, it->second, slotvalue)) {
                settledown = true;
              } else {
                // 从词性标注数据和NER中没有确定槽位值
              }
            }
          } else { // 自定义词典
            VLOG(3) << __func__ << " check against customdicts";

            if(boost::starts_with(it->second, "#")) {
              // 属于正则表达式词典
              for(const PatternDictMatch& pdm : patternDictMatches) {
                if(it->second == ("#" + pdm.dictname)) {
                  settledown = true;
                  slotvalue = pdm.val;
                  break;
                }
              }
            } else if(lookup_word_by_dictname_in_leveldb(*_dictwords_leveldb, entity->dictname(), it->second)) {
              // 基于词表的词典
              VLOG(3) << __func__ << " resolve slot: " << it->first << " as value: " << it->second << " successfully.";
              settledown = true;
              slotvalue = it->second;
            } else {
              // NER识别到的自定义词典的值并不在自定义词典中，此处可以看成是机器学习到的新词
              // TODO 视为不准确，放弃该新词，此处可以进一步校验，识别该词的置信度，从而提升智能水平
              //   continue; // 处理下一个槽位候选
              VLOG(2) << __func__ << " probably new word learns by machine, slotname: \t" << it->first << "\t" << it->second;

              // 查找terms，获得NER值的词性
              if(get_slotvalue_by_ner_and_pos(payload, it->second, slotvalue)) {
                settledown = true;
              }
            }
          }
        }

        // 确定该槽位候选
        if(settledown) {
          setSessionSlotValueByName(it->first, slotvalue, session);
        } else {
          // TODO 没有确定该槽位候选
          VLOG(3) << __func__ << " discard entity candidate, slotname: " << it->first << ", value " << it->second;
        }
      }
    }

    /**
     * Step 2: 处理回复
     */
    // check session is resolve
    if(is_resolved_intent_by_session(session, *intent, reply)) {
      session.set_resolved(true);
      session.set_is_proactive(false);
      session.set_is_fallback(false);
      session.clear_proactive_slotname();        // 停止追问
      session.clear_proactive_dictname();        // 停止追问
      session.clear_proactive_question();        // 停止追问
    }
  } else {
    // can find matched intent
    // should not happen
    VLOG(3) << __func__ << " can find matched intent";
    return false;
  }

  return true;
};

} // namespace clause
} // namespace bot
} // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
