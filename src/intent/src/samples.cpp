/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/intent/src/samples.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-17_10:16:47
 * @brief
 *
 **/


#include "samples.h"

#include "glog/logging.h"

using namespace std;

namespace chatopera  {
namespace bot {
namespace intent {

extern const set<string> CL_LEGAL_ID_CHAR_SET = {
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
  "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
  "W", "X", "Y", "Z",
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
  "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v",
  "w", "x", "y", "z",
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "_"
};

/**
 * 判断是否是合法的ID字符
 * [a-zA-Z0-9_]
 */
inline void isClLegalIdChar(const string& ch) {
  return CL_LEGAL_ID_CHAR_SET.find(ch) != CL_LEGAL_ID_CHAR_SET.end();
}

/**
 * 根据槽位名称返回词典
 */
inline bool getDictnameBySlotname(const ::google::protobuf::RepeatedPtrField< ::chatopera::bot::intent::TIntentSlot >& slots,
                                  const string& slotname,
                                  string& dictname) {

  for(const TIntentSlot& slot : slots) {
    VLOG(4) << __func__ << " " << slot.name();

    if(slot.name() == slotname) {
      dictname = slot.dictname();
      return true;
    }
  }

  return false;
}

/**
 * Parse tokens
 */
inline void parseTemplateTokens(const TIntentUtter& utter,
                                const ::google::protobuf::RepeatedPtrField< ::chatopera::bot::intent::TIntentSlot >& slots,
                                SampleTemplate* tpl) {
  VLOG(3) << __func__ << " utter: " << utter.utterance();

  tpl->set_utterance(utter.utterance());
  tpl->set_hasslot(false);

  std::vector<std::string> ch;
  CharSegment(utter.utterance(), ch);

  size_t cursor = 0;
  size_t processed = 0;
  size_t start = 0;
  size_t end = 0;
  stringstream ss;
  bool inslot = false;

  for(vector<string>::iterator it = ch.begin(); it != ch.end(); it++) {
    VLOG(3) << __func__ << " " << *it << ", ischinese: "
            << isChinese(*it) << ", cursor: " << cursor
            << ", processed: " << processed;

    const bool isch = isChinese(*it);

    if(isch) {
      if(inslot)
        inslot = false;

      ss << *it;
      processed += 1;

      // 判断下一个是不是end
      if(it + 1 == ch.end()) {
        SampleTemplate::Token* token = tpl->add_tokens();
        token->set_val(ss.str());
        token->set_isslot(false);
        token->set_start(start);
        token->set_end(processed);
      } else if(!isChinese(*(it + 1))) { // 判断下一个是不是中文
        // 这个中文的下一个字符不是中文，进行截断
        SampleTemplate::Token* token = tpl->add_tokens();
        token->set_val(ss.str());
        token->set_isslot(false);
        token->set_start(start);
        token->set_end(processed);
        ss.str(""); // create new
        start = processed;
      }
    } else if( (*it) == "{" ) { // begining of slot

      inslot = true;

      if(ss.rdbuf()->in_avail() != 0) { // check if stringstream has value.
        // create previous token
        SampleTemplate::Token* token = tpl->add_tokens();
        token->set_val(ss.str());
        token->set_isslot(false);
        token->set_start(start);
        token->set_end(processed);
      }

      ss.str(""); // create new
      start = processed;

    } else if(inslot && (*it) == "}") { // end of slot
      inslot = false;
      processed += 1;

      string slotname = ss.str();
      string dictname;  // add dict name

      if(getDictnameBySlotname(slots, slotname, dictname)) {
        // create slot token
        SampleTemplate::Token* token = tpl->add_tokens();
        token->set_slotname(slotname);
        token->set_isslot(true);
        token->set_start(start);
        token->set_end(processed);
        token->set_dictname(dictname);

        if(!tpl->hasslot()) { // 设置为包含槽位
          tpl->set_hasslot(true);
        }
      } else {
        // can not find dict
        VLOG(2) << __func__ << " WARN: can not find slotname " << slotname << "'s dict in slots, so ignore it.";
        SampleTemplate::Token* token = tpl->add_tokens();
        token->set_val(slotname);
        token->set_isslot(false);
        token->set_start(start);
        token->set_end(processed);
      }

      ss.str(""); // create new
      start = processed;

    } else if(inslot && (!isch)) {
      // 不是中文，在槽位中
      ss << (*it);
    } else { // 不是中文，不在槽位中
      ss << (*it);

      if((it + 1) == ch.end()) { // 下一个是结束
        processed += 1;
        SampleTemplate::Token* token = tpl->add_tokens();
        token->set_val(ss.str());
        token->set_isslot(false);
        token->set_start(start);
        token->set_end(processed);
      } else { // 下一个不是结束
        // 下一个是中文或槽位开始
        if(isChinese(*(it + 1)) || ((*(it + 1)) == "{") ) {
          processed += 1;
          SampleTemplate::Token* token = tpl->add_tokens();
          token->set_val(ss.str());
          token->set_isslot(false);
          token->set_start(start);
          token->set_end(processed);

          ss.str(""); // create new
          start = processed;
        }
      }
    }

    cursor += 1;
  }

  VLOG(3) << __func__ << " template: " << FromProtobufToUtf8DebugString(*tpl);
}

/**
 * 生成Utterance
 */
inline bool addUtteranceWithVariant(const SampleTemplate& tpl,
                                    const vector<string>& variant,
                                    Augmented::Sample& ts) {
  stringstream ss;

  size_t i = 0; // 索引
  size_t processed = 0;

  for(const SampleTemplate::Token& token : tpl.tokens()) {
    VLOG(4) << __func__ << " token processed " << processed;

    if(token.isslot()) {  // 槽位
      vector<string> vs;
      CharSegment(variant[i], vs);

      ss << variant[i];
      // 增加命名实体标注
      Augmented::Entity* entity = ts.add_entities();
      entity->set_val(variant[i]);
      entity->set_slotname(token.slotname());
      entity->set_dictname(token.dictname());
      entity->set_start(processed);
      processed += vs.size();
      entity->set_end(processed);
      i++;
    } else {
      ss << token.val();
      processed += (token.end() - token.start());
    }
  }

  ts.set_utterance(ss.str());
  VLOG(3) << __func__ << " set utterance: " << ts.utterance();
};


/**
 * 分词
 */
inline void tokenize(const cppjieba::Jieba& tokenizer,
                     const std::string& query,
                     std::vector<pair<string, string> >& tokens) {
  tokenizer.Tag(query, tokens);
}

/**
 * 在sample中，增加系统词性作为特征
 * 该值是建立在标注的槽位之外的字符上
 */
inline void patch_builtin_pos_labels(Augmented::Sample& ts, const string pos) {
  if(StartsWith(pos, "@x")) {
    // 非语素字
    ts.add_labels("O");
  } else if(StartsWith(pos, "@w")) {
    // 符号
    ts.add_labels("O");
  } else {
    ts.add_labels("B-" + pos);
  }
}

/**
 * 在Sample中增加Term和词性pos
 */
inline void patch_tokens_n_poss(const cppjieba::Jieba& tokenizer,
                                Augmented::Sample& ts) {
  if(!ts.utterance().empty()) {
    VLOG(3) << "[patch_tokens_n_poss] input sample: \n" << FromProtobufToUtf8DebugString(ts);

    // 增加分词
    std::vector<pair<string, string> > tokens;
    tokenize(tokenizer, ts.utterance(), tokens);

    // entity分段
    size_t processed = 0;
    size_t postprocessed = 0;
    bool process_entity = ts.entities_size() > 0;
    size_t entity_num = ts.entities_size() - 1;
    size_t entity_cur = 0;
    google::protobuf::internal::RepeatedPtrIterator<const Augmented_Entity>::iterator entity;

    if(process_entity) {
      entity = ts.entities().begin();
    }

    for(std::vector<pair<string, string> >::iterator it = tokens.begin();
        it != tokens.end(); it++) {
      // define every builtin label prefix @ to distinguish
      // entity name define by developer
      string pos = "@" + it->second;
      ts.add_terms()->assign(it->first);
      ts.add_poss()->assign(pos);

      // 处理分词labels，该标记用于生成特征
      size_t length = CharLength(it->first);
      VLOG(3) << "processed: " << processed
              << ", term: " <<  it->first
              << ", pos: " << pos
              << ", length: " << length;

      if(process_entity)
        VLOG(3) << "entity: \n" << FromProtobufToUtf8DebugString(*entity);

      if(process_entity) {
        postprocessed = processed + length;

        if(postprocessed <= entity->start()) {
          // 不与槽位相交
          patch_builtin_pos_labels(ts, pos);
          processed = postprocessed;
          continue;
        }

        // 识别是否在槽位中
        if(processed <= entity->start() &&
            postprocessed > entity->start()) {
          // 槽位开始
          ts.add_labels("B-" + entity->slotname());
        } else if(processed > entity->start() &&
                  processed < entity->end() &&
                  postprocessed < entity->end()) {
          // 槽位中
          ts.add_labels("I-" + entity->slotname());
        }

        if(processed < entity->end() && postprocessed >= entity->end()) {
          entity_cur += 1;

          if(entity_cur <= entity_num) {
            entity += 1; // 指定下一个槽位
          } else {
            process_entity = false; // 已经处理完所有槽位
          }
        }
      } else {
        // 不考虑槽位
        patch_builtin_pos_labels(ts, pos);
      }

      processed = postprocessed;
    }
  }
}

/**
 * 从模版中扩展槽位，增强samples集合
 */
inline void extendEntitiesForSlotInSample(const cppjieba::Jieba& tokenizer,
    const SampleTemplate& tpl,
    const std::map<std::string, std::vector<string> >& predefined_dicts,
    const std::map<std::string, std::vector<string> >& dicts,
    const Augmented::IntentTrainingSample& its) {
  VLOG(3) << __func__ << " utterance: " << tpl.utterance();

  size_t slots_num = 0; // 槽位数
  vector<pair<string/*slotname*/, string/*dictid*/> > sequence;

  // 生成笛卡尔积
  vector<vector<string> > all;

  for(const SampleTemplate::Token& token : tpl.tokens()) {
    if(token.isslot()) {
      slots_num += 1;
      VLOG(3) << __func__ << " push " << token.slotname()
              << ": " << token.dictname();
      sequence.push_back(std::make_pair(token.slotname(), token.dictname()));
      all.push_back(boost::starts_with(token.dictname(), "@") ? predefined_dicts[token.dictname()] : dicts[token.dictname()]);
    }
  }

  VLOG(3) << __func__ << " has slot number: " << slots_num;

  vector<vector<string> > extended = chatopera::utils::cart_product(all);

  for(size_t i = 0; i < extended.size(); i++) {
    // 每种组成
    vector<string> variant;

    for (size_t j = 0; j < extended[i].size(); j++) {
      VLOG(3) << i << j << ":" << extended[i][j];
      variant.push_back(extended[i][j]);
    }

    if(variant.size() == sequence.size()) {
      VLOG(3) << "generate new record: " << join(variant, ",");
      Augmented::Sample* ts = its.add_tss();
      ts->set_intent_name(tpl.intentname());
      addUtteranceWithVariant(tpl, variant, *ts);
      patch_tokens_n_poss(tokenizer, *ts);
    } else {
      VLOG(3) << __func__ << " unexpected variant word size: " << join(variant, ",");
    }
  }
};


void SampleGenerator::generateTemplates(const cppjieba::Jieba& tokenizer,
                                        const ::google::protobuf::RepeatedPtrField< ::chatopera::bot::intent::TIntent >& intents,
                                        const std::map<std::string, std::vector<string> >& predefined_dicts,
                                        const std::map<std::string, std::vector<string> >& dicts,
                                        Augmented& augmented
                                       ) {
  for(const TIntent& intent : intents) { // 意图
    Augmented::IntentTrainingSample* its = augmented.add_itss();
    // 没有槽位
    its->set_intent(intent.name());

    for(const TIntentUtter& utter : intent.utters()) {
      VLOG(3) << __func__ << " " << intent.name() << ", utterance: " << utter.utterance();
      SampleTemplate tpl;
      tpl.set_intentname(intent.name());
      parseTemplateTokens(utter, intent.slots(), &tpl);

      // 使用template生成增强数据
      if(tpl.hasslot()) {
        VLOG(3) << __func__ << " has slot, extend the entities.";
        extendEntitiesForSlotInSample(tokenizer, tpl, predefined_dicts, dicts, *its);
        VLOG(3) << __func__ << " done " << tpl.utterance();
      } else {
        VLOG(3) << __func__ << " dont have slot: " << tpl.utterance();
        Augmented::Sample* ts = its->add_tss();
        ts->set_intent_name(intent.name());
        ts->set_utterance(tpl.utterance());
        patch_tokens_n_poss(tokenizer, *ts);
      }

      // TODO 增加Sample的termlabs作为特征
    }
  }

  VLOG(3) << __func__ << " augmented \n" << FromProtobufToUtf8DebugString(augmented);
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
 * 追加训练数据
 */
inline void appendNerTrainingData(const Augmented::Sample& sample, ofstream& f) {
  VLOG(3) << "[appendNerTrainingData] sample: \n" << FromProtobufToUtf8DebugString(sample);

  if(sample.terms_size() == 0) {
    VLOG(3) << "[appendNerTrainingData] invalid terms size, ignore this sample: \n" << FromProtobufToUtf8DebugString(sample);
    return;
  }

  if(sample.poss_size() != sample.terms_size() ||
      sample.labels_size() != sample.terms_size()) {
    VLOG(3) << "[appendNerTrainingData] invalid pos and label data, ignore this sample: \n" << FromProtobufToUtf8DebugString(sample);
    return;
  }

  const signed int length = (sample.terms_size() - 1);

  // 标识位
  signed int curr, pre2, pre1, post1, post2;

  google::protobuf::RepeatedPtrField<std::basic_string<char> >::const_iterator term = sample.terms().begin();
  google::protobuf::RepeatedPtrField<std::basic_string<char> >::const_iterator pos = sample.poss().begin();
  google::protobuf::RepeatedPtrField<std::basic_string<char> >::const_iterator label = sample.labels().begin();

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
    // VLOG(3) << "[appendNerTrainingDatas] length: " << length << "curr: " << curr
    //         << ", pre2: " << pre2 << ", pre1: " << pre1
    //         << ", post1: " << post1 << ", post2: " << post2;
    f << *(label + curr) << "\t";

    // feature extract 1: w[t-2]
    // feature extract 2: pos[t-2]
    // feature extract 3: pos[t-2]|pos[t-1]
    // feature extract 4: pos[t-2]|pos[t-1]|pos[t]
    if(pre2 >= 0) {
      f << "w[t-2]=" << *(term + pre2) << "\t";
      f << "pos[t-2]=" << *(pos + pre2) << "\t";
      f << "pos[t-2]|pos[t-1]=" << *(pos + pre2) << "|" << *(pos + pre1) << "\t";
      f << "pos[t-2]|pos[t-1]|pos[t]=" << *(pos + pre2) << "|" << *(pos + pre1) << "|" << *(pos + curr) << "\t";
    }

    // feature extract 5: w[t-1]
    // feature extract 6: pos[t-1]
    // feature extract 7: w[t-1]|w[t]
    // feature extract 8: pos[t-1]|pos[t]
    if(pre1 >= 0) {
      f << "w[t-1]=" << *(term + pre1) << "\t";
      f << "pos[t-1]=" << *(pos + pre1) << "\t";
      f << "w[t-1]|w[t]=" << *(term + pre1) << "|" << *(term + curr) << "\t";
      f << "pos[t-1]|pos[t]=" << *(pos + pre1) << "|" << *(pos + curr) << "\t";
    }

    // feature extract 9: pos[t-1]|pos[t]|pos[t+1]
    if(pre1 >= 0 && post1 > 0) {
      f << "pos[t-1]|pos[t]|pos[t+1]=" << *(pos + pre1) << "|"
        << *(pos + curr) << "|" << *(pos + post1) << "\t";
    }

    // feature extract 10: w[t]
    // feature extract 11: pos[t]
    if(curr >= 0) {
      f << "w[t]=" << *(term + curr) << "\t";
      f << "pos[t]=" << *(pos + curr) << "\t";
    }

    // feature extract 12: w[t+1]
    // feature extract 13: pos[t+1]
    // feature extract 14: w[t]|w[t+1]
    // feature extract 15: pos[t]|pos[t+1]
    if(post1 > 0) {
      f << "w[t+1]=" << *(term + 1) << "\t" ;
      f << "pos[t+1]=" << *(pos + 1) << "\t";
      f << "w[t]|w[t+1]=" << *(term + curr) << "|" << *(term + post1) << "\t";
      f << "pos[t]|pos[t+1]=" << *(pos + curr) << "|" << *(pos + post1) << "\t";
    }

    // feature extract 16: w[t+2]
    // feature extract 17: pos[t+2]
    // feature extract 18: pos[t+1]|pos[t+2]
    // feature extract 19: pos[t]|pos[t+1]|pos[t+2]
    if(post2 > 0) {
      f << "w[t+2]=" << *(term + post2) << "\t";
      f << "pos[t+2]=" << *(pos + post2) << "\t";
      f << "pos[t+1]|pos[t+2]=" << *(pos + post1) << "|" << *(pos + post2) << "\t";
      f << "pos[t]|pos[t+1]|pos[t+2]=" << *(pos + curr) << "|" << *(pos + post1) << "|" << *(pos + post2) << "\t";
    }

    if(curr == 0) {
      f << "\t__BOS__";
    }

    if( curr == length) {
      f << "\t__EOS__";
    }

    f << endl;
    curr++;
  };
}

/**
 * 生成crfsuite训练文件
 */
void  SampleGenerator::generateCrfSuiteTraingData(const Augmented& augmented,
    const string& filepath) {
  VLOG(3) << "[generateCrfSuiteTraingData] filepath: " << filepath;
  // open file
  ofstream f(filepath, ios::app);

  CHECK(f.is_open()) << "[generateCrfSuiteTraingData] ERROR: can not create training file path: " << filepath;

  for(const Augmented::IntentTrainingSample& its : augmented.itss()) {
    for(const Augmented::Sample& sample : its.tss()) {
      appendNerTrainingData(sample, f);
      f << "\n";
    }
  }

  f.close();
}


}; // namespace intent
}; // namespace bot
}; // namespace chatopera

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
