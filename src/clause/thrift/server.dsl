/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

namespace cpp chatopera.bot.clause
namespace java com.chatopera.clause
namespace csharp com.chatopera.clause
namespace py clause
namespace go clause

typedef string Timestamp

/**
 * 引用系统词典关联
 */
struct BotSysdict {
    1: optional string chatbotID;
    2: optional string dict_id;
    3: optional string id;
    4: optional Timestamp createdate;
}

/**
 * 聊天消息
 */
struct ChatMessage {
    1: optional string session_id;
    2: optional string chatbotID;
    3: optional string type;
    // direction 1: BOT发送的消息；2: 消费者发送的消息
    4: optional i32 direction;
    // branch 1: 调试分支；2: 生产环境分支
    5: optional i32 branch;
    6: optional string sender;
    7: optional string receiver;
    8: optional Timestamp createdate;
    9: optional string id;
    10: optional string textMessage;
    11: optional string audioMessage;
    12: optional string videoMessage;
    13: optional string imageMessage;
    14: optional list<string> terms;
    15: optional list<string> tags;
    16: optional bool is_fallback;
    17: optional bool is_proactive;
}

/**
 * 命名实体标识
 */
struct Entity {
    1: optional string name;
    2: optional string val;
    3: optional bool requires;   // 是否必填，必填时有追问
    4: optional string dictname; // 词典名称
}

/**
 * 聊天会话
 */
struct ChatSession {
    1: optional string intent_name;
    2: optional string chatbotID;
    3: optional string uid;
    4: optional string channel;
    5: optional bool resolved;
    6: optional string id;
    7: optional list<Entity> entities;
    8: optional string branch;
    9: optional Timestamp createdate;
    10: optional Timestamp updatedate;
}

/**
 * 词条
 */
struct DictWord {
    1: optional string word;
    2: optional string dict_id;
    3: optional string synonyms;
    4: optional string tag;
    5: optional string lang;
    6: optional Timestamp createdate;
    7: optional Timestamp updatedate;
}

/**
 * 词典
 */
struct Dict {
    1: optional string name;
    2: optional string chatbotID;
    3: optional bool builtin;          // 是否是系统词典
    4: optional bool active;           // 是否活跃，和系统词典相关
    5: optional string id;
    6: optional string description;
    7: optional string samples;        
    8: optional Timestamp createdate;
    9: optional Timestamp updatedate;
    10: optional bool referred;        // 是否被引用，和系统词典相关
}

/**
 * 自定义词典
 */
typedef Dict CustomDict;

/**
 * 系统词典
 */
typedef Dict SysDict;

/**
 * 意图
 */
struct Intent {
    1: optional string chatbotID;
    2: optional string name;
    3: optional string id;
    4: optional string description;
    5: optional Timestamp createdate;
    6: optional Timestamp updatedate;
}

/**
 * 意图槽位
 */
struct IntentSlot {
    1: optional string intent_id;
    2: optional string name;
    3: optional string dict_id;
    4: optional bool requires;
    5: optional string id;
    6: optional Timestamp createdate;
    7: optional Timestamp updatedate;
    8: optional string question;
    // 其他信息，关联关系表
    9: optional Dict dict;
}

/**
 * 意图说法
 */
struct IntentUtter {
    1: optional string intent_id;
    2: optional string utterance;
    3: optional string id;
    4: optional Timestamp createdate;
    5: optional Timestamp updatedate;
}

/**
 * 上线版本
 */
struct ProdVersion {
    1: optional string chatbotID;
    2: optional string version;
    3: optional bool active;
    4: optional bool latest;
    5: optional string id;
    6: optional string operated;
    7: optional string notes;
    8: optional Timestamp createdate;
}

/**
 * 调试版本
 */
struct DevelopVersion {
    1: optional string chatbotID;
    2: optional string version;
    3: optional bool published;
    4: optional string id;
    5: optional Timestamp createdate;
    6: optional string operated;
}

/**
 * Clause Data Bundle
 */
struct Data {
    1: optional i32 rc;                   // 返回代码
    2: optional string error;             // 异常信息
    3: optional string msg;               // 成功返回的文本说明
    4: optional string id;                // 对象的ID
    5: optional string chatbotID;         // 机器人ID

    // 词典管理
    6: optional list<CustomDict> customdicts;
    7: optional list<SysDict> sysdicts;
    8: optional list<BotSysdict> botsysdicts;
    9: optional list<DictWord> dictwords;  // 词条
    10: optional CustomDict customdict;    // 自定义词典
    11: optional SysDict sysdict;          // 系统词典
    12: optional BotSysdict botsysdict;    // 系统词典关联
    13: optional DictWord dictword;        // 词条

    // 意图管理
    14: optional list<Intent> intents;
    15: optional list<IntentSlot> slots;
    16: optional list<IntentUtter> utters;
    17: optional Intent intent;           // 意图
    18: optional IntentSlot slot;         // 槽位
    19: optional IntentUtter utter;       // 说法
    // 版本
    20: optional list<DevelopVersion> devvers;
    21: optional list<ProdVersion> provers;
    22: optional DevelopVersion devver;   // 调试版本
    23: optional ProdVersion prover;      // 上线版本
    // 聊天管理
    24: optional list<ChatSession> sessions;
    25: optional list<ChatMessage> messages;
    26: optional ChatSession session;     // 会话
    27: optional ChatMessage message;     // 聊天消息
    // 分页信息
    28: optional i32 currpage;            // 当前页
    29: optional i32 totalpage;           // 全部页面
    30: optional i32 totalrows;           // 全部数据条数
    31: optional i32 pagesize;            // 每页数据条数
    32: optional i32 page;                // 页面索引
    33: optional string query;            // 查询条件
}

/**
 * API 
 */
service Serving {
    /**
     * 词典管理
     */
    // 自定义词典管理
    Data postCustomDict(1: Data request); // 创建自定义词典
    Data putCustomDict(1: Data request);  // 更新自定义词典
    Data getCustomDicts(1: Data request); // 获得自定义词典列表
    Data getCustomDict(1: Data request);  // 获得自定义词典详情
    Data delCustomDict(1: Data request);  // 删除自定义词典
    
    // 系统词典管理
    Data postSysDict(1: Data request);    // 创建系统字典
    Data putSysDict(1: Data request);     // 更新系统字典
    Data getSysDicts(1: Data request);    // 获得系统词典列表
    Data getSysDict(1: Data request);     // 获得系统词典详情
    Data refSysDict(1: Data request);     // 引用系统词典
    Data unrefSysDict(1: Data request);   // 取消引用系统字典
    Data myDicts(1: Data request);        // 通过chatbotID获取所有自定义词典和被引用的词典
    Data mySysdicts(1: Data request);     // 通过chatbotID获取所有被引用的词典

    // 词条管理
    Data putDictWord(1: Data request);    // 创建或更新词条
    Data getDictWords(1: Data request);   // 获得词条列表
    Data delDictWord(1: Data request);    // 删除词条
    Data hasDictWord(1: Data request);    // 检测一个词条的标准词是否唯一

    /**
     * 意图
     */
    // 意图管理
    Data postIntent(1: Data request);     // 创建意图
    Data putIntent(1: Data request);      // 更新意图
    Data getIntents(1: Data request);     // 获得意图列表
    Data getIntent(1: Data request);      // 获得意图详情
    Data delIntent(1: Data request);      // 删除意图

    // 意图说法管理
    Data postUtter(1: Data request);      // 创建说法
    Data putUtter(1: Data request);       // 更新说法
    Data getUtters(1: Data request);      // 获得说法列表
    Data getUtter(1: Data request);       // 获得说法详情
    Data delUtter(1: Data request);       // 删除说法

    // 意图槽位管理
    Data postSlot(1: Data request);       // 创建槽位
    Data putSlot(1: Data request);         // 更新槽位
    Data getSlots(1: Data request);        // 获得槽位列表
    Data getSlot(1: Data request);         // 获得槽位详情
    Data delSlot(1: Data request);         // 删除槽位

    /**
     * 对话管理
     */
    Data train(1: Data request);           // 训练语言模型
    Data status(1: Data request);          // 查看语言模型训练状态
    Data devver(1: Data request);          // 查看调试版本信息
    Data prover(1: Data request);          // 查看生产版本信息
    Data version(1: Data request);         // 查看调试及生产版本信息
    Data chat(1: Data request);            // 聊天
    Data online(1: Data request);          // 上线版本
    Data offline(1: Data request);         // 下线版本
    Data putSession(1: Data request);      // 创建或更新会话
    Data getSession(1: Data request);      // 获得会话信息
}