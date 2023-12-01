#include "Vector2D.h"
#include "frametime.h"
#include "player.h"
#include "bullet.h"
#include "define.h"
#include "item.h"


void player::UpdateHeat()
{ // 움직임 업데이트와 발사 관련 업데이트 따로 수정 필요
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
	if (!collide)
		pos -= dir * speed;
	else
		pos += dir * speed;

	if (dir.GetLenth() != 0)
		fdir = dir;
	if (pos.x < 15) pos.x = 15;
	if (pos.y < 15) pos.y = 15;
	if (pos.x > 1135) pos.x = 1135;
	if (pos.y > 1100) pos.y = 1100; //1185
}

void player::UpdatePlayerStats(int itemType)
{
	switch (itemType)
	{
	case POWERUP_ITEM:
		PowerUp();
		break;
	case SPEEDUP_ITEM:
		SpeedUp();
		break;
	case HEAL_ITEM:
		Heal();
		break;
	case ICE_ITEM:
		CoolHeat();
		break;
	case COIN:
		ScoreUp();
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
	// 플레이어 본체 그리기 (좌측 상단을 기준으로)

	// 좌표 기준을 좌측 상단으로 변경
	int cannonLength = 25;
	int cannonWidth = 10;

	Rectangle(hdc, pos.x, pos.y, pos.x + PLAYER_SIZE, pos.y + PLAYER_SIZE);
	Rectangle(hdc,
		pos.x + 15 - (fdir.x < 0 ? cannonLength : cannonWidth / 2), // 왼쪽 또는 중앙에서 시작
		pos.y + 15 - (fdir.y < 0 ? cannonLength : cannonWidth / 2), // 위쪽 또는 중앙에서 시작
		pos.x + 15 + (fdir.x > 0 ? cannonLength : cannonWidth / 2), // 오른쪽 또는 중앙에서 시작
		pos.y + 15 + (fdir.y > 0 ? cannonLength : cannonWidth / 2)); // 아래쪽 또는 중앙에서 시작
	Ellipse(hdc, pos.x + 10, pos.y + 10, pos.x + 20, pos.y + 20);
	SelectObject(hdc, ob);
	DeleteObject(mb);
}

void player::PowerUp()
{
	power += 2;
}

void player::SpeedUp()
{
	speed += 0.5; // 일단 이렇게 해보자
}

void player::Heal()
{
	if (HP += 30 > 100)
		HP = 100;
	else
		HP += 30;
}

void player::CoolHeat()
{
	heat = 0;
}

void player::ScoreUp()
{
	score += 5;
}

void player::SetDir(const Vector2D<float>& newDir)
{
	dir = newDir;
}

void player::SetHeat(int count) {
	if (heat < 10)
		heat += count;
}

void player::SetHeatCount(float newCount)
{
	heat_count = newCount;
}
