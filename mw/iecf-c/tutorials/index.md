
[Start Here](@ref start-here.md) to begin developing applications and plugins that use the iotkit-comm connectivity library. Otherwise, continue
reading to learn more.

iotkit-comm allows network-connected devices to conveniently discover and communicate with each other and the cloud. More specifically,
the iotkit-comm library enables developers to write distributed applications composed of clients, servers, and peers (both client and
server in one). It was designed primarily for the Intel® Edison platform, but works just as well on other platforms. The iotkit-comm
is a library that comes in two flavors: C and node.js. This documentation focuses on the C version of the library.

<B> How to read this documentation </B>

&ensp;&ensp;&bull;  For browsing the library and plugins:<BR>

&ensp;&ensp;    1) libiotkit-comm<BR>
&ensp;&ensp;    2) iotkitpubsub<BR>
&ensp;&ensp;    3) mqttpubsub<BR>
&ensp;&ensp;    4) zmqpubsub<BR>
&ensp;&ensp;    5) zmqreqrep<BR><BR>
&ensp;&ensp;&bull;  For tutorials on how to use the API, the entry point is the 'Tutorials' section (we suggest starting with the ['Start Here'](@ref start-here.md) tutorial).<BR>

<I>Note: For the purposes of this documentation, a module is a logical grouping of classes, methods, and variables. Please be aware
that this is the <B> logical structure of the documentation only</B> and not the iotkit-comm codebase.</I>

<B> Prerequisites </B> <BR>

This document assumes that: <BR>

1. Your Edison has Internet access <BR>
2. The included sample programs and commands are run on the Edison <BR>