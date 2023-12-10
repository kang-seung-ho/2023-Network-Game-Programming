#pragma once 

class gameObject {
protected:
    int x, y;
    int objectSize;

public:
    gameObject(int x, int y, int objectSize) :x(x), y(y), objectSize(objectSize) {};
    ~gameObject() {};

    //getter ÇÔ¼öµé
    int GetPosX() const;
    int GetPosY() const;

    void SetPosX(int x);
    void SetPosY(int y);

    int GetObjectSize() const;

    bool CheckCollision(const gameObject* other);
};
