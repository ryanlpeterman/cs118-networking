# CS118 Project 1

Template for for [UCLA CS118 Spring 2017 Project 1](http://web.cs.ucla.edu/classes/spring17/cs118/project-1.html)

Name: Ryan Peterman
UID: 704269982

## Problems and Solutions
1. Dynamically resolving the IP address/hostname instead of hardcoding as localhost - used getaddrinfo() from beej's guide
2. Counting the number of connections in threadsafe way - use a mutex to lock and unlock while counting
3. Timing outs 10sec of no data - used select to listen on the socketfd to see if it was available for read and set timeout as 10 sec
4. Multithreading the server - used a thread pool that each block and wait to handle any requests
5. Transmitting 100 MiB file - send and received files in chunks of 255 bytes

## Additional Libraries Used
1. fstream - used to write to and from

## Acknowledgements of Code used
Aside from StackOverflow for small code examples, the main place I got code from was Beej's guide to networking. It provided examples on how to use getaddrinfo() and on how to use the socket system calls.

