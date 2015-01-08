To start working with the iotkit-comm library, you will need to:

1. Install it.
2. Run a sample program (optional).
3. Learn more (optional).

<B> Install it </B>

No installation is required on the device (support for other platforms coming soon).

<B> Run a sample program </B>

On the device, iotkit-comm sample programs can be found in /usr/share/iotkit-comm/examples/c. Here is how you run the
distributed thermostat sample application included in the iotkit-comm sources:

$ cd /usr/share/iotkit-comm/examples/c <BR>
$ ./sensor <BR>
Then, in another terminal:

$ cd /usr/share/iotkit-comm/examples/c <BR>
$ ./thermostat <BR>
In the thermostat terminal, you will see output

You can run multiple instances of './sensor'; the thermostat will find those sensors and include their temperature data in the mean.


<B> Learn More </B>

&bull;&ensp; Write a [service](@ref service.md) <BR>
&bull;&ensp; Write a [client](@ref client.md) <BR>
&bull;&ensp; Understand [service specifications and queries] (@ref service-spec-query.md) (important)<BR>
&bull;&ensp; Learn to write a [distributed application](@ref application.md) using iotkit-comm <BR>
&bull;&ensp; Learn to publish data to the [cloud](@ref cloud.md) and subscribe to it <BR>
&bull;&ensp; Study the unit tests included in the iotkit-comm sources (coming soon) <BR>
&bull;&ensp; Understand the iotkit-comm architecture (coming soon) <BR>