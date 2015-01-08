To demonstrate how to write applications using iotkit-comm, we will build a dummy *home thermostat*. Now this is no
ordinary thermostat, it is a *distributed thermostat*. The idea is that there is a central thermostat and many tiny
temperature sensors all around the house publishing their temperature readings. The central thermostat discovers these
sensors and subscribes to the temperature information they are publishing. It then computes the mean of the
temperatures received and publishes it for others to subscribe. As temperature readings arrive from the
sensors, the mean is updated and published again. A home dashboard eventually subscribes to this constantly changing
mean temperature and displays it for the home owner to see.

Now that we have the synopsis out of the way, let's dive straight in. We will start with the temperature sensor,
then describe the thermostat, and finally the dashboard.

#### Temperature Sensor

An application using the iotkit-comm library can act like a service, a client, or both. In this case,
we are implementing the temperature sensor as a service that periodically publishes the ambient temperature. Here's
that portion of the code:

```js
iotkit.createService(spec, function (service) {
  setInterval(function () {
    service.comm.publish("mytemp: " + getRandomInt(60, 90));
  }, 1000);
});
```

Above, the created service `service` publishes a randomly selected ambient temperature once every second. Notice that
`createService` takes two arguments: `spec`, a specification for the service you want to create; and a callback,
which is invoked with an instance of the created service. At this point, it is important to understand the following
things about creating a service using iotkit-comm:

1. A service is created based on a specification: this is a JSON file that describes various attributes of the
service you wish to create. We discuss this next.
2. An application contains a service as opposed to *being* a service: this is because it is given an instance of the
running service via the callback. It can also contain another service, or another client,
or multiples of each. Notice also, that the application does not contain any of the details needed to create the
service, for example, which port to use, or the protocol the service will be speaking etc. All such details are
mentioned in the JSON service specification.

###### Service Specification

So now, here is what the service specification for our temperature sensor looks like:

```json
{
  "name": "/my/home/thermostat/sensor",
  "type": {
    "name": "zmqpubsub",
    "protocol": "tcp"
  },
  "port": 8999,
  "properties": {"dataType": "float", "unit": "F", "sensorType": "ambient"}
}
```

Let's go through each of the above attributes:

* name *(compulsory)*: a string, preferably a user-friendly one, since service names might be displayed by other
applications
* type *(compulsory)*
  * name *(compulsory)*: name of the protocol this service will be speaking. Here, 'zmqpubsub' implies that the service
   will be using [zeromq][1] pub/sub sockets to communicate. More specifically, a zeromq publisher is a service that
   writes to sockets of type `pub`. A zeromq subscriber is a client that can then "subscribe" to data written to that
    socket. The iotkit-comm library supports other protocols like `mqtt` and `zmqreqrep` that are implemented as
    "plugins". You are not required to use supported communication protocols, but it is quite convenient to do so.
    More on this in the {@tutorial plugin} tutorial.
  * protocol *(compulsory)*: this is the transport protocol; only 'tcp' or 'udp' is supported
* port *(compulsory)*: port number the service will run on
* properties *(optional)*: any user-defined properties the service has. Each property must be a `"name": value` pair.
 Here, the properties indicate that the sensor is publishing the ambient temperature in Fahrenheit using a
 floating-point format. More on these properties when we talk about the thermostat.
* *A service specification can also contain a few other attributes. For more details, see the {@tutorial service-spec-query}
  tutorial*

A JSON service specification file is useful only if it can be passed around as an object; to do that,
it must be read in and validated:

```
var spec = new iotkit.ServiceSpec('temperature-sensor-spec.json');
```

The resulting object `spec` can now be passed as an argument to `createService` (the complete commented
source code can be found {@link example/distributed-thermostat/temperature-sensor.js|here}). It is now time for the
thermostat to find this temperature sensor.

#### Thermostat

The thermostat needs to be able to find any existing temperature sensors in the house and any new ones that are
added later. The iotkit-comm library makes this convenient: it uses [mDNS][2] underneath to advertise and discover
services running on the local network. So as long as the temperature sensors are advertising their presence,
the thermostat will be able to find them. In fact, a service created using iotkit-comm is advertised on the local
network by default. If a service would not like to advertise itself, it must say so in the service specification (see
 {@tutorial service-spec-query}). Thus, our temperature sensor above is already advertising itself on the LAN.

To find the temperature sensors, the thermostat must query the network using a 'service query'. A service query is
 very much like a service specification. Here's what a query for the temperature sensor looks like:

 ```json
 {
   "name" : ".*thermostat/sensor",
   "type" : {
     "name": "zmqpubsub",
     "protocol" : "tcp"
   },
   "properties": {"dataType": "float", "sensorType": "ambient"}
 }
 ```

 Using this query, an application will be able to find a service on the LAN that:

 * has a name containing `thermostat/sensor`
  * Notice the regular expression (Javascript RegExp) support
 * uses the [zeromq][1] publish/subscribe sockets to communicate over the tcp protocol
 * is publishing ambient temperatures in floating-point format

Now, that we have the service query, it must be converted into a valid query object:

```js
var sensorQuery = new iotkit.ServiceQuery('temperature-sensor-query.json');
```

The `sensorQuery` object can now be used to find and connect to temperature sensors on the network:

```
iotkit.createClient(sensorQuery, function (client) {
  console.log("Found new temperature sensor - " + client.spec.address + ':' + client.spec.port);
  ...
}, serviceFilter);

function serviceFilter(serviceSpec) {
  ...
  return true;
}
```

Let's study this a bit more. To `createClient`, iotkit-comm needs a service query (e.g. `sensorQuery`)
as input. It then searches for a service on the LAN whose attributes match those given in the query. Once a service is
found, iotkit-comm calls the `serviceFilter` with the specification of the service. The application must then confirm
 that it wants to connect to the found service; it does so by having `serviceFilter` return `true`. iotkit-comm then
creates a client instance connected to this new service and passes it back to the application via the callback
(see final argument of `createClient`).

At this point, it is worth spending a little more time on the question "How does iotkit-comm search for a service
using a query?". Like we mentioned before, the temperature sensors are already advertising themselves on the the
network: what this means, is that each sensor's *specification* is being advertised on the LAN. What iotkit-comm does,
is try to match the fields of a service query with the fields of each service specification available on the network.
Once a match is found, the application is notified using the `serviceFilter` callback.

An application can also choose **not** to connect to a found service. It might need to do so when, say,
it wants to limit the number of services it connects to or does not want to connect to a certain type of service. In
our example, the thermostat will only connect to a maximum of `10` *ambient* temperature sensors:

```
var sensorCount = 0;
function serviceFilter(serviceSpec) {
  if (serviceSpec.properties.sensorType !== 'ambient')
    return false;

  if (sensorCount == 10) {
    return false;
  }
  sensorCount++;

  return true;
}
```

A useful tip: an alternative to checking 'sensorType' in the `serviceFilter` is to set the
`properties.sensorType` field in the client's service query. Now, that our thermostat can find and connect to
temperature sensors, it needs to receive and parse the temperature readings:

```js
iotkit.createClient(sensorQuery, function (client) {
  console.log("Found new temperature sensor - " + client.spec.address + ':' + client.spec.port);
  client.comm.setReceivedMessageHandler(msgHandler);
  client.comm.subscribe("mytemp");
  function msgHandler(binmsg) {
    ...
  }
}, serviceFilter);
```

Here, the thermostat is subscribing to information published under the topic "mytemp". If you look at the source code
 for the temperature sensor, you will see that they are publishing temperature readings under this topic. As soon as
 a temperature reading arrives, the callback `msgHandler` is invoked:


```js
var cumulativeMovingAverage = 0;
var sampleCount = 0;
var mypublisher = null;

function msgHandler(binmsg) {
  var message = binmsg.toString();
  var temperature = parseFloat(message.substring(message.indexOf(':') + 1));

  sampleCount++;
  cumulativeMovingAverage = (temperature + sampleCount * cumulativeMovingAverage)/(sampleCount + 1);

  if (mypublisher) mypublisher.comm.publish("mean_temp: " + cumulativeMovingAverage);
}
```

Notice the last line: the thermostat itself is publishing the newly calculated mean temperature so that other
applications can subscribe to it. To successfully publish though, the thermostat needs to initialize `mypublisher`:


```
var spec = new iotkit.ServiceSpec('thermostat-spec.json');
iotkit.createService(spec, function (service) {
  mypublisher = service;
});
```

As you can see above, `mypublisher` is just a service within the thermostat application that publishes the mean
temperature once it is calculated. At the same time, the thermostat is also a client of the temperature
sensors. This emphasizes how an application built using iotkit-comm contains clients and servers as opposed to
*being* a client or a server. Let's get back to the `mypublisher` service; it is created based on the following
specification:

```json
{
  "name": "/my/home/thermostat",
  "type": {
    "name": "zmqpubsub",
    "protocol": "tcp"
  },
  "port": 9999,
  "properties": {"dataType": "float", "unit": "F"}
}
```

The dashboard can now use a corresponding service query to find and connect to the thermostat. Once that happens,
the dashboard can subscribe to the mean temperature readings being published. The full commented source code for the
thermostat is available {@link example/distributed-thermostat/thermostat.js|here}.

#### Dashboard

The dashboard is responsible for subscribing to the mean temperature published by the thermostat and display it:

```
var thermostatQuery = new iotkit.ServiceQuery('thermostat-query.json');
iotkit.createClient(thermostatQuery, function (client) {
  client.comm.setReceivedMessageHandler(msgHandler);
  client.comm.subscribe("mean_temp");
});

function msgHandler(binmsg) {
  ...
}
```

Here's the service query used to find the thermostat (in `thermostat-query.json`):

```
{
  "name" : "/my/home/thermostat",
  "type" : {
    "name": "zmqpubsub",
    "protocol" : "tcp"
  }
}
```

The full commented source code for the dashboard is available
{@link example/distributed-thermostat/dashboard.js|here}.

#### Running the example

To run the `distributed-thermostat` sample application go into the directory `example/distributed-thermostat/` and
run the following commands in separate terminals:

```bash
$ node thermostat.js
$ node temperature-sensor.js
$ node temperature-sensor.js
...as many as you like, but thermostat will only subscribe to first ten...
$ node dashboard.js
```

#### Source Code

* {@link example/distributed-thermostat/temperature-sensor.js|Temperature Sensor}
* [Thermostat]{@link example/distributed-thermostat/thermostat.js}
* [Dashboard]{@link example/distributed-thermostat/dashboard.js}

#### Learn More

* The thermostat could also publish the running mean to the cloud: to learn how, go [here]{@tutorial cloud}.
* The thermostat need not use communication plugins like `zmqpubsub`. It can use the iotkit-comm service directory to find
and advertise services, while implementing its own communication protocol. To learn how,
go [here]{@tutorial service-directory}.

[1]: http://zeromq.org/
[2]: http://en.wikipedia.org/wiki/Multicast_DNS