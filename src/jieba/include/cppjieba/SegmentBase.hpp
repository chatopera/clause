#ifndef CPPJIEBA_SEGMENTBASE_H
#define CPPJIEBA_SEGMENTBASE_H

#include "glog/logging.h"
#include "PreFilter.hpp"
#include <cassert>

namespace cppjieba {

const char* const SPECIAL_SEPARATORS = " \t\n\xEF\xBC\x8C\xE3\x80\x82";

using namespace limonp;

class SegmentBase {
 public:
  SegmentBase() {
    CHECK(ResetSeparators(SPECIAL_SEPARATORS));
  }
  virtual ~SegmentBase() {
  }

  virtual void Cut(const string& sentence, vector<string>& words) const = 0;

  bool ResetSeparators(const string& s) {
    symbols_.clear();
    RuneStrArray runes;

    if (!DecodeRunesInString(s, runes)) {
      VLOG(2) << "decode " << s << " failed";
      return false;
    }

    for (size_t i = 0; i < runes.size(); i++) {
      if (!symbols_.insert(runes[i].rune).second) {
        VLOG(2) << s.substr(runes[i].offset, runes[i].len) << " already exists";
        return false;
      }
    }

    return true;
  }
 protected:
  unordered_set<Rune> symbols_;
}; // class SegmentBase

} // cppjieba

#endif
