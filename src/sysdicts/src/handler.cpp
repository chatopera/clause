/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-05-22_14:22:23
 * @brief
 *
 **/

#include "handler.h"
#include "gflags/gflags.h"
#include "basictypes.h"

using namespace chatopera::bot::sysdicts;
using namespace std;

DECLARE_string(lac_conf_dir);

namespace chatopera {
namespace bot {
namespace sysdicts {

ServingHandler::ServingHandler() {
};

ServingHandler::~ServingHandler() {
  lac_destroy(_g_lac_handle);
};

bool ServingHandler::init() {
  VLOG(2) << __func__ << " init with config dir: " << FLAGS_lac_conf_dir;

  _g_lac_handle = lac_create(FLAGS_lac_conf_dir.c_str());

  if(_g_lac_handle == NULL) {
    VLOG(3) << __func__ << " init fails.";
    return false;
  }

  return true;
};

/**
 * 设置异常返回
 */
inline void rc_and_error(Data& data, const int rc, const std::string& error) {
  CHECK(rc != 0) << "[Error] error response code should not be 0, because 0 is for success.";
  data.rc = rc;
  data.error = error;
  data.__isset.rc = true;
  data.__isset.error = true;
}

/**
 * 保存实体信息到回复
 */
inline bool set_entity_into_response(Data& _return,
                                     const vector<Entity>& entities,
                                     const bool& fetchall,
                                     const string& dictname,
                                     const string& val) {
  VLOG(3) << __func__ << " dictname: " << dictname << ", value: " << val;

  if(fetchall) {
    Entity entity;
    entity.dictname = dictname;
    entity.val = val;
    entity.__isset.dictname = true;
    entity.__isset.val = true;
    _return.entities.push_back(entity);
    _return.__isset.entities = true;
    return true;
  } else {
    for(vector<Entity>::const_iterator it = entities.begin(); it != entities.end(); it++) {
      if(it->dictname == dictname) {
        // 多个词典属于同一个意图的不同槽位，有被覆盖的危险
        bool settledown = false;

        if( (*it).__isset.slotname &&
            !((*it).slotname.empty()) &&
            _return.__isset.entities &&
            _return.entities.size() > 0) {
          for(vector<Entity>::iterator it2 = _return.entities.begin(); it2 != _return.entities.end(); it2++) {
            if(it2->dictname == dictname &&
                (it2->slotname == it->slotname)) {
              settledown = true;
              break;
            }
          }
        }

        if(settledown) {
          continue;
        } else {
          Entity entity;
          entity.val = val;
          entity.dictname = dictname;

          if(((*it).__isset.slotname) && !((*it).slotname.empty())) {
            entity.slotname = it->slotname;
            entity.__isset.slotname = true;
          }

          entity.__isset.dictname = true;
          entity.__isset.val = true;
          _return.entities.push_back(entity);
          _return.__isset.entities = true;
          return true;
        }
      }
    }
  }

  return false;
}


void ServingHandler::label(Data& _return, const Data& request) {
  VLOG(3) << __func__ << " request " << FromThriftToUtf8DebugString(&request);

  if(request.__isset.query) {
    void* lac_buff = lac_buff_create(_g_lac_handle);

    if (lac_buff == NULL) {
      VLOG(2) << __func__ << " create lac_buff error";
      rc_and_error(_return, 12, "Can not create lac buff.");
      return;
    }

    bool fetchall = true;     // 返回所有结果

    if(request.__isset.entities && request.entities.size() > 0) {
      fetchall = false;
    }

    tag_t *results = new tag_t[CL_BOT_SYSDICT_MAX_RESULT_LEN];

    int result_num = lac_tagging(_g_lac_handle,
                                 lac_buff,
                                 request.query.c_str(),
                                 results,
                                 CL_BOT_SYSDICT_MAX_RESULT_LEN);

    if (result_num < 0) {
      VLOG(2) << __func__ << " tagging failed query: " << request.query;
      rc_and_error(_return, 13, "Can not tagging query.");
    }

    for (int i = 0; i < result_num; i++) {
      std::string val = request.query.substr(results[i].offset,
                                             results[i].length);

      VLOG(3) << __func__ << " parsed: " << val << " " << results[i].type;
      string type(results[i].type);

      if(type == CL_SYSDICT_LABEL_LOC) {
        set_entity_into_response(_return,
                                 request.entities,
                                 fetchall,
                                 CL_SYSDICT_DICT_LOC,
                                 val);
      } else if(type == CL_SYSDICT_LABEL_ORG) {
        set_entity_into_response(_return,
                                 request.entities,
                                 fetchall,
                                 CL_SYSDICT_DICT_ORG,
                                 val);
      } else if(type == CL_SYSDICT_LABEL_TIME) {
        set_entity_into_response(_return,
                                 request.entities,
                                 fetchall,
                                 CL_SYSDICT_DICT_TIME,
                                 val);
      } else if(type == CL_SYSDICT_LABEL_PER) {
        set_entity_into_response(_return,
                                 request.entities,
                                 fetchall,
                                 CL_SYSDICT_DICT_PER,
                                 val);
      }
    }

    _return.rc = 0;
    _return.__isset.rc = true;
  } else {
    rc_and_error(_return, 11, "Invalid params, query is required.");
  }

  VLOG(3) << __func__ << " response " << FromThriftToUtf8DebugString(&_return);
};

} // sysdicts
} // bot
} // chatopera