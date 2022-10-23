// lab3.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "lab3.h"

namespace fs = std::experimental::filesystem;

//extern "C" __declspec(dllimport) void Draw(HDC hdc, int x, int y, int radius);

typedef void (__cdecl* DLLFUNC)(HDC, int, int, int);
DLLFUNC proc;
HMODULE dll;

// Глобальные переменные:
#define MAX_LOADSTRING 100
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_LAB3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB3));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB3));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static POINT point = { 0 };
	static std::vector<fs::v1::path> *paths = new std::vector<fs::v1::path>();
	static int dllIndex;

    switch (message)
    {
	case WM_CREATE:
		{
			dllIndex = 0;
			point.x = 200;
			point.y = 200;

			std::string path = "D:/student/3 курс 1 сем/ОСиСП/lab3/lab3/plugins";
			for (auto entry : fs::directory_iterator(path)) {
				const fs::v1::path filename = entry.path().filename();
				if (filename.extension()._Mystr == L".dll") {
					paths->emplace_back(filename._Mystr);
				}
			}
			
			dll = LoadLibraryExW((LPCWSTR)(paths->at(dllIndex).c_str()), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			proc = (DLLFUNC)GetProcAddress(dll, "Draw");
		}
		break;
	case WM_LBUTTONDOWN: 
		{
			point.x = GET_X_LPARAM(lParam);
			point.y = GET_Y_LPARAM(lParam);
			InvalidateRect(hWnd, nullptr, true);
		}
		break;
	case WM_RBUTTONDOWN: 
		{
			dllIndex = dllIndex < paths->capacity() - 1 ? dllIndex + 1 : 0;

			FreeLibrary(dll);
			dll = LoadLibraryExW((LPCWSTR)(paths->at(dllIndex).c_str()), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
			proc = (DLLFUNC)GetProcAddress(dll, "Draw");
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
	//		Draw(hdc, point.x, point.y, 50);
			proc(hdc, point.x, point.y, 50);
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
