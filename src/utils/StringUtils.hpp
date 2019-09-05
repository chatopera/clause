/************************************
 * file enc : ascii
 * author   : wuyanyi09@gmail.com
 * https://raw.githubusercontent.com/yanyiwu/limonp/master/include/limonp/StringUtil.hpp
 ************************************/
#ifndef CHATOPERA_UTILS_STR_FUNCTS_H
#define CHATOPERA_UTILS_STR_FUNCTS_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <functional>
#include <locale>
#include <sstream>
#include <sys/types.h>
#include <iterator>
#include <algorithm>
#include <uuid/uuid.h>
#include "StdExtension.hpp"

using namespace std;

namespace chatopera {
namespace utils {

const char kWhitespaceASCII[] = {
  0x09,
  0x0A,
  0x0B,
  0x0C,
  0x0D,
  0x20,
  0
};

enum TrimPositions {
  TRIM_NONE     = 0,
  TRIM_LEADING  = 1 << 0,
  TRIM_TRAILING = 1 << 1,
  TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
};

/**
 * 获得字符串的单个字
 */
inline size_t CharSegment(const std::string &query, std::vector<std::string> &terms)  {
  terms.clear();
  size_t sz = 0;

  for (size_t i = 0; i < query.size();) {
    if ((query[i] & 0xFC) == 0xFC && i + 6 <= query.size()) {
      terms.push_back(query.substr(i, 6));
      i += 6;
      sz++;
      continue;
    }

    if ((query[i] & 0xF8) == 0xF8 && i + 5 <= query.size()) {
      terms.push_back(query.substr(i, 5));
      i += 5;
      sz++;
      continue;
    }

    if ((query[i] & 0xF0) == 0xF0 && i + 4 <= query.size()) {
      terms.push_back(query.substr(i, 4));
      i += 4;
      sz++;
      continue;
    }

    if ((query[i] & 0xE0) == 0xE0 && i + 3 <= query.size()) {
      terms.push_back(query.substr(i, 3));
      i += 3;
      sz++;
      continue;
    }

    if ((query[i] & 0xC0) == 0xC0 && i + 2 <= query.size()) {
      terms.push_back(query.substr(i, 2));
      i += 2;
      sz++;
      continue;
    }

    terms.push_back(query.substr(i, 1));
    i += 1;
    sz++;
  }

  return sz;
}

/**
 * 获得字符串单个字数
 * 比如: "中国"(2), "taxi"(4)
 */
inline size_t CharLength(const std::string &query)  {
  size_t sz = 0;

  for (size_t i = 0; i < query.size();) {
    if ((query[i] & 0xFC) == 0xFC && i + 6 <= query.size()) {
      i += 6;
      sz++;
      continue;
    }

    if ((query[i] & 0xF8) == 0xF8 && i + 5 <= query.size()) {
      i += 5;
      sz++;
      continue;
    }

    if ((query[i] & 0xF0) == 0xF0 && i + 4 <= query.size()) {
      i += 4;
      sz++;
      continue;
    }

    if ((query[i] & 0xE0) == 0xE0 && i + 3 <= query.size()) {
      i += 3;
      sz++;
      continue;
    }

    if ((query[i] & 0xC0) == 0xC0 && i + 2 <= query.size()) {
      i += 2;
      sz++;
      continue;
    }

    i += 1;
    sz++;
  }

  return sz;
}

inline string StringFormat(const char* fmt, ...) {
  int size = 256;
  std::string str;
  va_list ap;

  while (1) {
    str.resize(size);
    va_start(ap, fmt);
    int n = vsnprintf((char *)str.c_str(), size, fmt, ap);
    va_end(ap);

    if (n > -1 && n < size) {
      str.resize(n);
      return str;
    }

    if (n > -1)
      size = n + 1;
    else
      size *= 2;
  }

  return str;
}

template<class T>
void Join(T begin, T end, string& res, const string& connector) {
  if(begin == end) {
    return;
  }

  stringstream ss;
  ss << *begin;
  begin++;

  while(begin != end) {
    ss << connector << *begin;
    begin ++;
  }

  res = ss.str();
}

template<class T>
string Join(T begin, T end, const string& connector) {
  string res;
  Join(begin, end, res, connector);
  return res;
}

inline string& Upper(string& str) {
  transform(str.begin(), str.end(), str.begin(), (int (*)(int))toupper);
  return str;
}

inline string& Lower(string& str) {
  transform(str.begin(), str.end(), str.begin(), (int (*)(int))tolower);
  return str;
}

inline bool IsSpace(unsigned c) {
  // when passing large int as the argument of isspace, it core dump, so here need a type cast.
  return c > 0xff ? false : std::isspace(c & 0xff) != 0;
}

/**
 * Trim
 */

template<typename STR>
TrimPositions TrimStringT(const STR& input,
                          const typename STR::value_type trim_chars[],
                          TrimPositions positions,
                          STR* output) {

  const typename STR::size_type last_char = input.length() - 1;
  const typename STR::size_type first_good_char = (positions & TRIM_LEADING) ?
      input.find_first_not_of(trim_chars) : 0;
  const typename STR::size_type last_good_char = (positions & TRIM_TRAILING) ?
      input.find_last_not_of(trim_chars) : last_char;

  if (input.empty() ||
      (first_good_char == STR::npos) || (last_good_char == STR::npos)) {
    bool input_was_empty = input.empty();
    output->clear();
    return input_was_empty ? TRIM_NONE : positions;
  }

  *output =
    input.substr(first_good_char, last_good_char - first_good_char + 1);

  return static_cast<TrimPositions>(
           ((first_good_char == 0) ? TRIM_NONE : TRIM_LEADING) |
           ((last_good_char == last_char) ? TRIM_NONE : TRIM_TRAILING));
}

inline bool TrimString(const std::string& input,
                       const char trim_chars[],
                       std::string* output) {
  return TrimStringT(input, trim_chars, TRIM_ALL, output) != TRIM_NONE;
}

inline TrimPositions TrimWhitespaceASCII(const std::string& input,
    TrimPositions positions,
    std::string* output) {
  return TrimStringT(input, kWhitespaceASCII, positions, output);
}

inline TrimPositions TrimWhitespace(const std::string& input,
                                    TrimPositions positions,
                                    std::string* output) {
  return TrimWhitespaceASCII(input, positions, output);
}

inline std::string& LTrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<unsigned, bool>(IsSpace))));
  return s;
}

inline std::string& RTrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<unsigned, bool>(IsSpace))).base(), s.end());
  return s;
}

inline std::string& Trim(std::string &s) {
  return LTrim(RTrim(s));
}

inline std::string& LTrim(std::string & s, char x) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::bind2nd(std::equal_to<char>(), x))));
  return s;
}

inline std::string& RTrim(std::string & s, char x) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::bind2nd(std::equal_to<char>(), x))).base(), s.end());
  return s;
}

inline std::string& Trim(std::string &s, char x) {
  return LTrim(RTrim(s, x), x);
}

/**
 * Splits
 */

inline void Split(const string& src, vector<string>& res, const string& pattern, size_t maxsplit = string::npos) {
  res.clear();
  size_t Start = 0;
  size_t end = 0;
  string sub;

  while(Start < src.size()) {
    end = src.find_first_of(pattern, Start);

    if(string::npos == end || res.size() >= maxsplit) {
      sub = src.substr(Start);
      res.push_back(sub);
      return;
    }

    sub = src.substr(Start, end - Start);
    res.push_back(sub);
    Start = end + 1;
  }

  return;
}

inline vector<string> Split(const string& src, const string& pattern, size_t maxsplit = string::npos) {
  vector<string> res;
  Split(src, res, pattern, maxsplit);
  return res;
}

template<typename STR>
static void SplitStringT(const STR& str,
                         const typename STR::value_type s,
                         bool trim_whitespace,
                         std::vector<STR>* r) {
  size_t last = 0;
  size_t i;
  size_t c = str.size();

  for (i = 0; i <= c; ++i) {
    if (i == c || str[i] == s) {
      size_t len = i - last;
      STR tmp = str.substr(last, len);

      if (trim_whitespace) {
        STR t_tmp;
        TrimWhitespace(tmp, TRIM_ALL, &t_tmp);
        r->push_back(t_tmp);
      } else {
        r->push_back(tmp);
      }

      last = i + 1;
    }
  }
}

inline void SplitString(const std::string& str,
                        char s,
                        std::vector<std::string>* r) {
  SplitStringT(str, s, true, r);
}


inline bool StartsWith(const string& str, const string& prefix) {
  if(prefix.length() > str.length()) {
    return false;
  }

  return 0 == str.compare(0, prefix.length(), prefix);
}

inline bool EndsWith(const string& str, const string& suffix) {
  if(suffix.length() > str.length()) {
    return false;
  }

  return 0 == str.compare(str.length() -  suffix.length(), suffix.length(), suffix);
}

inline bool IsInStr(const string& str, char ch) {
  return str.find(ch) != string::npos;
}

inline uint16_t TwocharToUint16(char high, char low) {
  return (((uint16_t(high) & 0x00ff ) << 8) | (uint16_t(low) & 0x00ff));
}

template <class Uint16Container>
bool Utf8ToUnicode(const char * const str, size_t len, Uint16Container& vec) {
  if(!str) {
    return false;
  }

  char ch1, ch2;
  uint16_t tmp;
  vec.clear();

  for(size_t i = 0; i < len;) {
    if(!(str[i] & 0x80)) { // 0xxxxxxx
      vec.push_back(str[i]);
      i++;
    } else if ((uint8_t)str[i] <= 0xdf && i + 1 < len) { // 110xxxxxx
      ch1 = (str[i] >> 2) & 0x07;
      ch2 = (str[i + 1] & 0x3f) | ((str[i] & 0x03) << 6 );
      tmp = (((uint16_t(ch1) & 0x00ff ) << 8) | (uint16_t(ch2) & 0x00ff));
      vec.push_back(tmp);
      i += 2;
    } else if((uint8_t)str[i] <= 0xef && i + 2 < len) {
      ch1 = ((uint8_t)str[i] << 4) | ((str[i + 1] >> 2) & 0x0f );
      ch2 = (((uint8_t)str[i + 1] << 6) & 0xc0) | (str[i + 2] & 0x3f);
      tmp = (((uint16_t(ch1) & 0x00ff ) << 8) | (uint16_t(ch2) & 0x00ff));
      vec.push_back(tmp);
      i += 3;
    } else {
      return false;
    }
  }

  return true;
}

template <class Uint16Container>
bool Utf8ToUnicode(const string& str, Uint16Container& vec) {
  return Utf8ToUnicode(str.c_str(), str.size(), vec);
}

template <class Uint32Container>
bool Utf8ToUnicode32(const string& str, Uint32Container& vec) {
  uint32_t tmp;
  vec.clear();

  for(size_t i = 0; i < str.size();) {
    if(!(str[i] & 0x80)) { // 0xxxxxxx
      // 7bit, total 7bit
      tmp = (uint8_t)(str[i]) & 0x7f;
      i++;
    } else if ((uint8_t)str[i] <= 0xdf && i + 1 < str.size()) { // 110xxxxxx
      // 5bit, total 5bit
      tmp = (uint8_t)(str[i]) & 0x1f;

      // 6bit, total 11bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i + 1]) & 0x3f;
      i += 2;
    } else if((uint8_t)str[i] <= 0xef && i + 2 < str.size()) { // 1110xxxxxx
      // 4bit, total 4bit
      tmp = (uint8_t)(str[i]) & 0x0f;

      // 6bit, total 10bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i + 1]) & 0x3f;

      // 6bit, total 16bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i + 2]) & 0x3f;

      i += 3;
    } else if((uint8_t)str[i] <= 0xf7 && i + 3 < str.size()) { // 11110xxxx
      // 3bit, total 3bit
      tmp = (uint8_t)(str[i]) & 0x07;

      // 6bit, total 9bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i + 1]) & 0x3f;

      // 6bit, total 15bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i + 2]) & 0x3f;

      // 6bit, total 21bit
      tmp <<= 6;
      tmp |= (uint8_t)(str[i + 3]) & 0x3f;

      i += 4;
    } else {
      return false;
    }

    vec.push_back(tmp);
  }

  return true;
}

template <class Uint32ContainerConIter>
void Unicode32ToUtf8(Uint32ContainerConIter begin, Uint32ContainerConIter end, string& res) {
  res.clear();
  uint32_t ui;

  while(begin != end) {
    ui = *begin;

    if(ui <= 0x7f) {
      res += char(ui);
    } else if(ui <= 0x7ff) {
      res += char(((ui >> 6) & 0x1f) | 0xc0);
      res += char((ui & 0x3f) | 0x80);
    } else if(ui <= 0xffff) {
      res += char(((ui >> 12) & 0x0f) | 0xe0);
      res += char(((ui >> 6) & 0x3f) | 0x80);
      res += char((ui & 0x3f) | 0x80);
    } else {
      res += char(((ui >> 18) & 0x03) | 0xf0);
      res += char(((ui >> 12) & 0x3f) | 0x80);
      res += char(((ui >> 6) & 0x3f) | 0x80);
      res += char((ui & 0x3f) | 0x80);
    }

    begin ++;
  }
}

template <class Uint16ContainerConIter>
void UnicodeToUtf8(Uint16ContainerConIter begin, Uint16ContainerConIter end, string& res) {
  res.clear();
  uint16_t ui;

  while(begin != end) {
    ui = *begin;

    if(ui <= 0x7f) {
      res += char(ui);
    } else if(ui <= 0x7ff) {
      res += char(((ui >> 6) & 0x1f) | 0xc0);
      res += char((ui & 0x3f) | 0x80);
    } else {
      res += char(((ui >> 12) & 0x0f ) | 0xe0);
      res += char(((ui >> 6) & 0x3f ) | 0x80 );
      res += char((ui & 0x3f) | 0x80);
    }

    begin ++;
  }
}


template <class Uint16Container>
bool GBKTrans(const char* const str, size_t len, Uint16Container& vec) {
  vec.clear();

  if(!str) {
    return true;
  }

  size_t i = 0;

  while(i < len) {
    if(0 == (str[i] & 0x80)) {
      vec.push_back(uint16_t(str[i]));
      i++;
    } else {
      if(i + 1 < len) { //&& (str[i+1] & 0x80))
        uint16_t tmp = (((uint16_t(str[i]) & 0x00ff ) << 8) | (uint16_t(str[i + 1]) & 0x00ff));
        vec.push_back(tmp);
        i += 2;
      } else {
        return false;
      }
    }
  }

  return true;
}

template <class Uint16Container>
bool GBKTrans(const string& str, Uint16Container& vec) {
  return GBKTrans(str.c_str(), str.size(), vec);
}

template <class Uint16ContainerConIter>
void GBKTrans(Uint16ContainerConIter begin, Uint16ContainerConIter end, string& res) {
  res.clear();
  //pair<char, char> pa;
  char first, second;

  while(begin != end) {
    //pa = uint16ToChar2(*begin);
    first = ((*begin) >> 8) & 0x00ff;
    second = (*begin) & 0x00ff;

    if(first & 0x80) {
      res += first;
      res += second;
    } else {
      res += second;
    }

    begin++;
  }
}

/**
 * 判断是否为中文（不包括中文符号，非正则）
 * https://blog.csdn.net/u010317005/article/details/77511416
 */
inline bool isChinese(const string& str) {
  unsigned char utf[4] = {0};
  unsigned char unicode[3] = {0};
  bool res = false;

  for (int i = 0; i < str.length(); i++) {
    if ((str[i] & 0x80) == 0) {
      //ascii begin with 0
      res = false;
    } else { /*if ((str[i] & 0x80) == 1) */
      utf[0] = str[i];
      utf[1] = str[i + 1];
      utf[2] = str[i + 2];
      i++;
      i++;
      unicode[0] = ((utf[0] & 0x0F) << 4) | ((utf[1] & 0x3C) >> 2);
      unicode[1] = ((utf[1] & 0x03) << 6) | (utf[2] & 0x3F);

      if(unicode[0] >= 0x4e && unicode[0] <= 0x9f) {
        if (unicode[0] == 0x9f && unicode[1] > 0xa5) {
          res = false;
        } else {
          res = true;
        }
      } else {
        res = false;
      }
    }
  }

  return res;
}

/*
 * format example: "%Y-%m-%d %H:%M:%S"
 */
inline void GetTime(const string& format, string&  timeStr) {
  time_t timeNow;
  time(&timeNow);
  timeStr.resize(64);
  size_t len = strftime((char*)timeStr.c_str(), timeStr.size(), format.c_str(), localtime(&timeNow));
  timeStr.resize(len);
}

inline string PathJoin(const string& path1, const string& path2) {
  if(EndsWith(path1, "/")) {
    return path1 + path2;
  }

  return path1 + "/" + path2;
}

/**
 * Generate GUID
 */

typedef struct _GUID {
  unsigned long Data1;
  unsigned short Data2;
  unsigned short Data3;
  unsigned char Data4[8];
} GUID;

inline string generate_uuid() {
  GUID guid;
  uuid_generate(reinterpret_cast<unsigned char *>(&guid));
  char buf[64] = { 0 };
  snprintf(buf, sizeof(buf),
           "%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
           guid.Data1,
           guid.Data2,
           guid.Data3,
           guid.Data4[0],
           guid.Data4[1],
           guid.Data4[2],
           guid.Data4[3],
           guid.Data4[4],
           guid.Data4[5],
           guid.Data4[6],
           guid.Data4[7]) ;
  return std::string(buf) ;
}


} // namespace utils
} // namespace chatopera
#endif