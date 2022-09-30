#pragma once
#include "framework.h"

#define TRANSPARENT_COLOR 0x00A449A3

void ChangeBitmapPos(HWND hWnd, RECT *rect);
void Animate(HWND hWnd, RECT *rect, std::atomic<bool> *shouldAnimate);
void Bounce(HWND hWnd, RECT rect);
HDC CreateTransparentBitmap(HDC hdc, HBITMAP bitmap, BITMAP bitmapData, HBRUSH brush);