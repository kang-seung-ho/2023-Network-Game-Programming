#include "item.h"
#include "define.h"
std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> uidType(1, 5);
std::uniform_int_distribution<int> uidPos(200, 1000);

item::item() : gameObject(x, y, ITEM_SIZE)
{
	x = uidPos(dre);
	y = uidPos(dre);
	itemType = uidType(dre);
}

int item::getItemType()
{
	return itemType;
}

int item::getItemID()
{
	return itemID;
}

void item::setItemID(int id)
{
	itemID = id;
}
