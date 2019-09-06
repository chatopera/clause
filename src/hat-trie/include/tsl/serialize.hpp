/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/hat-trie/include/tsl/serialize.hpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-27_13:55:05
 * @brief
 *
 **/


#ifndef __CHATOPERA_BOT_HAT_TRIE_SERIALIZE_H__
#define __CHATOPERA_BOT_HAT_TRIE_SERIALIZE_H__

#include <string>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/set.hpp>
#include <cassert>
#include <cstdint>
#include <fstream>

namespace chatopera {
namespace bot {
namespace trie {

template<typename Archive>
struct serializer {
  Archive& ar;

  template<typename T>
  void operator()(const T& val) {
    ar & val;
  }

  template<typename CharT>
  void operator()(const CharT* val, std::size_t val_size) {
    ar.save_binary(reinterpret_cast<const void*>(val), val_size * sizeof(CharT));
  }
};

template<typename Archive>
struct deserializer {
  Archive& ar;

  template<typename T>
  T operator()() {
    T val;
    ar & val;
    return val;
  }

  template<typename CharT>
  void operator()(CharT* val_out, std::size_t val_size) {
    ar.load_binary(reinterpret_cast<void*>(val_out), val_size * sizeof(CharT));
  }
};

} // namespace trie
} // namespace bot
} // namespace chatopera

namespace boost {
namespace serialization {
template<class Archive, class CharT, class T>
void serialize(Archive & ar, tsl::htrie_map<CharT, T>& map, const unsigned int version) {
  split_free(ar, map, version);
};

template<class Archive, class CharT, class T>
void save(Archive & ar, const tsl::htrie_map<CharT, T>& map, const unsigned int version) {
  chatopera::bot::trie::serializer<Archive> serial{ar};
  map.serialize(serial);
};

template<class Archive, class CharT, class T>
void load(Archive & ar, tsl::htrie_map<CharT, T>& map, const unsigned int version) {
  chatopera::bot::trie::deserializer<Archive> deserial{ar};
  map = tsl::htrie_map<CharT, T>::deserialize(deserial);
};
} // namespace serialization
} // namespace boost

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
