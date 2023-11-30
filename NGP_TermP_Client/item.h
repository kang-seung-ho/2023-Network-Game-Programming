#pragma once

#include <Windows.h>
#include <random>
#include "gameObject.h"

class item : public gameObject
{
public:
	int itemType;
	HBITMAP hBitmap;

	item();
	~item() {};

	void draw(HDC hdc);
};
