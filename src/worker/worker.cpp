#include <common.hpp>
#include <sys/epoll.h>
#include <arpa/inet.h>

#define MAX_EVENTS 32

[[noreturn]] void* worker(void*) {
	epoll_event events[MAX_EVENTS];
	while(true) {
		size_t nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		for(size_t i=0; i<nfds; ++i) {
			Buffer* buffer = (Buffer*)(events[i].data.ptr);
			auto fd = buffer->fd;
			auto ev = events[i].events;

			if(fd == serverSocket) {
				// New connection
				struct sockaddr_in addr;
				uint32_t len;

				int conn = accept(serverSocket, (struct sockaddr*)&addr, &len);
				setNonBlocking(conn);

				epoll_ctl_add(conn, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP, true);
			} else if(ev & EPOLLIN) {
				// Bytes in the input buffer
				while(true) {
					int r = recv(fd, buffer->buff + buffer->ctr,
								 MAX_REQ_LEN - buffer->ctr, 0);
					if(r <= 0)
						break;
					buffer->ctr += r;

					if(buffer->buff[buffer->ctr-1] == '\n') {
						// Got it
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
						handle(buffer);
						break;
					}
				}
			} else if(ev & (EPOLLRDHUP | EPOLLHUP)) {
				// Connection closed
				closeAndFree(buffer);
			} else if(ev & EPOLLOUT) {
				// Maybe EPOLLONESHOT would be better?
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, buffer->fd, nullptr);
				handleSend(buffer);
			}
		}
	}
}
