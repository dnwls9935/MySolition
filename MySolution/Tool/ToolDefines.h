#pragma once

extern		HWND		g_hWnd;
extern		HINSTANCE g_hInst;

static long	g_WIN_WIDHT = 1280;
static long	g_WIN_HEIGHT = 720;


#ifdef _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include <iostream>