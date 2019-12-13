/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/intent/src/samples.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-17_10:16:58
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_INTENT_SAMPLES
#define __CHATOPERA_BOT_INTENT_SAMPLES

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "basictypes.h"
#include "StringUtils.hpp"
#include "VectorUtils.hpp"
#include "MathUtils.hpp"
#include "intent.pb.h"
#include "cppjieba/Jieba.hpp"

using namespace boost::algorithm;
using namespace chatopera::utils;

namespace chatopera {
namespace bot {
namespace intent {

class SampleGenerator {

 private: // constructors
  SampleGenerator() {};

 public: // function
  static void generateTemplates(const cppjieba::Jieba& tokenizer,
                                const ::google::protobuf::RepeatedPtrField< ::chatopera::bot::intent::TIntent >& intents,
                                const std::map<std::string, std::vector<string> >& predefined_dicts,
                                const std::map<std::string, std::vector<string> >& vocab_dicts,
                                const std::map<std::string, TDictPattern>& pattern_dicts,
                                Augmented& augmented);

  static bool generateCrfSuiteTraingData(const Augmented& augmented,
                                         const string& filepath);
 private: // variables

};

}; // namespace intent
}; // namespace bot
}; // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
