#include <Windows.h>
#include "obstacle.h"

void obstacle::draw(HDC hdc)
{
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

    BitBlt(hdc, x, y, ITEM_SIZE, ITEM_SIZE, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteDC(memDC);
}
