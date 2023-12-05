#include <tchar.h>
#include "ui.h"

void ui::DrawHeat(HDC hdc, int heat)
{
	HBRUSH heatBrush, oldBrush;

	SetBkColor(hdc, RGB(0, 0, 0)); // 텍스트 배경색을 검은색으로 설정
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상을 흰색으로 설정

	Rectangle(hdc, 50, 715, 250, 735);
	TextOut(hdc, 10, 715, L"과열", 2);
	heatBrush = CreateSolidBrush(RGB(55 + heat * 20, 100 - 10 * heat, 0)); // 그 외의 경우
	wchar_t heatText[10];
	swprintf_s(heatText, L"%d", heat);
	TextOut(hdc, 260, 715, heatText, wcslen(heatText));

	oldBrush = (HBRUSH)SelectObject(hdc, heatBrush);
	Rectangle(hdc, 50, 715, 50 + heat * 20, 735); // 과열 게이지 그리기
	SelectObject(hdc, oldBrush);
	DeleteObject(heatBrush);
}

void ui::DrawHP(HDC hdc, int hp)
{
	HBRUSH hpBrush, oldBrush;

	SetBkColor(hdc, RGB(0, 0, 0)); // 텍스트 배경색을 검은색으로 설정
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상을 흰색으로 설정

	Rectangle(hdc, 50, 740, 250, 760);
	TextOut(hdc, 10, 740, L"체력", 2);

	wchar_t hpText[10];
	swprintf_s(hpText, L"%d", hp);
	TextOut(hdc, 260, 740, hpText, wcslen(hpText));

	hpBrush = CreateSolidBrush(RGB(200 - hp * 2, hp * 2, 0)); // HP 게이지 색상
	oldBrush = (HBRUSH)SelectObject(hdc, hpBrush);
	Rectangle(hdc, 50, 740, 50 + hp * 2, 760); // HP 게이지 그리기
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

void ui::DrawScore(HDC hdc, int score) 
{
	SetBkColor(hdc, RGB(0, 0, 0)); // 텍스트 배경색을 검은색으로 설정
	SetTextColor(hdc, RGB(255, 255, 255)); // 텍스트 색상을 흰색으로 설정

	TextOut(hdc, 10, 690, L"점수", 2);

	wchar_t scoreText[10];
	swprintf_s(scoreText, L"%d", score);
	TextOut(hdc, 50, 690, scoreText, wcslen(scoreText));

}

void ui::DrawName(HDC hdc, int ID)
{
	SetBkColor(hdc, RGB(0, 0, 0));
	SetTextColor(hdc, RGB(255, 255, 255));

	TextOut(hdc, 10, 665, L"Player", 6);

	wchar_t nameText[10];
	swprintf_s(nameText, L"%d", ID);
	TextOut(hdc, 60, 665, nameText, wcslen(nameText));
}

void ui::DrawUI(HDC hdc, player* p, int remainingTime)
{
	DrawHeat(hdc, p->GetHeat());
	DrawHP(hdc, p->GetHP());
	DrawTimer(hdc, remainingTime);
	DrawScore(hdc, p->GetScore());
	DrawName(hdc, p->GetID());
}