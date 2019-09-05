/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-05-22_14:22:10
 * @brief
 *
 **/

#ifndef __CHATOPERA_BOT_SYSDICTS_HANLDER
#define __CHATOPERA_BOT_SYSDICTS_HANLDER

#include <map>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <boost/scoped_ptr.hpp>

#include "ilac.h"
#include "marcos.h"
#include "glog/logging.h"
#include "serving/Serving.h"
#include "serving/server_types.h"

using namespace std;

namespace chatopera {
namespace bot {
namespace sysdicts {

class ServingHandler : virtual public ServingIf {
 public:
  ServingHandler();
  ~ServingHandler();
  bool init();
  void label(Data& _return, const Data& request);

 protected:
 private:
  void* _g_lac_handle;    // lac labeling obj pointer
};

} // namespace sysdicts
} // namespace bot
} // namespace chatopera

#endif


/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */