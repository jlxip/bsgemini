#include <common.hpp>
#include "tasks.hpp"

int task01read(SSockets_ctx* ctx) {
	Data* data = (Data*)(ctx->data);
	int r = recv(ctx->fd, data->buff + data->ctr, MAX_REQ_LEN - data->ctr, 0);
	if(r == 0) {
		return SSockets_RET_ERROR;
	} else if(r < 0) {
		if(errno == EAGAIN)
			return SSockets_RET_READ;
		return SSockets_RET_ERROR;
	}

	data->ctr += r;
	if(data->buff[data->ctr-1] == '\n') {
		// Got it
		ctx->state = STATE_HANDLE;
	} else {
		// Not yet
	}

	return SSockets_RET_OK;
}
