#include "frametime.h"
#include "item.h"

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> uidType(1, 5);
std::uniform_int_distribution<int> uidPos(200, 1000);

item::item() : gameObject(x, y)
{
	x = uidPos(dre);
	y = uidPos(dre);
	itemType = uidType(dre);	

}

int item::getItemType()
{
	return itemType;
}
