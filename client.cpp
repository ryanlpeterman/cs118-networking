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
#include <fcntl.h>

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

// sets 10 sec timeout for being able to write
// returns true if timeout_occured or if error else false
bool timeout_occured(int fd) {
	// set of fds to check for available read
	fd_set writefds;
	FD_ZERO(&writefds);
	// add current socket to fd set
	FD_SET(fd, &writefds);

	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	//set timeout/check if there is data to write
	int retval = select(fd + 1, NULL, &writefds, NULL, &tv);
	if (retval == -1) {
		return true;
	} else if (retval == 0) {
		return true;
	}

	return false;
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

 	int status, sockfd;
	struct addrinfo hints;
	struct addrinfo *res;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

	// getaddrinfo(hostnameip, port, input datastruct, outparam)
	if ((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
	    std::cerr << "ERROR: getaddrinfo error" << std::endl;
	    exit(1);
	}

	// loop through all results and connect to the first one we can
	for(struct addrinfo *p = res; p != NULL; p = p->ai_next) {
		// if couldn't connect then go to next entry
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        // set fd to non-blocking connect
        long arg = fcntl(sockfd, F_GETFL, NULL);
        arg |= O_NONBLOCK;
        fcntl(sockfd, F_SETFL, arg);

        // try connecting to current socket
        int res = connect(sockfd, p->ai_addr, p->ai_addrlen);

        // not connected for various reasons yet
        if (res < 0) {
        	if(errno == EINPROGRESS) {
        		// sets 10 sec timer on connecting
        		if (timeout_occured(sockfd)) {
     				std::cerr << "ERROR: timeout occured when trying to connect" << std::endl;
     				return 1;
     			}

     			// timeout didn't occur if we get here

        	} else {
        		std::cerr << "ERROR: error when connecting" << std::endl;
        		return 1;
        	}
        }

        // set to blocking mode again
        arg = fcntl(sockfd, F_GETFL, NULL);
        arg &= (~O_NONBLOCK);
        fcntl(sockfd, F_SETFL, arg);

        break;
    }

	// if resolving addr resulted in no ip
	if(res == NULL) {
		std::cerr << "ERROR: could not resolve given domain/ip" << std::endl;
		exit(1);
	}

	char ipstr[INET_ADDRSTRLEN] = {'\0'};
	inet_ntop(res->ai_family, &(res->ai_addr), ipstr, sizeof ipstr);
	std::cout << "Set up a connection from: " << ipstr << ":" << port << std::endl;

	freeaddrinfo(res); // free the linked-list

	std::string file_contents = read_file(fileName);
	std::cout << "Number of bytes to write out:" << file_contents.size() << std::endl;
	size_t bytes_written = 0;

	// setting timeout for connection
    if (timeout_occured(sockfd)) {
    	close(sockfd);
    	std::cerr << "ERROR: timeout when sending" << std::endl;
    	return 1;
    }

	// while we haven't written entire file out
	while (bytes_written < file_contents.size()) {

		std::string to_send = file_contents.substr(bytes_written, 255);
		int send_res = send(sockfd, to_send.c_str(), to_send.size(), 0);
		if (send_res < 0) {
			std::cerr << "ERROR: could not send data to server" << std::endl;
			return 1;
		}
		bytes_written += send_res;
		// std::cout << "Sent the following bytes: " << send_res << std::endl;

		if (bytes_written < 0) {
			std::cerr << "ERROR: when sending bytes from file" << std::endl;
			exit(1);
		}

		// setting timeout for connection
	    if (timeout_occured(sockfd)) {
	    	close(sockfd);
	    	std::cerr << "ERROR: timeout when sending" << std::endl;
	    	return 1;
	    }
 	}

 	std::cout << "Transfer complete! Total number of bytes written: " << bytes_written << std::endl;


  	close(sockfd);
  	return 0;
}