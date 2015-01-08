
At the moment, iotkit-comm supports only communication plugins. Thus the following topics will be specific to these plugins:

a) About communication plugins. <BR>
b) How communication plugins are instantiated. <BR>
c) Learn more.

<B> About communication plugins </B>

A communication plugin abstracts away the details of how messages are sent (or received), allowing developers to focus
on the contents of messages. These plugins provide functions like send and sendTo. The major difference is how the
send and sendTo are implemented: In one plugin, the underlying protocol might be MQTT, while in another plugin zeromq
sockets might be used. This gives developers the flexibility to switch the underlying communication protocol
(e.g. MQTT or zeromq) with minor code changes and without the need to learn the details of these underlying protocols.

Plugins will appear in code in the CommHandle * field of the service or client object. If you have not gone through the
tutorials for building a client or service, we suggest you do that before continuing further. Coming back to the
CommHandle * field, here's how a client would send a message to the server using the zmqreqrep plugin:

    int (**send)(char *, Context context);
    Context context;

    send = commInterfacesLookup(commHandle, "send");
    (*send)("toys",context);


Each plugin will document the functions it provides. For a list of plugins and their API, see the [supported plugins]
(@ref supported-plugins.md) Supported Plugins page. At this point, it might be worth spending some time understanding
how plugins get instantiated in the .comm field; that is the topic of the next section.

<B> How communication plugins are instantiated </B>

This section assumes that you know how to write a client application using iotkit-comm. If not, please go through the client
tutorial first.

The service's CommHandle *  or client's CommHandle * field is instantiated automatically. iotkit-comm knows which plugin to
use from the type.name field of the service specification or query. Let's consider the example of a client that wants
to connect to a service providing temperature readings; this is the service query it might issue:

    {
        "name" : ".*temperature_sensor",
        "type" : {
            "name": "zmqpubsub",
            "protocol" : "tcp"
            }
    }

The above query says that this client is looking for a service which uses the zmqpubsub communication plugin. The
likely implication is that the client uses the same plugin to communicate. Now when a matching service is found, iotkit-comm
will pass the corresponding service specification to the client side constructor of the zmqpubsub plugin. The
constructor will then connect to the service. If all goes well, iotkit-comm will set the client's CommHandle *  field to this
newly created plugin object.

<B> Learn more </B>

1) Write a communication plugin (coming soon). <BR>
2) Browse the list of [supported plugins](@ref supported-plugins.md) and their respective APIs.