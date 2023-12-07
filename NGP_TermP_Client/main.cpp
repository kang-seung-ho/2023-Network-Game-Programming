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
#include "Protocol.h"

#include "Common.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Simple Shooting Game";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void CreateObstacles();
void DrawAllObjects(HDC hdc);
void GameUpdate();
void CreateItem();

double frame_time = 0.0;
double item_time = 0.0;
double remainingTime = 120.0;

#define BUFFERSIZE 1024
#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

player* p = new player(100, 100);
ui* UI = new ui;
std::vector<player*> players; // p�� �������� ������ ������ �޾ƿͼ� 3���� �÷��̾� ���Ϳ� ����
std::vector<bullet*> bullets;
std::vector<item*> items;
std::vector<obstacle*> obstacles;

static TCHAR text[100];
int GameOverCnt{};

char ClientID{};
sc_login myClientInfo;
int ret;

//void sendKEY(SOCKET client_sock, char key)
//{
//	cs_move a;
//	a.size = sizeof(cs_move);
//	a.type = KEY;
//	int retval = send(client_sock, (char*)&a, sizeof(cs_move), 0);
//	if (client_sock == INVALID_SOCKET) err_quit("send()");
//}

SOCKET client_sock;
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;
	//char buffer[BUFFERSIZE];
	char recv_buf[48];

	// ���� ����
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(client_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	//retval = recv(client_sock, (char*)&myClientInfo, sizeof(sc_login), 0);
	//ClientID = myClientInfo.id;
	//p->SetID((int)ClientID);

	////if (ClientID == 1) {
	////	//���� ����
	////}
	////else if (ClientID == 2) {

	////}
	////else if (ClientID == 3) {

	////}
	//sc_InitPos posSet;
	//retval = recv(client_sock, (char*)&posSet, sizeof(sc_InitPos), 0);
	//p->SetPosX(posSet.x);
	//p->SetPosY(posSet.y);

	while (true) {
		retval = recv(client_sock, recv_buf, sizeof(recv_buf), 0);
		if (retval == SOCKET_ERROR) { err_display("recv()");  return 0; }
		char* buf = recv_buf;
		//while (buf < recv_buf + retval) {
			char packet_size = *buf; // ù ����Ʈ�� ��Ŷ ũ��
			char packet_type = *(buf + 1); // �� ���� ����Ʈ�� ��Ŷ Ÿ��
			//if (packet_size <= 0) break;
			switch (packet_type) {
			case SC_P_INIT: {
				player* temp = new player(0, 0);
				sc_InitPlayer* packet = (sc_InitPlayer*)(buf);
				temp->SetID(packet->id);
				temp->SetPosX(packet->x);
				temp->SetPosY(packet->y);
				temp->SetColor(packet->color);
				players.emplace_back(temp);

				remainingTime = 120;
				break;
			}
			case SC_P_OTHER_INFO: {
				sc_move* packet = reinterpret_cast<sc_move*>(buf);
				// �ٸ� Ŭ���̾�Ʈ�� �׷��ִ� ��ü�� id�ο�
				// �ٸ� Ŭ���̾�Ʈ ��ü.id = packet->id;
				packet->id;
				
				break;
			}
			case SC_P_GAME_START: {
				// ���� �������� ����
				//start_game = true;
				
				break;
			}

			case SC_P_MOVE: {
				sc_move* packet = (sc_move*)buf;
				for (auto& player : players) {
					if (player->GetID() == packet->id) {
						player->SetPosX(packet->pos_x);
						player->SetPosY(packet->pos_y);
						player->SetFDirX(packet->fdir_x);
						player->SetFDirY(packet->fdir_y);
					}
				}
				
				// �۾�
				break;
			}

			case SC_P_DEAD: {
				sc_dead* packet = reinterpret_cast<sc_dead*>(buf);
				// �ش� id�� �ش��ϴ� id�����ֱ�
				break;
			}
			case SC_P_ITEM: {

				break;
			}
			case SC_P_BULLET: {
				sc_bullet* packet = reinterpret_cast<sc_bullet*>(buf);
				//int b_id = packet->id;
				// ~
				break;
			}
			case SC_P_HIT: {
				sc_hit* packet = reinterpret_cast<sc_hit*> (buf);
				//int p_id = packet->id;
				//std::cout << "hit" << p_id << endl;
				// ���� id�� hp�����̿��� ü�� ��ĭ�� ��������
				break;
			}
			buf = buf + packet_size;

		}

	}
	//closesocket(client_sock);
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
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 560, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC	hdc, mdc, mapdc;
	static HBITMAP hBitmap, map;

	cs_move* m = new cs_move;

	switch (iMessage) {
	case WM_CREATE:
		CreateObstacles(); // ��ֹ� ���� �Լ�
		SetTimer(hWnd, 1, 16, NULL); // ���� ������Ʈ�Ǵ� ������ ���� ���� ��ü ������ �ӵ��� �޶���

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		mapdc = CreateCompatibleDC(mdc);
		hBitmap = CreateCompatibleBitmap(hdc, 800, 1000);
		map = CreateCompatibleBitmap(hdc, 1200, 1200);
		SelectObject(mdc, hBitmap);
		SelectObject(mapdc, map);

		DrawAllObjects(mapdc);
		StretchBlt(mdc, 0, 0, 800, 800, mapdc, 0, 0, 1200, 1200, SRCCOPY);
		for (auto& player : players) {
			UI->DrawUI(mdc, player, remainingTime, player->GetID());
		}
		//UI->DrawUI(mdc, p, remainingTime, p->GetID());
		BitBlt(hdc, 0, 0, 800, 1000, mdc, 0, 0, SRCCOPY);

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
			p->SetDirX(0);
			p->SetDirY(-1);

			m->dir = wParam;
			m->id = p->GetID();
			send(client_sock, (char*)m, sizeof(cs_move), 0);
		}
		else if (wParam == VK_LEFT) {
			p->SetDirX(-1);
			p->SetDirY(0);

			m->dir = wParam;
			m->id = p->GetID();
			send(client_sock, (char*)m, sizeof(cs_move), 0);
		}
		else if (wParam == VK_DOWN) {
			p->SetDirX(0);
			p->SetDirY(1);

			m->dir = wParam;
			m->id = p->GetID();
			send(client_sock, (char*)m, sizeof(cs_move), 0);
		}
		else if (wParam == VK_RIGHT) {
			p->SetDirX(1);
			p->SetDirY(0);

			m->dir = wParam;
			m->id = p->GetID();
			send(client_sock, (char*)m, sizeof(cs_move), 0);
		}

		
		if (wParam == 'D' && p->GetHeat() < 10)
		{
			bullets.emplace_back(new bullet(p->GetID(), p->GetPosX() + p->GetFDirX() * 25, p->GetPosY() + p->GetFDirY() * 25, p->GetFDirX(), p->GetFDirY()));
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
		remainingTime -= frame_time;
		item_time += frame_time;
		if (item_time > CREATE_ITEM_TIME) {
			CreateItem();
			item_time -= CREATE_ITEM_TIME;
		}

		GameUpdate(); // ���� ���� ������Ʈ

		if (remainingTime < 0) {
			MessageBox(hWnd, L"�ð��� �� �Ǿ����ϴ�!", L"���� ����", MB_OK);
			PostQuitMessage(0);
		}

		InvalidateRect(hWnd, NULL, false);

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

void DrawAllObjects(HDC hdc)
{
	//p->Draw(hdc);
	for (auto& player : players)
		player->Draw(hdc);
	for (auto& obstacle : obstacles)
		obstacle->draw(hdc);
	for (auto& b : bullets)
		b->draw(hdc);
	for (auto& item : items)
		item->draw(hdc);
}

void CreateItem()
{
	bool collide = false;
	while (!collide) {
		item* newitem = new item;
		for (auto& obstacle : obstacles) { // ���� ��ֹ��� �浹 �˻�
			if (obstacle->CheckCollision(newitem)) {
				delete newitem;
				return;
			}
		}

		for (auto& item : items) {
			if (item->CheckCollision(newitem)) {
				delete newitem;
				return;
			}
		}

		collide = true;
		items.emplace_back(newitem);
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

	// �Ѿ�-�� �浹 üũ
	for (auto it = bullets.begin(); it != bullets.end();) {
		(*it)->update();
		if (((*it)->GetDirX() + (*it)->GetDirY()) == 0)
			it = bullets.erase(it);
		else
			++it;
	}
}