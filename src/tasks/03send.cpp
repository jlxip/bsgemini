#include <common.hpp>
#include "tasks.hpp"
#include <sys/sendfile.h>

int task03send(SSockets_ctx* ctx) {
	Data* data = (Data*)(ctx->data);

	size_t rem = data->sz - data->off;
	auto r = sendfile(ctx->fd, data->filefd, &(data->off), rem);
	if(r < 0) {
		if(errno == EAGAIN)
			return SSockets_RET_WRITE;
		else
			return SSockets_RET_ERROR;
	}

	if(data->off == data->sz)
		return SSockets_RET_FINISHED;
	else
		return SSockets_RET_OK; // Again
}
