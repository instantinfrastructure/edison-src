
Writing a client using iotkit-comm requires three fundamental steps:
    a) Write a service query. <BR>
    b) Create a client that queries for the service. <BR>
    c) Run the client. <BR>

<B> Write a service query </B>

A service query is a JSON string that specifies the attributes of the service a client wishes to connect to. In
this example, the service query is exactly the same as the service specification shown in the [service](@ref service.md)
tutorial. It is assumed that the service specification is in the file server-spec.json.

Create a client that queries for the service

Now here's the source code for the client itself:

    ServiceQuery *query = (ServiceQuery *) parseServiceQuery("./server-spec.json");

    if (query) {
        discoverServicesBlocking(query, subDiscoveryCallback);
    }

Notice that the client does not need to know the IP address of the service or even how to communicate with it. Instead,
the service query takes care of such details. For example, this client specifies the name of the service and the
protocol the service should be speaking (zmqreqrep). When the service is found, iotkit-comm returns a client object with an
appropriately initialized communication handle client.comm. This handle is an instance of the zmqreqrep plugin
connected to the service in question. More on communication plugins later, but for now, it is enough to understand
that all communication plugins provide functions like send and sendTo. The main difference between the various
communication plugins is how the send and sendTo happen (e.g. different packet format and headers). Note that iotkit-comm
comes bundled with a few default communication plugins, but its also easy to write your own if necessary.

<B> Run the client </B>

$ ./client

<B> Learn More </B>

&bull; Write the corresponding [service](@ref service.md)  <BR>
&bull; Understand service [specifications and queries](@ref service-spec-query.md) (important) <BR>
&bull; Learn to write a more comprehensive [distributed application](@ref application.md)  <BR>
&bull; Understand and write [communication plugins](@ref plugin.md)  <BR>