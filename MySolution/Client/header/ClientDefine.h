#pragma once

#include <process.h>

static const unsigned int g_WIN_WIDTH = 1280;
static const unsigned int g_WIN_HEIGHT = 720;

extern HWND g_hWnd;

enum class LVL { LVL_STATIC, LVL_LOGO, LVL_LOADING, LVL_GAMEPLAY, LVL_END };

namespace Client {}

using namespace Client;
