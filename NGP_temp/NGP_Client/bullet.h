#pragma once
#include <Windows.h>
#include "Vector2D.h"
#include "gameObject.h"

class bullet : public gameObject
{
public:
	Vector2D<float> dir;
	bullet(Vector2D<float> pos, Vector2D<float> dir) : gameObject(Vector2D<float>(pos), BULLET_SIZE), dir(dir) {}
	~bullet() {};
	void update();
	void draw(HDC hdc);
};
