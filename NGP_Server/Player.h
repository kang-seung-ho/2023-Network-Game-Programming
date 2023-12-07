#pragma once
#define PLAYER_SIZE 30
#define WIN32_LEAN_AND_MEAN 

#include <Windows.h>
#include "header.h"

class Player  {
public:
	Player();
	Player(SOCKET& s, int id);
	~Player();

public:
	void update(bool collide);   // 플레이어 덥데이트
	void setPrevPos();
	void SetColor(int red, int green, int blue);
	
public:
	int m_id;
	float pos_x, pos_y;
	float dx, dy;
	float px, py;
	int fdir_x, fdir_y;
	COLORREF color;
	int power;
	int heat;
	float heat_count;
	int score;
	int hp = 100;
	float speed;

	SOCKET c_socket;
	int m_prev_size;
	char m_buf[BUFSIZE];
	RECT collision_rect;	// collision rect

	bool act;  // 생존여부
};

