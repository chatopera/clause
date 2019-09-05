/*
 * test serialization.
 *
 * @author   hain
 * @email    hain@chatopera.com
 * https://github.com/Tessil/hat-trie
 */
#include "gtest/gtest.h"
#include "glog/logging.h"

#include <tsl/htrie_map.h>
#include <tsl/htrie_set.h>
#include <tsl/serialize.hpp>
#include <string>

using namespace std;

TEST(HattrieTest, SER) {
  const tsl::htrie_map<char, std::int64_t> map = {{"one", 1}, {"two", 2},
    {"three", 3}, {"four", 4}
  };

  const char* file_name = "htrie_map.data";
  {
    std::ofstream ofs;
    ofs.exceptions(ofs.badbit | ofs.failbit);
    ofs.open(file_name, std::ios::binary);

    boost::iostreams::filtering_ostream fo;
    fo.push(boost::iostreams::zlib_compressor());
    fo.push(ofs);

    boost::archive::binary_oarchive oa(fo);

    oa << map;
  }

  {
    std::ifstream ifs;
    ifs.exceptions(ifs.badbit | ifs.failbit | ifs.eofbit);
    ifs.open(file_name, std::ios::binary);

    boost::iostreams::filtering_istream fi;
    fi.push(boost::iostreams::zlib_decompressor());
    fi.push(ifs);

    boost::archive::binary_iarchive ia(fi);

    tsl::htrie_map<char, std::int64_t> map_deserialized;
    ia >> map_deserialized;

    EXPECT_TRUE(map == map_deserialized);

    // Find longest match prefix.
    auto longest_prefix = map_deserialized.longest_prefix("one2222");

    if(longest_prefix != map_deserialized.end()) {
      // {apple, 1}
      LOG(INFO) << "{" << longest_prefix.key() << ", "
                << *longest_prefix << "}";
    }
  }

}