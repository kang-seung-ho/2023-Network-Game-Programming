#include "gameObject.h"

Vector2D<float> gameObject::GetPos() const
{
    return pos;
}

int gameObject::GetObjectSize() const
{
    return objectSize;
}

bool gameObject::CheckCollision(const gameObject* other)
{
    return (this->GetPos().x < (other->GetPos().x + other->GetObjectSize())) &&
        ((this->GetPos().x + this->GetObjectSize()) > other->GetPos().x) &&
        (this->GetPos().y < (other->GetPos().y + other->GetObjectSize())) &&
        ((this->GetPos().y + this->GetObjectSize()) > other->GetPos().y);
}

