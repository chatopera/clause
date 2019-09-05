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

test.skip("Chatopera Clause#create new customdict", t => {
  let req = new ttypes.Data({
    customdict: {
      name: "电",
      chatbotID: "测试2"
    }
  });
  t.context.client.postCustomDict(req, (err, res) => {
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

test.skip("Chatopera Clause#update customdict", t => {
  let req = new ttypes.Data({
    customdict: {
      description: "电流",
      name: "电",
      chatbotID: "测试2"
    }
  });
  t.context.client.putCustomDict(req, (err, res) => {
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

test.skip("Chatopera Clause#get customdict", t => {
  let req = new ttypes.Data({
    page: 0,
    pagesize: 10,
    chatbotID: "test2"
  });
  t.context.client.getCustomDicts(req, (err, res) => {
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

test.skip("Chatopera Clause#del customdict", t => {
  let req = new ttypes.Data({
    customdict: {
      chatbotID: "test2",
      name: "car"
    }
  });
  t.context.client.delCustomDict(req, (err, res) => {
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

test.skip("Chatopera Clause#create words", t => {
  let req = new ttypes.Data({
    customdict: {
      name: "car",
    },
    msg:"uuuu;iii;xixixi",
    chatbotID: "test2"

  });
  t.context.client.putDictWord(req, (err, res) => {
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

test.skip("Chatopera Clause#get words", t => {
  let req = new ttypes.Data({
    customdict: {
      name: "天空",
    },
    chatbotID: "test2",
    page: 0,
    pagesize: 3
  });
  t.context.client.getDictWords(req, (err, res) => {
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

test.skip("Chatopera Clause#delete words", t => {
  let req = new ttypes.Data({
    customdict: {
      name: "天空",
    },
    msg: "good",
    chatbotID: "test2"
  });
  t.context.client.delDictWord(req, (err, res) => {
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

test.cb("Chatopera Clause#get all dicts", t => {
  let req = new ttypes.Data({
    chatbotID: "test2"
  });
  t.context.client.getDictnamesByChatbotID(req, (err, res) => {
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