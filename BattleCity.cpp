// BATTLECITY.cpp : Defines the entry point for the application.
//
module;

#include <windows.h>
#include "gdiplus.h"

#include <tchar.h>

#include "resource.h"

import game;
import constants;

using namespace Gdiplus;
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                     // current instance
TCHAR szTitle[MAX_LOADSTRING];       // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

GameManager gameInstance;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // ���������� ��� ������������� GDI+
  ULONG_PTR gdipToken = NULL;
  GdiplusStartupInput gdipStartupInput;

  // ������������� GDI+
  GdiplusStartup(&gdipToken, &gdipStartupInput, NULL);

  MSG msg;
  HACCEL hAccelTable;

  // Initialize global strings
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_BATTLECITY, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BATTLECITY));

  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  // ��������������� GDI+
  GdiplusShutdown(gdipToken);

  return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this
//    function so that the application will get 'well formed' small icons
//    associated with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BATTLECITY));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCE(IDC_BATTLECITY);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  HWND hWnd;

  hInst = hInstance; // Store instance handle in our global variable

  long windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  int scoreboardHeight = 16;
  int w = (COLUMN_COUNT + 1) * CELLS_SIZE - (CELLS_SIZE / 2); // we -0.5 from width to crop
  int h = (ROW_COUNT + 1) * CELLS_SIZE + 32 + scoreboardHeight + scoreboardHeight; // 32 is the header probably idn and scoreboard on top and bottom

  hWnd = CreateWindow(szWindowClass, szTitle, windowStyle, CW_USEDEFAULT, 0, w,
                      h, NULL, NULL, hInstance, NULL);

  if (!hWnd) {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message) {
  case WM_CREATE:
    gameInstance.init();
    // Go.loadTextures();
    SetTimer(hWnd, FRAME_TIMER, FRAME_PERIOD, (TIMERPROC)NULL);
    SetTimer(hWnd, ARMOR_TIMER, ARMOR_PERIOD, (TIMERPROC)NULL);
    SetTimer(hWnd, HEALTH_TIMER, HEALTH_PERIOD, (TIMERPROC)NULL);
    break;
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // Parse the menu selections:
    switch (wmId) {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_NEWGAME:
      gameInstance.reset();
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      system("exit");
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
  case WM_ERASEBKGND:
    break;
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    gameInstance.draw(hdc);
    EndPaint(hWnd, &ps);
    break;
  case WM_TIMER:
    gameInstance.update();
    switch (wParam) {
      case HEALTH_TIMER:
        gameInstance.spawnHealthPickup();
        break;
      case ARMOR_TIMER:
        gameInstance.spawnArmorPickup();
        break;
    }
    InvalidateRect(hWnd, NULL, false);
    // Go.processTimers(hWnd, wParam);
    break;
  case WM_DESTROY:
    KillTimer(hWnd, ARMOR_TIMER);
    KillTimer(hWnd, HEALTH_TIMER);
    KillTimer(hWnd, FRAME_TIMER);
    // Go.freeTextures();
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (message) {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}

export module battlecity;
