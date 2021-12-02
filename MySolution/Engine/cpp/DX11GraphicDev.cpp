#include "..\header\DX11GraphicDev.h"

IMPLEMENT_SINGLETON(DX11GraphicDev)


DX11GraphicDev::DX11GraphicDev()
{
}


HRESULT DX11GraphicDev::ReadyDX11Device(HWND _hWnd, DX11GraphicDev::WINMODE _mode, _uint _width, _uint _height, ID3D11Device** _dx11DeviceOut, ID3D11DeviceContext** _dx11DeviceContextOut)
{
	_uint pFlag = 0;

#ifdef _DEBUG
	pFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FreatureLV;

	// ��ġ ����
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, pFlag, nullptr, 0, D3D11_SDK_VERSION, &dx11Device, &FreatureLV, &dx11DeviceContext)))
		return E_FAIL;

	// ���� ü��
	if (FAILED(ReadySwapChain(_hWnd, _mode, _width, _height)))
		return E_FAIL;

	// ����� ����
	if (FAILED(ReadyBackBufferView()))
		return E_FAIL;

	// ���� ���ٽ� ���� ����
	if (FAILED(ReadyDepthStencilView(_width, _height)))
		return E_FAIL;

	/* 
		��ġ�� �����(���ε�) RenderTarget�� Depth Stencil View�� �����Ѵ�.
		OM �ܰ迡 ���ε��� RTV, DepthView, StencilView ����

		(OM(ouput-merger) �ܰ� : ���������� ����, �ȼ� ���̴��� ���� ������ �ȼ� ������, ������ ����� ����, ����/���ٽ� ������ ������ �����Ͽ� ���� �������� �ȼ� ������ �����մϴ�)
	*/
	dx11DeviceContext->OMSetRenderTargets(1, &dx11RenderTargetView, dx11DepthStencilView);

	D3D11_VIEWPORT pDx11ViewPort;
	ZeroMemory(&pDx11ViewPort, sizeof(D3D11_VIEWPORT));

	pDx11ViewPort.TopLeftX = 0;
	pDx11ViewPort.TopLeftY = 0;
	pDx11ViewPort.Width = (_float)_width;
	pDx11ViewPort.Height = (_float)_height;
	pDx11ViewPort.MinDepth = 0.f;
	pDx11ViewPort.MaxDepth = 1.f;

	/* ����Ʈ ������ ���� */
	dx11DeviceContext->RSSetViewports(1, &pDx11ViewPort);

	*_dx11DeviceOut = dx11Device;
	*_dx11DeviceContextOut = dx11DeviceContext;

	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DeviceContext);


	return S_OK;
}

HRESULT DX11GraphicDev::ClearBackBufferView(XMFLOAT4 _color)
{
	if (nullptr == dx11DeviceContext)
		return E_FAIL;

	dx11DeviceContext->ClearRenderTargetView(dx11RenderTargetView, (_float*)&_color);

	return S_OK;
}

HRESULT DX11GraphicDev::ClearDepthStencilView()
{
	if (nullptr == dx11DeviceContext)
		return E_FAIL;

	dx11DeviceContext->ClearDepthStencilView(dx11DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0.f);

	return S_OK;
}

HRESULT DX11GraphicDev::Present()
{
	if (nullptr == dx11SwapChain)
		return E_FAIL;

	return dx11SwapChain->Present(0, 0);
}

HRESULT Engine::DX11GraphicDev::ReadySwapChain(HWND _hWnd, WINMODE _mode, _uint _width, _uint _height)
{
	if (nullptr == dx11Device)
		return E_FAIL;

	/* ����
		1. CreateDevice�� ������ dx11Device�� IDXGIDevice�� ��ĳ��Ʈ�Ͽ� �������̽��� �޾ƿ�
		2. IDXGIDevice�� �θ��ڵ��� IDXGIAdapter�� �޾ƿ�
		3. IDXGIAdapter�� �θ��ڵ��� IDXGIFactory�� �޾ƿ�
		4. IDXGIFactory�� SwapChain ���� �� ����
	*/

	IDXGIDevice*			pIDevice = nullptr;
	/* 
		QueryInterface :: ��� COM ��ü�� IUnknown�� ��ӹް�, Interface ���� �̸��� �ߺ��� ���ϱ� ���� ������ ID�� ������ �ִ�.
							�׷��� QueryInterface�� ���� InterfaceID�� �Ѱ��ָ� �ش� Interface�� ��ȯ�Ѵ�.
		__uuidof(����) :: ���ڷ� ���� Ÿ���� ���� GUID(�׳� ID��� �����ϸ� �ɵ�)�� ġȯ����
		(void**)	   :: ���̵� ���� �����ͷ� �������̽��� �ּҰ��� �޾ƿ�
	*/
	if (FAILED(dx11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pIDevice)))
		return E_FAIL;

	IDXGIAdapter*			pAdapter = nullptr;
	/*
		GetParent      :: ���ڰ��� �θ� �ڵ��� ��ȯ����
	*/
	if (FAILED(pIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter)))
		return E_FAIL;

	IDXGIFactory*			pFactory = nullptr;
	if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory)))
		return E_FAIL;

	DXGI_SWAP_CHAIN_DESC		pSwapChainDesc;
	ZeroMemory(&pSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	pSwapChainDesc.BufferDesc.Width = _width; // BackBuffer ����
	pSwapChainDesc.BufferDesc.Height = _height; // BackBuffer ����
	pSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // SwapChain �ֱ� �и�
	pSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // SwapChain �ֱ� ����
	pSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // ����, �� RGBA�� 8��Ʈ�� ǥ���ϸ� unsigned normal(�Ǽ���)
	pSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	pSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	pSwapChainDesc.SampleDesc.Quality = 0; // ��¥�� ���̴��� �ϸ鼭 ȿ���� �ֱ⿡ ���� �ߺ�ó�� ���� �ʴ´ٰ� �ϼ��� ����
	pSwapChainDesc.SampleDesc.Count = 1; // ���� ����
	pSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // BackBuffer�� �뵵�� �����ϴ� ��, �ĸ���ۿ� �������� �� ���̹Ƿ� "ǥ�� �Ǵ� ���ҽ��� ��� ������ ������� ��� �մϴ�."��� �ǹ�
	pSwapChainDesc.BufferCount = 1;
	pSwapChainDesc.OutputWindow = _hWnd; // ��� ������ �ڵ�
	pSwapChainDesc.Windowed = (_int)_mode; // â or ��ü ���
	pSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // ��� �� BackBuffer�� ������ ������!

	if (FAILED(pFactory->CreateSwapChain(dx11Device, &pSwapChainDesc, &dx11SwapChain)))
		return E_FAIL;

	// ��� �� �ٸ� ������ �� �� ������ �� ������
	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pIDevice);

	return S_OK;
}

HRESULT Engine::DX11GraphicDev::ReadyBackBufferView()
{
	if (nullptr == dx11Device)
		return E_FAIL;

	/* ����
		���� BackBuffer�� SwapChain �������̽��� �޾ƿ��鼭 SwapChainDesc���� �����Ǿ����� 
		�Ŀ� dx11DeviceContext->OMSetRenderTarget(~~~) ���� BackBuffer�� RenderTarget Ÿ������ �Ѱ���� �Ѵ�.

		����. ���⼭ ����ȯ ������ �����Ѵ�.
	*/

	ID3D11Texture2D*		pDx11Texture2D;
	/* IDXGISwapChain::GetBuffer() :: SwapChain�� ������ Buffer �� �ϳ��� �����´�. (�ҽ� �󿡼��� 0��°(BackBuffer)�� ������) */
	if (FAILED(dx11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pDx11Texture2D)))
		return E_FAIL;

	/* CreateRenderTargetView :: Render Target View�� �����Ѵ�. */
	if (FAILED(dx11Device->CreateRenderTargetView(pDx11Texture2D, nullptr, &dx11RenderTargetView)))
		return E_FAIL;

	Safe_Release(pDx11Texture2D);

	return S_OK;
}

HRESULT Engine::DX11GraphicDev::ReadyDepthStencilView(_uint _width, _uint _height)
{
	if (nullptr == dx11Device)
		return E_FAIL;

	/* DepthStencilView�� ������� ID3D11Texture2D�� �־�� �Ѵ�. */
	ID3D11Texture2D*			pDx11Texture2D = nullptr;

	D3D11_TEXTURE2D_DESC		pTextureDesc;
	ZeroMemory(&pTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	pTextureDesc.Width = _width; // DepthStencilBuffer�� �ݵ�� BackBuffer�� ����� �����ؾ� �Ѵ�. �ƴϸ� ������(?)
	pTextureDesc.Height = _height;
	pTextureDesc.MipLevels = 1; // MipLevel(Mipmap) �̸� ������ �ӵ��� �ø��� ���� ����� �������� ������ �ؽ����� ������ �����. �ҽ��� ���� ���常 ������ 1
	pTextureDesc.ArraySize = 1; // MipLevel�� 1�̶� ArrSize == 1;
	pTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 32��Ʈ �������� 24��Ʈ�� Depth, 8��Ʈ�� Stencil ���۷� ����Ѵ�.

	pTextureDesc.SampleDesc.Quality = 0;
	pTextureDesc.SampleDesc.Count = 1;

	pTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	pTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	pTextureDesc.CPUAccessFlags = 0;
	pTextureDesc.MiscFlags = 0;

	if (FAILED(dx11Device->CreateTexture2D(&pTextureDesc, nullptr, &pDx11Texture2D)))
		return E_FAIL;

	if (FAILED(dx11Device->CreateDepthStencilView(pDx11Texture2D, nullptr, &dx11DepthStencilView)))
		return E_FAIL;

	Safe_Release(pDx11Texture2D);

	return S_OK;
}

void Engine::DX11GraphicDev::Free()
{
	Safe_Release(dx11SwapChain);
	Safe_Release(dx11DepthStencilView);
	Safe_Release(dx11RenderTargetView);
	Safe_Release(dx11DeviceContext);
	Safe_Release(dx11Device);
}
