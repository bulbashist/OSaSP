#pragma once
#include "Drawing.h"

typedef struct _TimerArgs {
	HWND hWnd;
	RECT *rect;
	HANDLE timer;
} TimerArgs;

void createAPCTimer(HWND hWnd, RECT* rect);