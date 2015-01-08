To publish data to the cloud, you will need to:

1. Create a cloud account
1. Register your Edison
1. Setup your Edison to publish data
1. Publish data
1. View the published data *(optional)*

*Note: the first three steps only need to be done once*

#### Create a cloud account

Go to Intel's [enableiot cloud][1] site and follow instructions to create an account. Once an account is created,
an `activation key` will be available in the account details section ![account details](../images/cloud-account-details
.png). Record this activation key.

#### Register your Edison

```sh
iotkit-admin activate [your-activation-key]
```

#### Setup your Edison to publish data

```sh
systemctl enable iotkit-agent
systemctl start iotkit-agent
```

#### Publish data

*This section assumes that you know how to write a client application using iotkit-comm. If not,
please go through the [client]{@tutorial client} tutorial first.*

Create a service specification for the cloud (`enableiot-cloud-spec.json`):

```json
{
    "type" : {
        "name": "enableiot-cloud",
        "protocol" : "tcp"
    },
    "address" : "127.0.0.1",
    "port" : 1884
}
```

Write the code to publish data:

```js
      var iotkit = require('iotkit-comm');
      var spec = new iotkit.ServiceSpec("enableiot-cloud-spec.json");
      iotkit.createClient(spec, function (client) {
        client.comm.registerSensor("garage","temperature.v1.0");
        client.comm.publish(JSON.stringify({"n": "garage","v": 68}));
      });
```

It is important to know that data can only be published using sensors. The cloud supports two types of sensors by
default: temperature (`temperature.v1.0`) and humidity (`humidity.v1.0`). You may create other types by going to
the account details ![account details](../images/cloud-account-details.png) page; then, clicking on the 'Catalog'
tab; and then finally, clicking on the 'Add a New Catalog Item' button.

After you've registered the sensor, you can publish a reading or observation by using `client.comm.publish`. Make sure
the observation is a valid JSON object that contains both the `n` (name) and `v` (value) fields. Also,
`registerSensor` only needs to be called once per sensor; any future calls for the same sensor are ignored.

#### View the published data

Go to Intel's [enableiot cloud][1] site, login, click on the "Menu" button ![Menu](../images/menu.png),
and click "Charts". Then, select your device and sensor-type to see a graph of your published data vs.
time.

[1]: https://dashboard.us.enableiot.com