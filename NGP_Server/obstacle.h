#pragma once
#include "gameObject.h"

class obstacle : public gameObject
{
public:
	HBITMAP hBitmap;

	obstacle(int x, int y) : gameObject(x, y)
	{
		
	};


};

