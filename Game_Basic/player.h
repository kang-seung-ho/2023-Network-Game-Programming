#pragma once
#include <Windows.h>
#include "Vector2D.h"

class player
{
public:
	Vector2D<float> pos, dir, fdir;
	COLORREF color;
	int heat;
	float heat_count;
	int score;
	int HP;
	float speed;
	player() :pos(100, 100), dir(0, 0), fdir(1, 0), color(RGB(100, 255, 100)),
		heat(0), score(0), HP(100), speed(5.0) {}
	void update();
	void draw(HDC hdc);
};

