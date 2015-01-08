Writing a service using iotkit-comm requires three fundamental steps:
1. Write a service specification
2. Create a service based on that specification
3. Run the service

#### Write a service specification

A service specification is a JSON string that describes a service. It specifies attributes such as the port on which
the service will run, the name of the service, the protocol it will use to communicate etc. Here is the specification
 for the service we will be writing (place in `server-spec.json`):

 ```json
 {
   "name": "/ndg/temperature/cpuTemp",
   "type": {
   "name": "zmqreqrep",
   "protocol": "tcp"
 },
   "port": 8333,
   "properties": {"dataType": "float", "unit": "F"},
   "advertise": {"locally": true, "cloud": false}
 }
 ```

#### Create a service based on that specification

Now here's the source code for the service itself (place in `server.js`):

```js
var iotkit-comm = require('iotkit-comm');
var path = require('path');
var spec = new iotkit.ServiceSpec(path.join(__dirname, "server-spec.json"));
iotkit.createService(spec, function (service) {
  service.comm.setReceivedMessageHandler(function(client, msg, context) {
    console.log("received from client: " + msg.toString());
    service.comm.sendTo(client, "hi");
  });
});
```

Notice that the service does not need to worry about *how* messages will be delivered,
it only needs to worry about the contents of those messages. Specifying `zmqreqrep` in the `type.name` field of the
specification is enough to let iotkit-comm know how to send messages. The underlying communication details are
handled by *communication plugins*; in this case, the communication plugin (`service.comm`) is an instance of the
`zmqreqrep` plugin. More on communication plugins later, but for now, it is enough to understand that all
communication plugins provide functions like `send` and `sendTo`. The main difference between the various
communication plugins is *how* the `send` and `sendTo` happen (e.g. different packet format and headers). Note that
iotkit-comm comes bundled with a few default communication plugins, but its also easy to write your own if necessary.

#### Run the service

Assuming the service specification (`server-spec.json`) and the source code (`server.js`) are in the same directory,
you can run the service using:

```sh
node server.js
```

#### Source Code

* [server.js]{@link example/basic-client-server/server.js}

#### Learn More

* Write the corresponding [client]{@tutorial client} application
* Understand [service specifications and queries]{@tutorial service-spec-query} **(important)**
* Learn to write a more comprehensive [distributed application]{@tutorial apps}
* Understand and write [communication plugins]{@tutorial plugin}