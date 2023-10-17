# ClientServerMssgnr
I am Kashkin Gleb Petrovich and this is Task 1.

## Building
```bash
$ mkdir build && cd build
$ cmake .. && echo "enjoy the capy!"
$ make -j
```

## Running 
```bash
$ ./example_client
```
The following commands are allowed:
* connect <ip>
* send <msg>

```bash
$ ./example_server
```
The following commands are allowed:
* send <id> <msg>
* broadcast <msg>
* list

## "Architecture" (\*ironic\*)
I am not relly familiar with networking stack, so I decided to use one
of the popular [projects](https://github.com/eminfedar/async-sockets-cpp.git) from github as the base for network-level communications.
It provides async tcp server and client interface that I utilise.
The library seems good enough to separate me from plain sockets (I don't want to die),
and basic enough to be used in this demo task.

The application and server/client logic is divided by a clear hook-based interface.
I would love to have something a bit more interesting, but the time was really short.

## TODO
The project isn't really even an MVP, so there is a lot to fix:
* GoogleTests, I used my usual CMakeLists config with test lib dependency, but had no time even for unit tests
* Better UI, I have a pretty good idea, how to implement a nice console-based interface with readline, but again, time
* Better async, especially for IO
* Better MessageStructure, clients need MessageRecieved notification too
* Encryption, I had an idea in mind to implement obscure pub-key based encryption, similar to Shadowsocks, it would be hard to distinguish it on ISP-level because there aren't any headers, but...
