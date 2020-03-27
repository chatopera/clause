/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-05-22_14:22:10
 * @brief
 *
 **/
#include "trainer.h"
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <climits>
#include <stdlib.h>
#include <iostream>

#include "StringUtils.hpp"
#include "FileUtils.hpp"
#include "crfsuite.hpp"
#include "tsl/serialize.hpp"
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

using namespace std::chrono;
using namespace chatopera::utils;
namespace fs = boost::filesystem;

namespace chatopera {
namespace bot {
namespace intent {

Trainer::Trainer() {
  currentpath = fs::current_path().string();
  tokenizer_dict_default = FLAGS_data + "/jieba/default";
  tokenizers = new map<std::string, cppjieba::Jieba* >();

  /**
   * 初始化系统词典信息
   */
  vector<string> dict;
  PREDEFINED_DICTS.clear();
  // 时间
  dict.push_back("@TIME");
  PREDEFINED_DICTS["@TIME"] = dict;
  // 组织机构
  dict.clear();
  dict.push_back("@ORG");
  PREDEFINED_DICTS["@ORG"] = dict;
  // 地点
  dict.clear();
  dict.push_back("@LOC");
  PREDEFINED_DICTS["@LOC"] = dict;
  // 人名
  dict.clear();
  dict.push_back("@PER");
  PREDEFINED_DICTS["@PER"] = dict;
  // 任意字符串
  dict.clear();
  dict.push_back("@ANY");
  PREDEFINED_DICTS["@ANY"] = dict;
};

Trainer::~Trainer() {
  delete tokenizers;
};

bool Trainer::init() {
  VLOG(3) << __func__ << " application current_path: " << currentpath;

  if(!fs::exists(FLAGS_workarea)) {
    VLOG(2) << __func__ << " workarea [" << FLAGS_workarea << "] not exist.";
    return false;
  }

  if(!fs::exists(FLAGS_data)) {
    VLOG(2) << __func__ << " data [" << FLAGS_data << "] not exist.";
    return false;
  }

  if(!fs::exists(FLAGS_data + "/jieba/default")) {
    VLOG(2) << __func__ << " data [" <<  FLAGS_data + "/jieba/default" << "] not exist.";
    return false;
  }

  // 获得事件发布实例
  publisher = BrokerPublisher::getInstance();

  return true;
};

/**
 * 生成新的版本号
 * 根据版本号查看版本创建日期：
 * 1) 在线进制转化
 * https://tool.oschina.net/hexconvert
 * 十六进制转十进制，该十进制就是timestamp
 * 2) 查看日期
 * https://www.epochconverter.com/
 * 十进制转化为Date
 */
inline std::string generateNewVersionNumber() {
  milliseconds ms = duration_cast< milliseconds >(
                      system_clock::now().time_since_epoch()
                    );
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  std::stringstream stream;
  stream << std::hex << ms.count();
  return stream.str();
}

/**
 * 在文件夹不存在的情况下进行创建
 */
inline bool create_on_notexist(const std::string& folder) {
  VLOG(3) << __func__ << " folder: " << folder;

  if(!fs::exists(folder)) {
    if(!boost::filesystem::create_directory(folder)) {
      VLOG(2) << __func__ << " error: can not create " << folder;
      return false;
    }
  }

  return true;
};

/**
 * 创建系统
 */
inline bool create_version_folder(const string& botdir, const string& versiondir) {
  if(create_on_notexist(botdir)) {
    if(!create_on_notexist(versiondir)) {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

/**
 * 验证数据完整
 */
inline bool validate_profile(const Profile& profile) {
  bool result = true;

  if(profile.chatbotid().empty()) {
    VLOG(3) << __func__ << " invalid profile: chatbotID not exist.";
    return false;
  }

  return result;
}

/**
 * 输出正则表达式词表到分词器
 */
inline void writeCustomizedPatternDictWords(const string& customdictfile, const string& dictname) {
  VLOG(3) << __func__ << " customdictfile: " << customdictfile << ", term: " << dictname;
  // open file
  ofstream f(customdictfile, ios::app);

  if(!f.is_open()) {
    VLOG(2) << __func__ << " warn: can not find lex-customized.lex";
    return;
  }

  stringstream ss;
  ss << CL_DICT_PATTERN_PREFIX << dictname; // 正则表达式词表使用 # 作为前缀
  ss << " 50000 gregex"; // 词性gregex代表正则表达式词典词汇
  // 输出到词典
  VLOG(4) << __func__ << " word record: " << ss.str();
  f << ss.str() << endl;
  f.close();
}

/**
 * 输出词表数据到分词器
 */
inline void writePredefinedDictWords(const string& customdictfile,
                                     const map<string, vector<string> >& predefined_dicts) {
  VLOG(3) << __func__ << " customdictfile: " << customdictfile;

  // open file
  ofstream f(customdictfile, ios::app);

  if(!f.is_open()) {
    VLOG(2) << __func__ << " warn: can not find lex-customized.lex";
    return;
  }

  stringstream ss;

  for(map<string, vector<string> >::const_iterator it = predefined_dicts.begin(); it != predefined_dicts.end(); ++it) {
    // v.push_back(it->first);
    VLOG(4) << __func__ << " insert: word " << it->first;
    ss.str("");
    ss << it->first;
    ss << " 50000 gb"; // 词性gb代表Chatopera中的自定义词汇, b stands for builtin
    // 输出到词典
    f << ss.str() << endl;
  }

  f.close();
}


/**
 * 输出词表数据到分词器
 */
inline void writeCustomizedVocabDictWords(const string& customdictfile,
    const ::google::protobuf::RepeatedPtrField< ::chatopera::bot::intent::TDictWord >& dictwords,
    const std::vector<std::string>& vocab) {
  VLOG(3) << __func__ << " customdictfile: " << customdictfile;

  // open file
  ofstream f(customdictfile, ios::app);

  if(!f.is_open()) {
    VLOG(2) << __func__ << " warn: can not find lex-customized.lex";
    return;
  }

  stringstream ss;

  for(const TDictWord& word : dictwords) {
    VLOG(4) << __func__ << " insert: word " << word.word() << ", synonyms: " << word.synonyms();
    ss.str("");
    ss << word.word();
    ss << " 50000 g"; // 词性g代表Chatopera中的自定义词汇
    // 输出到词典
    VLOG(4) << __func__ << " word record: " << ss.str();
    f << ss.str() << endl;
    vocab.push_back(word.word());

    if(!word.synonyms().empty()) {
      std::vector<std::string > aps;
      split(aps, word.synonyms(), boost::is_any_of(";"));
      VLOG(4) << __func__ << " Get synonyms size: " << aps.size();

      for(std::vector<std::string >::iterator it = aps.begin(); it != aps.end(); it++) {
        trim(*it);

        if(!it->empty()) {
          ss.str("");
          ss << *it;
          ss << " 50000 g";
          // 输出到词典
          VLOG(4) << __func__ << " word record: " << ss.str();
          f << ss.str() << endl;
          vocab.push_back(*it);
        }
      }
    }
  }

  f.close();
}

//  释放旧分词器
inline void free_tokenizer_by_chatbotID(std::map<std::string, cppjieba::Jieba* >& tokenizers,
                                        const std::string& chatbotID) {
  VLOG(3) << __func__ << " chatbotID " << chatbotID;
  map<std::string, cppjieba::Jieba* >::const_iterator find_tokenizer = tokenizers.find(chatbotID);

  if(find_tokenizer != tokenizers.end()) {
    VLOG(3) << __func__ << " free tokenizer";
    delete tokenizers[chatbotID];
    tokenizers.erase(chatbotID);
  }
}

/**
 * 训练对话模型
 */
void Trainer::train(const string payload) {
  Profile profile;
  profile.ParseFromString(payload);
  VLOG(3) << "train: profile \n" << FromProtobufToUtf8DebugString(profile);

  // #TODO Validate data
  if(!validate_profile(profile)) {
    // 数据不合法
    // #TODO 返回错误信息
    return;
  }

  /**
   * new version file(s) info
   */
  const std::string chatbotID = profile.chatbotid();
  const std::string ver = generateNewVersionNumber();
  const std::string botdir(FLAGS_workarea + "/" + chatbotID);
  const std::string versiondir(botdir + "/" + ver);
  const std::string indexdir(versiondir + "/xapian");
  const std::string dictfile(versiondir + "/dictwords.trie.bin");
  const std::string dictdb(versiondir + "/leveldb");
  const std::string dictdir(versiondir + "/jieba");
  const std::string customdictfile(dictdir + "/user.dict.utf8");
  const std::string nertrainfile(versiondir + "/crfsuite.train.txt");
  const std::string nermodelfile(versiondir + "/crfsuite.ner.model");
  const fs::path profileJsonFile(versiondir + "/profile.json");
  const fs::path profileStringifyFile(versiondir + "/profile.pbs");
  const fs::path augmentedfile(versiondir + "/augmented.json");
  VLOG(3) << __func__ << " new version " << ver;

  if(!create_version_folder(botdir, versiondir)) {
    // 创建文件夹失败
    // #TODO 返回错误信息
    return;
  };

  // 创建词表文件
  // #TODO 拷贝失败发生error导致服务crash，需要优化
  copyDirectoryRecursively(tokenizer_dict_default, dictdir);

  // 输出预置词典到词表
  writePredefinedDictWords(customdictfile, PREDEFINED_DICTS);

  // 基于词汇表的词典，dictid and its words
  const std::map<std::string, std::vector<std::string> > vocab_dicts;

  // 基于正则表达式的词典
  const std::map<std::string, TDictPattern> pattern_dicts;

  for(const TDict& dict : profile.dicts()) {
    VLOG(3) << "profile dict: " << dict.name();

    // 自定义词典: 基于词表类型词汇数量大于0
    if((!dict.builtin())) {
      if((dict.type() == CL_DICT_TYPE_VOCAB) && dict.dictwords_size() > 0) {
        VLOG(3) << "vocab dict: " << dict.name() << ", dict size: " << dict.dictwords_size();
        std::vector<std::string> words;
        writeCustomizedVocabDictWords(customdictfile, dict.dictwords(), words);

        if(words.size() > 0) {
          vocab_dicts[dict.name()] = words;
        }
      } else if ((dict.type() == CL_DICT_TYPE_PATTERN) && dict.has_dictpattern()) {
        VLOG(3) << "regex dict: " << dict.name() << ", dict patterns size: " << dict.dictpattern().patterns_size();
        writeCustomizedPatternDictWords(customdictfile, dict.name());
        pattern_dicts[dict.name()] = dict.dictpattern();
      }
    }
  }

  // 创建分词器
  VLOG(3) << __func__ << " start to init tokenizer ...";
  cppjieba::Jieba* tokenizer = new cppjieba::Jieba( dictdir + "/jieba.dict.utf8",
      dictdir + "/hmm_model.utf8",
      customdictfile,
      dictdir + "/idf.utf8",
      dictdir + "/stop_words.utf8");
  VLOG(3) << __func__ << " init tokenizer done.";

  // 保存到tokenizers
  free_tokenizer_by_chatbotID(*tokenizers, chatbotID);

  // 保存新分词器
  (*tokenizers)[profile.chatbotid()] = tokenizer;

  // 生成Samples
  Augmented augmented;
  SampleGenerator::generateTemplates(*tokenizer,
                                     profile.intents(),
                                     PREDEFINED_DICTS,
                                     vocab_dicts,
                                     pattern_dicts,
                                     augmented);
  VLOG(3) << __func__ << "done with generateTemplates";

  // NER
  if(SampleGenerator::generateCrfSuiteTraingData(augmented, nertrainfile)) {
    chatopera::bot::crfsuite::Trainer ner;
    // init
    ner.select("lbfgs", "crf1d");
    ner.init();
    // hyper params
    ner.set("feature.minfreq", "0.000000");
    ner.set("feature.possible_states", "0");
    ner.set("feature.possible_transitions", "0");
    ner.set("c1", "0.000000");
    ner.set("c2", "1.000000");
    ner.set("max_iterations", "2147483647");
    ner.set("num_memories", "6");
    ner.set("epsilon", "0.000010");
    ner.set("period", "10");
    ner.set("delta", "0.000010");
    ner.set("linesearch", "MoreThuente");
    ner.set("max_linesearch", "20");
    // input training samples
    ner.read_data(nertrainfile);
    // training
    ner.train(nermodelfile, -1);
    VLOG(3) << __func__ << " crfsuite model is generated at " << nermodelfile;
  } else {
    // TODO 返回报错信息
    // 训练失败
  }

  /**
   * 分类
   */
  if(create_on_notexist(indexdir)) {
    // Open the database for update, creating a new database if necessary.
    Xapian::WritableDatabase db(indexdir, Xapian::DB_CREATE_OR_OPEN);

    Xapian::TermGenerator indexer;
    Xapian::Stem stemmer("english");
    indexer.set_stemmer(stemmer);
    indexer.set_stemming_strategy(indexer.STEM_SOME_FULL_POS);

    // 添加posts
    for(const Augmented::IntentTrainingSample& samples : augmented.itss()) {
      for(const Augmented::Sample& sample : samples.tss()) {
        VLOG(3) << __func__ << " index utterance: " << sample.utterance();

        Xapian::Document doc;

        // add terms as posting to doc
        Xapian::termpos term_position(0);

        for(const string& term : sample.terms()) {
          // TODO add boosting as term weight
          doc.add_posting(term, ++term_position, 1.0 /*term weight*/);
        }

        // set docId
        string idterm = "Q" + generate_uuid();
        doc.add_boolean_term(idterm);

        string data;
        // keep more data for debugging
        // sample.clear_terms();
        // sample.clear_labels();
        // sample.clear_poss();
        sample.SerializeToString(&data);
        doc.set_data(data);

        // create or update
        db.replace_document(idterm, doc);
      }
    }

    db.commit();
    db.close();
    VLOG(3) << __func__ << " generate index done in " << indexdir;
  } else {
    VLOG(2) << __func__ << " fail to create index dir.";
    return;
  }

  /**
   * dump dictwords to HAT-Trie data
   * dump dictwords to LevelDB
   * trie: 用于前缀查找
   * leveldb: 用于验证一个词是否属于某词典
   */
  const tsl::htrie_map<char, set<string> > dictwords;

  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = false;
  leveldb::Status status = leveldb::DB::Open(options, dictdb, &db);
  CHECK(status.ok()) << "Can not create leveldb at " << dictdb;
  leveldb::WriteBatch batch;

  for(const TDict& tdict : profile.dicts()) {
    // 只检查词表词典
    if(tdict.type() != CL_DICT_TYPE_VOCAB)
      continue;

    for(const TDictWord& dictword : tdict.dictwords()) {
      if(!dictword.word().empty()) {
        dictwords[dictword.word().c_str()].insert(tdict.name());
        VLOG(3) << __func__ << " dictwords dump: " << dictword.word() << " | " << dictwords[dictword.word().c_str()];

        stringstream ss;
        ss << tdict.name() << '\001' << dictword.word();
        batch.Put(ss.str(), dictword.word());
      }

      if(!dictword.synonyms().empty()) {
        vector<string> words;
        split(words, dictword.synonyms(), boost::is_any_of(";"));

        for(const string& word : words) {
          if(!word.empty()) {
            dictwords[word.c_str()].insert(tdict.name());
            VLOG(3) << __func__ << " dictwords dump: " << word << " | " << dictwords[word.c_str()];

            stringstream ss;
            ss << tdict.name() << '\001' << word;
            batch.Put(ss.str(), dictword.word());
          }
        }
      }
    }
  }

  // write hat-trie db
  {
    std::ofstream ofs;
    ofs.exceptions(ofs.badbit | ofs.failbit);
    VLOG(3) << __func__ << " dump dict words to hat-trie: " << dictfile;
    ofs.open(dictfile, std::ios::binary);

    boost::iostreams::filtering_ostream fo;
    fo.push(boost::iostreams::zlib_compressor());
    fo.push(ofs);

    boost::archive::binary_oarchive oa(fo);

    oa << dictwords;
  }

  // write leveldb
  leveldb::WriteOptions write_options;
  write_options.sync = true; // async is much 10x faster then sync, but for safe reason, use sync.
  status = db->Write(write_options, &batch);
  CHECK(status.ok()) << "fail to dump leveldb data";
  VLOG(3) << __func__ << " dump dict words to leveldb: " << dictdb;
  delete db; // close db

  /**
   * dump augmented data into JSON File for debugging purpose
   */
  string augmentedJSON;
  google::protobuf::util::MessageToJsonString(augmented, &augmentedJSON);
  fs::save_string_file(augmentedfile, augmentedJSON);

  // dump profile to JSON File for debugging purpose.
  string profileJSON;
  google::protobuf::util::MessageToJsonString(profile, &profileJSON);
  VLOG(3) << __func__ << " profile json: \n" << profileJSON;
  fs::save_string_file(profileJsonFile, profileJSON);

  // dump profile to String File, 10x faster then JSON format.
  string profileStringify;
  profile.SerializeToString(&profileStringify);
  fs::save_string_file(profileStringifyFile, profileStringify);

  // 创建测试分支的软连接
  fs::path devsymlink(botdir + "/develop");

  if(fs::exists(devsymlink)) {
    fs::remove(devsymlink);
  }

  fs::create_symlink(ver, devsymlink);

  // 将训练结果发送回 activemq
  Profile response;
  response.mutable_devver()->set_chatbotid(chatbotID);
  response.mutable_devver()->set_version(ver);
  response.mutable_devver()->set_published(true);

  string serialized;
  response.SerializeToString(&serialized);

  publisher->publish(chatbotID, "train/finish", serialized);
  VLOG(3) << __func__ << " chatbotID " << chatbotID << " built version " << ver << " is done.";
};

} // namespace intent
} // namespace bot
} // namespace chatopera