const net = require("net");
const eol = require("os").EOL;
const crypto = require("crypto");

const HASH_ALG = "sha256";
const NAME_LEN = 10;

var srvr = net.createServer();
var clientList = [];

srvr.on("connection", function (client) {
  client.name = calcName(client.remoteAddress, client.remotePort);
  client.write("Welcome, " + client.name + eol);

  // First, add the client to clientList.
  clientList.push(client);

  // Next, add a listener to the client for a 'data' event.
  // This event means that the client typed something in.
  // Broadcast that message to all **other** clients in clientList.
  client.on("data", function (data) {
    let msg = data.toString().trim();
    if (!msg) return;

    if (msg === "\\list") {
      let others = clientList.filter(c => c !== client).map(c => c.name);
      client.write("Other users: " + (others.length > 0 ? others.join(", ") : "None") + eol);
    } else if (msg.startsWith("\\rename ")) {
      let newName = msg.substring(8).trim();
      if (newName) {
        let oldName = client.name;
        client.name = newName;
        client.write("Name changed to " + client.name + eol);
        clientList.forEach(c => {
          if (c !== client) c.write("* " + oldName + " is now known as " + client.name + eol);
        });
      }
    } else if (msg.startsWith("\\private ")) {
      let parts = msg.split(" ", 3);
      if (parts.length >= 3) {
        let recipientName = parts[1];
        let privateMsg = msg.substring("\\private ".length + recipientName.length + 1);
        let recipient = clientList.find(c => c.name === recipientName);
        if (recipient) {
          recipient.write("(Private) " + client.name + ": " + privateMsg + eol);
        } else {
          client.write("User " + recipientName + " not found." + eol);
        }
      }
    } else {
      clientList.forEach(function (c) {
        if (c !== client) {
          c.write(client.name + ": " + msg + eol);
        }
      });
    }
  });

  client.on("end", function () {
    let index = clientList.indexOf(client);
    if (index !== -1) {
      clientList.splice(index, 1);
    }
  });

  client.on("error", function (err) {
    let index = clientList.indexOf(client);
    if (index !== -1) {
      clientList.splice(index, 1);
    }
  });
});

function calcName(remoteAddress, remotePort) {
  let data = remoteAddress + ":" + remotePort;
  let h = crypto.createHash(HASH_ALG).update(data).digest("hex");
  return h.substring(0, NAME_LEN);
}

srvr.listen(9000);
