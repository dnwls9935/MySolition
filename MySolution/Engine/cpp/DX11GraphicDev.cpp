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

	// 장치 생성
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, pFlag, nullptr, 0, D3D11_SDK_VERSION, &dx11Device, &FreatureLV, &dx11DeviceContext)))
		return E_FAIL;

	// 스왑 체인
	if (FAILED(ReadySwapChain(_hWnd, _mode, _width, _height)))
		return E_FAIL;

	// 백버퍼 생성
	if (FAILED(ReadyBackBufferView()))
		return E_FAIL;

	// 깊이 스텐실 버퍼 생성
	if (FAILED(ReadyDepthStencilView(_width, _height)))
		return E_FAIL;

	/* 
		장치에 묶어둘(바인드) RenderTarget과 Depth Stencil View를 설정한다.
		OM 단계에 바인드할 RTV, DepthView, StencilView 설정

		(OM(ouput-merger) 단계 : 파이프라인 상태, 픽셀 셰이더에 의해 생성된 픽셀 데이터, 렌더링 대상의 내용, 깊이/스텐실 버퍼의 내용을 조합하여 최종 렌더링된 픽셀 색상을 생성합니다)
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

	/* 뷰포트 설정값 지정 */
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

	/* 순서
		1. CreateDevice로 생성된 dx11Device를 IDXGIDevice로 업캐스트하여 인터페이스를 받아옴
		2. IDXGIDevice의 부모핸들인 IDXGIAdapter를 받아옴
		3. IDXGIAdapter의 부모핸들인 IDXGIFactory를 받아옴
		4. IDXGIFactory로 SwapChain 셋팅 후 생성
	*/

	IDXGIDevice*			pIDevice = nullptr;
	/* 
		QueryInterface :: 모든 COM 객체는 IUnknown을 상속받고, Interface 들은 이름의 중복을 피하기 위해 고유한 ID를 가지고 있다.
							그래서 QueryInterface를 통해 InterfaceID를 넘겨주면 해당 Interface를 반환한다.
		__uuidof(인자) :: 인자로 들어온 타입의 고유 GUID(그냥 ID라고 생각하면 될듯)로 치환해줌
		(void**)	   :: 보이드 더블 포인터로 인터페이스의 주소값을 받아옴
	*/
	if (FAILED(dx11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pIDevice)))
		return E_FAIL;

	IDXGIAdapter*			pAdapter = nullptr;
	/*
		GetParent      :: 인자값의 부모 핸들을 반환해줌
	*/
	if (FAILED(pIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter)))
		return E_FAIL;

	IDXGIFactory*			pFactory = nullptr;
	if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory)))
		return E_FAIL;

	DXGI_SWAP_CHAIN_DESC		pSwapChainDesc;
	ZeroMemory(&pSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	pSwapChainDesc.BufferDesc.Width = _width; // BackBuffer 가로
	pSwapChainDesc.BufferDesc.Height = _height; // BackBuffer 세로
	pSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // SwapChain 주기 분모
	pSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // SwapChain 주기 분자
	pSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 포맷, 각 RGBA를 8비트로 표현하며 unsigned normal(실수형)
	pSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	pSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	pSwapChainDesc.SampleDesc.Quality = 0; // 어짜피 쉐이더를 하면서 효과를 주기에 굳이 중복처리 하지 않는다고 하셨음 ㅇㅇ
	pSwapChainDesc.SampleDesc.Count = 1; // 이하 동문
	pSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // BackBuffer의 용도를 서술하는 곳, 후면버퍼에 렌더링을 할 것이므로 "표면 또는 리소스를 출력 렌더링 대상으로 사용 합니다."라는 의미
	pSwapChainDesc.BufferCount = 1;
	pSwapChainDesc.OutputWindow = _hWnd; // 출력 윈도우 핸들
	pSwapChainDesc.Windowed = (_int)_mode; // 창 or 전체 모드
	pSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 출력 후 BackBuffer의 내용을 지워라!

	if (FAILED(pFactory->CreateSwapChain(dx11Device, &pSwapChainDesc, &dx11SwapChain)))
		return E_FAIL;

	// 사용 후 다른 곳에서 쓸 일 없으니 다 지워라
	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pIDevice);

	return S_OK;
}

HRESULT Engine::DX11GraphicDev::ReadyBackBufferView()
{
	if (nullptr == dx11Device)
		return E_FAIL;

	/* 참고
		실제 BackBuffer는 SwapChain 인터페이스를 받아오면서 SwapChainDesc에서 생성되었지만 
		후에 dx11DeviceContext->OMSetRenderTarget(~~~) 에서 BackBuffer를 RenderTarget 타입으로 넘겨줘야 한다.

		따라서. 여기서 형변환 과정을 수행한다.
	*/

	ID3D11Texture2D*		pDx11Texture2D;
	/* IDXGISwapChain::GetBuffer() :: SwapChain에 설정된 Buffer 중 하나를 가져온다. (소스 상에서는 0번째(BackBuffer)를 가져옴) */
	if (FAILED(dx11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pDx11Texture2D)))
		return E_FAIL;

	/* CreateRenderTargetView :: Render Target View를 생성한다. */
	if (FAILED(dx11Device->CreateRenderTargetView(pDx11Texture2D, nullptr, &dx11RenderTargetView)))
		return E_FAIL;

	Safe_Release(pDx11Texture2D);

	return S_OK;
}

HRESULT Engine::DX11GraphicDev::ReadyDepthStencilView(_uint _width, _uint _height)
{
	if (nullptr == dx11Device)
		return E_FAIL;

	/* DepthStencilView를 뽑을라면 ID3D11Texture2D가 있어야 한다. */
	ID3D11Texture2D*			pDx11Texture2D = nullptr;

	D3D11_TEXTURE2D_DESC		pTextureDesc;
	ZeroMemory(&pTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	pTextureDesc.Width = _width; // DepthStencilBuffer는 반드시 BackBuffer와 사이즈가 동일해야 한다. 아니면 구겨짐(?)
	pTextureDesc.Height = _height;
	pTextureDesc.MipLevels = 1; // MipLevel(Mipmap) 이며 렌더링 속도를 늘리기 위해 사이즈가 여러개인 동일한 텍스쳐의 집합을 만든다. 소스상엔 원본 한장만 유지로 1
	pTextureDesc.ArraySize = 1; // MipLevel이 1이라 ArrSize == 1;
	pTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 32비트 구조에서 24비트는 Depth, 8비트는 Stencil 버퍼로 사용한다.

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
