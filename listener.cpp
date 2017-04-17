#include "listener.h"

// Spec states: "you can assume that the folder is always correct"
Listener::Listener(int port, std::string fileDir, int numThreads)
	: port_(port), fileDir_(fileDir), numThreads_(numThreads)
{
	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd_ < 0) {
		std::cerr << "ERROR: could not open socket" << std::endl;
		exit(1);
	}

	// struct containing internet address
	struct sockaddr_in serv_addr;

	// zero out the struct
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	// htons converts port num from hostbyte order to network byte order
	serv_addr.sin_port = htons(port);
	// INADDR_ANY == host ip addr
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		std::cerr << "ERROR: could not bind socket" << std::endl;
		exit(1);
	}

	// max on most systems is 5
	int backlog_size = 5;
	listen(sockfd_, backlog_size);
}

void Listener::run(){
	std::cout << "Server started on Port: " << port_ << std::endl;

	// struct containing connection address
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	// block until connection accepted
	int conn_sock_fd = accept(sockfd_, (struct sockaddr *) &cli_addr, &clilen);
	if (conn_sock_fd < 0) {
		std::cerr << "ERROR: could not accept connection" << std::endl;
		exit(1);
	}

	// buffer to read connection into
	char buffer[256];
	bzero(buffer, 256);

	if(read(conn_sock_fd, buffer, 255) < 0) {
		std::cerr << "ERROR: could not read from socket" << std::endl;
		exit(1);
	}

	cout << "Message Received: " << buffer << endl;

	if(write(conn_sock_fd, "Hello world", 11) < 0) {
		std::cerr << "ERROR: could not write to socket" << std::endl;
		exit(1);
	}
}