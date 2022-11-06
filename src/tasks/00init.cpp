#include <common.hpp>
#include "tasks.hpp"

int task00init(SSockets_ctx* ctx) {
	if(!ctx->data) {
		Data* data = new Data;
		ctx->data = data;
		data->buff = new char[MAX_REQ_LEN];
	}

	ctx->state = STATE_READ;
	return SSockets_RET_OK;
}
