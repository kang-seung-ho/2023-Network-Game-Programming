#include "SocketFunc.h"

// WaitingPlayer
int sendWaitingPlayer(SOCKET sock, WaitingPlayer data)
{
	int retval = send(sock, (char*)&data, sizeof(WaitingPlayer), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return -1;
	}
	return retval;

}

WaitingPlayer recvWaitingPlayer(SOCKET& sock)
{
	int retval;
	WaitingPlayer data;
	retval = recv(sock, (char*)&data, 52, 0);
	return data;
}

inline void sendGamePlayer(SOCKET& sock, GamePlayer& data)
{
	int retval;
	retval = send(sock, (char*)&data, 52, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return;
	}
}

inline GamePlayer recvGamePlayer(SOCKET& sock)
{
	int retval;
	GamePlayer data;
	retval = recv(sock, (char*)&data, 52, 0);
	return data;
}

void sendLeavePlayer(SOCKET sock, LeavePlayer data)
{
	int retval;
	retval = send(sock, (char*)&data, 52, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return;
	}
}

LeavePlayer recvLeavePlayer(SOCKET sock)
{
	int retval;
	LeavePlayer data;
	retval = recv(sock, (char*)&data, 52, 0);

	return data;
}