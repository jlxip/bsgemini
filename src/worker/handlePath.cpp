#include <common.hpp>
#include <iostream>
#include <fcntl.h>

static std::string notFound = "51 bsgemini: not found\r\n";
static std::string couldNotOpen = "51 bsgemini: could not open\r\n";

void handlePath(Buffer* buffer, std::string& path) {
	// This is actually the only "real" part of bsgemini, the rest is
	//   networking boilerplate

	auto fd = buffer->fd;

	replaceAll(path, "/../", "/");
	replaceAll(path, "/./", "/");
	replaceAll(path, "//", "/");

	// Finally, remove last slash
	if(path.size() > 1)
		path.pop_back();

	// Add BSG_ROOT
	path = root + '/' + path;

	// And now we check the file
	if(!fileExists(path.c_str())) {
		sendstr(fd, notFound);
		closeAndFree(buffer);
		return;
	}

	if(isDir(path.c_str())) {
		if(isFile((path + "/index.gemini").c_str())) {
			path += "/index.gemini";
		} else if(isFile((path + "/index.gmi").c_str())) {
			path += "/index.gmi";
		} else {
			sendstr(fd, notFound);
			closeAndFree(buffer);
			return;
		}
	}

	// It's guaranteed now that "path" exists and is a file
	replaceAll(path, "//", "/");

	// MIME type
	std::string mime;
	auto ext = lastSplit(path, '.');
	if(ext == "gemini" || ext == "gmi") {
		mime = "text/gemini";
	} else {
		auto it = MIMEs.find(ext);
		if(it == MIMEs.end())
			mime = "application/octet-stream";
		else
			mime = (*it).second;
	}

	// Let's set some values in buffer
	buffer->filefd = open(path.c_str(), O_RDONLY);
	if(buffer->filefd < 0) {
		sendstr(fd, couldNotOpen);
		closeAndFree(buffer);
		return;
	}
	buffer->sz = getFileSize(path.c_str());

	// Send the MIME type
	std::string header = "20 " + mime + "\r\n";
	sendstr(fd, header);

	// Let's try to sendfile
	handleSend(buffer);
}
