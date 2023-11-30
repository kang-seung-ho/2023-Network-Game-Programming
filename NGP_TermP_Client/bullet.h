#pragma once
#include <Windows.h>
#include "gameObject.h"
#include "define.h"

class bullet : public gameObject
{
public:
	int dx, dy;
	bullet(int x, int y, int dx, int dy) : gameObject(x, y, BULLET_SIZE), dx(dx), dy(dy) {}
	~bullet() {};

	int GetDirX() const { return dx; }
	int GetDirY() const { return dy; }

	void SetDirX(int newX) { dx = newX; }
	void SetDirY(int newY) { dy = newY; }
	void update();
	void draw(HDC hdc);

};