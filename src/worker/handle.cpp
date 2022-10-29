#include <common.hpp>
#include <iostream>

const std::string protodel = "://";
const std::string crlf = "\r\n";

static std::string noProto = "59 bsgemini: no protocol";
static std::string badProto = "53 bsgemini: protocol is not \"gemini\"\r\n";

void handle(Buffer* buffer) {
	int fd = buffer->fd;

	// Give me a string
	buffer->buff[buffer->ctr - 2] = '\0';
	std::string rec = buffer->buff;
	delete buffer->buff;
	buffer->buff = nullptr;

	size_t pos = rec.find(crlf);
	std::string path = rec.substr(0, pos);
	//std::string ip = rec.substr(pos+2); // <-- Not relevant to this project

	pos = rec.find(protodel);
	if(pos == std::string::npos) {
		// No protocol?
		sendstr(fd, noProto);
		closeAndFree(buffer);
		return;
	}

	std::string proto = path.substr(0, pos);
	path = path.substr(pos + protodel.size());
	if(proto != "gemini") {
		sendstr(fd, badProto);
		closeAndFree(buffer);
		return;
	}

	pos = path.find("/");
	if(pos == std::string::npos) {
		path = "/";
	} else {
		//std::string host = path.substr(0, pos); // <-- Not relevant either
		path = path.substr(pos);
		path += '/'; // This will mitigate path traversal in some edge cases
	}

	delete [] buffer->buff;
	buffer->buff = nullptr;
	handlePath(buffer, path);
}
