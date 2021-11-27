#pragma once

#include "DX9GraphicDev.h"

BEGIN(Engine)
// DirectX9 GraphicDev
inline HRESULT		ReadyDirectX9Device(HWND _hWnd, WINMODE _mode, const _ulong& _sizeX, const _ulong& _sizeY, LPDIRECT3DDEVICE9* _directX9Device);
inline void			DirectX9RenderBegin(D3DXCOLOR _color);
inline void			DirectX9RenderEnd();

inline void EngineRelease();

#include "EngineExport.inl"

END