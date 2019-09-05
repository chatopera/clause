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

test.cb("Chatopera Clause#create sysdict", t => {
  let req = new ttypes.Data({
    sysdict: {
      name: "@TIME",
      description: "时间",
      samples: "今天;下午一时"
    }
  });
  t.context.client.postSysDict(req, (err, res) => {
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

test.skip("Chatopera Clause#update sysdict", t => {
  let req = new ttypes.Data({
    sysdict: {
      name: "@home13",
      description: "home2",
      samples: "house2",
      active: true
    }
  });
  t.context.client.putSysDict(req, (err, res) => {
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

test.skip("Chatopera Clause#ref sysdict", t => {
  let req = new ttypes.Data({
    sysdict: {
      name: "@home13"
    },
    chatbotID: "test2"
  });
  t.context.client.refSysDict(req, (err, res) => {
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

test.skip("Chatopera Clause#unref sysdict", t => {
  let req = new ttypes.Data({
    sysdict: {
      name: "@home13"
    },
    chatbotID: "test2"
  });
  t.context.client.unrefSysDict(req, (err, res) => {
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

test.skip("Chatopera Clause#get sysdicts", t => {
  let req = new ttypes.Data({
    pagesize: 10,
    page: 1,
    chatbotID: "test2"
  });
  t.context.client.getSysDicts(req, (err, res) => {
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
