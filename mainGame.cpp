#include <windows.h>
#include <random>
#include <chrono>
using namespace std;

uniform_int_distribution uid1{ 50, 700 }; //����
uniform_int_distribution uid2{ 50, 500 }; //����
default_random_engine dre{ random_device{}() };

// Character Ŭ���� ����
class Character {
private:
    int x, y;
    const int size = 20;  // ĳ���� ũ��

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

	void item_SPEED_UP() {
		speed = 5;
	}

	void revert_SPEED() { //�ð� ���� �� �ٽ� ���� �ӵ��� ����
		speed = 3;
	}

    void item_Power_UP() { 
        power = 15;
    }

    void revert_Power() { //�ð� ���� �� �ٽ� ���� �ӵ��� ����
        power = 10;
    }



    void hp_UP() {
        if (hp + 30 >= 100) {
            hp = 100;
        }
        else {
            hp += 30;
        }
    }
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

class Ice {
private:
    int x, y;
    HBITMAP hBitmap;

public:
    Ice(int x, int y, LPCWSTR imagePath) : x(x), y(y)
    {
        hBitmap = (HBITMAP)LoadImage(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
    }

    ~Ice() {
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

class PowerUp {
private:
	int x, y;
	HBITMAP hBitmap;

public:
    PowerUp(int x, int y, LPCWSTR imagePath) : x(x), y(y)
	{
		hBitmap = (HBITMAP)LoadImage(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		if (hBitmap == NULL) {
			MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
		}
	}

	~PowerUp() {
		DeleteObject(hBitmap);  // �̹��� ���ҽ� ����
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


class Coin {
private:
    int x, y;
    HBITMAP hBitmap;

public:
    Coin(int x, int y, LPCWSTR imagePath) : x(x), y(y)
    {
        hBitmap = (HBITMAP)LoadImage(NULL, imagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap == NULL) {
            MessageBox(NULL, L"Failed to load image", L"Error", MB_OK);
        }
    }

    ~Coin() {
        DeleteObject(hBitmap);  // �̹��� ���ҽ� ����
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

Character* player = NULL;  // ���� �����ͷ� ����
Wall* walls[5] = { NULL, NULL, NULL, NULL, NULL };

vector<Ice*> ices;
auto lastIceTime = std::chrono::high_resolution_clock::now();  // ������ Ice ���� �ð� �ʱ�ȭ

vector<Coin*> coins;
auto lastCoinTime = std::chrono::high_resolution_clock::now();  // ������ Coin ���� �ð� �ʱ�ȭ

bool IsColliding(int x1, int y1, int x2, int y2, int size) {
    return (x1 < x2 + size && x1 + size > x2 && y1 < y2 + size && y1 + size > y2);
}


void SetRandomPositionForCoin(Coin* coin) {
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
            coin->SetPosition(randX, randY);
        }
    } while (collides);
}


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


void SetRandomPositionForIce(Ice* ice) {
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
            ice->SetPosition(randX, randY);
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
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr);



    // ��ü�� �����ϰ� ���� �����Ϳ� �Ҵ�
    player = new Character(100, 100, hwnd);

    // �� ��ü �ʱ�ȭ ��ġ (����)
    for (int i = 0; i < 5; ++i) {
        walls[i] = new Wall(0, 0, L"wall.bmp"); // �ʱ� ��ġ�� ���Ƿ� 0,0���� ����
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

        for (auto iceInstance : ices) {
            iceInstance->Draw(memDC);  // ��� ice ��ü�� �׸�
        }

        for (auto coinInstance : coins) {
            coinInstance->Draw(memDC);  // ��� coin ��ü�� �׸�
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
        std::chrono::duration<double> elapsed = now - lastIceTime;

        if (elapsed.count() >= 13.0) {  // 13�ʰ� �������� Ȯ��
            Ice* newIce = new Ice(0, 0, L"ice.bmp");  // �ʱ� ��ġ�� ���Ƿ� 0,0���� ����
            SetRandomPositionForIce(newIce);  // Ice ��ġ ����
            ices.push_back(newIce);  // vector�� ���ο� ice �߰�
            lastIceTime = now;
        }

        auto nowCoin = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedCoin = nowCoin - lastCoinTime;

        if (elapsedCoin.count() >= 5.0) {  // 5�ʰ� �������� Ȯ��
            Coin* newCoin = new Coin(0, 0, L"coin.bmp");  // �ʱ� ��ġ�� ���Ƿ� 0,0���� ����
            SetRandomPositionForCoin(newCoin);  // Coin ��ġ ����
            coins.push_back(newCoin);  // vector�� ���ο� coin �߰�
            lastCoinTime = nowCoin;
        }


        //CheckCollisions();  // ice�� player ���� �浹 Ȯ��

        InvalidateRect(hwnd, nullptr, TRUE);  // ȭ�� ����
    } break;

    case WM_CREATE: {
        SetTimer(hwnd, 1, 1000 / 60, NULL);  // 60 FPS�� Ÿ�̸� ����
    } break;

    case WM_DESTROY:
        KillTimer(hwnd, 1);  // Ÿ�̸� ����
        PostQuitMessage(0);
        for (auto iceInstance : ices) {
            delete iceInstance;
        }
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
