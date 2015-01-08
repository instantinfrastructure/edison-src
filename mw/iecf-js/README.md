**{@tutorial start-here}** to begin developing applications and plugins that use the iotkit-comm connectivity
library. Otherwise, continue reading to learn more.

iotkit-comm allows network-connected devices to conveniently discover and communicate
with each other and the cloud. More specifically, the iotkit-comm library enables developers to write distributed
applications composed of clients, servers, and peers (both client and server in one). It was designed primarily for
the Intel<sup>&reg;</sup> Edison platform, but works just as well on other platforms. The iotkit-comm is a library
that comes in two flavors: C and node.js. This documentation focuses on the node.js version of the library.

#### How to read this documentation

There are two entry points:
* For browsing the API reference, the entry point is the 'Modules' section (we suggest starting with the {@link
module:main|'main'} module).
* For tutorials on how to use the API, the entry point is the 'Tutorials' section (we suggest starting with the
'{@tutorial start-here}' tutorial).

*Note: For the purposes of this documentation, a module is a logical grouping of classes, methods,
and variables. Please be aware that this is the **logical structure of the documentation only**
and not the iotkit-comm codebase.*

#### Prerequisites

This document assumes that:
1. Your Edison has Internet access
1. The included sample programs and commands are run on the Edison