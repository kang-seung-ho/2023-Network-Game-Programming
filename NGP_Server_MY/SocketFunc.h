#pragma once
#include "Common.h"
#include "Network.h"

// WaitingPlayer
int sendWaitingPlayer(SOCKET sock, WaitingPlayer data);

WaitingPlayer recvWaitingPlayer(SOCKET& sock);


// GamePlayer
inline void sendGamePlayer(SOCKET& sock, GamePlayer& data);

inline GamePlayer recvGamePlayer(SOCKET& sock);

void sendLeavePlayer(SOCKET sock, LeavePlayer data);

LeavePlayer recvLeavePlayer(SOCKET sock);