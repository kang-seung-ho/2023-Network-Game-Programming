#include <tchar.h>
#include "ui.h"

void ui::DrawBoundary(HDC hdc)
{
	HBRUSH Brush, oldBrush;

	Brush = CreateSolidBrush(RGB(0, 200, 200));
	oldBrush = (HBRUSH)SelectObject(hdc, Brush);
	Rectangle(hdc, 0, 800, 800, 820);
	SelectObject(hdc, oldBrush);
	DeleteObject(Brush);
}

void ui::DrawHeat(HDC hdc, int heat, int ID)
{
	HBRUSH heatBrush, oldBrush;
	int setUIPos = (ID - 1) * 230;

	SetBkColor(hdc, RGB(0, 0, 0)); // 텍스트 배경색을 검은색으로 설정
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상을 흰색으로 설정

	Rectangle(hdc, 50 + setUIPos, 915, 200 + setUIPos, 935);
	TextOut(hdc, 10 + setUIPos, 915, L"과열", 2);
	heatBrush = CreateSolidBrush(RGB(55 + heat * 20, 100 - 10 * heat, 0)); // 그 외의 경우
	wchar_t heatText[10];
	swprintf_s(heatText, L"%d", heat);
	TextOut(hdc, 210 + setUIPos, 915, heatText, wcslen(heatText));

	oldBrush = (HBRUSH)SelectObject(hdc, heatBrush);
	Rectangle(hdc, 50 + setUIPos, 915, 50 + heat * 15 + setUIPos, 935); // 과열 게이지 그리기
	SelectObject(hdc, oldBrush);
	DeleteObject(heatBrush);
}

void ui::DrawHP(HDC hdc, int hp, int ID)
{
	HBRUSH hpBrush, oldBrush;
	int setUIPos = (ID - 1) * 230;

	SetBkColor(hdc, RGB(0, 0, 0)); // 텍스트 배경색을 검은색으로 설정
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상을 흰색으로 설정

	Rectangle(hdc, 50 + setUIPos, 940, 200 + setUIPos, 960);
	TextOut(hdc, 10 + setUIPos, 940, L"체력", 2);

	wchar_t hpText[10];
	swprintf_s(hpText, L"%d", hp);
	TextOut(hdc, 210 + setUIPos, 940, hpText, wcslen(hpText));

	hpBrush = CreateSolidBrush(RGB(200 - hp * 2, hp * 2, 0)); // HP 게이지 색상
	oldBrush = (HBRUSH)SelectObject(hdc, hpBrush);
	Rectangle(hdc, 50 + setUIPos, 940, 50 + hp * 1.5 + setUIPos, 960); // HP 게이지 그리기
	SelectObject(hdc, oldBrush);
	DeleteObject(hpBrush);
}

void ui::DrawTimer(HDC hdc, int remainingTime)
{
	RECT timerRect = { 380, 10, 420, 30 }; // 타이머 위치 및 크기 설정
	FillRect(hdc, &timerRect, CreateSolidBrush(RGB(0, 0, 0))); // 배경 채우기

	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상 설정
	wchar_t text[10];
	int minutes = remainingTime / 60;
	int seconds = remainingTime % 60;
	swprintf_s(text, L"%02d:%02d", minutes, seconds);
	DrawText(hdc, text, -1, &timerRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER); // 타이머 텍스트 그리기
}

void ui::DrawScore(HDC hdc, int score, int ID)
{
	int setUIPos = (ID - 1) * 230;

	SetBkColor(hdc, RGB(0, 0, 0)); // 텍스트 배경색을 검은색으로 설정
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상을 흰색으로 설정

	TextOut(hdc, 10 + setUIPos, 890, L"점수", 2);

	wchar_t scoreText[10];
	swprintf_s(scoreText, L"%d", score);
	TextOut(hdc, 50 + setUIPos, 890, scoreText, wcslen(scoreText));

}

void ui::DrawName(HDC hdc, int ID)
{
	int setUIPos = (ID - 1) * 230;

	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));

	TextOut(hdc, 10 + setUIPos, 865, L"Player", 6);

	wchar_t nameText[10];
	swprintf_s(nameText, L"%d", ID);
	TextOut(hdc, 60 + setUIPos, 865, nameText, wcslen(nameText));
}

void ui::DrawUI(HDC hdc, player* p, int remainingTime, int ID)
{
	DrawBoundary(hdc);
	DrawHeat(hdc, p->GetHeat(), p->GetID());
	DrawHP(hdc, p->GetHP(), p->GetID());
	DrawTimer(hdc, remainingTime);
	DrawScore(hdc, p->GetScore(), p->GetID());
	DrawName(hdc, p->GetID());
}