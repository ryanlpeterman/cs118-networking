# CS118 Project 1

Template for for [UCLA CS118 Spring 2017 Project 1](http://web.cs.ucla.edu/classes/spring17/cs118/project-1.html)

- Name: Ryan Peterman
- UID: 704269982

## High Level Design
Client:
- Does some checking on the input and resolves addr to connect to client
- Puts socket into nonblocking mode to set select timeout the sets back to blocking mode
- Once connection is established, the client sends 255 bytes of the file at a time until all bytes are written
- Select sets a timeout at every chunk that is sent

Server:
- Does some input checks and registers signal handlers for SIGTERM and SIGQUIT
- Creates a listener object which takes all the inputs and opens a socket for the process to listen on
- Runs the listener object, which creates a thread pool of 20 threads that each run an infinite while loop that all accept/block waiting to service a request
- Once the accept call unblocks the thread adds one to connection count
- Then a timeout is set at each iteration while chunks of bytes are read into a buffer and written out to a file

## Problems and Solutions
1. Dynamically resolving the IP address/hostname instead of hardcoding as localhost - used getaddrinfo() from beej's guide
2. Counting the number of connections in threadsafe way - use a mutex to lock and unlock while counting
3. Timing outs 10sec of no data - used select to listen on the socketfd to see if it was available for read and set timeout as 10 sec
4. Multithreading the server - used a thread pool that each block and wait to handle any requests
5. Transmitting 100 MiB file - send and received files in chunks of 255 bytes

## Additional Libraries Used
1. fstream - used to write to and from files
2. iostream - used to write to and from stdout
3. vector - for dynamic arrays
4. sys/types - for structs
5. sys/socket - for sockets
6. sys/stat - for stat to on file
7. arpa/inet - for inet_ntop function
8. string - for memset and strings
9. unistd - for close sys call
10. netdb - for getaddrinfo
11. fcntl - for fnctl to make connection nonblocking
12. csignal - to catch SIGTERM/SIGQUIT

## Extra Credit: Docker
In order to build the image  and run it use the following commands with docker installed (Dockerfile must be in current directory):
```
docker build -t httpserver.build .
docker run httpserver.build
```
To save an image:
```
docker save -o webserver_img httpserver
```

To load an image:
```
docker load -i webserver_img
docker run -d -p 2020:2020 httpserver:latest ./server 5000 temp
```

## Acknowledgements of Code used
Aside from StackOverflow for small code examples, the main place I got code from was Beej's guide to networking. It provided examples on how to use getaddrinfo() and on how to use the socket system calls.

