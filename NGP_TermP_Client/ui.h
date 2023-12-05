#include<windows.h>
#include "player.h"

class ui
{
public:
    ui() { }

    ~ui() { }

    void DrawBoundary(HDC hdc);
    void DrawHeat(HDC hdc, int heat, int ID);
    void DrawHP(HDC hdc, int hp, int ID);
    void DrawTimer(HDC hdc, int remainingTime);
    void DrawScore(HDC hdc, int score, int ID);
    void DrawName(HDC hdc, int ID);
    void DrawUI(HDC hdc, player* p, int remainingTime, int ID);
};

