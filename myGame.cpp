#define POWERUP_ITEM 1
#define SPEEDUP_ITEM 2
#define HEAL_ITEM 3
#define ICE_ITEM 4
#define WALL 5
#define BULLET 6
#define CHARACTER 10

#define BULLET_SPEED 10
#define BULLEAT_UP 11
#define BULLET_DOWN 12
#define BULLET_RIGHT 13
#define BULLET_LEFT 14

#define ITEM_SIZE 20
#define WALL_SIZE 30
#define CHARACTER_SIZE 30
#define WINDOW_SIZE 800

#include <windows.h>
#include <random>
#include <chrono>

using namespace std;

uniform_int_distribution randomWallPos{ 300, 500 };
uniform_int_distribution randomItemPos{ 100, 700 };
//uniform_int_distribution uid2{ 50, 500 }; //세로
uniform_int_distribution randomItemType{ 1,4 }; //아이템
default_random_engine dre{ random_device{}() };

class GameObject {
protected:
    int x, y; // 객체의 좌측 상단 모서리 좌표
    int objectSize;
    char objectType;
    HBITMAP hBitmap;

public:
    GameObject(int x, int y, int objectSize, char objectType) : x(x), y(y), objectSize(objectSize), objectType(objectType) {
        /* hBitmap = (HBITMAP)LoadImage(NULL, bitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
         if (hBitmap == NULL) {
             MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
         }*/ // 하위 클래스에서 생성자를 부를 때 부모 클래스 생성자가 먼저 불려서 비트맵이 NULL이라 에러가 계속 나옴
    };
    virtual ~GameObject() {
        if (hBitmap) {
            DeleteObject(hBitmap);
        }
    };

    //getter 함수들
    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetObjectSize() const { return objectSize; }
    char GetObjectType() const { return objectType; }

    void SetPosition(int x, int y) { // 오브젝트 랜덤 위치 지정 시 호출
        this->x = x;
        this->y = y;
    }

    bool CheckCollision(const GameObject* obj1, const GameObject* obj2) // 충돌체크
    {
        return (obj1->GetX() < (obj2->GetX() + obj2->GetObjectSize())) && ((obj1->GetX() + obj1->GetObjectSize()) > obj2->GetX()) &&
            (obj1->GetY() < (obj2->GetY() + obj2->GetObjectSize())) && ((obj1->GetY() + obj1->GetObjectSize()) > obj2->GetY());
    }

    void Draw(HDC hdc) {
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

        BitBlt(hdc, x, y, objectSize, objectSize, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBitmap);
        DeleteDC(memDC);
    }

};

class Character : public GameObject {
private:
    int hp;
    int speed;
    int power;
    int overHeating;
    char fireDir;

public:
    Character() : GameObject(50, 50, CHARACTER_SIZE, CHARACTER) {
        hBitmap = (HBITMAP)LoadImage(NULL, L"resources/player.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
        hp = 100;
        speed = 3;
        power = 6;
        overHeating = 0;
        fireDir = 0;
    };

    ~Character() {};

    void Draw(HDC hdc) {
        GameObject::Draw(hdc);
    };

    void Move(int dx, int dy) { // 위치 갱신 및 화면 밖으로 못나가게, 논리는 맞는데 실제로는 화면 크기가 좀 다름.. 맞출 방법 고민해야함.
        if ((x + dx >= 0) && (x + dx + CHARACTER_SIZE <= WINDOW_SIZE))
            x += dx;
        if ((y + dy >= 0) && (y + dy + CHARACTER_SIZE <= WINDOW_SIZE))
            y += dy;
    }

    char GetDir() const {
        return fireDir;
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
        power = 6;
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

class Item : public GameObject {
public:
    Item() : GameObject(0, 0, ITEM_SIZE, randomItemType(dre)) {
        hBitmap = (HBITMAP)LoadImage(NULL, GetBitmapPath(objectType), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
    }

    LPCWSTR GetBitmapPath(char itemType) {
        switch (itemType) {
        case POWERUP_ITEM:
            return L"resources/power.bmp";
        case SPEEDUP_ITEM:
            return L"resources/speed.bmp";
        case HEAL_ITEM:
            return L"resources/heal.bmp";
        case ICE_ITEM:
            return L"resources/ice.bmp";
        default:
            MessageBox(NULL, L"Unknown item type", L"Error", MB_OK);
            return L"";
        }
    }

    ~Item() {
        DeleteObject(hBitmap);
    }

    void Draw(HDC hdc) {
        GameObject::Draw(hdc);
    }
};

class Wall : public GameObject {
public:
    Wall() : GameObject(0, 0, WALL_SIZE, WALL) {
        hBitmap = (HBITMAP)LoadImage(NULL, L"resources/wall.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
    }

    ~Wall() {
        DeleteObject(hBitmap);  // 이미지 리소스 해제
    }

    void Draw(HDC hdc) {
        GameObject::Draw(hdc);
    }
};

class Bullet : public GameObject { // 고민이 좀 필요한 부분->나중에 작성하고 지우자
private:
    char dir;

public:
    Bullet() : GameObject(0, 0, WALL_SIZE, WALL) {
        hBitmap = (HBITMAP)LoadImage(NULL, L"resources/bullet.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
    }

    ~Bullet() {
        DeleteObject(hBitmap);  // 이미지 리소스 해제
    }

    void Draw(HDC hdc) {
        GameObject::Draw(hdc);
    }
};

Character* player = NULL;  // 전역 포인터로 선언
Wall* walls[5] = { NULL, NULL, NULL, NULL, NULL };

vector<Item*> items;
vector<Bullet*> bullets;
auto lastCreateTime = std::chrono::high_resolution_clock::now();  // 마지막 Item 생성 시간 초기화

void SetRandomPosition(GameObject* obj) { // 벽, 아이템 생성
    bool collision = false;
    do {
        collision = false;
        int randX, randY;
        if (obj->GetObjectType() == WALL)
        {
            randX = randomWallPos(dre);
            randY = randomWallPos(dre);
        }
        else {
            randX = randomItemPos(dre);
            randY = randomItemPos(dre);
        }

        obj->SetPosition(randX, randY);

        // 기존 벽들과 충돌하는지 확인
        for (int i = 0; i < 5; ++i) {
            if (walls[i] && walls[i] != obj && obj->CheckCollision(obj, walls[i])) {
                collision = true;
                break;
            }
        }

        for (Item* item : items) {
            if (obj->CheckCollision(obj, item)) {
                collision = true;
                break;
            }
        }
    } while (collision);
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
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_SIZE, WINDOW_SIZE,
        nullptr, nullptr, hInstance, nullptr);



    // 객체를 생성하고 전역 포인터에 할당
    player = new Character;

    // 벽 객체 초기화 위치 (랜덤)
    for (int i = 0; i < 5; ++i) {
        walls[i] = new Wall;
        SetRandomPosition(walls[i]);     // 여기서 위치를 랜덤하게 설정
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
            Item* newItem = new Item;  // 초기 위치는 임의로 0,0으로 설정
            SetRandomPosition(newItem);  // Ice 위치 설정
            items.emplace_back(newItem);  // vector에 새로운 아이템 추가
            lastCreateTime = now;
        }

        InvalidateRect(hwnd, nullptr, TRUE);  // 화면 갱신
    } break;

    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000 / 60, NULL);  // 60 FPS로 타이머 설정
    } break;

    case WM_DESTROY: {
        KillTimer(hwnd, 1);  // 타이머 해제
        PostQuitMessage(0);
        for (auto iceInstance : items) {
            delete iceInstance;
        }
        return 0;
    }


    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}