#include "..\header\DX9GraphicDev.h"

USING(Engine)
IMPLEMET_SINGLETON(DX9GraphicDev)

DX9GraphicDev::DX9GraphicDev()
	: directX9Device(NULL)
{
}


DX9GraphicDev::~DX9GraphicDev()
{
}

HRESULT DX9GraphicDev::ReadyDirectX9Device(HWND _hWnd, WINMODE _mode, const _ulong& _sizeX, const _ulong& _sizeY, LPDIRECT3DDEVICE9* _directX9Device)
{
	LPDIRECT3D9 pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	if (nullptr == pSDK || NULL == pSDK)
		return E_FAIL;

	D3DCAPS9	pDeviceCaps;
	ZeroMemory(&pDeviceCaps, sizeof(D3DCAPS9));

	if(FAILED(pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &pDeviceCaps)))
		return E_FAIL;

	_ulong	pFlag = 0;
	if (pDeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		pFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	else
		pFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS			pPresentParameter;
	ZeroMemory(&pPresentParameter, sizeof(D3DPRESENT_PARAMETERS));

	pPresentParameter.BackBufferWidth = _sizeX;
	pPresentParameter.BackBufferHeight = _sizeY;
	pPresentParameter.BackBufferFormat = D3DFMT_A8R8G8B8;
	pPresentParameter.BackBufferCount = 1;
	
	pPresentParameter.MultiSampleType = D3DMULTISAMPLE_NONE;
	pPresentParameter.MultiSampleQuality = 0;
	
	pPresentParameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pPresentParameter.hDeviceWindow = _hWnd;
	pPresentParameter.Windowed = (_int)_mode;
	pPresentParameter.EnableAutoDepthStencil = TRUE;
	pPresentParameter.AutoDepthStencilFormat = D3DFMT_D24S8;
	
	pPresentParameter.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	pPresentParameter.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;


	if (FAILED(pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _hWnd, pFlag, &pPresentParameter, &directX9Device)))
		return E_FAIL;

	_directX9Device = &directX9Device;

	return S_OK;
}

void DX9GraphicDev::DirectX9RenderBegin(D3DXCOLOR _color)
{
	directX9Device->Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _color, 1.f, 0);
	directX9Device->BeginScene();
}

void DX9GraphicDev::DirectX9RenderEnd()
{
	directX9Device->EndScene();
	directX9Device->Present(NULL, NULL, NULL, NULL);
}

void DX9GraphicDev::Free()
{
	SafeRelease(directX9Device);
}
