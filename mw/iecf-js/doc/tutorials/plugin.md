At the moment, iotkit-comm supports *communication plugins* only. Thus the following topics are specific to these
 plugins:

1. About communication plugins
1. How communication plugins are instantiated
1. Communicating without plugins
1. Writing your own plugin
1. Sharing your plugins

#### About communication plugins

A communication plugin abstracts away the details of *how* messages are sent (or received),
allowing developers to focus on the contents of messages. These plugins provide functions like `send` and `sendTo`.
The major difference is how the `send` and `sendTo` are implemented: in one plugin the underlying protocol might be
[MQTT][1] while in another plugin [zeromq][2] sockets might be used. This gives developers the flexibility to switch
the underlying communication protocol (e.g. MQTT or zeromq) with minor code changes and without the need to learn the
 details of these underlying protocols.

Plugins will appear in code in the `.comm` field of the `service` or `client` object. If you have not gone through
the tutorials for building a [client]{@tutorial client} or [service]{@tutorial service},
we suggest you do that before continuing further. Coming back to the `.comm` field, here's how a client would send a
message to the server using the `zmqreqrep` plugin:

```js
client.comm.send("hello");
```

Each plugin will document the functions it provides. For a list of plugins and their API see the
[supported plugins]{@tutorial supported-plugins} page. At this point, it might be worth spending some time
understanding how plugins get instantiated in the `.comm` field; that is the topic of the next section.

#### How communication plugins are instantiated

*This section assumes that you know how to write a client application using iotkit-comm. If not,
please go through the [client]{@tutorial client} tutorial first.*

The `service.comm` or `client.comm` field is instantiated automatically. iotkit-comm knows which plugin to use from
the `type.name` field of the service specification or query. Let's consider the example of a client that wants to
connect to a service providing temperature readings; this is the service query it might issue:

 ```json
 {
   "name" : ".*temperature_sensor",
   "type" : {
     "name": "zmqpubsub",
     "protocol" : "tcp"
   }
 }
 ```

The above query says that this client is looking for a service which uses the `zmqpubsub` communication plugin.
The likely implication is that the client uses the same plugin to communicate. Now when a matching service is found,
iotkit-comm will pass the corresponding service specification to the client side constructor of the `zmqpubsub`
plugin. The constructor will then `connect` to the service. If all goes well, iotkit-comm will set the `client.comm`
field to this newly created plugin object.

#### Communicating without plugins

iotkit-comm supports various communication protocols implemented using "plugins" (see [supported plugins]{@tutorial
supported-plugins}). However, services and clients are not required to use plugins. They can simply use iotkit-comm's
service discovery and advertisement features and implement the communication themselves. To learn more go
[here]{@tutorial service-directory}.

Alternatively, you can choose to abstract away communication details inside a plugin you write yourself. This plugin
can then be used and shared like any other communication plugin.

#### Writing your own plugin

*(coming soon)*

#### Sharing your plugins

*(coming soon)*

[1]: http://mqtt.org/
[2]: http://zguide.zeromq.org/
