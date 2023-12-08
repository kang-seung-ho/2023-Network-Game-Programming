#include "gameObject.h"

int gameObject::GetPosX() const
{
    return x;
}

int gameObject::GetPosY() const
{
    return y;
}

void gameObject::SetPosX(int newX)
{
    x = newX;
}

void gameObject::SetPosY(int newY)
{
    y = newY;
}

int gameObject::GetObjectSize() const
{
    return objectSize;
}


bool gameObject::CheckCollision(const gameObject* other)
{
    return (this->GetPosX() < (other->GetPosX() + other->GetObjectSize())) &&
        ((this->GetPosX() + this->GetObjectSize()) > other->GetPosX()) &&
        (this->GetPosY() < (other->GetPosY() + other->GetObjectSize())) &&
        ((this->GetPosY() + this->GetObjectSize()) > other->GetPosY());
}

