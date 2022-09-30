#include "Timers.h"
#include <Windows.h>

void CALLBACK TimerAPCProc(LPVOID lpArg, DWORD dwTimerLowValue, DWORD dwTimerHighValue) {
	TimerArgs *args = (TimerArgs*)lpArg;
	PostMessage(args->hWnd, WM_USER, 0, (LPARAM)args->timer);
	WaitForSingleObjectEx(args->timer, INFINITE, true);
}

void createAPCTimer(HWND hWnd, RECT *rect) {
	LARGE_INTEGER delay = {};
	TimerArgs args;
	args.hWnd = hWnd;
	args.rect = rect;

	HANDLE hTimer = CreateWaitableTimer(nullptr, false, L"TestTimer");
	args.timer = hTimer;
	SetWaitableTimer(hTimer, &delay, 15, TimerAPCProc, &args, false);          // Do not restore a suspended system
	SleepEx(15, true);
}