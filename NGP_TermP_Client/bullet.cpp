#include "player.h"
#include "bullet.h"


void bullet::update() {
	x += dx * 10;
	y += dy * 10;
	if (x < 0 || y < 0 || x > 1200 || y > 1200) {
		SetDirX(0);
		SetDirY(0);
	}
}
void bullet::draw(HDC hdc) {
	HBRUSH mb, ob;
	mb = CreateSolidBrush(RGB(255, 100, 100));
	ob = (HBRUSH)SelectObject(hdc, mb);
	//Ellipse(hdc, pos.x - 5, pos.y - 5, pos.x + 5, pos.y + 5);
	Ellipse(hdc, x + 10, y + 10, x + 20, y + 20);
	SelectObject(hdc, ob);
	DeleteObject(mb);
}