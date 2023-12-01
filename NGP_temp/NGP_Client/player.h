#include <Windows.h>
#include "Vector2D.h"
#include "gameObject.h"
#include "define.h"

class player : public gameObject
{
private:
	Vector2D<float> dir, fdir;
	COLORREF color;
	int power;
	int heat;
	float heat_count;
	int score;
	int HP;
	float speed;
public:
	player(Vector2D<float> pos) : gameObject(Vector2D<float>(pos), PLAYER_SIZE), // 부모 클래스의 생성자 호출
		dir(0, 0),
		fdir(1, 0),
		color(RGB(100, 255, 100)),
		power(10),
		heat(0),
		score(0),
		HP(100),
		speed(5.0) {}

	Vector2D<float> GetPos() const { return pos; }
	Vector2D<float> GetDir() const { return dir; }
	Vector2D<float> GetFDir() const { return fdir; }
	int GetPower() const { return power; }
	int GetHeat() const { return heat; }
	float GetHeatCount() const { return heat_count; }
	int GetScore() const { return score; }
	int GetHP() const { return HP; }
	float GetSpeed() const { return speed; }

	void UpdateHeat();
	void UpdateMove(bool collide);
	void UpdatePlayerStats(int itmeType);
	void Draw(HDC hdc);

	void SetDir(const Vector2D<float>& newDir);
	void SetHeat(int count);
	void SetHeatCount(float newCount);

	void PowerUp();
	void SpeedUp();
	void Heal();
	void CoolHeat();
	void ScoreUp();
};

