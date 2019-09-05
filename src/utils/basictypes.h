/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __CHATOPERA_UTILS_BASICTYPES_H__
#define __CHATOPERA_UTILS_BASICTYPES_H__

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TDebugProtocol.h"
#include "thrift/protocol/TJSONProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include <google/protobuf/text_format.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

typedef uint32_t uint32;
typedef uint8_t uint8;

// A macro to disallow the copy constructor and operator= functions
// This should be used in the priavte:declarations for a class
// https://www.cnblogs.com/dwdxdy/archive/2012/07/16/2594113.html
#define    DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);                \
  TypeName& operator=(const TypeName&)

inline void versionPrint(const std::string& projectname) {
#ifndef GIT_COMMIT_BRANCH
#define GIT_COMMIT_BRANCH "unknown"
#endif

#ifndef GIT_COMMIT_HASH
#define GIT_COMMIT_HASH "0000000" // 0000000 means uninitialized
#endif

#ifndef GIT_USER_NAME
#define GIT_USER_NAME "Chatopera Inc."
#endif

#ifndef GIT_USER_EMAIL
#define GIT_USER_EMAIL "info@chatopera.com"
#endif

#ifndef GCC_DUMP_VERSION
#define GCC_DUMP_VERSION "UNKNOWN"
#endif

#ifndef GIT_BUILD_DATE
#define GIT_BUILD_DATE "UNKNOWN"
#endif
  // 打印版本信息
  std::stringstream is;
  is << "\n***[ " << projectname << " ]***\n";
  is << "Copyright (c) 2019 北京华夏春松科技有限公司 <https://www.chatopera.com>.\n";
  is << "All Rights Reserved.\n";
  is << "Powered by https://gitlab.chatopera.com/chatopera\n";
  is << "Built by:\t " << GIT_USER_NAME << "\n";
  is << "Email:\t\t " << GIT_USER_EMAIL << "\n";
  is << "Branch:\t\t " << GIT_COMMIT_BRANCH << "\n";
  is << "Version:\t " << GIT_COMMIT_HASH << "\n";
  is << "Date:\t\t " << GIT_BUILD_DATE << "\n";
  is << "Compiler:\t gcc-" << GCC_DUMP_VERSION << "\n";
  is << "**************************************************\n";
  std::cout << is.str() << std::endl;
}

template <typename T>
const std::string FromThriftToDebugString(const T *object,
    int32_t string_limit = TDebugProtocol::DEFAULT_STRING_LIMIT) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer());

  TDebugProtocol* dprotocol = new TDebugProtocol(membuffer);
  dprotocol->setStringSizeLimit(string_limit);
  int32_t  prefix_len = (string_limit - 16) > 16 ? (string_limit - 16) : 16;
  dprotocol->setStringPrefixSize(prefix_len);
  stdcxx::scoped_ptr<TProtocol> protocol(dprotocol);

  object->write(protocol.get());
  uint8_t* buffer = NULL;
  uint32_t buffer_size = 0;
  membuffer->getBuffer(&buffer, &buffer_size);
  return std::string(reinterpret_cast<const char*>(buffer), buffer_size);
}

template <typename T>
const std::string FromThriftToUtf8DebugString(const T *object,
    int32_t string_limit = TDebugProtocol::DEFAULT_STRING_LIMIT) {
  std::string debug_string = FromThriftToDebugString(object, 10 * string_limit);
  debug_string.append(" ");
  std::string out;

  for (int i = 0; i < debug_string.length();) {
    if (debug_string[i] == '\\') {
      switch (debug_string[i + 1]) {
        case 'x': {
          std::string hex_string;
          hex_string.insert(hex_string.end(), debug_string[i + 2]);
          hex_string.insert(hex_string.end(), debug_string[i + 3]);
          int hex_value = 0;
          std::stringstream ss;
          ss << std::hex << hex_string;
          ss >> hex_value;
          out.insert(out.end(), static_cast<unsigned char>(hex_value));
          i += 4;
          break;
        }

        case 'a':
          out.insert(out.end(), '\a');
          i += 2;
          break;

        case 'b':
          out.insert(out.end(), '\b');
          i += 2;
          break;

        case 'f':
          out.insert(out.end(), '\f');
          i += 2;
          break;

        case 'n':
          out.insert(out.end(), '\n');
          i += 2;
          break;

        case 'r':
          out.insert(out.end(), '\r');
          i += 2;
          break;

        case 't':
          out.insert(out.end(), '\t');
          i += 2;
          break;

        case 'v':
          out.insert(out.end(), '\v');
          i += 2;
          break;

        case '\\':
          out.insert(out.end(), '\\');
          i += 2;
          break;

        case '"':
          out.insert(out.end(), '"');
          i += 2;
          break;
      }
    } else {
      out.insert(out.end(), debug_string[i]);
      i++;
    }
  }

  return out;
}

template <typename T>
const std::string FromThriftToString(const T *object) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer());
  stdcxx::scoped_ptr<TProtocol> protocol(new TBinaryProtocol(membuffer));
  object->write(protocol.get());
  uint8* buffer = NULL;
  uint32 buffer_size = 0;
  membuffer->getBuffer(&buffer, &buffer_size);
  return std::string(reinterpret_cast<const char*>(buffer), buffer_size);
}

template <typename T>
bool FromThriftToString(const T& object, std::string* buffer) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer());
  stdcxx::scoped_ptr<TProtocol> protocol(new TBinaryProtocol(membuffer));

  try {
    object.write(protocol.get());
  } catch(const TException &ex) {
    std::cout << ex.what();
    return false;
  }

  uint8* buf = NULL;
  uint32 buf_size = 0;
  membuffer->getBuffer(&buf, &buf_size);
  buffer->assign(reinterpret_cast<const char*>(buf), buf_size);
  return true;
}

template <typename T>
inline bool FromStringToThriftFast(const std::string &buffer, T *object) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer(
        const_cast<uint8*>(reinterpret_cast<const uint8*>(buffer.c_str())),
        buffer.size()));
  stdcxx::scoped_ptr<TProtocol> binaryprotocol(new TBinaryProtocol(membuffer));

  try {
    object->read(binaryprotocol.get());
  } catch(const TException &ex) {
    std::cout << ex.what();
    return false;
  }

  return true;
}

template <typename T>
bool FromStringToThrift(const std::string &buffer, T *object) {
  T tmp_object;

  if (FromStringToThriftFast(buffer, &tmp_object)) {
    *object = tmp_object;
    return true;
  }

  return false;
}


template <typename T>
void FromDebugStringToThrift(const std::string &buffer, T *object) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer(
        const_cast<uint8*>(reinterpret_cast<const uint8*>(buffer.c_str())),
        buffer.size()));
  stdcxx::scoped_ptr<TProtocol> binaryprotocol(new TDebugProtocol(membuffer));
  T tmp_object;
  tmp_object.read(binaryprotocol.get());
  *object = tmp_object;
}

template <typename T>
const std::string FromThriftToJson(const T *object) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer());
  stdcxx::scoped_ptr<TProtocol> protocol(new TJSONProtocol(membuffer));
  object->write(protocol.get());
  uint8* buffer = NULL;
  uint32 buffer_size = 0;
  membuffer->getBuffer(&buffer, &buffer_size);
  return std::string(reinterpret_cast<const char*>(buffer), buffer_size);
}

template <typename T>
void FromJsonToThrift(const std::string &buffer, T *object) {
  stdcxx::shared_ptr<TMemoryBuffer> membuffer(new TMemoryBuffer(
        const_cast<uint8*>(reinterpret_cast<const uint8*>(buffer.c_str())),
        buffer.size()));
  stdcxx::scoped_ptr<TProtocol> binaryprotocol(new TJSONProtocol(membuffer));
  T tmp_object;
  tmp_object.read(binaryprotocol.get());
  *object = tmp_object;
}

/**
 * get protobuf message strings
 * Fix chaos for Chinese chars
 */
static std::string FromProtobufToUtf8DebugString(const google::protobuf::Message& message,
    bool singleLineMode = false) {
  std::string debug_string;
  google::protobuf::TextFormat::Printer printer;

  if(singleLineMode)
    printer.SetSingleLineMode(true);

  printer.SetUseUtf8StringEscaping(true);
  printer.PrintToString(message, &debug_string);

  // Single line mode currently might have an extra space at the end.
  if (singleLineMode &&
      debug_string.size() > 0 &&
      debug_string[debug_string.size() - 1] == ' ') {
    debug_string.resize(debug_string.size() - 1);
  }

  return debug_string;
}


#endif  //__CHATOPERA_UTILS_BASICTYPES_H__