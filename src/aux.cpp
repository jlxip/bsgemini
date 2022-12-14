#include <common.hpp>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

// Auxiliary functions

// How many threads in the CPU?
size_t nproc() {
	size_t a=11, b=0, c=1, d=0;
	asm volatile("cpuid"
				 : "=a" (a),
				   "=b" (b),
				   "=c" (c),
				   "=d" (d)
				 : "0" (a), "2" (c));
	return b;
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
