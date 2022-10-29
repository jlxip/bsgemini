#include <common.hpp>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include <signal.h>

/*
	Get env vars, create server socket and epoll fd, etc.
	Finishes listening on epoll

	This is a lot of C boilerplate so I'm taking it out of main.cpp
*/

#define MAX_CONNECTIONS 16

int serverSocket, epoll_fd;
std::string root;

void start() {
	const char* port_c = getenv("BSG_PORT");
	uint16_t port = 1964;
	if(port_c)
		port = atoi(port_c);

	const char* root_c = getenv("BSG_ROOT");
	root = root_c ? root_c : "/gemini";

	// Ignore SIGPIPE, the standard practice in all TCP servers
	signal(SIGPIPE, SIG_IGN);

	if((epoll_fd = epoll_create1(0)) < 0) {
		std::cerr << "Could not create epoll fd" << std::endl;
		exit(1);
	}

	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Unable to create socket" << std::endl;
		exit(1);
	}

	const int enable = 1;
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		std::cerr << "Could not set SO_REUSEADDR" << std::endl;
		exit(1);
	}

	if(bind(serverSocket, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
		std::cerr << "Unable to bind to port " << port << std::endl;
		exit(1);
	}

	setNonBlocking(serverSocket);

	if(listen(serverSocket, MAX_CONNECTIONS) < 0) {
		std::cerr << "Unable to listen on port " << std::endl;
		exit(1);
	}

	std::cout << "Listening on port " << port << std::endl;

	epoll_ctl_add(serverSocket, EPOLLIN | EPOLLOUT | EPOLLET, false);
}
