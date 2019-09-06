/*
 * dynamatic array test program.
 *
 * @author   hain
 * @email    hain@chatopera.com
 */
#include "gtest/gtest.h"
#include "glog/logging.h"

#include <tsl/htrie_map.h>
#include <tsl/htrie_set.h>
#include <string>
#include <vector>
#include <set>

using namespace std;

TEST(HattrieTest, INIT) {
  LOG(INFO) << " init.";
  /*
   * Map of strings to int having char as character type.
   * There is no support for wchar_t, char16_t or char32_t yet,
   * but UTF-8 strings will work fine.
   */
  tsl::htrie_map<char, int> map = {{"one", 1}, {"two", 2}};
  map["three"] = 3;
  map["four"] = 4;

  map.insert("five", 5);
  map.insert_ks("six_with_extra_chars_we_ignore", 3, 6);

  map.erase("two");
  /*
  * Due to the compression on the common prefixes, the letters of the string
  * are not always stored contiguously. When we retrieve the key, we have to
  * construct it.
  *
  * To avoid a heap-allocation at each iteration (when SSO doesn't occur),
  * we reuse the key_buffer to construct the key.
  */
  std::string key_buffer;

  for(auto it = map.begin(); it != map.end(); ++it) {
    it.key(key_buffer);
    LOG(INFO) << "{" << key_buffer << ", " << it.value() << "}";
  }

  /*
  * If you don't care about the allocation.
  */
  for(auto it = map.begin(); it != map.end(); ++it) {
    LOG(INFO) << "{" << it.key() << ", " << *it << "}";
  }

};


TEST(HattrieTest, PREFIX_SEARCH) {

  tsl::htrie_map<char, string> map2 = {{"苹果", "foo"}, {"mango", "foo1"}, {"apricot", "foo2"},
    {"mandarin", "foo1"}, {"melon", "foo4"}, {"macadamia", "foo5"}
  };

  LOG(INFO) << "Prefix search";
  auto prefix_range = map2.equal_prefix_range("苹果");

  // {mandarin, 4} {mango, 2} {macadamia, 6}
  for(auto it = prefix_range.first; it != prefix_range.second; ++it) {
    LOG(INFO) << "{" << it.key() << ", " << *it << "}";
  }

  LOG(INFO) << " Find longest match prefix";
  auto longest_prefix = map2.longest_prefix("苹果111 juice");

  if(longest_prefix != map2.end()) {
    // {apple, 1}
    LOG(INFO) << "{" << longest_prefix.key() << ", "
              << *longest_prefix << "}";
  }
}

TEST(HattrieTest, EXIST) {

  tsl::htrie_map<char, string> map2 = {{"苹果", "foo"}, {"mango", "foo1"}, {"apricot", "foo2"},
    {"mandarin", "foo1"}, {"melon", "foo4"}, {"macadamia", "foo5"}
  };

  LOG(INFO) << "Prefix search";
  const char* s = "苹果";
  string val = map2[s];

  if(val.empty()) {
    LOG(INFO) << "NOT EXIST.";
  } else {
    LOG(INFO) << " EXIST VALUE: " << val;
    map2[s] = val + ";bar";
    LOG(INFO) << " POST SET:" << map2["苹果"];
  }
}

TEST(HattrieTest, SET) {

  tsl::htrie_map<char, set<string> > map2;

  map2["苹果"].insert("foo");
  map2["苹果"].insert("foo");

  if(map2["苹果"].size() == 0) {
    LOG(INFO) << " SIZE = 0";
  } else {
    LOG(INFO) << " SIZE = " << map2["苹果"].size();

    for (set<string>::iterator it = map2["苹果"].begin(); it != map2["苹果"].end(); it++) {
      LOG(INFO) << "VAL: " << *it;
    }

    LOG(INFO) << "contains foo2: " << (map2["苹果"].find("foo2") != map2["苹果"].end());
  }
}

TEST(HattrieTest, HTRIE_SET) {

  string key_buffer;

  tsl::htrie_set<char> set = {"one", "two", "three"};
  set.insert({"four", "five"});
  set.insert("six");

  // {one} {two} {five} {four} {three}
  for(auto it = set.begin(); it != set.end(); ++it) {
    it.key(key_buffer);
    std::cout << "{" << key_buffer << "}" << std::endl;
  }

  LOG(INFO) << "contains six: " << (set.find("six") != set.end());

  tsl::htrie_map<char, tsl::htrie_set<char> > map2;

  map2["苹果"].insert("foo");
  map2["苹果"].insert("foo2");

  if(map2["苹果"].size() == 0) {
    LOG(INFO) << " SIZE = 0";
  } else {
    LOG(INFO) << " SIZE = " << map2["苹果"].size();

    for (tsl::htrie_set<char>::iterator it = map2["苹果"].begin(); it != map2["苹果"].end(); it++) {
      LOG(INFO) << "VAL: " << it.key();
    }

    LOG(INFO) << "contains foo2: " << (map2["苹果"].find("foo2") != map2["苹果"].end());
  }
}