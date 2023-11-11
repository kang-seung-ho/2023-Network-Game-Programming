#include "Vector2D.h"
#include "frametime.h"
#include "player.h"
#include "bullet.h"


void player::update() {
	if (heat_count > 0)
		heat_count -= frame_time;
	else if (heat_count < 0)
		heat_count = 0;
	else if (heat_count == 0 && heat > 0)
	{
		heat--;
		heat_count = 0.5;
	}
	pos += dir * 5;
	if (dir.GetLenth() != 0)
		fdir = dir;
	if (pos.x < 15) pos.x = 15;
	if (pos.y < 15) pos.y = 15;
	if (pos.x > 1185) pos.x = 1185;
	if (pos.y > 1185) pos.y = 1185;
}
void player::draw(HDC hdc) {
	HBRUSH mb, ob;
	mb = CreateSolidBrush(color);
	ob = (HBRUSH)SelectObject(hdc, mb);
	Rectangle(hdc, pos.x - 15, pos.y - 15, pos.x + 15, pos.y + 15);
	Rectangle(hdc, pos.x - 5 - (fdir.x < 0 ? 25 : 0), pos.y - 5 - (fdir.y < 0 ? 25 : 0),
		pos.x + 5 + (fdir.x > 0 ? 25 : 0), pos.y + 5 + (fdir.y > 0 ? 25 : 0));
	Ellipse(hdc, pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10);
	SelectObject(hdc, ob);
	DeleteObject(mb);
}