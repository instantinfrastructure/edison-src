Writing a client using iotkit-comm requires three fundamental steps:
1. Write a service query
2. Create a client that queries for the service
3. Run the client

#### Write a service query

A service query is a JSON string that specifies the attributes of the service a client wishes to connect to. Place
the following service query in the file `server-query.json`:

```json
{
  "name": "/ndg/temperature/cpuTemp",
  "type": {
    "name": "zmqreqrep",
    "protocol": "tcp"
  },
  "properties": {"dataType": "float", "unit": "F"}
}
```

#### Create a client that queries for the service

Now here's the source code for the client itself (place in `client.js`):

```js
var iotkit = require('iotkit-comm');
var path = require('path');
var query = new iotkit.ServiceQuery(path.join(__dirname, "server-query.json"));
iotkit.createClient(query, function (client) {
  client.comm.setReceivedMessageHandler(function (message, context) {
    console.log("received from server: " + message.toString());
    client.comm.send("hello");
  });
  client.comm.send("hello");
});
```

Notice that the client does not need to know the IP address of the service or even *how* to communicate with it.
Instead, the service query takes care of such details. For example,
this client specifies the name of the service and the protocol the service should be speaking
(`zmqreqrep`). When the service is found, iotkit-comm returns a client object with an appropriately initialized
communication handle `client.comm`. This handle is an instance of the `zmqreqrep`
plugin connected to the service in question. More on communication plugins later, but for now,
it is enough to understand that all communication plugins provide functions like `send` and `sendTo`. The main
difference between the various communication plugins is *how* the `send` and `sendTo` happen (e.g. different packet
format and headers). Note that iotkit-comm comes bundled with a few default communication plugins,
but its also easy to write your own if necessary.

#### Run the client

Assuming the service query `server-query.json` and the source code `client.js` are in the same directory,
you can run the client using:

```sh
node client.js
```

#### Source Code

* [client.js]{@link example/basic-client-server/client.js}

#### Learn More

* Write the corresponding [service]{@tutorial service}
* Understand [service specifications and queries]{@tutorial service-spec-query} **(important)**
* Learn to write a more comprehensive [distributed application]{@tutorial apps}
* Understand and write [communication plugins]{@tutorial plugin}