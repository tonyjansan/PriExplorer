// LoginInterface.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PriExplorer.h"
#include "PriLoader.h"
#include "PriChanger.h"

#define MAX_LOADSTRING MAX_PATH
#define MAIN_WINDOW_WIDTH 800

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle[MAX_LOADSTRING], *pAppendTitle;   // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

int scrWidth, scrHeight;
int captionHeight, menuHeight;

ULONG_PTR pGdiplusStartupToken;
Gdiplus::GdiplusStartupInput gdiInput;
Gdiplus::Image* pImage = NULL;
HGLOBAL hGlobal = NULL;

PriLoader* pLoader = NULL;
PriChanger* pChanger = NULL;

// Forward declarations of functions included in this code module:
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

    // Initialize global parameters
	hGlobal = GlobalAlloc(GMEM_MOVEABLE, MAP_BUFFER_SIZE);
	if (!hGlobal) return FALSE;
	Gdiplus::GdiplusStartup(&pGdiplusStartupToken, &gdiInput, NULL);

    // Initialize global strings
	pAppendTitle = szTitle + LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_PRIEXPLORER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PRIEXPLORER));
	MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	delete pImage; pImage = NULL;
	delete pChanger; pChanger = NULL;
	delete pLoader; pLoader = NULL;
	Gdiplus::GdiplusShutdown(pGdiplusStartupToken);
	GlobalFree(hGlobal);

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PRIEXPLORER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_PRIEXPLORER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   scrWidth = GetSystemMetrics(SM_CXSCREEN);
   scrHeight = GetSystemMetrics(SM_CYSCREEN);
   captionHeight = GetSystemMetrics(SM_CYCAPTION);
   menuHeight = GetSystemMetrics(SM_CYMENU);

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
      MAIN_WINDOW_WIDTH, MAIN_WINDOW_WIDTH * scrHeight / scrWidth + captionHeight + menuHeight,
      nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   HMENU hMenu = GetMenu(hWnd);
   EnableMenuItem(hMenu, IDM_SAVE_AS, MF_DISABLED);
   EnableMenuItem(hMenu, IDM_CHANGE_JPG, MF_DISABLED);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL OpenFileDialog(HWND hWnd, TCHAR* path, TCHAR* filter) {
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = path;
	ofn.lpstrFile[0] = _T('\0');
	ofn.lpstrFilter = filter;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	return GetOpenFileName(&ofn);
}

BOOL SaveFileDialog(HWND hWnd, TCHAR* path, TCHAR* filter) {
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = path;
	ofn.lpstrFile[0] = _T('\0');
	ofn.lpstrFilter = filter;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER;
	return GetSaveFileName(&ofn);
}

BOOL isStringIgnoreCaseEndWith(const TCHAR* src, const TCHAR* foot) {
	if (!src || !foot)
		return FALSE;

	int srcLen = _tcslen(src), footLen = _tcslen(foot);
	if (srcLen < footLen)
		return FALSE;

	return _tcsicmp(src + srcLen - footLen, foot) == 0;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_FILE_OPEN: {
				TCHAR path[MAX_PATH];
				if (OpenFileDialog(hWnd, path, _T("Pri File (*.pri)\0*.pri\0*.*\0*.*\0\0"))) {
					TCHAR* name = _tcsrchr(path, _T('\\')) + 1;
					_tcscpy_s(pAppendTitle, MAX_PATH + szTitle - pAppendTitle, _T(" - "));
					_tcscat_s(pAppendTitle, MAX_PATH + szTitle - pAppendTitle, name);
					SetWindowText(hWnd, szTitle);

					if (pLoader) delete pLoader;
					pLoader = new PriFileLoader(path);

					if (pChanger) delete pChanger;
					pChanger = new PriChanger(pLoader->getPriEntry());

					if (pImage) {
						delete pImage;
						pImage = NULL;
					}

					BYTE* pBuffer = reinterpret_cast<BYTE*>(GlobalLock(hGlobal));
					pLoader->getDataByIndex(pBuffer, MAP_BUFFER_SIZE, 3);
					GlobalUnlock(hGlobal);
					IStream* pStream = NULL;
					if (CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) == S_OK) {
						pImage = Gdiplus::Image::FromStream(pStream);

						HMENU hMenu = GetMenu(hWnd);
						EnableMenuItem(hMenu, IDM_SAVE_AS, MF_ENABLED);
						EnableMenuItem(hMenu, IDM_CHANGE_JPG, MF_ENABLED);
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
				break;
			}
			case IDM_SAVE_AS: {
				TCHAR path[MAX_PATH];
				if (SaveFileDialog(hWnd, path, _T("Pri File (*.pri)\0*.pri\0\0"))) {
					int length = _tcslen(path);
					if (length <= 4 || !isStringIgnoreCaseEndWith(path, _T(".pri")))
						_tcscat_s(path, _T(".pri"));

					if (pChanger)
						pChanger->saveAs(path);
				}
				break;
			}
			case IDM_CHANGE_JPG: {
				TCHAR path[MAX_PATH];
				if (OpenFileDialog(hWnd, path, _T("Jpg File (*.jpg)\0*.jpg\0\0"))) {
					if (pChanger)
						pChanger->changeBackGroundJpg(path);

					if (pImage) {
						delete pImage;
						pImage = NULL;
					}

					BYTE* pBuffer = reinterpret_cast<BYTE*>(GlobalLock(hGlobal));
					pLoader->getDataByIndex(pBuffer, MAP_BUFFER_SIZE, 3);
					GlobalUnlock(hGlobal);
					IStream* pStream = NULL;
					if (CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) == S_OK) {
						pImage = Gdiplus::Image::FromStream(pStream);

						HMENU hMenu = GetMenu(hWnd);
						EnableMenuItem(hMenu, IDM_SAVE_AS, MF_ENABLED);
						EnableMenuItem(hMenu, IDM_CHANGE_JPG, MF_ENABLED);
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}
				break;
			}
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
	case WM_ERASEBKGND:
		return 1;
    case WM_PAINT:
        {
			RECT rc;
			GetClientRect(hWnd, &rc);
			Gdiplus::Rect rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			HDC hMemDC = CreateCompatibleDC(hdc);
			HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, rect.Width, rect.Height);
			SelectObject(hMemDC, hMemBitmap);
			if (pImage) {
				Gdiplus::Graphics graphics(hMemDC);
				graphics.DrawImage(pImage, rect);
				BitBlt(hdc, rect.GetTop(), rect.GetTop(), rect.Width, rect.Height, hMemDC, 0, 0, SRCCOPY);
			}
			DeleteObject(hMemBitmap);
			DeleteDC(hMemDC);
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}