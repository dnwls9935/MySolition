#pragma once

#include <process.h>

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;



/* 원형들을보관하낟. */
/* 객체들을 모아서 관리하낟.(Object_Manager) */
/* Component_Manager */
/* map<const _tchar*, Cdata*>		Data[LEVEL_END] */ 	
enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

extern HWND g_hWnd;

namespace Client {}

using namespace Client;
