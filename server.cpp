#include <iostream>
#include <string>

#include "listener.h"

int main(int argc, char* argv[])
{
   	// only two arg to server possible
    if (argc != 3)
    {
      std::cerr << "Usage: " << argv[0] << " <port-num> <file-dir>\n";
      return 1;
    }

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
