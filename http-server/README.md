# Description
This is an implementation of HTTP server. It will be used to serve the club's webpage content.

# How to build & run
To build the server, run the following command. This will create a ./builds folder that will contain all the built binaries and object files.
```bash
    > make
```
Next, to run a simple echo server.
```bash
    > ./builds/server
```
To interact with the server you could use either netcat or telnet to connect to localhost at port 8080. Any you type will be printed ("echo") on the server
```bash
    > nc localhost 8080
```
Somtimes, you might want to build from "scratch". If so, run this to clean up current builds
```bash
    > make clean
```
