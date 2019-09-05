/**
 * ActiveMQ lib
 */
require("dotenv").config();
const debug = require("debug")("chatopera:clause:activemq");
const Stomp = require("stomp-client");

const _client = new Stomp(
  process.env["ACTIVEMQ_IP"],
  process.env["ACTIVEMQ_PORT"],
  process.env["ACTIVEMQ_USER"],
  process.env["ACTIVEMQ_PASS"],
  "1.0" /* protocolVersion */,
  null /* vhost */,
  {
    retries: 20,
    delay: 1000
  }
);

_client.on("error", function(err) {
  console.log("activemq connection error", err);
});

_client.on("reconnect", function(sessionId) {
  debug("reconnect sessionId %s successfully", sessionId);
});

class ActiveMQ {
  /**
   * Connect to Broker
   */
  connect() {
    console.log(
      `activemq: start to connect ${process.env["ACTIVEMQ_IP"]}:${
        process.env["ACTIVEMQ_PORT"]
      } with username [${process.env["ACTIVEMQ_USER"]}]  password [*****] ...`
    );
    return new Promise((resolve, reject) => {
      _client.connect(
        function(sessionId) {
          console.log("activemq: connected sessionId [%s]", sessionId);
          resolve(sessionId);
        },
        function(err) {
          console.log("activemq: fail to connect, error", err);
          reject(err);
        }
      );
    });
  }

  /**
   * The meaning of queue names is not defined by the STOMP spec,
   * but by the Broker. However, with ActiveMQ, they should begin
   * with "/queue/" or with "/topic/", see STOMP1.0 for more detail.
   * @param  {[type]}   destination [description]
   * @param  {Function} cb          [description]
   * @return {[type]}               [description]
   */
  subscribe(destination, cb) {
    debug("subscribe: %s", destination);
    return new Promise((resolve, reject) => {
      if (
        destination.startsWith("/queue/") ||
        destination.startsWith("/topic/")
      ) {
        _client.subscribe(destination, cb);
        resolve();
      } else {
        reject("Wrong topic name, should begin with /queue/ or /topic/");
      }
    });
  }

  /**
   * queue to unsubscribe from
   * @param  {[type]} destination [description]
   * @return {[type]}             [description]
   */
  unsubscribe(destination) {
    debug("unsubscribe %s", destination);
    return new Promise((resolve, reject) => {
      if (
        destination.startsWith("/queue/") ||
        destination.startsWith("/topic/")
      ) {
        _client.unsubscribe(destination);
        resolve();
      } else {
        reject("Wrong topic name, should begin with /queue/ or /topic/");
      }
    });
  }

  /**
   * queue to publish to
   * @param  {[type]} destination queue to publish to
   * @param  {[type]} message     message to publish, a string or buffer
   * @return {[type]}             [description]
   */
  publish(destination, message) {
    debug("publish [%s]: %s", destination, message);
    return new Promise((resolve, reject) => {
      if (
        destination.startsWith("/queue/") ||
        destination.startsWith("/topic/")
      ) {
        _client.publish(destination, message, {
          "amq-msg-type": "text"
        });
        resolve();
      } else {
        reject("Wrong topic name, should begin with /queue/ or /topic/");
      }
    });
  }
}

module.exports = exports = new ActiveMQ();
