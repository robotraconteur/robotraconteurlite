<p align="center"><img src="https://robotraconteurpublicfiles.s3.amazonaws.com/RRheader2.jpg"></p>

# robotraconteurlite

Robot Raconteur Lite is a pure ANSI C (C89) implementation of the Robot Raconteur library intended for use with embedded, real-time, or performance critical systems.

*Warning: Robot Raconteur Lite is actively under development and is subject to change.*

Other implementations of Robot Raconteur:

* Robot Raconteur Core: https://github.com/robotraconteur/robotraconteur
* Robot Raconteur Web: https://github.com/robotraconteur/RobotRaconteurWeb

Robot Raconteur Lite is designed to be as fast, compact, and portable. It is being designed with the 
[NASA/JPL Rule of 10](https://en.wikipedia.org/wiki/The_Power_of_10:_Rules_for_Developing_Safety-Critical_Code#:~:text=The%20Power%20of%2010%20Rules,to%20review%20or%20statically%20analyze.) in mind to allow for eventual certification for safety-critical use, although it currently does
not fully comply. Robot Raconteur Lite is not designed for ease of use. Robot Raconteur Core and Robot Raconteur Lite
are intended to be easy to use and resilient to inexperienced users. Robot Raconteur Lite is designed for experts,
and requires advanced programming skills and knowledge of how Robot Raconteur communication works.

The Robot Raconteur is an object protocol built on top of a simple message passing system. Robot Raconteur Lite
does not implement the object protocol layer, and instead provides a minimal implementation of the message passing
and serialization system. `array.h` and `message.h` contain the functions for reading and writing messages to
a buffer. `node.h` and `connection.h` provide the core of the message passing capability. `tcp_transport.h`
provide the implementation of the TCP transport. The TCP transport for communication. The TCP transport
supports the Robot Raconteur protocol and WebSockets, but does not currently support TLS. This feature
will be added in the future.

Note that Robot Raconteur Lite does not use function pointers or double pointers. This is intended to allow
for future use in specialized applications. This also means that the library user has to do more work
calling functions directly when a function pointer would otherwise be used.

See the examples `robotraconteur_tiny_client.c` and `robotraconteur_tiny_service.c` for examples
on using Robot Raconteur Lite.

Incomplete or missing features:

* Discovery
* Subscriptions
* Message and HTTP handshake verification
* Message Version 4
* Transports other than TCP
* TLS encryption
* Full API documentation and user instructions
* Support for platforms other thar Linux

These missing features are all actively being developed.

Robot Raconteur Lite is developed by Wason Technology, LLC, New York, USA

## Acknowledgment

This work was supported in part by the Advanced Robotics for Manufacturing ("ARM") Institute under Agreement Number W911NF-17-3-0004 sponsored by the Office of the Secretary of Defense. The views and conclusions contained in this document are those of the authors and should not be interpreted as representing the official policies, either expressed or implied, of either ARM or the Office of the Secretary of Defense of the U.S. Government. The U.S. Government is authorized to reproduce and distribute reprints for Government purposes, notwithstanding any copyright notation herein.

This work was supported in part by the New York State Empire State Development Division of Science, Technology and Innovation (NYSTAR) under contract C160142. 

![](docs/figures/arm_logo.jpg) ![](docs/figures/nys_logo.jpg)

