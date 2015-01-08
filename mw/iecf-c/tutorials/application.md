
To demonstrate how to write applications using iotkit-comm, we will build a dummy home thermostat. Now this is no ordinary
thermostat it is a distributed thermostat. The idea is that there is a central thermostat and many tiny temperature
sensors all around the house publishing their temperature readings. The central thermostat discovers these sensors and
subscribes to the temperature information they are publishing. It then computes the mean of the temperatures received and
publishes it for others to subscribe. As temperature readings arrive from the sensors, the mean is updated and published
again. A home dashboard eventually subscribes to this constantly changing mean temperature and displays it for the home
owner to see.

<B> Temperature Sensor </B>

An application using the iotkit-comm library can act like a service, a client, or both. In this case, we are implementing the
temperature sensor as a service that periodically publishes the ambient temperature. Here's that portion of the code:

    void pubServiceCallback(ServiceSpec *servSpec, int32_t error_code, CommHandle *serviceHandle) {

        if (serviceHandle != NULL) {
            int (**publish)(char *,Context context);
            publish = commInterfacesLookup(serviceHandle, "publish");
            Context context;
            while(1) {
                char addr[128];
                double random = floor(rand() % 90 + 60);
                sprintf(addr, "mytemp: %f",random);
                (*publish)(addr,context);
                sleep(2);
            }
        } else {
            fprintf(stderr, "Comm Handle is NULL\n");
        }
    }


Above, the created service service publishes a randomly selected ambient temperature once every 2 seconds. In the below
block of service spec validation code you will notice advertiseServiceBlocking() takes two arguments: spec, a specification for
the service you want to create and a callback, which is invoked with an instance of the created service. At this point,
it is important to understand the following things about creating a service using iotkit-comm

1) A service is created based on a specification: this is a JSON file that describes various attributes of the service
you wish to create. We discuss this next. <BR>
2) An application contains a service as opposed to being a service: This is because it is given an instance of the
running service via the callback. It can also contain another service, or another client, or a combination of both. Notice
also that the application does not contain any of the details needed to create the service for example, which port to
use, or the protocol the service will be speaking etc. All such details are mentioned in the JSON service specification. <BR>

<B> Service Specification </B> <BR>

So now, here is what the service specification for our temperature sensor looks like:

    {
      "name": "/my/home/thermostat/sensor",
      "type": {
        "name": "zmqpubsub",
        "protocol": "tcp"
      },
      "port": 8999,
      "properties": {"dataType": "float", "unit": "F", "sensorType": "ambient"}
    }

Let's go through each of the above attributes:

name (<I>compulsory</I>): a string, preferably a user-friendly one, since service names might be displayed by other applications <BR>
type (<I>compulsory</I>) <BR>
&ensp;&ensp;&ensp;&ensp; name (<I>compulsory</I>): name of the protocol this service will be speaking. Here, 'zmqpubsub'
    implies that the service will be using zeromq pub/sub sockets to communicate. More specifically, a zeromq publisher
    is a service that writes to sockets of type pub. A zeromq subscriber is a client that can then "subscribe" to data
    written to that socket. The iotkit-comm library supports other protocols like mqtt and zmqreqrep that are implemented as "plugins".
    You are not required to use supported communication protocols, but it is quite convenient to do so. More on this in
    the [Plugin](@ref plugin.md) tutorial. <BR>
&ensp;&ensp;&ensp;&ensp; protocol (<I>compulsory</I>): this is the transport protocol; only 'tcp' or 'udp' is supported <BR>
port (<I>compulsory</I>): port number the service will run on <BR>
properties (<I>optional</I>): any properties the service has. Each property must be a "name": value pair. Here,
the properties indicate that the sensor is publishing the ambient temperature in Fahrenheit using a floating-point format.
More on these properties when we talk about the thermostat.

A service specification can also contain a few other attributes. For more details, see the [Service Specification and Query]
(@ref service-spec-query.md) tutorial.

A JSON service specification file is useful only if it can be passed around as an object; to do that, it must first be
read and then validated:

    ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/temperature-sensor-spec.json");
    if (serviceSpec)
        advertiseServiceBlocking(serviceSpec, pubServiceCallback);

The resulting object spec can now be passed as an argument to advertiseServiceBlocking(). It's now time for the thermostat
to find this temperature sensor.

<B> Thermostat </B>

The thermostat needs to be able to find any existing temperature sensors in the house and any new ones that are added later.
The iotkit-comm library makes this convenient it uses mDNS underneath to advertise and discover services running on the local
network. So as long as the temperature sensors are advertising their presence, the thermostat will be able to find them.
In fact, a service created using iotkit-comm is advertised on the local network by default. If a service would not like to
advertise itself, it must say so in the service specification (see [Service Specification and Query](@ref service-spec-query.md)).
Thus, our temperature sensor above is already advertising itself on the LAN.

To find the temperature sensors, the thermostat must query the network using a 'service query'. A service query is very
much like a service specification. Here's what a query for the temperature sensor looks like:

     {
       "name" : ".*thermostat/sensor",
       "type" : {
         "name": "zmqpubsub",
         "protocol" : "tcp"
       },
       "properties": {"dataType": "float", "sensorType": "ambient"}
     }

Using this query, an application will be able to find a service on the LAN that:

&bull; has a name containing thermostat/sensor <BR>
&ensp;&ensp; &bull; Notice the regular expression support <BR>
&bull; uses the zeromq publish/subscribe sockets to communicate over the tcp protocol <BR>
&bull; is publishing ambient temperatures in floating-point format <BR>

Now, that we have the service query, it must be converted into a valid query object:

    ServiceQuery *query = (ServiceQuery *) parseServiceQuery("./serviceQueries/temperature-sensor-query.json");
    if (query) {
        discoverServicesBlocking(query, subCallback);
    }

The sensorQuery object can now be used to find and connect to temperature sensors on the network:

    void subCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

    }

Let's study this a bit more. To createClient iotkit-comm needs a service query (e.g. sensorQuery) as input. It then searches
for a service on the LAN whose attributes match those given in the query. Once a service is found, iotkit-comm calls the
serviceFilter with the specification of the service. The application must then confirm that it wants to connect to the
found service; it does so by having serviceFilter return true. iotkit-comm then creates a client instance connected to this
new service and passes it back to the application via the callback (see final argument of discoverServicesBlocking()).

At this point, it is worth spending a little more time on the question "How does iotkit-comm search for a service using a query?"
like we mentioned before, the temperature sensors are already advertising themselves on the the network: what this means,
is that each sensor's specification is being advertised on the LAN. What iotkit-comm does, is try to match the fields of a
service query with the fields of each service specification available on the network. Once a match is found, the
application is notified using the serviceFilter callback.

Now that our thermostat can find and connect to temperature sensors, it needs to receive and parse the temperature readings:

    void subCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

        int (**subscribe)(char *);
        int (**receive)(void (*)(char *, Context));
        ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/thermostat-spec.json");
        if (serviceSpec) {
            advertiseService(serviceSpec);
            CommHandle *serviceHandle = createService(serviceSpec);
            mypublisher = commInterfacesLookup(serviceHandle, "publish");
        }
        if (commHandle != NULL) {
            subscribe = commInterfacesLookup(commHandle, "subscribe");
            receive = commInterfacesLookup(commHandle, "receive");
            while (1) {
                (*subscribe)("mytemp");
                (*receive)(clientMessageCallback);
                sleep(2);
            }
        } else {
            fprintf(stderr, "Comm Handle is NULL\n");
        }
    }

Here, the thermostat is subscribing to information published under the topic "mytemp". If you look at the source code
for the temperature sensor, you will see that they are publishing temperature readings under this topic. As soon as a
temperature reading arrives, the callback clientMessageCallback is invoked:

    int cumulativeMovingAverage = 0;
    int sampleCount = 0;
    int (**mypublisher)(char *,Context context);

    void clientMessageCallback(char *message, Context context) {

        // remove the topic from the message content (the temperature itself)
        char *temperature = strstr(message,":");
        if (temperature != NULL) {
                temperature++;
        fprintf(stdout,"Received sample temperature %s\n",temperature);
        // compute the mean of the temperatures as they arrive
        double value = atof(temperature);
        sampleCount++;
        cumulativeMovingAverage = (value + sampleCount * cumulativeMovingAverage)/(sampleCount + 1);
        char addr[256];
        sprintf(addr, "%d", cumulativeMovingAverage);
        fprintf(stdout,"New average ambient temperature (cumulative) %s:\n",addr);
        char mean[256];
        sprintf(mean, "mean_temp: %s", addr);
        // the master (thermostat) publishes the average temperature so others
        // can subscribe to it.
        if (mypublisher != NULL)
            (*mypublisher)(mean,context);
        }
    }

Notice the last line, the thermostat itself is publishing the newly calculated mean temperature so that other applications
can subscribe to it. To successfully publish though, the thermostat needs to initialize mypublisher:

        ServiceSpec *serviceSpec = (ServiceSpec *) parseServiceSpec("./serviceSpecs/thermostat-spec.json");
        if (serviceSpec) {
            advertiseService(serviceSpec);
            CommHandle *serviceHandle = createService(serviceSpec);
            mypublisher = commInterfacesLookup(serviceHandle, "publish");
        }

As you can see above, mypublisher is just a service within the thermostat application that publishes the mean temperature
once it is calculated. At the same time, the thermostat is also a client of the temperature sensors. This emphasizes how
an application built using iotkit-comm contains clients and servers as opposed to being a client or a server. Let's get back to
the mypublisher service; it is created based on the following specification:

    {
      "name": "/my/home/thermostat",
      "type": {
        "name": "zmqpubsub",
        "protocol": "tcp"
      },
      "port": 9999,
      "properties": {"dataType": "float", "unit": "F"}
    }

The dashboard can now use a corresponding service query to find and connect to the thermostat. Once that happens, the
dashboard can subscribe to the mean temperature readings being published. The full commented source code for the
thermostat is available here.

<B> Dashboard </B>

The dashboard is responsible for subscribing to the mean temperature published by the thermostat and display it:

    ServiceQuery *query = (ServiceQuery *) parseServiceQuery("./serviceQueries/thermostat-query.json");
    if (query) {
        discoverServicesBlocking(query, subDiscoveryCallback);
    }

    void subDiscoveryCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

        int (**subscribe)(char *);
        int (**receive)(void (*)(char *, Context));
        subscribe = commInterfacesLookup(commHandle, "subscribe");
        receive = commInterfacesLookup(commHandle, "receive");
        if (commHandle != NULL && subscribe != NULL && receive != NULL) {
            while (1) {
                (*subscribe)("mean_temp");
                (*receive)(clientMessageCallback);
                 sleep(2);
            }
        } else {
            fprintf(stderr, "Comm Handle is NULL\n");
        }
    }

    void clientMessageCallback(char *message, Context context) {

      // remove the topic from the message content (the temperature)
      char *temperature = strstr(message,":");
      if (temperature != NULL) {
            temperature++;
            fprintf(stdout,"Received mean temperature : %s\n", temperature);
      }
    }


Here's the service query used to find the thermostat (in thermostat-query.json):

    {
      "name" : "/my/home/thermostat$",
      "type" : {
        "name": "zmqpubsub",
        "protocol" : "tcp"
      }
    }

The full commented source code for the dashboard is available here.

<B> Running the example </B>

To run the distributed-thermostat sample application go into the directory /usr/share/iotkit-comm/examples/c/
and run the following commands in separate terminals:

$ ./thermostat <BR>
$ ./sensor <BR>
$ ./sensor <BR>
...as many as you like <BR>
$ ./dashboard <BR>

<B> Learn More </B>

1) The thermostat can alternatively publish the running mean to the cloud and the dashboard can then get it from there.
To learn how to communicate with the cloud, go [here](@ref cloud.md)  <BR>
2) An application can directly connect to a service by providing its address and port in the service specification. <BR>
