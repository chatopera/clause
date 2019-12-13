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

#ifndef __CHATOPERA_BOT_INTENT_TRAINER
#define __CHATOPERA_BOT_INTENT_TRAINER

#include <map>
#include <string>
#include <vector>
#include <set>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/string_file.hpp>
#include <google/protobuf/util/json_util.h>

#include "cppjieba/Jieba.hpp"
#include "tsl/htrie_map.h"
#include "xapian.h"
#include "glog/logging.h"
#include "intent.pb.h"
#include "basictypes.h"
#include "marcos.h"
#include "samples.h"
#include "publisher.h"

DECLARE_string(workarea);
DECLARE_string(data);

using namespace std;
using namespace boost::algorithm;
namespace fs = boost::filesystem;

namespace chatopera {
namespace bot {
namespace intent {

class BrokerPublisher;

class Trainer {
 public: // constructors
  Trainer();
  ~Trainer();

 public: // functions
  bool init();
  void train(const string payload);

 private: // variables
  const string currentpath;
  const string tokenizer_dict_default; // tokenizer dict data template
  std::map<std::string, cppjieba::Jieba* >* tokenizers;
  BrokerPublisher* publisher;
  map<string, vector<string> > PREDEFINED_DICTS; // 系统词典
};

} // namespace intent
} // namespace bot
} // namespace chatopera

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */