#include <Windows.h>
#include "gameObject.h"
#include "define.h"

class player : public gameObject
{
private:
	int ID;
	int dx, dy;
	int fdx, fdy;
	COLORREF color;
	int power;
	int heat;
	float heat_count;
	int score;
	int HP;
	int speed;
public:
	player(int x, int y) : gameObject(x, y, PLAYER_SIZE), // 부모 클래스 생성자 호출
		ID(1), dx(0), dy(0), fdx(1), fdy(0),
		color(RGB(100, 255, 100)), power(10),
		heat(0), score(0), HP(100), speed(5)
	{};

	//getter..
	int GetDirX() const { return dx; }
	int GetDirY() const { return dy; }
	int GetFDirX() const { return fdx; }
	int GetFDirY() const { return fdy; }

	int GetID() const { return ID; }
	int GetPower() const { return power; }
	int GetHeat() const { return heat; }
	float GetHeatCount() const { return heat_count; }
	int GetScore() const { return score; }
	int GetHP() const { return HP; }
	int GetSpeed() const { return speed; }

	// setter..
	void SetDirX(int newX) { dx = newX; }
	void SetDirY(int newY) { dy = newY; }
	void SetFDirX(int newX) { fdx = newX; }
	void SetFDirY(int newY) { fdy = newY; }

	void SetID(int newID) { ID = newID; }
	void SetPower(int newPower) { power = newPower; }
	void SetHeat(int newHeat) { if (heat < 10) heat = newHeat; }
	void SetHeatCount(float newHeat_count) { heat_count = newHeat_count; }
	void SetScore(int newScore) { score = newScore; }
	void SetHP(int newHP) { HP = newHP; }
	void SetSpeed(int newSpeed) { speed = newSpeed; }
	void SetColor(COLORREF newColor) { color = newColor; }

	void UpdateHeat();
	void UpdateMove(bool collide);
	void UpdatePlayerStats(int itmeType);
	void Draw(HDC hdc);
};

