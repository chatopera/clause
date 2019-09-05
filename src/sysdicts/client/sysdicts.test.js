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
test.cb("Chatopera Sysdicts#label", t => {
  t.context.client.label(
    {
      query:
        "近日，第14届莫斯科航展拉开帷幕，在展出的一大批军用飞机中，苏-57无疑是关注的焦点，该机的出口型号苏-57E也将于本次航展上展出。",
      entities: [
        {
          slotname: "place",
          dictname: "@LOC"
        }
      ]
    },
    (err, result) => {
      if (err) {
        t.pass(err);
        t.end();
        return;
      }
      debug("label: %j", result);
      t.pass();
      t.end();
    }
  );
});
