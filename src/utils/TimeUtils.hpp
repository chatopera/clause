#ifndef CHATOPERA_UTILS_TIME_FUNCTS_H
#define CHATOPERA_UTILS_TIME_FUNCTS_H

#include <cstdio>
#include <ctime>
#include <string>

/**
 * 获得当前时间
 * 符合MySQL Timestamp
 */
inline std::string GetCurrentTimestampFormatted() {
  std::time_t rawtime;
  std::tm* timeinfo;
  char buffer [80];
  std::time(&rawtime);
  timeinfo = std::localtime(&rawtime);

  std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
  std::puts(buffer);
  std::string now(buffer);
  return now;
}

#endif