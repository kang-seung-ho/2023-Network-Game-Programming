#pragma once 

#include "Vector2D.h"
class gameObject {
protected:
    Vector2D<float> pos;
    int objectSize;

public:
    gameObject(Vector2D<float> pos, int objectSize) :pos(pos.x, pos.y), objectSize(objectSize) {};
    ~gameObject() {};

    //getter ÇÔ¼öµé
    Vector2D<float> GetPos() const;
    int GetObjectSize() const;

    bool CheckCollision(const gameObject* other);
};
