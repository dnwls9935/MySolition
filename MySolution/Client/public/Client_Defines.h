#pragma once

#include <process.h>

static const unsigned int g_iWinCX = 1280;
static const unsigned int g_iWinCY = 720;



/* �������������ϳ�. */
/* ��ü���� ��Ƽ� �����ϳ�.(Object_Manager) */
/* Component_Manager */
/* map<const _tchar*, Cdata*>		Data[LEVEL_END] */ 	
enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };

extern HWND g_hWnd;

namespace Client {}

using namespace Client;
