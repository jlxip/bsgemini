#include <common.hpp>
#include <iostream>
#include <ssockets.h>
#include "tasks/tasks.hpp"
#include <unistd.h>

#define MAX_EVENTS 32

std::string root;

void destroy(SSockets_ctx* ctx);

int main() {
	prepareMIMEs();

	const char* port_c = getenv("BSG_PORT");
	uint16_t port = 1964;
	if(port_c)
		port = atoi(port_c);
	const char* root_c = getenv("BSG_ROOT");
	root = root_c ? root_c : "/gemini";
	const char* nthreads_c = getenv("BSG_THREADS");
	size_t nthreads = nthreads_c ? atoi(nthreads_c) : 0;

	SSockets_addState(task00init);
	SSockets_addState(task01read);
	SSockets_addState(task02handle);
	SSockets_addState(task03send);
	SSockets_setDestroyCallback(destroy);
	SSockets_run("0.0.0.0", port, nthreads);
}

void destroy(SSockets_ctx* ctx) {
	Data* data = (Data*)(ctx->data);
	if(!data)
		return;

	if(data->buff)
		delete [] data->buff;
	if(data->filefd)
		close(data->filefd);

	delete data;
}
