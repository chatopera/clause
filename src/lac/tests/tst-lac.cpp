/*
 * dynamatic array test program.
 *
 * @author   hain
 * @email    hain@chatopera.com
 */
#include "gtest/gtest.h"
#include "glog/logging.h"

#include <map>
#include <list>
#include <vector>
#include <string>
#include <sys/time.h>
#include <iostream>
#include "stdlib.h"
#include "ilac.h"

using namespace std;

void* g_lac_handle = NULL;
int max_result_num = 100;


int init_dict(const char* conf_dir) {
  g_lac_handle = lac_create(conf_dir);
  std::cerr << "create lac handle successfully" << std::endl;
  return 0;
}

int destroy_dict() {
  lac_destroy(g_lac_handle);
  return 0;
}

TEST(LacTest, INIT) {
  LOG(INFO) << " init.";

  init_dict("../../../../var/test/lac/conf");

  if (g_lac_handle == NULL) {
    std::cerr << "creat g_lac_handle error" << std::endl;
  }

  void* lac_buff = lac_buff_create(g_lac_handle);

  if (lac_buff == NULL) {
    std::cerr << "creat lac_buff error" << std::endl;
  }

  std::string line("9月2日开始，北京中小学正式开学。据介绍，公交集团将启用新版行车时刻表，在早晚高峰时段增发3500车次，便利市民集中出行。");

  tag_t *results = new tag_t[max_result_num];

  int result_num = lac_tagging(g_lac_handle,
                               lac_buff, line.c_str(), results, max_result_num);

  if (result_num < 0) {
    std::cerr << "lac tagging failed : line = " << line
              << std::endl;
  }

  for (int i = 0; i < result_num; i++) {
    std::string name = line.substr(results[i].offset,
                                   results[i].length);

    if (i >= 1) {
      LOG(INFO) << "\t";
    }

    LOG(INFO) << name << " " << results[i].type << " "
              << results[i].offset << " " << results[i].length;
  }

}