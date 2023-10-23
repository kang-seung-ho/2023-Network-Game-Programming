#define POWERUP 1
#define SPEEDUP 2
#define HEAL 3
#define ICE 4
#define COIN 5

#include <windows.h>
#include <random>
#include <chrono>
using namespace std;

uniform_int_distribution uid1{ 50, 700 }; //가로
uniform_int_distribution uid2{ 50, 500 }; //세로
uniform_int_distribution uid3{ 1,5 }; //아이템
default_random_engine dre{ random_device{}() };

// Character 클래스 선언
class Character {
private:
    int x, y;
    const int size = 20;  // 캐릭터 크기

    int hp = 100;
    int speed = 3;
    int power = 10;
    int overHeating = 0;

public:

    HWND hwnd;
    Character(int x, int y, HWND hwnd) : x(x), y(y), hwnd(hwnd) {}

    void Draw(HDC hdc) {
        Rectangle(hdc, x, y, x + size, y + size);
    }

    void Move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void SpeedUp() {
        speed = 5;
    }

    void Speed() { //시간 지난 뒤 다시 원래 속도로 복원
        speed = 3;
    }

    void PowerUp() {
        power = 15;
    }

    void Power() { //시간 지난 뒤 다시 원래 속도로 복원
        power = 10;
    }

    void HealingHp() {
        if (hp + 30 >= 100) {
            hp = 100;
        }
        else {
            hp += 30;
        }
    }
};

class Item {
private:
    int x, y;
    int itemType;
    HBITMAP hBitmap;

public:
    Item(int x, int y) : x(x), y(y)
    {
        itemType = uid3(dre); // 랜덤 아이템

        if (itemType == POWERUP) {
            hBitmap = (HBITMAP)LoadImage(NULL, L"resources/power.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
            }
        }
        else if (itemType == SPEEDUP) {
            hBitmap = (HBITMAP)LoadImage(NULL, L"resources/speed.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
            }
        }
        else if (itemType == HEAL) {
            hBitmap = (HBITMAP)LoadImage(NULL, L"resources/heal.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
            }
        }
        else if (itemType == ICE) {
            hBitmap = (HBITMAP)LoadImage(NULL, L"resources/ice.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
            }
        }
        else if (itemType == COIN) {
            hBitmap = (HBITMAP)LoadImage(NULL, L"resources/coin.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
            }
        }
    }

    ~Item() {
        DeleteObject(hBitmap);
    }

    void Draw(HDC hdc) {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        BitBlt(hdc, x, y, 20, 20, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }

    void SetPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int GetX() const { return x; }
    int GetY() const { return y; }

};

class Wall {
private:
    int x, y;
    HBITMAP hBitmap;

public:
    Wall(int x, int y, LPCWSTR imagePath) : x(x), y(y)
    {
        hBitmap = (HBITMAP)LoadImage(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
    }

    ~Wall() {
        DeleteObject(hBitmap);  // 이미지 리소스 해제
    }

    void Draw(HDC hdc) {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        BitBlt(hdc, x, y, 30, 30, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }

    void SetPosition(int x, int y) {
        this->x = x;
        this->y = y;
    }

    int GetX() const { return x; }
    int GetY() const { return y; }

};


Character* player = NULL;  // 전역 포인터로 선언
Wall* walls[5] = { NULL, NULL, NULL, NULL, NULL };

vector<Item*> items;
auto lastCreateTime = std::chrono::high_resolution_clock::now();  // 마지막 Ice 생성 시간 초기화

//void CheckCollisions() {
//    for (auto it = ices.begin(); it != ices.end();) {
//        if (IsColliding(player->GetX(), player->GetY(), (*it)->GetX(), (*it)->GetY(), 20)) {
//            delete* it;
//            it = ices.erase(it);
//        }
//        else {
//            ++it;
//        }
//    }
//}

bool IsColliding(int x1, int y1, int x2, int y2, int size) {
    return (x1 < x2 + size && x1 + size > x2 && y1 < y2 + size && y1 + size > y2);
}

void SetRandomPositionForIce(Item* item) {
    bool collides = false;
    do {
        collides = false;
        int randX = uid1(dre);
        int randY = uid2(dre);

        // 기존 벽들과 충돌하는지 확인
        for (int i = 0; i < 5; ++i) {
            if (walls[i] && IsColliding(randX, randY, walls[i]->GetX(), walls[i]->GetY(), 30)) {
                collides = true;
                break;
            }
        }
        if (!collides) {
            item->SetPosition(randX, randY);
        }
    } while (collides);
}



void SetRandomPositionForWall(Wall* wall) {
    bool collides = false;
    do {
        collides = false;
        int randX = uid1(dre);
        int randY = uid2(dre);

        // 기존 벽들과 충돌하는지 확인
        for (int i = 0; i < 5; ++i) {
            if (walls[i] && IsColliding(randX, randY, walls[i]->GetX(), walls[i]->GetY(), 30)) {
                collides = true;
                break;
            }
        }
        if (!collides) {
            wall->SetPosition(randX, randY);
        }
    } while (collides);
}




LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {
    const TCHAR CLASS_NAME[] = L"NetworkGame";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    //화면크기 고정.
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"네트워크게임프로그래밍",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr);



    // 객체를 생성하고 전역 포인터에 할당
    player = new Character(100, 100, hwnd);

    // 벽 객체 초기화 위치 (랜덤)
    for (int i = 0; i < 5; ++i) {
        walls[i] = new Wall(0, 0, L"resources/wall.bmp"); // 초기 위치는 임의로 0,0으로 설정
        SetRandomPositionForWall(walls[i]);     // 여기서 위치를 랜덤하게 설정
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete player;  // 종료 전에 메모리 해제
    for (int i = 0; i < 5; ++i) {
        delete walls[i];
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        // 배경을 검정색으로 그리기
        FillRect(memDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        for (int i = 0; i < 5; ++i) {
            walls[i]->Draw(memDC);
        }

        for (auto item : items) {
            item->Draw(memDC);  // 모든 item 객체를 그림
        }

        player->Draw(memDC);  // 메모리 DC에 그림을 그리기

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);  // 메모리 DC의 내용을 실제 DC로 복사

        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
    } break;


    case WM_TIMER: {
        if (GetAsyncKeyState(VK_UP)) {
            player->Move(0, -3);
        }
        if (GetAsyncKeyState(VK_DOWN)) {
            player->Move(0, 3);
        }
        if (GetAsyncKeyState(VK_LEFT)) {
            player->Move(-3, 0);
        }
        if (GetAsyncKeyState(VK_RIGHT)) {
            player->Move(3, 0);
        }

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - lastCreateTime;

        if (elapsed.count() >= 5.0) {  // 5초가 지났는지 확인
            Item* newItem = new Item(0, 0);  // 초기 위치는 임의로 0,0으로 설정
            SetRandomPositionForIce(newItem);  // 위치 설정
            items.push_back(newItem);  // vector에 새로운 아이템 추가
            lastCreateTime = now;
        }

        //CheckCollisions();  // 아이템과 player 간의 충돌 확인

        InvalidateRect(hwnd, nullptr, TRUE);  // 화면 갱신
    } break;

    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000 / 60, NULL);  // 60 FPS로 타이머 설정
    } break;

    case WM_DESTROY:
        KillTimer(hwnd, 1);  // 타이머 해제
        PostQuitMessage(0);
        for (auto iceInstance : items) {
            delete iceInstance;
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}