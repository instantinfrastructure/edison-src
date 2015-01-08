
To demonstrate how to write applications using iotkit-comm, we will build a dummy home thermostat. Now this is no
ordinary thermostat, it is a distributed thermostat. The idea is that there is a central thermostat and many tiny
temperature sensors all around the house publishing their temperature readings. The central thermostat discovers these
sensors and subscribes to the temperature information they are publishing. It then computes the mean of the
temperatures received and publishes it for others to subscribe. As temperature readings arrive from the
sensors, the mean is updated and published again. A home dashboard eventually subscribes to this constantly changing
mean temperature and displays it for the home owner to see. Now that we have the synopsis out of the way,let's dive
straight in.

We will start with the temperature sensor, then describe the thermostat, and finally the dashboard.

<B> Temperature Sensor </B>

An application using the iotkit-comm library can act like a service, a client, or both. In this case, we are implementing the
temperature sensor as a service that periodically publishes the ambient temperature. Here's that portion of the code:

/** Callback function. Once the service is advertised this callback function will be invoked

* @param servDesc the service description object
* @param error_code the error code
* @param serviceHandle the communication handle used to invoke the interfaces
 */

        void pubServiceCallback(ServiceDescription *servDesc, int32_t error_code, CommHandle *serviceHandle) {

            if (serviceHandle != NULL) {
                int (**publish)(char *,Context context);
                publish = commInterfacesLookup(serviceHandle, "publish");
                if (publish != NULL) {
                    Context context;
                    while(1) {
                        char addr[128];
                        double random = floor(rand() % 90 + 60);
                        sprintf(addr, "mytemp: %f",random);
                        (*publish)(addr,context);
                        sleep(2);
                    }
                } else {
                    fprintf(stderr, "Interface lookup failed\n");
                }
            } else {
                fprintf(stderr, "Comm Handle is NULL\n");
            }
        }

Above, the service publishes a randomly selected ambient temperature once every 2 seconds.At this point, it is
important to understand the following things about creating a service using iotkit-comm:

A service is created based on a specification: this is a JSON file that describes various attributes of the service you
wish to create. We discuss this next.
An application contains a service as opposed to being a service: this is because it is given an instance of the running
service via the callback. It can also contain another service, or another client, or multiple of each. Notice also, that
the application does not contain any of the details needed to create the service, for example, which port to use, or the
protocol the service will be speaking etc. All such details are mentioned in the JSON service specification.

<B> Service Specification </B>

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

a) name (<I>compulsory</I>): a string, preferably a user-friendly one, since service names might be displayed by other
applications <BR>
b) type (<I>compulsory</I>) <BR>
&ensp;&ensp;&ensp;&ensp;name (<I>compulsory</I>): name of the protocol this service will be speaking. Here, 'zmqpubsub' implies that the service
                          will be using zeromq pub/sub sockets to communicate. More specifically, a zeromq publisher is
                          a service that writes to sockets of type pub. A zeromq subscriber is a client that can then
                          "subscribe" to data written to that socket. The iotkit-comm library supports other protocols like mqtt
                          and zmqreqrep that are implemented as "plugins". You are not required to use supported
                          communication protocols, but it is quite convenient to do so. More on this in the Plugin tutorial. <BR>
&ensp;&ensp;&ensp;&ensp;protocol (<I>compulsory</I>): this is the transport protocol; only 'tcp' or 'udp' is supported <BR>
c) port (<I>compulsory</I>): port number the service will run on <BR>
d) properties (<I>optional</I>): any user defined properties the service has. Each property must be a "name": value pair. Here,
   the properties indicate that the sensor is publishing the ambient temperature in Fahrenheit using a floating-point
   format. More on these properties when we talk about the thermostat. <BR>
e) A service specification can also contain a few other attributes. For more details, see the [Service Specification and Query]
(@ref service-spec-query.md) tutorial <BR>

A JSON service specification file is useful only if it can be passed around as an object; to do that, it must first be
read and then validated:

    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./temperature-sensor-spec.json");
    if (serviceDescription)
        WaitToAdvertiseService(serviceDescription, pubServiceCallback);

The resulting object serviceDescription can now be passed as an argument to WaitToAdvertiseService (the complete
commented source code can be found here). Its now time for the thermostat to find this temperature sensor.

<B> Thermostat </B>

The thermostat needs to be able to find any existing temperature sensors in the house and any new ones that are added
later. The iotkit-comm library makes this convenient: it uses mDNS underneath to advertise and discover services running on
the local network. So as long as the temperature sensors are advertising their presence, the thermostat will be able to
find them. In fact, a service created using iotkit-comm is advertised on the local network by default. If a service would not
like to advertise itself, it must say so in the service specification (see Service Specification and Query). Thus, our
temperature sensor above is already advertising itself on the LAN.

To find the temperature sensors, the thermostat must query the network using a 'service query'. A service query is very
much like a service specification. Here's what a query for the temperature sensor looks like:

    {
       "name" : ".*thermostat/sensor",
       "type" : {
         "name": "zmqpubsub",
         "protocol" : "tcp"
       },
    }

Using this query, an application will be able to find a service on the LAN that:

a) has a name containing thermostat/sensor <BR>
&ensp;&ensp;&ensp;&ensp;Notice the regular expression (Javascript RegExp) support <BR>
b) uses the zeromq publish/subscribe sockets to communicate over the tcp protocol <BR>
c) is publishing ambient temperatures in floating-point format <BR>

Now, that we have the service query, it must be converted into a valid query object:

    ServiceQuery *query = (ServiceQuery *) parseClientServiceQuery("./temperature-sensor-query.json");

The sensorQuery object can now be used to find and connect to temperature sensors on the network:

/** Callback function. Once the service is discovered this callback function will be invoked
* @param queryDesc the query description object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
*/

    void subCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

        int (**subscribe)(char *);
        int (**receive)(void (*)(char *, Context));
        ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./serviceSpecs/thermostat-spec.json");
        if (serviceDescription) {
            advertiseService(serviceDescription);
            CommHandle *serviceHandle = createService(serviceDescription);
            mypublisher = commInterfacesLookup(serviceHandle, "publish");
        }
        if (commHandle != NULL) {
            subscribe = commInterfacesLookup(commHandle, "subscribe");
            receive = commInterfacesLookup(commHandle, "receive");
            if (subscribe != NULL && receive != NULL) {
                while (1) {
                    (*subscribe)("mytemp");
                    (*receive)(clientMessageCallback);
                    sleep(2);
                }
            } else {
                fprintf(stderr, "Interface lookup failed\n");
            }
        } else {
            fprintf(stderr, "Comm Handle is NULL\n");
        }
    }

    if (query) {
        WaitToDiscoverServices(query, subCallback);
    }

Let's study this a bit more. To WaitToDiscoverServices, iotkit-comm needs a service query (e.g. sensorQuery) as input. It then
searches for a service on the LAN whose attributes match those given in the query. Once a service is found, iotkit-comm calls
the serviceFilter with the specification of the service. The application must then confirm that it wants to connect to
the found service; it does so by having serviceFilter return true. iotkit-comm then creates a client instance connected to this
new service and passes it back to the application via the callback (see final argument of WaitToDiscoverServices).

At this point, it is worth spending a little more time on the question "How does iotkit-comm searches for a service using a
query?". Like we mentioned before, the temperature sensors are already advertising themselves on the the network: what
this means, is that each sensor's specification is being advertised on the LAN. What iotkit-comm does, is try to match the
fields of a service query with the fields of each service specification available on the network. Once a match is found,
the application is notified using the callback.

Here, the thermostat is subscribing to information published under the topic "mytemp". If you look at the source code
for the temperature sensor, you will see that they are publishing temperature readings under this topic. As soon as a
temperature reading arrives, the callback clientMessageCallback is invoked:

/** Callback function. To to be invoked when it receives any messages for the subscribed topic
* @param message the message received from service/publisher
* @param context a context object
*/

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

Notice the last line: the thermostat itself is publishing the newly calculated mean temperature so that other
applications can subscribe to it. To successfully publish though, the thermostat needs to initialize mypublisher:

    ServiceDescription *serviceDescription = (ServiceDescription *) parseServiceDescription("./thermostat-spec.json");
    if (serviceDescription) {
        advertiseService(serviceDescription);
        CommHandle *serviceHandle = createService(serviceDescription);
        mypublisher = commInterfacesLookup(serviceHandle, "publish");
    }

As you can see above, mypublisher is just a service within the thermostat application that publishes the mean
temperature once it is calculated. At the same time, the thermostat is also a client of the temperature sensors. This
emphasizes how an application built using iotkit-comm contains clients and servers as opposed to being a client or a server.
Let's get back to the mypublisher service; it is created based on the following specification:

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

    void clientMessageCallback(char *message, Context context){

          char *temperature = strstr(message,":");
          if (temperature != NULL) {
                temperature++;
                fprintf(stderr,"Received mean temperature : %s\n", temperature);
          }
    }

/** Callback function. Once the service is discovered this callback function will be invoked
* @param queryDesc the query description object
* @param error_code the error code
* @param commHandle the communication handle used to invoke the interfaces
 */


    void subDiscoveryCallback(ServiceQuery *queryDesc, int32_t error_code, CommHandle *commHandle) {

        int (**subscribe)(char *);
        int (**receive)(void (*)(char *, Context));
        if (commHandle != NULL) {
            subscribe = commInterfacesLookup(commHandle, "subscribe");
            receive = commInterfacesLookup(commHandle, "receive");
            if (subscribe != NULL && receive != NULL) {
                while (1) {
                    (*subscribe)("mean_temp");
                    (*receive)(clientMessageCallback);
                     sleep(2);
                }
            } else {
                fprintf(stderr, "Interface lookup failed\n");
            }
        } else {
            fprintf(stderr, "Comm Handle is NULL\n");
        }
    }

    ServiceQuery *query = (ServiceQuery *) parseClientServiceQuery("./thermostat-query.json");

    if (query) {
	    WaitToDiscoverServices(query, subDiscoveryCallback);
	}

Here's the service query used to find the thermostat (in thermostat-query.json):

    {
      "name" : "/my/home/thermostat",
      "type" : {
        "name": "zmqpubsub",
        "protocol" : "tcp"
      }
    }

The full commented source code for the dashboard is available here.

<B> Running the example </B>

To run the distributed-thermostat sample application go into the directory
<B>/usr/share/iotkit-comm/examples/c/</B> and run the following commands in separate terminals:

$ ./dashboard <BR>
$ ./sensor <BR>
$ ./thermostat <BR>

<B>  Documentation  </B>

[Temperature Sensor](@ref temperatureSensor.c) <BR>
[Thermostat](@ref thermostat.c) <BR>
[Dashboard](@ref dashboard.c) <BR>