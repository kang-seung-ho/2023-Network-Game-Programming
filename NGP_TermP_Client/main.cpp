#include <windows.h>
#include <tchar.h>
#include <random>
#include <vector>
//#include <list>
#include <chrono>

#include "frametime.h"
#include "gameObject.h"
//#include "player.h"
#include "bullet.h"
#include "item.h"
#include "obstacle.h"
#include "ui.h"

#include "Common.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Simple Shooting Game";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void CreateObstacles();
void DrawAll(HDC hdc);
void GameUpdate();
void CreateItem();

double frame_time = 0.0;


auto gameStartTime = std::chrono::high_resolution_clock::now();
int remainingTime = 120;

#define BUFFERSIZE 1024
#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000


struct cs_move {
	char size;
	char	type;
	char	direction;			// 0 : up, 1: down, 2:left, 3:right
};

/*
struct cs_attack {
	char size;
	char type;
	float aim_x;
	float aim_y;
};
*/
#define KEY 1
void sendKEY(SOCKET client_sock, char key)
{
	cs_move a;
	a.size = sizeof(cs_move);
	a.type = KEY;
	int retval = send(client_sock, (char*)&a, sizeof(cs_move), 0);
	if (client_sock == INVALID_SOCKET) err_quit("send()");
}

SOCKET CLIENT;
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	char buffer[BUFFERSIZE];

	// ���� ����
	CLIENT = socket(AF_INET, SOCK_STREAM, 0);
	if (CLIENT == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(CLIENT, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	/*closesocket(client_sock);*/

	return 0;
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
int  WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpszCmdParam, _In_  int nCmdShow)
{
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ��� ������ ����
	CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);


	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_QUESTION);
	WndClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	//--- ũ�� ���� ���� (���� (1024, 768))
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 560, 140, 800, 800, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

player* p = new player(100, 100);
ui* u = new ui;
std::vector<bullet*> bullets;
std::vector<item*> items;
std::vector<obstacle*> obstacles;
auto lastCreateTime = std::chrono::high_resolution_clock::now();

static TCHAR text[100];
int GameOverCnt{};

char ClientID{};
struct sc_login {
	char size;
	char type;
	char id;
};
sc_login myClientInfo;
int ret;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC	hdc, mdc, mapdc;
	static HBITMAP hBitmap, map;
	static HBRUSH BLACK, WHITE;

	switch (iMessage) {
	case WM_CREATE:
		CreateObstacles(); // ��ֹ� ���� �Լ�
		SetTimer(hWnd, 1, 32, NULL); // ���� ������Ʈ�Ǵ� ������ ���� ���� ��ü ������ �ӵ��� �޶���

		ret = recv(CLIENT, (char*)&myClientInfo, sizeof(sc_login), 0);
		ClientID = myClientInfo.id;

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		mapdc = CreateCompatibleDC(mdc);
		hBitmap = CreateCompatibleBitmap(hdc, 800, 800);
		map = CreateCompatibleBitmap(hdc, 1200, 1200);
		SelectObject(mdc, hBitmap);
		SelectObject(mapdc, map);

		////TextOut(hdc, 450, 400, L"GAME OVER", 9);
		DrawAll(mapdc);
		StretchBlt(mdc, 0, 0, 800, 800, mapdc, 0, 0, 1200, 1200, SRCCOPY);
		u->DrawHeat(mdc, p->GetHeat());
		u->DrawHP(mdc, p->GetHP());
		u->DrawTimer(mdc, remainingTime);
		u->DrawScore(mdc, p->GetScore());
		BitBlt(hdc, 0, 0, 800, 800, mdc, 0, 0, SRCCOPY);

		DeleteDC(mapdc);
		DeleteDC(mdc);
		DeleteObject(map);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		else if (wParam == VK_UP) {
			sendKEY(CLIENT, 'U');
			p->SetDirX(0);
			p->SetDirY(-1);
		}
		else if (wParam == VK_LEFT) {
			sendKEY(CLIENT, 'L');
			p->SetDirX(-1);
			p->SetDirY(0);
		}
		else if (wParam == VK_DOWN) {
			sendKEY(CLIENT, 'D');
			p->SetDirX(0);
			p->SetDirY(1);
		}
		else if (wParam == VK_RIGHT) {
			sendKEY(CLIENT, 'R');
			p->SetDirX(1);
			p->SetDirY(0);
		}
		else if (wParam == 'D' && p->GetHeat() < 10)
		{
			sendKEY(CLIENT, 'A');
			bullets.emplace_back(new bullet(p->GetPosX() + p->GetFDirX() * 25, p->GetPosY() + p->GetFDirY() * 25, p->GetFDirX(), p->GetFDirY()));
			p->SetHeat(p->GetHeat() + 1);
			p->SetHeatCount(3.0);
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_UP && p->GetDirY() == -1) {
			p->SetDirX(0);
			p->SetDirY(0);
		}

		else if (wParam == VK_LEFT && p->GetDirX() == -1) {
			p->SetDirX(0);
			p->SetDirY(0);
		}

		else if (wParam == VK_DOWN && p->GetDirY() == 1) {
			p->SetDirX(0);
			p->SetDirY(0);
		}
		else if (wParam == VK_RIGHT && p->GetDirX() == 1) {
			p->SetDirX(0);
			p->SetDirY(0);
		}
		break;
	case WM_TIMER:
		frame_time = GetFrameTime();

		GameUpdate(); // ���� ���� ������Ʈ

		{ // ��ȣ ������ ������ �ȵ�
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = now - lastCreateTime;

			if (elapsed.count() >= 3.0) {
				CreateItem(); // ������ ����
				lastCreateTime = now;
			}

			std::chrono::duration<double> elapsedTotal = now - gameStartTime;

			// ���� ���� ���� �� ����� �ð��� ����Ͽ� ���� �ð��� ������Ʈ
			remainingTime = 120 - static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(elapsedTotal).count());
			if (remainingTime <= 0) {
				// �ð��� �� ���� �� ������ �ڵ�
				GameOverCnt++;
				if (GameOverCnt == 1) {
					MessageBox(hWnd, L"�ð��� �� �Ǿ����ϴ�!", L"���� ����", MB_OK);
					PostQuitMessage(0);
				}
				if (GameOverCnt > 1) {
					break;
				}
			}

			InvalidateRect(hWnd, NULL, false);
		}

		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void CreateObstacles()
{
	// ���� ���
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(300 + i * OBSTACLE_SIZE, 300);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(300, 300 + i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

	// ���� ���
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(900 - i * OBSTACLE_SIZE, 300);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(900, 300 + i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

	// ���� �ϴ�
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(300 + i * OBSTACLE_SIZE, 900);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(300, 900 - i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

	// ���� �ϴ�
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(900 - i * OBSTACLE_SIZE, 900);
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(900, 900 - i * OBSTACLE_SIZE);
		obstacles.emplace_back(wall);
	}

}

void DrawAll(HDC hdc)
{
	p->Draw(hdc);
	for (auto& obstacle : obstacles)
		obstacle->draw(hdc);
	for (auto& b : bullets)
		b->draw(hdc);
	for (auto& item : items)
		item->draw(hdc);
}

void CreateItem()
{
	bool collide = true;
	while (collide) {
		item* newitem = new item;
		for (auto& obstacle : obstacles) { // ���� ��ֹ��� �浹 �˻�
			if (obstacle->CheckCollision(newitem)) {
				collide = true;
				break;
			}
			else
				collide = false;
		}

		if (!collide) { // ��ֹ��� �浹������ ����� �ǳʶٱ�
			for (auto& item : items) {
				if (item->CheckCollision(newitem)) {
					collide = true;
					break;
				}
				else
					collide = false;
			}
		}

		if (!collide) { // ��� ��ü�� �浹���� �ʾҾ�� �߰�
			items.emplace_back(newitem);
			break;
		}
		else
			delete newitem;
	}
}

void GameUpdate()
{
	// �ϴ� �����̰�, ������ ��ǥ���� ��ֹ��� �浹�ϸ� �ٽ� ���� ��ġ�� �ǵ���
	p->UpdateMove(true);
	// �÷��̾�-��ֹ� �浹üũ
	for (auto it = obstacles.begin(); it != obstacles.end();) {
		if (p->CheckCollision(*it)) {
			p->UpdateMove(false);
			p->SetDirX(0);
			p->SetDirY(0);
			break;
		}
		else
			++it;
	}

	p->UpdateHeat();

	// �÷��̾�-������ �浹 üũ
	for (auto it = items.begin(); it != items.end();) {
		if (p->CheckCollision(*it)) {
			p->UpdatePlayerStats((*it)->itemType);
			it = items.erase(it);
		}
		else
			++it;
	}
	// �÷��̾�-�Ѿ� �浹üũ(���߿� ��Ƽ���� ����)

	// �Ѿ�-�� �浹 üũ
	for (auto obs = obstacles.begin(); obs != obstacles.end(); ++obs) {
		for (auto bul = bullets.begin(); bul != bullets.end();) {
			if ((*bul)->CheckCollision(*obs))
				bul = bullets.erase(bul);
			else
				++bul;
		}
	}

	// �Ѿ� ���� �浹 üũ
	for (auto it = bullets.begin(); it != bullets.end();) {
		(*it)->update();
		if (((*it)->GetDirX() + (*it)->GetDirY()) == 0)
			it = bullets.erase(it);
		else
			++it;
	}
}