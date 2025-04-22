# Description
This is an implementation of HTTP server. It will be used to serve the club's webpage content.

## Build the Project
Before building the project, you need to create a build directory. This is where all the object 
files and binaries will be stored. You can do this by running the following command:
```bash
    > mkdir build
```
Additionally, you need to run the CMake configuration step:
```bash
    > cmake -S . -B build
```
After that, you can build the project using the following command:
```bash
    > cmake --build build
```
Somtimes, you might want to build from "scratch". If so, you can delete the build directory and start over:
```bash
    > rm -rf build
```
(Optional) You can generate the `compile_commands.json` file for IDEs:
```bash
    > cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    > ln -s build/compile_commands.json .
```

## Run the Project
Next, to run a simple echo server.
```bash
    > ./builds/server
```
To interact with the server you could use either netcat or telnet to connect to localhost at port 8080. Any you type will be printed ("echo") on the server
```bash
    > nc localhost 8080
```
