#include "frametime.h"
#include "item.h"
#include "define.h"

std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> uidType(1, 5);
std::uniform_int_distribution<int> uidPos(200, 1000);

item::item() : gameObject(x, y, ITEM_SIZE)
{
	
	switch (itemType)
	{
	case POWERUP_ITEM:
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/power.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case SPEEDUP_ITEM:
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/speed.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case HEAL_ITEM:
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/heal.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case ICE_ITEM:
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/ice.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	case COIN:
		hBitmap = (HBITMAP)LoadImage(NULL, L"resources/coin.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	default:
		break;
	}

}

void item::draw(HDC hdc)
{
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

	BitBlt(hdc, x, y, ITEM_SIZE, ITEM_SIZE, memDC, 0, 0, SRCCOPY);

	SelectObject(memDC, oldBitmap);
	DeleteDC(memDC);
}
