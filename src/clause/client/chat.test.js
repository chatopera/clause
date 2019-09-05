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

/**
 * 通过chatbotID等信息获得session信息
 */
test.skip("Chatopera Clause#get session", t => {
  t.context.client.getSession(
    {
      session: {
        id: "4B254E38FB8B0FF65778C648B03C4A29"
      }
    },
    (err, result) => {
      if (err) {
        t.pass(err);
        t.end();
        return;
      }
      debug("create session: %j", result);
      t.pass();
      t.end();
    }
  );
});

test.skip("Chatopera Clause#create session", t => {
  t.context.client.putSession(
    {
      session: {
        chatbotID: "mychatbotID2",
        uid: "node",
        channel: "testsdk",
        branch: "dev"
      }
    },
    (err, result) => {
      if (err) {
        t.pass(err);
        t.end();
        return;
      }
      debug("create session: %j", result);
      t.pass();
      t.end();
    }
  );
});

test.skip("Chatopera Clause#chat", t => {
  t.context.client.chat(
    {
      session: {
        id: "B2767BE50BAC418E8AEB0DDD3E000000"
      },
      message: {
        textMessage: "我想做计程车"
        // textMessage: "我想从中国首都打计程车"
        // textMessage: "我想叫计程车"
        // textMessage: "我想叫个pizza"
      }
    },
    (err, result) => {
      if (err) {
        t.pass(err);
        t.end();
        return;
      }
      debug("chat: %j", result);
      t.pass();
      t.end();
    }
  );
});

test.cb("Chatopera Clause#chat sysdicts", t => {
  t.context.client.putSession(
    {
      session: {
        chatbotID: "mychatbotID3",
        uid: "node",
        channel: "testsdk",
        branch: "dev"
      }
    },
    (err, result) => {
      if (err) {
        t.pass(err);
        t.end();
        return;
      }
      debug("new session: %s", result.session.id);
      // 进行对话
      t.context.client.chat(
        {
          session: {
            id: result.session.id
          },
          message: {
            // textMessage: "郭德纲明天帮我送一个青柠pizza"
            textMessage: "郭德纲明天帮我送一个芒果pizza"
          }
        },
        (err, result) => {
          if (err) {
            t.pass(err);
            t.end();
            return;
          }
          debug("chat: %j", result);
          t.pass();
          t.end();
        }
      );
    }
  );
});
