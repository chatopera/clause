#include "gtest/gtest.h"
#include "cppjieba/PreFilter.hpp"
#include "StringUtils.hpp"

using namespace cppjieba;
using namespace chatopera::utils;

const char* const DICT_PATH = "../../../../var/test/jieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../../../../var/test/jieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../../../../var/test/jieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../../../../var/test/jieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../../../../var/test/jieba/dict/stop_words.utf8";

TEST(JiebaTest, PreFilterTest1) {
  unordered_set<Rune> symbol;
  symbol.insert(65292u); // "，"
  symbol.insert(12290u); // "。"
  string expected;
  string res;

  {
    string s = "你好，美丽的，世界";
    PreFilter filter(symbol, s);
    expected = "你好/，/美丽的/，/世界";
    ASSERT_TRUE(filter.HasNext());
    vector<string> words;

    while (filter.HasNext()) {
      PreFilter::Range range;
      range = filter.Next();
      words.push_back(GetStringFromRunes(s, range.begin, range.end - 1));
    }

    res = Join(words.begin(), words.end(), "/");
    ASSERT_EQ(res, expected);
  }

  {
    string s = "我来自北京邮电大学。。。学号123456，用AK47";
    PreFilter filter(symbol, s);
    expected = "我来自北京邮电大学/。/。/。/学号123456/，/用AK47";
    ASSERT_TRUE(filter.HasNext());
    vector<string> words;

    while (filter.HasNext()) {
      PreFilter::Range range;
      range = filter.Next();
      words.push_back(GetStringFromRunes(s, range.begin, range.end - 1));
    }

    res = Join(words.begin(), words.end(), "/");
    ASSERT_EQ(res, expected);
  }
}
