*Prerequisites for understanding this tutorial are the [client]{@tutorial client}, [service]{@tutorial service},
and [service specification]{@tutorial service-spec-query} tutorials.*

The iotkit-comm library allows applications to advertise and query for services on the LAN; these features are
provided as part of the iotkit-comm *service directory*. Normally, the advertising and querying happen automatically as a
part of `createService` or `createClient`. However, there are certain situations in which you might want to use the
service directory independent of creating a client or service. For example, your service may want to speak a protocol
 that is not implemented as an iotkit-comm plugin. In this scenario, you would use the service directory to advertise and
 find services, but implement your own communication protocol. Consider the following specification for a simple
 echo server (`9888-service-custom-comm.json`) that uses `myprotocol` to communicate:

```json
{
  "name" : "/service/without/plugin",
  "type" : {
    "name": "myprotocol",
    "protocol" : "tcp"
  },
  "port" : 9888
}
```

and here's the code for the server:

```js
var iotkit = require('iotkit-comm');
var spec = new iotkit.ServiceSpec(path.join(__dirname, "9888-service-custom-comm.json"));
var net = require('net');
var server = net.createServer(function (c) { //'connection' listener
  c.on('data', function (msg) {
    c.write(msg);
  });
});
server.listen(spec.port, function () {
  var directory = new iotkit.ServiceDirectory();
  directory.advertiseService(spec);
});
```

You can see that the server above creates its own sockets and event handlers, something that is usually done by an
iotkit-comm communication plugin. Also notice the last three lines where `iotkit.ServiceDirectory` is used to advertise the
service. Now, here's the code for the corresponding client:

```js
var iotkit = require('iotkit-comm');
var serviceDirectory = new iotkit.ServiceDirectory();
var query = new iotkit.ServiceQuery(path.join(__dirname, "resources/serviceQueries/service-query-custom-comm.json"));
serviceDirectory.discoverServices(query, function (serviceSpec) {
  var net = require('net');
  var client = net.connect({port: serviceSpec.port, host: serviceSpec.address},
    function () {
      client.write('hello');
    });
  client.on('data', function (data) {
    expect(data.toString()).to.equal("hello");
    client.end();
  });
  client.on('end', function () {
    done();
  });
});
```

and here's the service query this client uses to find our echo server:

```json
{
  "name" : "/service/without/plugin",
  "type" : {
    "name": "myprotocol",
    "protocol" : "tcp"
  }
}
```

#### Learn more

* To learn how services are advertised over the network, read the
[service specification and query]{@tutorial service-spec-query} tutorial.