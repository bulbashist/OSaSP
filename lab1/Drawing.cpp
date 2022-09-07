#include "Drawing.h"

int MOVE_RIGHT = 5;
int MOVE_BOTTOM = 5;

HDC CreateTransparentBitmap(HDC hdc, HBITMAP bitmap, BITMAP bitmapData, HBRUSH brush) {
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP memMap = CreateCompatibleBitmap(hdc, bitmapData.bmWidth, bitmapData.bmHeight);
	SelectObject(memDC, memMap);

	RECT rect = { 0, 0, bitmapData.bmWidth, bitmapData.bmHeight };
	FillRect(memDC, &rect, brush);

	HDC imgDC = CreateCompatibleDC(hdc);
	HDC maskDC = CreateCompatibleDC(hdc);
	SelectObject(imgDC, bitmap);
	COLORREF color = GetPixel(imgDC, 1, 1);
	HBITMAP mask = CreateBitmap(bitmapData.bmWidth, bitmapData.bmHeight, 1, 1, nullptr);
	SelectObject(maskDC, mask);

	SetBkColor(imgDC, color);
	BitBlt(maskDC, 0, 0, bitmapData.bmWidth, bitmapData.bmHeight, imgDC, 0, 0, SRCCOPY);

	BitBlt(memDC, 0, 0, bitmapData.bmWidth, bitmapData.bmHeight, imgDC, 0, 0, SRCINVERT);
	BitBlt(memDC, 0, 0, bitmapData.bmWidth, bitmapData.bmHeight, maskDC, 0, 0, SRCAND);
	BitBlt(memDC, 0, 0, bitmapData.bmWidth, bitmapData.bmHeight, imgDC, 0, 0, SRCINVERT);
	DeleteDC(imgDC);
	DeleteDC(maskDC);

	return memDC;
}

void Animate(HWND hWnd, RECT *rect, std::atomic<bool> *shouldAnimate) {
	while (true) {
		Sleep(40);
		if (shouldAnimate->load(std::memory_order_relaxed)) ChangeBitmapPos(hWnd, rect);
	}
}

void ChangeBitmapPos(HWND hWnd, RECT *rect) {
	rect->right += MOVE_RIGHT;
	rect->left += MOVE_RIGHT;
	rect->top += MOVE_BOTTOM;
	rect->bottom += MOVE_BOTTOM;
	Bounce(hWnd, *rect);
	InvalidateRect(hWnd, nullptr, true);
}

void Bounce(HWND hWnd, RECT rect) {
	RECT windowRect;
	GetClientRect(hWnd, &windowRect);

	if (rect.top <= windowRect.top) {
		MOVE_BOTTOM = 5;
	}
	else if (rect.bottom >= windowRect.bottom) {
		MOVE_BOTTOM = -5;
	}

	if (rect.right >= windowRect.right) {
		MOVE_RIGHT = -5;
	}
	else if (rect.left <= windowRect.left) {
		MOVE_RIGHT = 5;
	}
}

