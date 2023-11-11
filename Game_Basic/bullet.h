#pragma once
#include <Windows.h>
#include "Vector2D.h"

class bullet
{
public:
	Vector2D<float> pos, dir;
	bullet(Vector2D<float> p, Vector2D<float> d) :pos(p), dir(d) {}
	void update();
	void draw(HDC hdc);
};