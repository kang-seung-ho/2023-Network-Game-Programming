#include "Player.h"

Player::Player() : pos_x(0), pos_y(0), m_id(0)
{
	power = 10;
	speed = 5.0;
	score = 0;
	heat = 0;
	act = true;
}

Player::Player(SOCKET& s, int id) :  c_socket(s), m_id(id)
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
	px = pos_x;
	py = pos_y;

	//��ġ ������Ʈ
	pos_x += dx;
	pos_y += dy;

	//�̵��� �ʱ�ȭ->Ű 1���� 1���� �̵��ؾ� �ϹǷ� dir �ʱ�ȭ
	dx = 0;
	dy = 0;
}

void Player::setPrevPos()
{
	pos_x = px;
	pos_y = py;
}

void Player::SetColor(int red, int green, int blue)
{
	// ������ �����ϴ� �Լ�
	color = RGB(red, green, blue);
};
