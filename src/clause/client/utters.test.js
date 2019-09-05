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

test.skip("Chatopera Clause#create new utter", t => {
  t.context.client.postUtter(
    {
      intent: {
        name: "foo2",
        chatbotID: "avtr001"
      },
      utter: {
        utterance: "I want a pizza2"
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

test.skip("Chatopera Clause#update utter", t => {
  t.context.client.putUtter(
    {
      utter: {
        id: "7D755331F68B082F85477410107F45C5",
        utterance: "I want a pizza2"
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

test.skip("Chatopera Clause#get utter detail", t => {
  t.context.client.getUtter(
    {
      utter: {
        id: "7D755331F68B082F85477410107F45C5"
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

test.skip("Chatopera Clause#delete utter", t => {
  t.context.client.delUtter(
    {
      utter: {
        id: "7D755331F68B082F85477410107F45C5"
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

test.cb("Chatopera Clause#get utters", t => {
  t.context.client.getUtters(
    {
      intent: {
        chatbotID: "avtr001",
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
