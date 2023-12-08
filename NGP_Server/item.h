#pragma once

#include <Windows.h>
#include <random>
#include "gameObject.h"
#include "Protocol.h"

class item : public gameObject
{
public:
	int itemType;
	HBITMAP hBitmap;

	item();
	~item() {};
	int getItemType();

};
