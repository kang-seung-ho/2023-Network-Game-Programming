#include<windows.h>
#include "player.h"

class ui
{
public:
    ui() { }

    ~ui() { }

    void DrawHeat(HDC hdc, int heat);
    void DrawHP(HDC hdc, int hp);
    void DrawTimer(HDC hdc, int remainingTime);
    void DrawScore(HDC hdc, int score);
};

