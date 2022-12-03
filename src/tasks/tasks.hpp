#ifndef TASKS_HPP
#define TASKS_HPP

#include <ssockets.h>

enum {
	STATE_INIT,
	STATE_READ,
	STATE_HANDLE,
	STATE_SEND,
};

int task00init(SSockets_ctx* ctx);
int task01read(SSockets_ctx* ctx);
int task02handle(SSockets_ctx* ctx);
int task03send(SSockets_ctx* ctx);

#endif
