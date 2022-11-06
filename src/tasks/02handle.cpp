#include <common.hpp>
#include "tasks.hpp"
#include <fcntl.h>

const std::string protodel = "://";
const std::string crlf = "\r\n";

static std::string noProto = "59 bsgemini: no protocol";
static std::string badProto = "53 bsgemini: protocol is not \"gemini\"\r\n";
static std::string notFound = "51 bsgemini: not found\r\n";
static std::string couldNotOpen = "51 bsgemini: could not open\r\n";

int handlePath(SSockets_ctx* ctx, std::string& path);

int task02handle(SSockets_ctx* ctx) {
	Data* data = (Data*)(ctx->data);

	// Give me a string
	data->buff[data->ctr - 2] = '\0';
	std::string rec = data->buff;
	delete data->buff;
	data->buff = nullptr;

	size_t pos = rec.find(crlf);
	std::string path = rec.substr(0, pos);
	//std::string ip = rec.substr(pos+2); // <-- Not relevant to this project

	pos = rec.find(protodel);
	if(pos == std::string::npos) {
		// No protocol?
		sendstr(ctx->fd, noProto);
		return SSockets_RET_ERROR;
	}

	std::string proto = path.substr(0, pos);
	path = path.substr(pos + protodel.size());
	if(proto != "gemini") {
		sendstr(ctx->fd, badProto);
		return SSockets_RET_ERROR;
	}

	pos = path.find("/");
	if(pos == std::string::npos) {
		path = "/";
	} else {
		//std::string host = path.substr(0, pos); // <-- Not relevant either
		path = path.substr(pos);
		path += '/'; // This will mitigate path traversal in some edge cases
	}

	return handlePath(ctx, path);
}

// Now for part 2
int handlePath(SSockets_ctx* ctx, std::string& path) {
	Data* data = (Data*)(ctx->data);

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
		sendstr(ctx->fd, notFound);
		return SSockets_RET_ERROR;
	}

	if(isDir(path.c_str())) {
		if(isFile((path + "/index.gemini").c_str())) {
			path += "/index.gemini";
		} else if(isFile((path + "/index.gmi").c_str())) {
			path += "/index.gmi";
		} else {
			sendstr(ctx->fd, notFound);
			return SSockets_RET_ERROR;
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
	data->filefd = open(path.c_str(), O_RDONLY);
	if(data->filefd < 0) {
		sendstr(ctx->fd, couldNotOpen);
		return SSockets_RET_ERROR;
	}
	data->sz = getFileSize(path.c_str());

	// Send the MIME type
	std::string header = "20 " + mime + "\r\n";
	sendstr(ctx->fd, header);

	// Now we jump
	ctx->state = STATE_SEND;
	return SSockets_RET_OK;
}
