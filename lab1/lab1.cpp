#include "framework.h"
#include "lab1.h"

#define MAX_LOADSTRING 100


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
extern int MOVE_RIGHT;
extern int MOVE_BOTTOM;
HBRUSH brush = CreateSolidBrush(0x0000ff00);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
         if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
         {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  =  brush;
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC imageDC;
	static bool shouldMove = false;
	static std::atomic<bool> shouldAnimate = false;
	static BITMAP bitmapData;
	static RECT rect;
	static std::thread *timerThread;

    switch (message)
    {
	case WM_CREATE: {
		HBITMAP bitmap = (HBITMAP)LoadImage(hInst, L"D:\\earth.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		GetObject(bitmap, sizeof BITMAP, &bitmapData);
		rect = { 0, 0, bitmapData.bmWidth, bitmapData.bmHeight };

		HDC hdc = GetDC(hWnd);
		imageDC = CreateTransparentBitmap(hdc, bitmap, bitmapData, brush);
		ReleaseDC(hWnd, hdc);

		timerThread = new std::thread(Animate, hWnd, &rect, &shouldAnimate);
		timerThread->detach();

		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_KEYDOWN: 
		{
		switch (wParam)
		{
		case VK_UP: {
			rect.top -= 10;
			rect.bottom -= 10;
			break;
		}
		case VK_RIGHT: 
		{
			rect.left += 10;
			rect.right += 10;
			break;
		}
		case VK_DOWN:
		{
			rect.top += 10;
			rect.bottom += 10;
			break;
		}
		case VK_LEFT:
		{
			rect.left -= 10;
			rect.right -= 10;
			break;
		}
		case VK_SPACE: {
			shouldAnimate.store(!shouldAnimate.load(std::memory_order_relaxed), std::memory_order_relaxed);
			break;
		}
		}
		InvalidateRect(hWnd, nullptr, true);
		break;
		}	
	case WM_LBUTTONDOWN: {
		shouldMove = true;
		break;
	}
	case WM_MOUSEMOVE: {
		if (shouldMove) {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			rect = { x - bitmapData.bmWidth / 2, y - bitmapData.bmHeight / 2, x + bitmapData.bmWidth / 2, y + bitmapData.bmHeight / 2};
			InvalidateRect(hWnd, nullptr, true);
		}
		break;
	}
	case WM_LBUTTONUP: {
		shouldMove = false;
		break;
	}
	case WM_MOUSEWHEEL: {
		int vKeys = GET_KEYSTATE_WPARAM(wParam);
		int minDelta = (int)(GET_WHEEL_DELTA_WPARAM(wParam) / 5);

		if (vKeys & MK_SHIFT) {
			rect.right -= minDelta;
			rect.left -= minDelta;
		}
		else {
			rect.top -= minDelta;
			rect.bottom -= minDelta;
		}
		InvalidateRect(hWnd, nullptr, true);
		break;
	}
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, rect.left, rect.top, bitmapData.bmWidth, bitmapData.bmHeight, imageDC, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



int CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (int)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (int)TRUE;
        }
        break;
    }
    return (int)FALSE;
}
	