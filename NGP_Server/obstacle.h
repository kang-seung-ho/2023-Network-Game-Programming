#pragma once
#include "gameObject.h"
#include "define.h"

class obstacle : public gameObject
{
public:
	HBITMAP hBitmap;

	obstacle(int x, int y) : gameObject(x, y, OBSTACLE_SIZE)
	{
		
	};


};

