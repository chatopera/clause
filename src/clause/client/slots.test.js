require("dotenv").config();
const curdir = __dirname;
const path = require("path");
const fs = require("fs");
const thrift = require("thrift");
const Serving = require("./gen-nodejs/Serving");
const ttypes = require("./gen-nodejs/server_types");
const debug = require("debug")("chatopera:bot:clause:test");
const test = require("ava");
const _ = require("lodash");
const shortid = require("shortid");

var transport = thrift.TFramedTransport;
var protocol = thrift.TBinaryProtocol;

/**
 * Clear null value
 * @param {JSON} res
 */
function res2data(res) {
  let data = {};
  for (let x of _.keys(res)) {
    if (res[x] != null) {
      data[x] = res[x];
    }
  }
  return data;
}

test.before(t => {
  var connection = thrift.createConnection(
    process.env["SERVING_HOST"] || "192.168.2.219",
    process.env["SERVING_PORT"] || 8056,
    {
      transport: transport,
      protocol: protocol,
      max_attempts: 1000,
      retry_max_delay: 2000,
      connect_timeout: 6000
    }
  );

  connection.on("error", function(err) {
    console.log(err);
  });

  t.context.client = thrift.createClient(Serving, connection);
});

test.skip("Chatopera Clause#create new intent slot with customdict", t => {
  const chatbotID = "avtr001";
  t.context.client.postSlot(
    {
      // 关联意图
      intent: {
        chatbotID: chatbotID,
        name: "foo2"
      },
      // 关联自定义字典
      customdict: {
        chatbotID: chatbotID,
        name: "pizza"
      },
      // 槽位信息
      slot: {
        name: "PizzaTypes1",
        requires: true,
        question: "foo"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#create new intent slot with sysdict", t => {
  const chatbotID = "avtr001";
  t.context.client.postSlot(
    {
      // 关联意图
      intent: {
        chatbotID: chatbotID,
        name: "foo2"
      },
      // 关联自定义字典
      sysdict: {
        name: "pizza"
      },
      // 槽位信息
      slot: {
        name: "PizzaTypes2",
        requires: true,
        question: "foo"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#get slot details by name", t => {
  const chatbotID = "avtr001";
  t.context.client.getSlot(
    {
      // 关联意图
      intent: {
        chatbotID: chatbotID,
        name: "foo2"
      },
      // 槽位信息
      slot: {
        name: "PizzaTypes1"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#get slot details by id", t => {
  const chatbotID = "avtr001";
  t.context.client.getSlot(
    {
      // 槽位信息
      slot: {
        id: "A36EEC4ED39A486694B51C8A382CEC06"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#delete slot by id", t => {
  const chatbotID = "avtr001";
  t.context.client.delSlot(
    {
      // 槽位信息
      slot: {
        id: "A36EEC4ED39A486694B51C8A382CEC06"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#delete slot by name", t => {
  const chatbotID = "avtr001";
  t.context.client.delSlot(
    {
      // 关联意图
      intent: {
        chatbotID: chatbotID,
        name: "foo2"
      },
      // 槽位信息
      slot: {
        name: "PizzaTypes1"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#put slot with customdict", t => {
  const chatbotID = "avtr001";

  t.context.client.putSlot(
    {
      customdict: {
        chatbotID: chatbotID,
        name: "pizza"
      },
      // 槽位信息
      // 可选值如果不存在代表不更新
      slot: {
        id: "399FFA4AC2A8C733F097AAB0083C743F", // 必填
        name: "PizzaTypes3", // 可选
        requires: true, // 可选
        question: "fff" // 可选
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#put slot with sysdict", t => {
  const chatbotID = "avtr001";

  t.context.client.putSlot(
    {
      sysdict: {
        name: "pizza"
      },
      // 槽位信息
      // 可选值如果不存在代表不更新
      slot: {
        id: "399FFA4AC2A8C733F097AAB0083C743F", // 必填
        name: "PizzaTypes3", // 可选
        requires: false, // 可选
        question: "fff2" // 可选
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});

test.skip("Chatopera Clause#get slots", t => {
  const chatbotID = "avtr001";

  t.context.client.getSlots(
    {
      intent: {
        chatbotID: chatbotID,
        name: "foo2"
      }
    },
    (err, result) => {
      debug("result: %j", result);
      if (err) {
        debug("error: %j", err);
        t.pass(err);
        t.end();
      } else {
        t.pass();
        t.end();
      }
    }
  );
});
