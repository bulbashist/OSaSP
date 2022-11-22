// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include <cmath>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

typedef struct __Vector {
	int x;
	int y;
} Vector;

//16 mod 10 + 3 = 9
extern "C" __declspec(dllexport) void Draw(HDC hDC, int x, int y, int radius) {
	constexpr auto size = 6;
	double step = 2 * 3.1415 / size;
	int arr[size] = { 0 };
	Vector vectors[size] = { 0 };
	int angleIteration = 0;
	for (int i = 0; i < size; i++) {
		vectors[i].x = (int)(x + radius * cos(step*i));
		vectors[i].y = (int)(y + radius * sin(step*i));
	}

	Ellipse(hDC, x - radius, y - radius, x + radius, y + radius);

	MoveToEx(hDC, x + radius, y, nullptr);
	for (int i = 1; i < size; i++) {
		LineTo(hDC, vectors[i].x, vectors[i].y);
	}
	LineTo(hDC, x + radius, y);
}

