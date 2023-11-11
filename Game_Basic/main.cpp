#include <windows.h>
#include <tchar.h>
#include <random>
#include <vector>
//#include <list>
#include <chrono>

#include "Vector2D.h"
#include "frametime.h"
#include "gameObject.h"
#include "player.h"
#include "bullet.h"
#include "item.h"
#include "obstacle.h"

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Simple Shooting Game";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
void CreateObstacles();
void DrawAll(HDC hdc);
void GameUpdate();
void CreateItem();

double frame_time = 0.0;



//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
int  WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpszCmdParam, _In_  int nCmdShow)
{
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

	//--- 크기 변경 가능 (기존 (1024, 768))
	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 560, 140, 800, 800, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

player* p = new player(Vector2D<float>(100, 100));
std::vector<bullet*> bullets;
std::vector<item*> items;
std::vector<obstacle*> obstacles;
auto lastCreateTime = std::chrono::high_resolution_clock::now();

static TCHAR text[100];

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC	hdc, mdc, mapdc;
	static HBITMAP hBitmap, map;
	static HBRUSH BLACK, WHITE;

	switch (iMessage) {
	case WM_CREATE:
		CreateObstacles(); // 장애물 생성 함수
		SetTimer(hWnd, 1, 16, NULL);
		BLACK = CreateSolidBrush(RGB(0, 0, 0));
		WHITE = CreateSolidBrush(RGB(255, 255, 255));
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		mapdc = CreateCompatibleDC(mdc);
		hBitmap = CreateCompatibleBitmap(hdc, 800, 800);
		map = CreateCompatibleBitmap(hdc, 1200, 1200);
		SelectObject(mdc, hBitmap);
		SelectObject(mdc, WHITE);
		SelectObject(mapdc, map);
		//TextOut(hdc, 450, 400, L"GAME OVER", 9);
		DrawAll(mapdc); // 객체 전부 그리기

		Rectangle(mdc, -1, -1, 800, 800);
		//StretchBlt(mdc, 0, 0, 800, 800, mapdc, p->GetPos().x - 200, p->GetPos().y - 200, 400, 400, SRCCOPY);
		StretchBlt(mdc, 0, 0, 800, 800, mapdc, 0, 0, 1200, 1200, SRCCOPY);
		swprintf_s(text, L"score : %3d", p->GetScore());
		TextOut(mdc, 10, 692, text, 11);
		swprintf_s(text, L"%2d", p->GetHeat());
		TextOut(mdc, 255, 717, text, 2);
		swprintf_s(text, L"%3d", p->GetHP());
		TextOut(mdc, 255, 742, text, 3);
		TextOut(mdc, 10, 715, L"과열", 2);
		TextOut(mdc, 10, 740, L"체력", 2);
		Rectangle(mdc, 50, 715, 250, 735);
		Rectangle(mdc, 50, 740, 250, 760);
		HBRUSH mb, ob;
		if (p->GetHeat() == 10)
			mb = CreateSolidBrush(RGB(255, 0, 0));
		else
			mb = CreateSolidBrush(RGB(255, 200, 100));
		ob = (HBRUSH)SelectObject(mdc, mb);
		Rectangle(mdc, 50, 715, 50 + p->GetHeat() * 20, 735);
		SelectObject(mdc, ob);
		DeleteObject(mb);
		mb = CreateSolidBrush(RGB(255, 100, 100));
		ob = (HBRUSH)SelectObject(mdc, mb);
		Rectangle(mdc, 50, 740, 50 + p->GetHP() * 2, 760);
		SelectObject(mdc, ob);
		DeleteObject(mb);

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
		else if (wParam == VK_UP)
			p->SetDir({ 0, -1 });
		else if (wParam == VK_LEFT)
			p->SetDir({ -1, 0 });
		else if (wParam == VK_DOWN)
			p->SetDir({ 0, 1 });
		else if (wParam == VK_RIGHT)
			p->SetDir({ 1, 0 });
		else if (wParam == 'D' && p->GetHeat() < 10)
		{
			bullets.emplace_back(new bullet(p->GetPos() + p->GetFDir() * 25, p->GetFDir()));
			p->SetHeat(1);
			p->SetHeatCount(3.0);
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_UP && p->GetDir() == Vector2D<float>(0, -1))
			p->SetDir({ 0, 0 });
		else if (wParam == VK_LEFT && p->GetDir() == Vector2D<float>(-1, 0))
			p->SetDir({ 0, 0 });
		else if (wParam == VK_DOWN && p->GetDir() == Vector2D<float>(0, 1))
			p->SetDir({ 0, 0 });
		else if (wParam == VK_RIGHT && p->GetDir() == Vector2D<float>(1, 0))
			p->SetDir({ 0, 0 });
		break;
	case WM_TIMER:
		frame_time = GetFrameTime();

		GameUpdate(); // 게임 상태 업데이트

		{ // 괄호 없으면 컴파일 안됨
			auto now = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = now - lastCreateTime;

			if (elapsed.count() >= 5.0) {
				CreateItem(); // 아이템 생성
				lastCreateTime = now;
			}
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
	// 좌측 상단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(300 + i * OBSTACLE_SIZE, 300));
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(300, 300 + i * OBSTACLE_SIZE));
		obstacles.emplace_back(wall);
	}

	// 우측 상단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(900 - i * OBSTACLE_SIZE, 300));
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(900, 300 + i * OBSTACLE_SIZE));
		obstacles.emplace_back(wall);
	}

	// 좌측 하단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(300 + i * OBSTACLE_SIZE, 900));
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(300, 900 - i * OBSTACLE_SIZE));
		obstacles.emplace_back(wall);
	}

	// 우측 하단
	for (int i = 0; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(900 - i * OBSTACLE_SIZE, 900));
		obstacles.emplace_back(wall);
	}

	for (int i = 1; i < 5; i++)
	{
		obstacle* wall = new obstacle(Vector2D<float>(900, 900 - i * OBSTACLE_SIZE));
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
		for (auto& obstacle : obstacles) { // 먼저 장애물과 충돌 검사
			if (obstacle->CheckCollision(newitem)) {
				collide = true;
				break;
			}
			else
				collide = false;
		}

		if (!collide) { // 장애물과 충돌했으면 여기는 건너뛰기
			for (auto& item : items) {
				if (item->CheckCollision(newitem)) {
					collide = true;
					break;
				}
				else
					collide = false;
			}
		}

		if (!collide) { // 모든 객체와 충돌하지 않았어야 추가
			items.emplace_back(newitem);
			break;
		}
		else
			delete newitem;
	}
}

void GameUpdate()
{
	// 일단 움직이고, 움직인 좌표에서 장애물과 충돌하면 다시 원래 위치로 되돌림
	p->UpdateMove(true);
	// 플레이어-장애물 충돌체크
	for (auto it = obstacles.begin(); it != obstacles.end();) {
		if (p->CheckCollision(*it)) {
			p->UpdateMove(false);
			p->SetDir({ 0, 0 });
			break;
		}
		else
			++it;
	}

	p->UpdateHeat();

	// 플레이어-아이템 충돌 체크
	for (auto it = items.begin(); it != items.end();) {
		if (p->CheckCollision(*it)) {
			p->UpdatePlayerStats((*it)->itemType);
			it = items.erase(it);
		}
		else
			++it;
	}
	// 플레이어-총알 충돌체크(나중에 멀티에서 구현)

	// 총알-벽 충돌 체크
	for (auto obs = obstacles.begin(); obs != obstacles.end(); ++obs) {
		for (auto bul = bullets.begin(); bul != bullets.end();) {
			if ((*bul)->CheckCollision(*obs))
				bul = bullets.erase(bul);
			else
				++bul;
		}
	}

	// 총알 벽과 충돌 체크
	for (auto it = bullets.begin(); it != bullets.end();) {
		(*it)->update();
		if ((*it)->dir.GetLenth() == 0)
			it = bullets.erase(it);
		else
			++it;
	}
}