#include <windows.h>
#include <tchar.h>
#include <random>
#include <list>
#include "Vector2D.h"
#include "frametime.h"
#include "player.h"
#include "bullet.h"



HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Simple Shooting Game";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);



std::random_device rd;
std::default_random_engine dre(rd());
std::uniform_int_distribution<int> uid(0, 255);

double frame_time = 0.0;


struct item
{
	int x, y;
	COLORREF color;
	item() :color(RGB(uid(dre), uid(dre), uid(dre))) {}
};

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

player* p;
std::list<bullet*> bullets;
static TCHAR text[100];

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC	hdc, mdc, mapdc;
	static HBITMAP hBitmap, map;
	static HBRUSH BLACK, WHITE;

	switch (iMessage) {
	case WM_CREATE:
		p = new player;
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
		p->draw(mapdc);
		for (auto& b : bullets)
			b->draw(mapdc);

		Rectangle(mdc, -1, -1, 800, 800);
		StretchBlt(mdc, 0, 0, 800, 800, mapdc, p->pos.x - 200, p->pos.y - 200, 400, 400, SRCCOPY);
		swprintf_s(text, L"score : %3d", p->score);
		TextOut(mdc, 10, 692, text, 11);
		swprintf_s(text, L"%2d", p->heat);
		TextOut(mdc, 255, 717, text, 2);
		swprintf_s(text, L"%3d", p->HP);
		TextOut(mdc, 255, 742, text, 3);
		TextOut(mdc, 10, 715, L"과열", 2);
		TextOut(mdc, 10, 740, L"체력", 2);
		Rectangle(mdc, 50, 715, 250, 735);
		Rectangle(mdc, 50, 740, 250, 760);
		HBRUSH mb, ob;
		if (p->heat == 10)
			mb = CreateSolidBrush(RGB(255, 0, 0));
		else
			mb = CreateSolidBrush(RGB(255, 200, 100));
		ob = (HBRUSH)SelectObject(mdc, mb);
		Rectangle(mdc, 50, 715, 50 + p->heat * 20, 735);
		SelectObject(mdc, ob);
		DeleteObject(mb);
		mb = CreateSolidBrush(RGB(255, 100, 100));
		ob = (HBRUSH)SelectObject(mdc, mb);
		Rectangle(mdc, 50, 740, 50 + p->HP * 2, 760);
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
			p->dir = { 0, -1 };
		else if (wParam == VK_LEFT)
			p->dir = { -1, 0 };
		else if (wParam == VK_DOWN)
			p->dir = { 0, 1 };
		else if (wParam == VK_RIGHT)
			p->dir = { 1, 0 };
		else if (wParam == 'D' && p->heat < 10)
		{
			bullets.emplace_back(new bullet(p->pos + p->fdir * 25, p->fdir));
			p->heat++;
			p->heat_count = 3.0;
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_UP && p->dir == Vector2D<float>(0, -1))
			p->dir = { 0, 0 };
		else if (wParam == VK_LEFT && p->dir == Vector2D<float>(-1, 0))
			p->dir = { 0, 0 };
		else if (wParam == VK_DOWN && p->dir == Vector2D<float>(0, 1))
			p->dir = { 0, 0 };
		else if (wParam == VK_RIGHT && p->dir == Vector2D<float>(1, 0))
			p->dir = { 0, 0 };
		break;
	case WM_TIMER:
		frame_time = GetFrameTime();
		p->update();
		for (auto it = bullets.begin(); it != bullets.end(); ++it) {
			(*it)->update();
			if ((*it)->dir.GetLenth() == 0)
				it = bullets.erase(it);
		}
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}