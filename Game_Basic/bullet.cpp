#include "Vector2D.h"
#include "player.h"
#include "bullet.h"


void bullet::update() {
	pos += dir * 10;
	if (pos.x < 0 || pos.y < 0 || pos.x > 1200 || pos.y > 1200) dir = { 0,0 };
}
void bullet::draw(HDC hdc) {
	HBRUSH mb, ob;
	mb = CreateSolidBrush(RGB(255, 100, 100));
	ob = (HBRUSH)SelectObject(hdc, mb);
	Ellipse(hdc, pos.x - 5, pos.y - 5, pos.x + 5, pos.y + 5);
	SelectObject(hdc, ob);
	DeleteObject(mb);
}