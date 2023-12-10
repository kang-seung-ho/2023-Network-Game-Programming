#pragma once

#include <random>
#include "gameObject.h"


class item : public gameObject
{
public:
	int itemType;
	int itemID;

	item();
	~item() {};
	int getItemType();
	int getItemID();

};
