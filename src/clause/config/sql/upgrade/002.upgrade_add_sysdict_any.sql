-- -----------------------------------------------------------------------------
-- Author       Hai Liang Wang
-- Created      2020-03-27_10:14:50
-- File         /Users/hain/chatopera/clause/src/clause/config/sql/upgrade/002.upgrade_add_sysdict_any.sql
-- Purpose      Enable pattern dict features
-- Copyright © 2019, Chatopera Inc. <https://www.chatopera.com>, All Rights Reserved
-- -----------------------------------------------------------------------------
-- Modification History
--
-- 27/03/20  Hai Liang Wang 
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
-- Records of cl_dicts
-- ----------------------------
BEGIN;
INSERT INTO `cl_dicts` VALUES ('017C12A93E9337A66BD5E109D0AD42AA', '@ANY', '@BUILTIN', '任意字符串', CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, '任意字符串', 1, 1, "Chatopera", "any");
COMMIT;