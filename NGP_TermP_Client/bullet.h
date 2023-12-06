#pragma once
#include <Windows.h>
#include "gameObject.h"
#include "define.h"

class bullet : public gameObject
{
public:
	int dx, dy;
	int ID;
	bullet(int ID, int x, int y, int dx, int dy) : gameObject(x, y, BULLET_SIZE), ID(ID), dx(dx), dy(dy) {}
	~bullet() {};

	int GetDirX() const { return dx; }
	int GetDirY() const { return dy; }
	int GetID() const { return ID; }

	void SetDirX(int newX) { dx = newX; }
	void SetDirY(int newY) { dy = newY; }
	void SetID(int newID) { ID = newID; }

	void update();
	void draw(HDC hdc);

};