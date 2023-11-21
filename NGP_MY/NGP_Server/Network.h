#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <list>
#include "../Game_Basic/Vector2D.h"

enum MSG
{
    MSG_WAITING, // 플레이어 로비상태(게임 시작 전) 메세지
    MSG_GAME, // 플레이어 인게임 상태 메세지
    MSG_COLLIDE, // 충돌 메세지
    MSG_LEAVE, // 플레이어 이탈 메세지
    MSG_CLEAR // 게임 클리어 메세지
};


class Data{
public:
    Data() { msg = -1; }
    ~Data() {}

    int getMsg() { return msg; }
    void setMsg(int setmsg) { msg = setmsg; }

protected:
    int msg;
};


class GamePlayer : public Data
{
public:
    GamePlayer() { }
    GamePlayer(char* id, Vector2D<float> pos, int dataMsg);
    ~GamePlayer() {};

    char* getId() { return ID; }
    Vector2D<float> getPos() { return position; }

private:
    char ID[10];
    Vector2D<float> position;
};


class WaitingPlayer : public Data      
{
public:
    WaitingPlayer() { }
    WaitingPlayer(char* id, Vector2D<float> state, int dataMsg);
    ~WaitingPlayer() {}

   char* getId() { return ID; }
   Vector2D<float> getState() { return ready; }

private: 
    char ID[10];
    Vector2D<float> ready;
};

class LeavePlayer : public Data
{
public:
    LeavePlayer() { }
    LeavePlayer(char* id, int dataMsg);
    ~LeavePlayer() { }

    char* GetID() { return ID; }

private:
    char ID[10];
};