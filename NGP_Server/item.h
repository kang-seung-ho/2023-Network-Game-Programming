#pragma once

#include <random>
#include "gameObject.h"


class item : public gameObject
{
public:
	int itemType;

	item();
	~item() {};
	int getItemType();

};
