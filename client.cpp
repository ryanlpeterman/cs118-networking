#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// reads a file into a string and returns it
std::string read_file(std::string filename) {
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    std::string str_rep(result.begin(), result.end());

    return str_rep;
}

// checks if file exists and is regular file
bool is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int main(int argc, char* argv[])
{
	// invalid num args
	if (argc != 4) {
		std::cerr << "Usage: ./client <HOSTNAME-OR-IP> <PORT> <FILENAME>" << std::endl;
		exit(1);
	}

	// save arguments
	std::string fileName(argv[3]);
    short port = std::atoi(argv[2]);
    const int MIN_NON_PRIV_PORT = 1024;
	const int MAX_PORT = 65535;
    if (port < MIN_NON_PRIV_PORT || port > MAX_PORT) {
    	std::cerr << "ERROR: invalid port/port out of range error" << std::endl;
    	exit(1);
    }

    // check if file exists
    if (!is_regular_file(argv[3])) {
    	std::cerr << "ERROR: filename provided does not exist or is not a regular file." << std::endl;
    	exit(1);
    }

 	int status;
	struct addrinfo hints;
	struct addrinfo *res;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	// hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	// getaddrinfo(hostnameip, port, input datastruct, outparam)
	if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
	    std::cerr << "ERROR: getaddrinfo error" << std::endl;
	    exit(1);
	}

	// if resolving addr resulted in no ip
	if(res == NULL) {
		std::cerr << "ERROR: could not resolve given domain/ip" << std::endl;
		exit(1);
	}

 	// create a socket using TCP IP
 	int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	// connect to the server
	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("connect");
		return 2;
	}

	char ipstr[INET_ADDRSTRLEN] = {'\0'};
	inet_ntop(res->ai_family, &(res->ai_addr), ipstr, sizeof ipstr);
	std::cout << "Set up a connection from: " << ipstr << ":" << port << std::endl;

	std::string file_contents = read_file(fileName);
	size_t bytes_written = 0;

	// while we haven't written entire file out
	while (bytes_written < file_contents.size()) {

		std::string to_send = file_contents.substr(bytes_written, 255);
		bytes_written += send(sockfd, to_send.c_str(), to_send.size(), 0);

		if (bytes_written < 0) {
			std::cerr << "ERROR: when sending bytes from file" << std::endl;
			exit(1);
		}
 	}

 	std::cout << "Transfer complete! Total number of bytes written: " << bytes_written << std::endl;

 	freeaddrinfo(res); // free the linked-list
  	close(sockfd);
  	return 0;
}