-- -----------------------------------------------------------------------------
-- Author       Hai Liang Wang
-- Created      2019-12-14_10:14:50
-- File         /Users/hain/chatopera/clause/src/clause/config/sql/upgrade/001.upgrade_add_patterndict.sql
-- Purpose      Enable pattern dict features
-- Copyright © 2019, Chatopera Inc. <https://www.chatopera.com>, All Rights Reserved
-- -----------------------------------------------------------------------------
-- Modification History
--
-- 14/12/19  Hai Liang Wang 
--      A comprehensive description of the changes. The description may use as 
--      many lines as needed.
-- -----------------------------------------------------------------------------

-- ----------------------------
-- Add Columns
-- ----------------------------
SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

USE `clause`;

-- ----------------------------
-- Add Columns
-- ----------------------------
alter table cl_dicts add vendor varchar(32) comment '发布者'; /* 目前属于保留字段，代表词典的发布者，系统词典的发布者为“Chatopera” */
alter table cl_dicts add type varchar(32) comment '词典类型：词汇表(vocab)，正则表达式(regex)，机器学习(ml)。';

update cl_dicts set type = 'vocab' where builtin = '0';
update cl_dicts set type = 'ml', vendor = 'Chatopera' where builtin = '1';

-- ----------------------------
-- Create Tables
-- ----------------------------

/*==============================================================*/
/* Table: cl_dict_pattern                                       */
/*==============================================================*/
create table if not exists cl_dict_pattern
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
create table if not exists  cl_patten_checks
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

-- ----------------------------
-- Add Keys
-- ----------------------------
alter table cl_dict_pattern add constraint FK_REF_DICT_PATTERN_DICT_ID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;

alter table cl_patten_checks add constraint FK_REF_DICT_PATTERN_CHECKS_DICT_ID foreign key (dict_id)
      references cl_dicts (id) on delete restrict on update restrict;
