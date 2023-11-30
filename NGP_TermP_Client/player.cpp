#include "frametime.h"
#include "player.h"
#include "bullet.h"
#include "define.h"
#include "item.h"


void player::UpdateHeat()
{ // ������ ������Ʈ�� �߻� ���� ������Ʈ ���� ���� �ʿ�
	if (heat_count > 0)
		heat_count -= frame_time;
	else if (heat_count < 0)
		heat_count = 0;
	else if (heat_count == 0 && heat > 0)
	{
		heat--;
		heat_count = 0.5;
	}
}
void player::UpdateMove(bool collide)
{
	if (!collide) {
		x -= dx * speed;
		y -= dy * speed;
	}

	else {
		x += dx * speed;
		y += dy * speed;
	}

	if ((dx + dy) != 0) { // �����̰� �ִٸ� �߻� ������ �� �������� ����
		fdx = dx;
		fdy = dy;
	}
	if (x < 15) x = 15;
	if (y < 15) y = 15;
	if (x > 1135) x = 1135;
	if (y > 1100) y = 1100; //1185
}

void player::UpdatePlayerStats(int itemType)
{
	switch (itemType)
	{
	case POWERUP_ITEM:
		SetPower(GetPower() + 2);
		break;
	case SPEEDUP_ITEM:
		SetSpeed(GetSpeed() + 1);
		break;
	case HEAL_ITEM:
		if (GetHP() < 70)
			SetHP(GetHP() + 30);
		else
			SetHP(100);
		break;
	case ICE_ITEM:
		SetHeat(0);
		break;
	case COIN:
		SetScore(GetScore() + 5);
		break;
	default:
		break;
	}
}
void player::Draw(HDC hdc) {
	HBRUSH mb, ob;
	mb = CreateSolidBrush(color);
	ob = (HBRUSH)SelectObject(hdc, mb);
	/*Rectangle(hdc, pos.x - 15, pos.y - 15, pos.x + 15, pos.y + 15);
	Rectangle(hdc, pos.x - 5 - (fdir.x < 0 ? 25 : 0), pos.y - 5 - (fdir.y < 0 ? 25 : 0),
		pos.x + 5 + (fdir.x > 0 ? 25 : 0), pos.y + 5 + (fdir.y > 0 ? 25 : 0));
	Ellipse(hdc, pos.x - 10, pos.y - 10, pos.x + 10, pos.y + 10);*/
	// �÷��̾� ��ü �׸��� (���� ����� ��������)

	// ��ǥ ������ ���� ������� ����
	int cannonLength = 25;
	int cannonWidth = 10;

	Rectangle(hdc, x, y, x + PLAYER_SIZE, y + PLAYER_SIZE);
	Rectangle(hdc,
		x + 15 - (fdx < 0 ? cannonLength : cannonWidth / 2), // ���� �Ǵ� �߾ӿ��� ����
		y + 15 - (fdy < 0 ? cannonLength : cannonWidth / 2), // ���� �Ǵ� �߾ӿ��� ����
		x + 15 + (fdx > 0 ? cannonLength : cannonWidth / 2), // ������ �Ǵ� �߾ӿ��� ����
		y + 15 + (fdy > 0 ? cannonLength : cannonWidth / 2)); // �Ʒ��� �Ǵ� �߾ӿ��� ����
	Ellipse(hdc, x + 10, y + 10, x + 20, y + 20);
	SelectObject(hdc, ob);
	DeleteObject(mb);
}
