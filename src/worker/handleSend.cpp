#include <common.hpp>
#include <sys/epoll.h>
#include <sys/sendfile.h>

void handleSend(Buffer* buffer) {
	int fd = buffer->fd;

	while(buffer->off < buffer->sz) {
		size_t rem = buffer->sz - buffer->off;
		auto r = sendfile(fd, buffer->filefd, &(buffer->off), rem);
		if(r < 0) {
			if(errno == EAGAIN) {
				// Set it up on epoll
				epoll_event ev;
				ev.events = EPOLLOUT | EPOLLET | EPOLLHUP;
				ev.data.ptr = buffer;
				if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
					closeAndFree(buffer);
					return;
				}
			} else {
				// Failed
				closeAndFree(buffer);
				return;
			}
		}

		buffer->off += r;
	}

	// Finished!
	closeAndFree(buffer);
}
