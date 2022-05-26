# Web Server

## What is this?
This is a rudimentary webserver I built for a class.

### To Build:

    make clean -f makefile.mak
    make -f makefile.mak

### Usage:
    ./build/WebServer.out <ip> <port>

### Notes:

* IP should be localhost (127.0.0.1)
* Port can be anything greater than 1024 unless you run as root, in which case you can use any port (particularly port 80). I use port 8080.

### Sample Web Addresses:

* 127.0.0.1:8080						=> index.html
* 127.0.0.1:8080/index					=> index.html
* 127.0.0.1:8080/index.html				=> index.html
* 127.0.0.1:8080/PrintTest.lua				=> Prints to console
* 127.0.0.1:8080/TableTest.lua?name=sally&pass=123	=> Displays HTML table
