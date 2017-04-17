#ifndef LISTENER_DEFINED
#define LISTENER_DEFINED

#include <string>
#include <iostream>

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

private:
	// note: order of declaration matters due to member init list
	int port_;
	// location of folder to save files to
	std::string fileDir_;
	int numThreads_;
	int sockfd_;
};


#endif