#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "network.h"

GamePlayer::GamePlayer(char* id, Vector2D<float> pos, int dataMsg)
{
    memset(ID, '\0', sizeof(ID));
    strcpy(ID, id);
    position = pos;
    msg = dataMsg;
}

WaitingPlayer::WaitingPlayer(char* id, Vector2D<float> state, int dataMsg)
{
    memset(ID, '\0', sizeof(ID));
    strcpy(ID, id);
    ready = state;
    msg = dataMsg;
}

LeavePlayer::LeavePlayer(char* id, int dataMsg)
{
    memset(ID, '\0', sizeof(ID));
    strcpy(ID, id);
    msg = dataMsg;
}
