#define POWERUP 1
#define SPEEDUP 2
#define HEAL 3
#define ICE 4
#define COIN 5
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define CHARACTER_WIDTH 30
#define CHARACTER_HEIGHT 30

#include <windows.h>
#include <random>
#include <chrono>
using namespace std;

uniform_int_distribution uid1{ 50, 1100 }; //����
uniform_int_distribution uid2{ 50, 630 }; //����
uniform_int_distribution uid3{ 1,5 }; //������
default_random_engine dre{ random_device{}() };

// Character Ŭ���� ����
class Character {
private:
    int x, y;
    const int size = 30;  // ĳ���� ũ��

    int hp = 100;
    int speed = 5;
    int power = 10;
    int overHeating = 0; //���� ������

    //������ Ű ���� ����
    int lastDirectionX = 1;  // 1: right, -1: left
    int lastDirectionY = 0;  // 1: down, -1: up

public:

    HWND hwnd;
    Character(int x, int y, HWND hwnd) : x(x), y(y), hwnd(hwnd) {}

    void Draw(HDC hdc) {
        Rectangle(hdc, x, y, x + size, y + size);
    }

    void Move(int dx, int dy) {
        int newX = x + dx;
        int newY = y + dy;

        // ȭ�� ��� üũ
        if (newX < 0) {
            newX = 0;
        }
        if (newX + CHARACTER_WIDTH > WINDOW_WIDTH) {//������ ���� �ʿ�
            newX = WINDOW_WIDTH - CHARACTER_WIDTH;
        }
        if (newY < 0) {
            newY = 0;
        }
        if (newY + 2*CHARACTER_HEIGHT + 10 > WINDOW_HEIGHT) {
            newY = WINDOW_HEIGHT - 2*CHARACTER_HEIGHT - 10;
        }

        x = newX;
        y = newY;
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

    int GetX() const { return x; }
    int GetY() const { return y; }

    void SetLastDirection(int dx, int dy) {
        if (dx != 0) {
            lastDirectionX = dx > 0 ? 1 : -1;
            lastDirectionY = 0;
        }
        if (dy != 0) {
            lastDirectionY = dy > 0 ? 1 : -1;
            lastDirectionX = 0;
        }
    }
    int GetLastDirectionX() const { return lastDirectionX; }
    int GetLastDirectionY() const { return lastDirectionY; }
};

//�򶧸��� Character�� overheating+1 ����� ��
class Bullet {
private:
    int x, y;
    const int size = 5;  // �Ѿ� ũ��
    const int speed = 10;  // �Ѿ� �ӵ�

    int directionX;
    int directionY;

public:
    Bullet(int x, int y, int directionX, int directionY) : x(x), y(y), directionX(directionX), directionY(directionY) {}

    void Draw(HDC hdc) {
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));  // �Ķ��� �귯�� ����
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);  // �귯�� ����

        Ellipse(hdc, x, y, x + size, y + size);  // �Ѿ� �׸���

        SelectObject(hdc, oldBrush);  // ���� �귯�� ����
        DeleteObject(hBrush);  // �귯�� ���ҽ� ����
    }

    void Move() {
        x += speed * directionX;
        y += speed * directionY;
    }

    int GetX() const { return x; }
    int GetY() const { return y; }
};


class Item {
private:
    int x, y;
    int itemType;
    HBITMAP hBitmap;

public:
    Item(int x, int y) : x(x), y(y)
    {
        itemType = uid3(dre); // ���� ������

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
        DeleteObject(hBitmap);  // �̹��� ���ҽ� ����
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


Character* player = NULL;  // ���� �����ͷ� ����
Wall* walls[5] = { NULL, NULL, NULL, NULL, NULL };

vector<Item*> items;
auto lastCreateTime = std::chrono::high_resolution_clock::now();  // ������ ������ ���� �ð� �ʱ�ȭ

vector<Bullet*> bullets;


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

        // ���� ����� �浹�ϴ��� Ȯ��
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

        // ���� ����� �浹�ϴ��� Ȯ��
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

    //ȭ��ũ�� ����.
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"��Ʈ��ũ�������α׷���",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr, nullptr, hInstance, nullptr);



    // ��ü�� �����ϰ� ���� �����Ϳ� �Ҵ�
    player = new Character(100, 100, hwnd);

    // �� ��ü �ʱ�ȭ ��ġ (����)
    for (int i = 0; i < 5; ++i) {
        walls[i] = new Wall(0, 0, L"resources/wall.bmp"); // �ʱ� ��ġ�� ���Ƿ� 0,0���� ����
        SetRandomPositionForWall(walls[i]);     // ���⼭ ��ġ�� �����ϰ� ����
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
            player->SetLastDirection(0, -1);
        }
        if (GetAsyncKeyState(VK_DOWN)) {
            player->Move(0, 3);
            player->SetLastDirection(0, 1);
        }
        if (GetAsyncKeyState(VK_LEFT)) {
            player->Move(-3, 0);
            player->SetLastDirection(-1, 0);
        }
        if (GetAsyncKeyState(VK_RIGHT)) {
            player->Move(3, 0);
            player->SetLastDirection(1, 0);
        }

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - lastCreateTime;

        if (elapsed.count() >= 5.0) {  // 5�ʰ� �������� Ȯ��
            Item* newItem = new Item(0, 0);  // �ʱ� ��ġ�� ���Ƿ� 0,0���� ����
            SetRandomPositionForIce(newItem);  // ��ġ ����
            items.push_back(newItem);  // vector�� ���ο� ������ �߰�
            lastCreateTime = now;
        }

        if (GetAsyncKeyState('D') & 0x8000) {
            Bullet* newBullet = new Bullet(
                player->GetX() + 20,
                player->GetY() + 10,
                player->GetLastDirectionX(),
                player->GetLastDirectionY()
            );
            bullets.push_back(newBullet);
        }

        for (auto bullet : bullets) {
            bullet->Move();  // ��� �Ѿ��� �����Դϴ�.
        }


        
        //CheckCollisions();  // �����۰� player ���� �浹 Ȯ��

        InvalidateRect(hwnd, nullptr, TRUE);  // ȭ�� ����
    } break;

    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000 / 60, NULL);  // 60 FPS�� Ÿ�̸� ����
    } break;

    case WM_DESTROY:
        KillTimer(hwnd, 1);  // Ÿ�̸� ����
        PostQuitMessage(0);
        for (auto itemInstance : items) {
            delete itemInstance;
        }

        for (auto bullet : bullets) {
            delete bullet;
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}