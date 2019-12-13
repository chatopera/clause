/*
 Navicat Premium Data Transfer

 Source Server         : gamera.clause.dev
 Source Server Type    : MySQL
 Source Server Version : 50645
 Source Host           : 192.168.2.219:8055
 Source Schema         : clause

 Target Server Type    : MySQL
 Target Server Version : 50645
 File Encoding         : 65001

 Date: 01/08/2019 23:14:49
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- Create Database
-- ----------------------------
CREATE DATABASE IF NOT EXISTS `clause` CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `clause`;

drop table if exists cl_bot_sysdict;
FLUSH TABLES cl_bot_sysdict;

drop table if exists cl_chat_msgs;
FLUSH TABLES cl_chat_msgs;

drop table if exists cl_chat_sessions;
FLUSH TABLES cl_chat_sessions;

drop table if exists cl_dev_vers;
FLUSH TABLES cl_dev_vers;

drop table if exists cl_dict_pattern;
FLUSH TABLES cl_dict_pattern;

drop table if exists cl_patten_checks;
FLUSH TABLES cl_patten_checks;

drop table if exists cl_dict_words;
FLUSH TABLES cl_dict_words;

drop table if exists cl_dicts;
FLUSH TABLES cl_dicts;

drop table if exists cl_intent_slots;
FLUSH TABLES cl_intent_slots;

drop table if exists cl_intent_utters;
FLUSH TABLES cl_intent_utters;

drop table if exists cl_intents;
FLUSH TABLES cl_intents;

drop table if exists cl_prod_vers;
FLUSH TABLES cl_prod_vers;


-- ----------------------------
-- Power Design Start
-- ----------------------------
/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2019/8/3 19:08:29                            */
/*==============================================================*/

/*==============================================================*/
/* Table: cl_bot_sysdict                                        */
/*==============================================================*/
create table cl_bot_sysdict
(
   id                   varchar(32) not null comment 'ID',
   chatbotID            varchar(50) not null comment '聊天机器人ID',
   dict_id              varchar(32) not null comment '关联词典ID',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期'
) default charset=utf8mb4 collate utf8mb4_general_ci;;

alter table cl_bot_sysdict comment '引用系统词典表';

alter table cl_bot_sysdict
   add primary key (id);

alter table cl_bot_sysdict
   add unique UNQ_REF_SYSDICT (chatbotID, dict_id);

/*==============================================================*/
/* Table: cl_chat_msgs                                          */
/*==============================================================*/
create table cl_chat_msgs
(
   id                   varchar(32) not null comment 'ID',
   session_id           varchar(32) comment '会话ID',
   chatbotID            varchar(50) not null comment '聊天机器人ID',
   type                 varchar(32) not null default 'textMessage' comment '消息类型',
   textMessage          varchar(500) comment '文本消息',
   audioMessage         varchar(500) comment '语音消息',
   videoMessage         varchar(500) comment '视频消息',
   imageMessage         varchar(500) comment '图片消息',
   direction            tinyint not null default 1 comment '消息发送方向',
   branch               tinyint not null default 1 comment '对话分支',
   sender               varchar(50) not null comment '发送者ID',
   receiver             varchar(50) not null comment '接收者ID',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_chat_msgs
   add primary key (id);

/*==============================================================*/
/* Table: cl_chat_sessions                                      */
/*==============================================================*/
create table cl_chat_sessions
(
   id                   varchar(32) not null comment 'ID',
   intent_id            varchar(32) comment '意图ID',
   chatbotID            varchar(32) not null comment '聊天机器人ID',
   uid                  varchar(32) not null comment '消费者ID，对话人标识',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '更新日期',
   entities             text comment '命名实体标识信息',
   resolved             tinyint not null default 0 comment '是否完成意图的槽位参数识别',
   channel              varchar(100) not null comment '对话渠道',
   branch               varchar(100) not null comment '对话分支'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_chat_sessions comment '对话会话管理';

alter table cl_chat_sessions
   add primary key (id);

/*==============================================================*/
/* Table: cl_dev_vers                                           */
/*==============================================================*/
create table cl_dev_vers
(
   id                   varchar(32) not null comment 'ID',
   chatbotID            varchar(50) not null comment '聊天机器人ID',
   version              varchar(32) not null comment '版本名',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   operator             varchar(200) comment '操作员用户名',
   published            tinyint not null default 1 comment '是否已经上线'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_dev_vers comment '聊天机器人调试版本';

alter table cl_dev_vers
   add primary key (id);

alter table cl_dev_vers
   add unique UNQ_DEV_VERSIONS_CHATBOTID_N_VERSION (chatbotID, version);

/*==============================================================*/
/* Table: cl_dict_pattern                                       */
/*==============================================================*/
create table cl_dict_pattern
(
   id                   varchar(32) not null comment '唯一标识',
   dict_id              varchar(32) comment '词典标识',
   createdate           timestamp default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '更新日期',
   patterns             text comment '正则表达式模式，支持多个，使用 \001 分隔',
   standard             varchar(32) comment '正则表达式标准'
);

alter table cl_dict_pattern comment '词典的正则表达式';

alter table cl_dict_pattern
   add primary key (id);

/*==============================================================*/
/* Table: cl_patten_checks                                      */
/*==============================================================*/
create table cl_patten_checks
(
   id                   varchar(32) not null comment '唯一标识',
   dict_id              varchar(32) not null comment '关联词典ID',
   createdate           timestamp default CURRENT_TIMESTAMP,
   input                text comment '输入文本',
   output               text comment '输出文本的JSON字符串'
);

alter table cl_patten_checks comment '正则表达式调试记录';

alter table cl_patten_checks
   add primary key (id);

/*==============================================================*/
/* Table: cl_dict_words                                         */
/*==============================================================*/
create table cl_dict_words
(
   word                 varchar(100) not null comment '标准词',
   dict_id              varchar(32) not null comment '自定义字典ID',
   synonyms             varchar(1000) comment '近义词集合，多个同义词使用分号分隔',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '上次更新日期',
   tag                  varchar(10) comment '词汇词性',
   lang                 varchar(10) comment '词汇所属语言'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_dict_words comment '用户自定义字典词条';

alter table cl_dict_words
   add primary key (word, dict_id);

/*==============================================================*/
/* Table: cl_dicts                                              */
/*==============================================================*/
create table cl_dicts
(
   id                   varchar(32) not null comment '唯一标识',
   name                 varchar(32) not null comment '名称',
   chatbotID            varchar(50) not null comment '聊天机器人ID',
   description          varchar(100) comment '描述',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '上次更新日期',
   samples              varchar(200) comment '示例词条',
   builtin              tinyint not null default 0 comment '是否是系统词典',
   active               tinyint default 1 comment '是否启用，只针对系统词典',
   vendor               varchar(32) comment '发布者', /* 目前属于保留字段，代表词典的发布者，系统词典的发布者为“Chatopera” */
   type                 varchar(32) comment '词典类型：词汇表(vocab)，正则表达式(regex)，机器学习(ml)。'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_dicts comment '词典';

alter table cl_dicts
   add primary key (id);

alter table cl_dicts
   add unique UNQ_CUSTOM_DICT_CHATBOTID_N_NAME (name, chatbotID);

-- ----------------------------
-- Records of cl_dicts
-- ----------------------------
BEGIN;
INSERT INTO `cl_dicts` VALUES ('017CCCA93E9337A66BD5E109D0AD42AA', '@PER', '@BUILTIN', '人名', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '郭德纲;于谦', 1, 1, "Chatopera", "ml");
INSERT INTO `cl_dicts` VALUES ('11BDC6794CA48E48F0D20DEED06DA5A1', '@TIME', '@BUILTIN', '时间', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '今天;下午一时', 1, 1, "Chatopera", "ml");
INSERT INTO `cl_dicts` VALUES ('3F49212119A70BE7424FBE43D08DC5A1', '@LOC', '@BUILTIN', '地名', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '北京市;东京', 1, 1, "Chatopera", "ml");
INSERT INTO `cl_dicts` VALUES ('7ABF83AE0492E2E8BC939307D07DB5A1', '@ORG', '@BUILTIN', '组织机构', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '北京华夏春松科技有限公司', 1, 1, "Chatopera", "ml");
COMMIT;

/*==============================================================*/
/* Table: cl_intent_slots                                       */
/*==============================================================*/
create table cl_intent_slots
(
   id                   varchar(32) not null comment 'ID',
   intent_id            varchar(32) not null comment '意图ID',
   name                 varchar(32) not null comment '槽位名称',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '更新日期',
   dict_id              varchar(32) not null comment '关联字典ID',
   requires             tinyint not null default 1 comment '是否必须',
   question             varchar(800) comment '追问'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_intent_slots comment '意图的槽位';

alter table cl_intent_slots
   add primary key (id);

alter table cl_intent_slots
   add unique AK_UNQ_INTENT_SLOT_INTENTID_N_NAME (intent_id, name);

/*==============================================================*/
/* Table: cl_intent_utters                                      */
/*==============================================================*/
create table cl_intent_utters
(
   id                   varchar(32) not null comment '说法ID',
   intent_id            varchar(32) comment '意图ID',
   utterance            varchar(800) not null comment '说法',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '更新日期'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_intent_utters comment '意图的说法';

alter table cl_intent_utters
   add primary key (id);

/*==============================================================*/
/* Table: cl_intents                                            */
/*==============================================================*/
create table cl_intents
(
   id                   varchar(32) not null,
   chatbotID            varchar(50) not null comment '聊天机器人ID',
   name                 varchar(32) not null comment '名称',
   description          varchar(50) comment '意图中文名',
   createdate           timestamp not null default CURRENT_TIMESTAMP comment '创建日期',
   updatedate           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP comment '上次更新日期'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_intents comment '聊天机器人意图';

alter table cl_intents
   add primary key (id);

alter table cl_intents
   add unique UNQ_INTENT_CHABOTID_N_NAME (chatbotID, name);

/*==============================================================*/
/* Table: cl_prod_vers                                          */
/*==============================================================*/
create table cl_prod_vers
(
   id                   varchar(32) not null comment 'ID',
   chatbotID            varchar(50) not null comment '聊天机器人ID',
   version              varchar(32) not null comment '版本号',
   createdate           timestamp not null comment '创建日期',
   operator             varchar(200) comment '操作员',
   active               tinyint default 0 comment '手否在线上运行',
   notes                text comment '描述',
   latest               tinyint not null default 0 comment '是否是当前线上工作版本'
) default charset=utf8mb4 collate utf8mb4_general_ci;

alter table cl_prod_vers comment '聊天机器人上线版本';

alter table cl_prod_vers add primary key (id);

alter table cl_bot_sysdict add constraint FK_REF_REF_SYSDICT_DICTID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;

alter table cl_chat_msgs add constraint FK_REF_CHAT_MESSAGE_SESSIONID foreign key (session_id)
      references cl_chat_sessions (id) on delete restrict on update restrict;

alter table cl_chat_sessions add constraint FK_REF_CHAT_STATE_TACKING foreign key (intent_id)
      references cl_intents (id) on delete restrict on update restrict;

alter table cl_dict_words add constraint FK_REF_CUSTOMDICT_WORDS_DICT_ID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;

alter table cl_intent_slots add constraint FK_REF_INTENT_SLOT_DICT_ID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;

alter table cl_intent_slots add constraint FK_REF_INTENT_SLOT_INTENT_ID foreign key (intent_id)
      references cl_intents (id) on delete restrict on update restrict;

alter table cl_intent_utters add constraint FK_REF_INTENT_UTTERS_INTENT_ID foreign key (intent_id)
      references cl_intents (id) on delete restrict on update restrict;

alter table cl_dict_pattern add constraint FK_REF_DICT_PATTERN_DICT_ID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;

alter table cl_patten_checks add constraint FK_REF_DICT_PATTERN_CHECKS_DICT_ID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;

-- ----------------------------
-- Power Design End
-- ----------------------------



SET FOREIGN_KEY_CHECKS = 1;
