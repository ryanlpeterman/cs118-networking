#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

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
    std::string hostnameOrIP(argv[1]);
    short port = std::atoi(argv[2]);
    std::string fileName(argv[3]);

    // check if file exists
    if (!is_regular_file(argv[3])) {
    	std::cerr << "ERROR: filename provided does not exist or is not a regular file." << std::endl;
    	exit(1);
    }

 	// create a socket using TCP IP
 	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

 	// struct sockaddr_in addr;
 	// addr.sin_family = AF_INET;
 	// addr.sin_port = htons(40001);     // short, network byte order
 	// addr.sin_addr.s_addr = inet_addr("127.0.0.1");
 	// memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));
 	// if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
 	//   perror("bind");
 	//   return 1;
 	// }

 	struct sockaddr_in serverAddr;
 	serverAddr.sin_family = AF_INET;
 	serverAddr.sin_port = htons(port);     // short, network byte order
 	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
 	memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

	// connect to the server
	if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
		perror("connect");
		return 2;
	}

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
		perror("getsockname");
		return 3;
	}

	char ipstr[INET_ADDRSTRLEN] = {'\0'};
	inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
	std::cout << "Set up a connection from: " << ipstr << ":" <<
	ntohs(clientAddr.sin_port) << std::endl;


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

  	close(sockfd);
  	return 0;
}