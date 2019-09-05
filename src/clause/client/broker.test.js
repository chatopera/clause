/**
 * Test Broker Messaging
 */
const debug = require("debug")("chatopera:test:broker");
const test = require("ava");
const activemq = require("./services/activemq.service");

function sleep(seconds) {
  return new Promise((resolve, reject) => {
    setTimeout(() => {
      resolve();
    }, seconds * 1000);
  });
}

test("Push message to ActiveMQ", async t => {
  await activemq.connect();
  await activemq.publish(
    "/queue/chatopera/bot/clause",
    JSON.stringify({
      foo: "bar"
    })
  );

  await sleep(3);
  t.pass();
});
