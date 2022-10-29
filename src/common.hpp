#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

extern std::unordered_map<std::string, std::string> MIMEs;
extern int serverSocket, epoll_fd;
extern std::string root;
extern size_t nthreads;

#define MAX_PATH_LEN 1024
#define MAX_REQ_LEN (MAX_PATH_LEN+2)

size_t nproc();
void prepareMIMEs();
void start();
[[noreturn]] void* worker(void*);
void epoll_ctl_add(int fd, uint32_t events, bool useBuffer);
void setNonBlocking(int conn);

struct Buffer {
	char* buff = nullptr;
	size_t ctr = 0; // Bytes written in "buff"
	int fd = 0;

	int filefd = 0;
	off_t off = 0;
	off_t sz = 0;
};

void closeAndFree(Buffer* buffer);
void handle(Buffer* buffer);
void handlePath(Buffer* buffer, std::string& path);
void handleSend(Buffer* buffer);
void replaceAll(std::string& a, const std::string& b, const std::string& c);

inline ssize_t sendstr(int fd, std::string& str) {
	return send(fd, str.c_str(), str.size(), 0);
}

// Now, some wrappers regarding files
bool fileExists(const char* path);
bool isDir(const char* path);
bool isFile(const char* path);
size_t getFileSize(const char* path);
std::string lastSplit(const std::string& str, char c);
std::vector<std::string> split(const std::string& str, char c);

#endif
