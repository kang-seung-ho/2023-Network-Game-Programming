#pragma once
#include "Common.h"
#include "Network.h"

int sendWaitingPlayer(SOCKET sock, WaitingPlayer data)
{
	int retval;
	retval = send(sock, (char*)&data, sizeof(WaitingPlayer), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return -1;
	}
	return retval;

}