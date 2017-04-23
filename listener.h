#ifndef LISTENER_DEFINED
#define LISTENER_DEFINED

#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>

// for bzero
#include <string.h>
// for read/write sys call
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Listener {
public:
	// constructor takes in port and fileDir to save files to
	Listener(int port, std::string fileDir, int numThreads);

	// runs listener
	void run();

	// functions for threads to run to connect to requests and handler them
	void connectAndHandle();

private:
	// note: order of declaration matters due to member init list
	int port_;
	// location of folder to save files to
	std::string fileDir_;
	int numThreads_;
	int sockfd_;
	// to count the number of connections
	int conCount_;
	// mutex to guard the connection counter
	std::mutex countMutex_;
};


#endif