#pragma once
#include "Vector2D.h"
#include "gameObject.h"
#include "define.h"

class obstacle : public gameObject
{
public:
	HBITMAP hBitmap;

	obstacle(Vector2D<float> pos) : gameObject(Vector2D<float>(pos), OBSTACLE_SIZE) 
	{
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/wall.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	};

	void draw(HDC hdc);
};

