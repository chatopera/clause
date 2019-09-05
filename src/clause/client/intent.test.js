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

test.skip("Chatopera Clause#create new intent", t => {
  let req = new ttypes.Data();
  req["intent"] = {
    // name and chatbotID are both required.
    name: shortid.generate(),
    chatbotID: "test1"
  };
  t.context.client.putIntent(req, (err, res) => {
    if (err) {
      t.pass(err);
      t.end();
    } else {
      var res = res2data(res);
      debug("response : %j", res);
      if (res.rc == 0) {
        t.pass();
      } else {
        t.pass("error on create");
      }
      t.end();
    }
  });
});

test.skip("Chatopera Clause#update intent", t => {
  let req = new ttypes.Data();
  req["intent"] = {
    id: "96837F77BFB160C45AAA851300000000",
    description: "describe3 ..."
  };
  t.context.client.putIntent(req, (err, res) => {
    if (err) {
      t.pass(err);
      t.end();
    } else {
      //   var res = res2data(res);
      debug("response %j", res);
      t.pass();
      t.end();
    }
  });
});

test.skip("Chatopera Clause#get intent details by id", t => {
  let req = new ttypes.Data();
  req["id"] = "96837F77BFB160C45AAA851300000000";

  t.context.client.getIntent(req, (err, res) => {
    if (err) {
      t.pass(err);
      t.end();
    } else {
      var res = res2data(res);
      debug("response %j", res);
      t.pass();
      t.end();
    }
  });
});

test.skip("Chatopera Clause#get intent details by chatbotID and name", t => {
  let req = new ttypes.Data({
    intent: {
      chatbotID: "test1",
      name: "gyShafwcC"
    }
  });

  t.context.client.getIntent(req, (err, res) => {
    if (err) {
      t.pass(err);
      t.end();
    } else {
      var res = res2data(res);
      debug("response %j", res);
      t.pass();
      t.end();
    }
  });
});

test.skip("Chatopera Clause#get intents list", t => {
  let req = new ttypes.Data({
    page: 1,
    pagesize: 3,
    chatbotID: "test1"
  });
  t.context.client.getIntents(req, (err, res) => {
    if (err) {
      t.pass(err);
      t.end();
    } else {
      var res = res2data(res);
      debug("response %j", res);
      t.pass();
      t.end();
    }
  });
});

test.cb("Chatopera Clause#delete intent by chatbotID and name", t => {
  let req = new ttypes.Data({
    intent: {
      name: "gyShafwcC",
      chatbotID: "test1"
    }
  });
  t.context.client.delIntent(req, (err, res) => {
    if (err) {
      t.pass(err);
      t.end();
    } else {
      var res = res2data(res);
      debug("response %j", res);
      t.pass();
      t.end();
    }
  });
});
