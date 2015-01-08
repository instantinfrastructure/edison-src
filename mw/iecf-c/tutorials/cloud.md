
To publish data to the cloud and later subscribe to it, you will need to:

1) Create a cloud account. <BR>
2) Create a profile for your device. <BR>
3) Connect your device to its profile in the cloud. <BR>
4) Publish data from your device. <BR>
5) Subscribe to data. <BR>

<B> Create a cloud account </B>

Go to Intel's enableiot cloud site [https://dashboard.us.enableiot.com] and follow instructions to create an account.
Once an account is created, an activation key will be available in the account details section which we will be using
shortly.

<B> Create a profile for your device </B>

Creating a profile involves:

a) Getting your device's ID <BR>
b) Using the ID to register your device with the cloud

<B> Getting your device's ID </B>

You can get the device id by running the following in an device terminal:

$ cd /usr/lib/node_modules/iotkit-agent <BR>
$ node iotkit-admin.js device-id <BR>

<B> Using the ID to register your device with the cloud </B>

When you log in to your account, the first page you see is the dashboard. Click on 'devices' devices; then, click on
the 'Add New Device' button. Enter appropriate details about your device; if you do not have a gateway id or don't
know what that means, just enter the device ID in this field. Once you have saved the profile, proceed to the next
step: Connecting your device to this profile.

<B> Connect your device to its profile in the cloud </B>

Before you connect your device, you should first test if it can reach the cloud:

$ cd /usr/lib/node_modules/iotkit-agent <BR>
$ node iotkit-admin.js test <BR>

If there are no errors, do the following:

$ cd /usr/lib/node_modules/iotkit-agent <BR>
$ node iotkit-admin.js activate [your-activation-key] <BR>

If there are no errors, your device is connected and ready to publish data.

<B> Publish data from your device </B>

This section assumes that you know how to write a client application using iotkit-comm. If not, please go through the client
tutorial first.

Create a service query for the cloud (temperatureServiceIoTKit.json):

    {
        "name" : "enableiot_cloud",
        "type" : {
            "name": "iotkit",
            "protocol" : "tcp"
         },
        "address" : "127.0.0.1",
        "port" : 1884
    }

Write the code to publish data:

    void callback(void *handle, int32_t error_code, void *serviceHandle)
    {
        Context context;
        char msg[256];

        if(serviceHandle != NULL){
            CommHandle *commHandle = (CommHandle *)serviceHandle;

            int (**send)(char *message,Context context);

            send = commInterfacesLookup(commHandle, "send");
            if(send == NULL){
                printf("Function \'send\' is not available; please verify the Plugin documentation !!\n");
            }


            context.name = "topic";
            context.value = "data";

            while(1){
                sprintf(msg, "{\"n\": \"garage\", \"v\": %d}", msgnumber++);
                printf("Publishing msg:%s\n", msg);

                (*send)(msg, context);
                sleep(2);
            }
        }
    }

    ServiceSpec *srvSpec = (ServiceSpec *) parseServiceSpec("./temperatureServiceIoTKit.json");

    if (srvSpec){
        createClientForGivenService(srvSpec, callback);
    }


Note above, that data can only be published using sensors. The cloud supports two types of sensors by default
temperature (temperature.v1.0) and humidity (humidity.v1.0). You may create other types by going to the account
details account details page; then, clicking on the 'Catalog' tab; and then finally, clicking on the 'Add a New
Catalog Item' button.

After you've registered the sensor you can publish a reading or observation by using 'send' interface of
CommHandle *commHandle. Make sure the observation is a valid JSON object that contains both the n (name) and v
(value) fields. Also, note that the topic under which the observation is published is "data". This is currently the
only topic supported by the cloud.

<B> Subscribe to data </B>

This section assumes that you know how to write a client application using iotkit-comm. If not, please go through the client
tutorial first.

Create a service query for the cloud (temperatureServiceIoTKit.json):

    {
        "name" : "enableiot_cloud",
        "type" : {
            "name": "iotkit",
            "protocol" : "tcp"
        },
        "address" : "127.0.0.1",
        "port" : 1884
    }

Write the code to subscribe to the data:

    void callback(void *handle, int32_t error_code, void *serviceHandle)
    {
        if(serviceHandle != NULL && !serviceStarted){
            CommHandle *commHandle = (CommHandle *) serviceHandle;
            int (**subscribe)() = NULL;
            int (**receive)(void (*)(char *, Context)) = NULL;

            subscribe = commInterfacesLookup(commHandle, "subscribe");
            if(subscribe == NULL){
                printf("Function \'subscribe\' is not available; please verify the Plugin documentation !!\n");
            }

            receive = commInterfacesLookup(commHandle, "receive"); //(int (*)(void (*)(char *, Context)))
            if(receive == NULL){
                printf("Function \'receive\' is not available; please verify the Plugin documentation !!\n");
            }

            (*receive)(message_callback);
            (*subscribe)();

            serviceStarted = 1;
        }
    }

    ServiceQuery *query = (ServiceQuery *) parseServiceSpec("./temperatureServiceIoTKit.json");

    if (query){
        createClientForGivenService(query, callback);
    }

To receive data you must first subscribe to it. Note that subscribe does not take a topic as argument. This is
because the only supported topic, for now, is "data" and so it is assumed by default.
