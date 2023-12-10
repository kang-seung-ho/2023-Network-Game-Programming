#include "Player.h"
#include "define.h"

Player::Player() : gameObject(x, y, PLAYER_SIZE), m_id(0)
{
	power = 10;
	speed = 5.0;
	score = 0;
	heat = 0;
	act = true;
}

Player::Player(SOCKET& s, int id) : gameObject(x, y, PLAYER_SIZE), c_socket(s), m_id(id)
{
	power = 10;
	speed = 5.0;
	score = 0;
	heat = 0;
	act = true;
}

Player::~Player()
{

}

void Player::update(bool collide)
{
	/*px = pos_x;
	py = pos_y;*/

	//위치 업데이트
	/*pos_x += dx;
	pos_y += dy;*/

	//이동량 초기화->키 1번당 1번만 이동해야 하므로 dir 초기화
	dx = 0;
	dy = 0;
}

void Player::setPrevPos()
{
	/*pos_x = px;
	pos_y = py;*/
}

void Player::SetColor(int red, int green, int blue)
{
	// 색상을 설정하는 함수
	color = RGB(red, green, blue);
};
