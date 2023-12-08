#pragma once 

class gameObject {
protected:
    int x, y;
    int objectSize;

public:
    gameObject(int x, int y) :x(x), y(y) {};
    ~gameObject() {};

    //getter �Լ���
    int GetPosX() const;
    int GetPosY() const;

    void SetPosX(int x);
    void SetPosY(int y);

    int GetObjectSize() const;

    bool CheckCollision(const gameObject* other);
};
