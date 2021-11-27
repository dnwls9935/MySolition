// DirectX9 GraphicDev
HRESULT		ReadyDirectX9Device(HWND _hWnd, WINMODE _mode, const _ulong& _sizeX, const _ulong& _sizeY, LPDIRECT3DDEVICE9* _directX9Device) {
	return DX9GraphicDev::GetInstance()->ReadyDirectX9Device(_hWnd, _mode, _sizeX, _sizeY, _directX9Device);
}
void		DirectX9RenderBegin(D3DXCOLOR _color) {
	DX9GraphicDev::GetInstance()->DirectX9RenderBegin(_color);
}
void		DirectX9RenderEnd() {
	DX9GraphicDev::GetInstance()->DirectX9RenderEnd();
}














/* Release */
void EngineRelease()
{
	DX9GraphicDev::DestroyInstance();
}