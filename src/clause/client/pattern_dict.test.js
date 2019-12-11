require("dotenv").config();
const curdir = __dirname;
const path = require("path");
const fs = require("fs");
const debug = require("debug")("chatopera:bot:clause:test");
const test = require("ava");

const thrift = require("thrift");
const Serving = require("./gen-nodejs/Serving");
const ttypes = require("./gen-nodejs/server_types");

var transport = thrift.TFramedTransport;
var protocol = thrift.TBinaryProtocol;

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

test("Test Pattern Dict#create dict", async () => {});
