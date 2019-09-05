#include "gtest/gtest.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include "redis.h"

using namespace chatopera::redis;

TEST(RedisTest, INIT) {
  LOG(INFO) << "INIT";

  Redis* redis = Redis::getInstance();

  // 初始化
  EXPECT_TRUE(redis->init("192.168.2.219", 8050, 6, "myredispass2025")) << "Fail to init.";

  // 测试 string
  redis->set("hello", "world");
  string result = redis->get("hello");
  LOG(INFO) << "result=" << result;
}

TEST(RedisTest, LIST) {
  LOG(INFO) << "LIST";

  Redis* redis = Redis::getInstance();

  // 初始化
  EXPECT_TRUE(redis->init("192.168.2.219", 8050, 6, "myredispass2025")) << "Fail to init.";
  // 测试 list
  vector<string> vec = {"1", "2", "3", "4"};
  redis->setList("foo", vec);
  vector<string> vecResult = redis->list("foo");

  for(int i = 0; i < vecResult.size(); i++) {
    LOG(INFO) << vecResult[i] << " ";
  }
}

TEST(RedisTest, RPUSH) {
  LOG(INFO) << "RPUSH";

  Redis* redis = Redis::getInstance();

  // 初始化
  EXPECT_TRUE(redis->init("192.168.2.219", 8050, 6, "myredispass2025")) << "Fail to init.";

  redis->rpush("foo", "1");
  // 测试 list
  vector<string> vecResult = redis->list("foo");

  for(int i = 0; i < vecResult.size(); i++) {
    LOG(INFO) << vecResult[i] << " ";
  }
}

TEST(RedisTest, RPOP) {
  LOG(INFO) << "RPOP";

  Redis* redis = Redis::getInstance();

  // 初始化
  EXPECT_TRUE(redis->init("192.168.2.219", 8050, 6, "myredispass2025")) << "Fail to init.";

  LOG(INFO) << redis->rpop("foo");
}

TEST(RedisTest, DEL) {
  LOG(INFO) << "DEL";

  Redis* redis = Redis::getInstance();

  // 初始化
  EXPECT_TRUE(redis->init("192.168.2.219", 8050, 6, "myredispass2025")) << "Fail to init.";

  redis->del("foo");
}