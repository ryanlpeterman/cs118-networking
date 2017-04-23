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
	// TODO: If the client doesn’t send any data during gracefully terminated TCP connection, the server should create an empty file with the name that corresponds to the connection number.
	// TODO: The server must assume error if no data received from the client for over 10 seconds. It should abort the connection and write a single ERROR string (without end-of-line/carret-return symbol) into the corresponding file. Note that any partial input must be discarded.

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
    int numThreads = 10;
    std::string fileDir(argv[2]);

    // configure server
   	Listener server(portNum, fileDir, numThreads);

    // run server
   	server.run();

    return 0;
}
