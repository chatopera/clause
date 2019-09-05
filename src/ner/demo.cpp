/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/crfsuitedemo/main.cpp
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-20_14:54:27
 * @brief
 *
 **/

#include <string>
#include <vector>
#include <gflags/gflags.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "glog/logging.h"
#include "crfsuite.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace chatopera::bot::crfsuite;

DEFINE_string(trainfile, "", "Training File path.");
DEFINE_string(modelfile, "", "Training Model path.");

int main(int argc, char *argv[]) {
  // 解析命令行参数
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  // 初始化日志库
  google::InitGoogleLogging(argv[0]);
  VLOG(3) << "trainfile: " << FLAGS_trainfile << ", modelfile: " << FLAGS_modelfile;

  Trainer trainer;
  // https://python-crfsuite.readthedocs.io/en/latest/pycrfsuite.html#pycrfsuite.Trainer.select
  CHECK(trainer.select("lbfgs", "crf1d")) << "Trainer select fails";
  trainer.init();
  std::vector<string> params = trainer.params();

  for(std::vector<string>::iterator it = params.begin(); it != params.end(); it++) {
    VLOG(3) << "param: " << *it << " : " << trainer.get(*it);
  }

  // more about these hyper params
  // http://www.chokkan.org/software/crfsuite/manual.html#idp8849147120
  trainer.set("feature.minfreq", "0.000000");
  trainer.set("feature.possible_states", "0");
  trainer.set("feature.possible_transitions", "0");
  trainer.set("c1", "0.000000");
  trainer.set("c2", "1.000000");
  trainer.set("max_iterations", "2147483647");
  trainer.set("num_memories", "6");
  trainer.set("epsilon", "0.000010");
  trainer.set("period", "10");
  trainer.set("delta", "0.000010");
  trainer.set("linesearch", "MoreThuente");
  trainer.set("max_linesearch", "20");

  trainer.read_data(FLAGS_trainfile);

  VLOG(3) << "Statistics the data set(s):"
          << "\n Number of instances: " << (*trainer.pdata()).num_instances
          << "\n Number of items: " << crfsuite_data_totalitems(trainer.pdata())
          << "\n Number of attributes: " << (*trainer.pdata()).attrs->num((*trainer.pdata()).attrs)
          << "\n Number of labels: " << (*trainer.pdata()).labels->num((*trainer.pdata()).labels);

  trainer.train(FLAGS_modelfile, -1);
}


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
