#include <iostream>
#include <string>
#include <csignal>

#include "listener.h"

void signalHandler(int signum) {
	std::cout << "SIGTERM/SIGQUIT received, exiting gracefully with return 0" << std::endl;
	exit(0);
}

int main(int argc, char* argv[])
{
   	// only two arg to server possible
    if (argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " <PORT> <FILE-DIR>\n";
      return 1;
    }

    // register signal handlers
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);

    // save arguments
    int portNum = std::atoi(argv[1]);
    int numThreads = 20;
    std::string fileDir(argv[2]);

    // configure server
   	Listener server(portNum, fileDir, numThreads);

    // run server
   	server.run();

    return 0;
}
