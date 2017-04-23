#include "listener.h"

// Spec states: "you can assume that the folder is always correct"
Listener::Listener(int port, std::string fileDir, int numThreads)
	: port_(port), fileDir_(fileDir), numThreads_(numThreads)
{
	const int MIN_NON_PRIV_PORT = 1024;
	const int MAX_PORT = 65535;

	conCount_ = 1;

	// invalid port
	if (port < MIN_NON_PRIV_PORT || port > MAX_PORT) {
		std::cerr << "ERROR: invalid port, out of range" << std::endl;
		exit(1);
	}

	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

	// allow socket to reuse the address
	int yes = 1;
	if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    	std::cerr << "ERROR: could not set socket options" << std::endl;
    	exit(1);
  	}

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

void Listener::connectAndHandle() {
	while(true) {
		// struct containing connection address
		struct sockaddr_in cli_addr;
		socklen_t clilen = sizeof(cli_addr);

		// block until connection accepted
		int conn_sock_fd = accept(sockfd_, (struct sockaddr *) &cli_addr, &clilen);
		if (conn_sock_fd < 0) {
			std::cerr << "ERROR: could not accept connection" << std::endl;
			exit(1);
		}

		// start time
		// auto start = std::chrono::steady_clock::now();

		// connection accepted at this point, we must determine its number
		countMutex_.lock();
		int conNumber = conCount_;
		conCount_++;
		countMutex_.unlock();

		std::ofstream myfile;
		std::string fileName = fileDir_ + "/" + std::to_string(conNumber) + ".file";
		myfile.open(fileName);

		// buffer to read connection into
		char buffer[256];
		bzero(buffer, 256);
		size_t bytes_read = 0;
		size_t total_bytes = 0;
		while((bytes_read = read(conn_sock_fd, buffer, 256)) > 0) {

			if (bytes_read < 0) {
				std::cerr << "ERROR: could not read from socket" << std::endl;
				exit(1);
			}

			total_bytes += bytes_read;
			myfile.write(buffer, bytes_read);
		}

		myfile.close();
		std::cout << "Connection Number " << conNumber << " Done: Received file!" << total_bytes << std::endl;
	}
}

void Listener::run(){
	std::cout << "Server started on Port: " << port_ << std::endl;

	std::thread* pool = new std::thread[numThreads_];

	for(int i = 0; i < numThreads_; i++) {
		pool[i] = std::thread([this]{connectAndHandle();});
	}

	for(int i = 0; i < numThreads_; i++) {
		pool[i].join();
	}

	delete pool;
}