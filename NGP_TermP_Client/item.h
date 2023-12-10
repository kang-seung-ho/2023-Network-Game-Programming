#pragma once

#include <Windows.h>
#include <random>
#include "gameObject.h"

class item : public gameObject
{
public:
	int itemType;
	int itemID;
	HBITMAP hBitmap;

	item();
	~item() {};

	void draw(HDC hdc);
	void setItemImage(int itemType);
	void setitemID(int id);
};
