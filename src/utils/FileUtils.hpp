/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __CHATOPERA_UTILS_FILES_H__
#define __CHATOPERA_UTILS_FILES_H__

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

void copyDirectoryRecursively(const fs::path& sourceDir,
                              const fs::path& destinationDir) {
  if (!fs::exists(sourceDir) || !fs::is_directory(sourceDir)) {
    throw std::runtime_error("Source directory " + sourceDir.string() + " does not exist or is not a directory");
  }

  if (fs::exists(destinationDir)) {
    throw std::runtime_error("Destination directory " + destinationDir.string() + " already exists");
  }

  if (!fs::create_directory(destinationDir)) {
    throw std::runtime_error("Cannot create destination directory " + destinationDir.string());
  }

  for (const auto& dirEnt : fs::recursive_directory_iterator{sourceDir}) {
    const auto& path = dirEnt.path();
    auto relativePathStr = path.string();
    boost::replace_first(relativePathStr, sourceDir.string(), "");
    fs::copy(path, destinationDir / relativePathStr);
  }
}

void copyDirectoryRecursively(const string& sourceDir, const string& destinationDir) {
  fs::path src(sourceDir);
  fs::path dest(destinationDir);
  copyDirectoryRecursively(src, dest);
};

#endif