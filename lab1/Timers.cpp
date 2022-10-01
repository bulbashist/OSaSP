#include "Timers.h"
#include <Windows.h>

void CALLBACK TimerAPCProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue) {
	PostMessage((HWND)lpArg, WM_USER, 0, 0);
}

HANDLE createAPCTimer(HWND hWnd) {
	LARGE_INTEGER delay;
	delay.QuadPart = -0;

	HANDLE hTimer = CreateWaitableTimer(nullptr, false, L"TestTimer");
	SetWaitableTimer(hTimer, &delay, 15, TimerAPCProc, hWnd, false);          // Do not restore a suspended system

	return hTimer;
}