#pragma once

#define BEGIN(NAMESPACE) namespace NAMESPACE {
#define END }

#ifdef ENGINE_EXPORTS
#define DLL_MODE _declspec(dllexport)
#else
#define DLL_MODE _declspec(dllimport)
#endif


#define MSGBOX(MESSAGE) MessageBox(0, TEXT(MESSAGE), TEXT("Error"), MB_OK);