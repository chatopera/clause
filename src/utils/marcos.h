/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file /Users/hain/chatopera/chatopera.io/clause/src/clause/src/marcos.h
 * @author Hai Liang Wang(hain@chatopera.com)
 * @date 2019-08-23_13:30:02
 * @brief
 *  Advanced System settings
 **/

#ifndef __CHATOPERA_BOT_CLAUSE_MARCOS__
#define __CHATOPERA_BOT_CLAUSE_MARCOS__

#define CL_SYSDICT_CHATBOT_ID "@BUILTIN"
#define CL_INTENTS_MAX_NUMBER_PERBOT 100
#define CL_SLOTS_MAX_NUMBER_PERINTENT 100
#define CL_UTTER_MAX_NUMBER_PERINTENT 200
#define CL_CUSTOMDICT_MAX_NUMBER 500
#define CL_CUSTOMWORD_MAX_NUMBER 10000
#define CL_MYDICTS_MAX_NUMBER 1000

#define CL_BOT_BRANCH_DEV "dev"
#define CL_BOT_BRANCH_PRO "pro"

/**
 * 词典类型
 */
// 正则表达式
#define CL_DICT_TYPE_PATTERN "regex"
// 机器学习
#define CL_DICT_TYPE_ML      "ml"
// 词汇表
#define CL_DICT_TYPE_VOCAB   "vocab"
// 正则表达式词典
#define CL_DICT_PATTERN_STANDARD "PCRE"
// 分词器中，正则表达式term前缀
#define CL_DICT_PATTERN_PREFIX  "#"

#define CL_BOT_PRO_STATUS_ONLINE "online"
#define CL_BOT_PRO_STATUS_OFFLINE "offline"

// tags from https://github.com/baidu/lac
#define CL_SYSDICT_LABEL_LOC "LOC"
#define CL_SYSDICT_DICT_LOC "@LOC"
#define CL_SYSDICT_LABEL_ORG "ORG"
#define CL_SYSDICT_DICT_ORG "@ORG"
#define CL_SYSDICT_LABEL_TIME "TIME"
#define CL_SYSDICT_DICT_TIME "@TIME"
#define CL_SYSDICT_LABEL_PER "PER"
#define CL_SYSDICT_DICT_PER "@PER"

/**
 * 会话
 */
// 最大空闲周期
#define CL_CHATSESSION_MAX_IDLE_PERIOD 3600

/**
 * BOT对话模型的不同状态
 */
// BOT构建状态：训练中
#define CL_CHATBOT_BUILD_TRAINING "training"
// BOT构建状态：训练完成
#define CL_CHATBOT_BUILD_DONE     "done"
// BOT构建状态：训练失败
#define CL_CHATBOT_BUILD_FAIL     "fail"
// BOT构建状态：刚刚创建
#define CL_CHATBOT_BUILD_EMTPY    "empty"
// BOT构建状态：取消训练
#define CL_CHATBOT_BUILD_CANCAL   "cancelled"

#endif

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */