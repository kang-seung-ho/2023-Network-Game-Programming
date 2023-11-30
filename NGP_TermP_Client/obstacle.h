#pragma once
#include "gameObject.h"
#include "define.h"

class obstacle : public gameObject
{
public:
	HBITMAP hBitmap;

	obstacle(int x, int y) : gameObject(x, y, OBSTACLE_SIZE)
	{
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/wall.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	};

	void draw(HDC hdc);
};

