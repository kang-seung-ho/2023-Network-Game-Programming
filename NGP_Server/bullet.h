#pragma once
#include <Windows.h>
#include "gameObject.h"
#include "define.h"

class bullet : public gameObject
{
public:
	int dx, dy;
	char id;
	int b_id;
	bullet(char id, int b_id, int x, int y, int dx, int dy) : gameObject(x, y, BULLET_SIZE), id(id), b_id(b_id), dx(dx), dy(dy) {}
	~bullet() {};

	int GetDirX() const { return dx; }
	int GetDirY() const { return dy; }
	int GetID() const { return id; }

	void SetDirX(int newX) { dx = newX; }
	void SetDirY(int newY) { dy = newY; }
	void SetID(int newID) { id = newID; }

	void update();
	void draw(HDC hdc);

};