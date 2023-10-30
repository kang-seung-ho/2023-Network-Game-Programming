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
//uniform_int_distribution uid2{ 50, 500 }; //����
uniform_int_distribution randomItemType{ 1,4 }; //������
default_random_engine dre{ random_device{}() };

class GameObject {
protected:
    int x, y; // ��ü�� ���� ��� �𼭸� ��ǥ
    int objectSize;
    char objectType;
    HBITMAP hBitmap;

public:
    GameObject(int x, int y, int objectSize, char objectType) : x(x), y(y), objectSize(objectSize), objectType(objectType) {
        /* hBitmap = (HBITMAP)LoadImage(NULL, bitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
         if (hBitmap == NULL) {
             MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
         }*/ // ���� Ŭ�������� �����ڸ� �θ� �� �θ� Ŭ���� �����ڰ� ���� �ҷ��� ��Ʈ���� NULL�̶� ������ ��� ����
    };
    virtual ~GameObject() {
        if (hBitmap) {
            DeleteObject(hBitmap);
        }
    };

    //getter �Լ���
    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetObjectSize() const { return objectSize; }
    char GetObjectType() const { return objectType; }

    void SetPosition(int x, int y) { // ������Ʈ ���� ��ġ ���� �� ȣ��
        this->x = x;
        this->y = y;
    }

    bool CheckCollision(const GameObject* obj1, const GameObject* obj2) // �浹üũ
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

    void Move(int dx, int dy) { // ��ġ ���� �� ȭ�� ������ ��������, ���� �´µ� �����δ� ȭ�� ũ�Ⱑ �� �ٸ�.. ���� ��� ����ؾ���.
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

    void Speed() { //�ð� ���� �� �ٽ� ���� �ӵ��� ����
        speed = 3;
    }

    void PowerUp() {
        power = 15;
    }

    void Power() { //�ð� ���� �� �ٽ� ���� �ӵ��� ����
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
        DeleteObject(hBitmap);  // �̹��� ���ҽ� ����
    }

    void Draw(HDC hdc) {
        GameObject::Draw(hdc);
    }
};

class Bullet : public GameObject { // ����� �� �ʿ��� �κ�->���߿� �ۼ��ϰ� ������
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
        DeleteObject(hBitmap);  // �̹��� ���ҽ� ����
    }

    void Draw(HDC hdc) {
        GameObject::Draw(hdc);
    }
};

Character* player = NULL;  // ���� �����ͷ� ����
Wall* walls[5] = { NULL, NULL, NULL, NULL, NULL };

vector<Item*> items;
vector<Bullet*> bullets;
auto lastCreateTime = std::chrono::high_resolution_clock::now();  // ������ Item ���� �ð� �ʱ�ȭ

void SetRandomPosition(GameObject* obj) { // ��, ������ ����
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

        // ���� ����� �浹�ϴ��� Ȯ��
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

    //ȭ��ũ�� ����.
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"��Ʈ��ũ�������α׷���",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_SIZE, WINDOW_SIZE,
        nullptr, nullptr, hInstance, nullptr);



    // ��ü�� �����ϰ� ���� �����Ϳ� �Ҵ�
    player = new Character;

    // �� ��ü �ʱ�ȭ ��ġ (����)
    for (int i = 0; i < 5; ++i) {
        walls[i] = new Wall;
        SetRandomPosition(walls[i]);     // ���⼭ ��ġ�� �����ϰ� ����
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete player;  // ���� ���� �޸� ����
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

        // ����� ���������� �׸���
        FillRect(memDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

        for (int i = 0; i < 5; ++i) {
            walls[i]->Draw(memDC);
        }

        for (auto item : items) {
            item->Draw(memDC);  // ��� item ��ü�� �׸�
        }

        player->Draw(memDC);  // �޸� DC�� �׸��� �׸���

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);  // �޸� DC�� ������ ���� DC�� ����

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

        if (elapsed.count() >= 5.0) {  // 5�ʰ� �������� Ȯ��
            Item* newItem = new Item;  // �ʱ� ��ġ�� ���Ƿ� 0,0���� ����
            SetRandomPosition(newItem);  // Ice ��ġ ����
            items.emplace_back(newItem);  // vector�� ���ο� ������ �߰�
            lastCreateTime = now;
        }

        InvalidateRect(hwnd, nullptr, TRUE);  // ȭ�� ����
    } break;

    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000 / 60, NULL);  // 60 FPS�� Ÿ�̸� ����
    } break;

    case WM_DESTROY: {
        KillTimer(hwnd, 1);  // Ÿ�̸� ����
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