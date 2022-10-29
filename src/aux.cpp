#include <common.hpp>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

// Auxiliary functions

void epoll_ctl_add(int fd, uint32_t events, bool useBuffer) {
	epoll_event ev;
	ev.events = events;

	Buffer* buffer = new Buffer;
	buffer->fd = fd;
	if(useBuffer)
		buffer->buff = new char[MAX_REQ_LEN];
	ev.data.ptr = buffer;

	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		std::cerr << "Could not add to epoll" << std::endl;
		exit(1);
	}
}

void setNonBlocking(int s) {
	int flags = fcntl(s, F_GETFL);
	if(flags < 0)
		close(s);
	if(fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0)
		close(s);
}

void closeAndFree(Buffer* buffer) {
	if(buffer->buff)
		delete [] buffer->buff;
	close(buffer->fd);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, buffer->fd, 0);
	delete buffer;
}

void replaceAll(std::string& a, const std::string& b, const std::string& c) {
	while(true) {
		size_t pos = a.find(b);
		if(pos == std::string::npos)
			break;
		a.replace(pos, b.size(), c);
	}
}

bool fileExists(const char* path) {
	struct stat statbuf;
	return stat(path, &statbuf) == 0;
}

bool isDir(const char* path) {
	struct stat statbuf;
	if(stat(path, &statbuf) != 0)
		return false;
	return S_ISDIR(statbuf.st_mode);
}

bool isFile(const char* path) {
	struct stat statbuf;
	if(stat(path, &statbuf) != 0)
		return false;
	return !S_ISDIR(statbuf.st_mode);
}

size_t getFileSize(const char* path) {
	struct stat statbuf;
	if(stat(path, &statbuf) != 0)
		return 0;
	return statbuf.st_size;
}

std::string lastSplit(const std::string& str, char c) {
	size_t begin = 0;
	bool changed = false;
	for(size_t i=0; i<str.size(); ++i) {
		if(str[i] == c) {
			begin = i;
			changed = true;
		}
	}

	if(!changed)
		return {};

	++begin;
	if(begin >= str.size())
		return {};

	return str.substr(begin);
}

std::vector<std::string> split(const std::string& str, char c) {
	std::vector<std::string> ret;

	size_t begin = 0;
	for(size_t i=0; i<str.size(); ++i) {
		if(str[i] == c) {
			ret.push_back(str.substr(begin, i-begin));
			begin = i+1;
		}
	}

	ret.push_back(str.substr(begin, str.size()));
	return ret;
}
