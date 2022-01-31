#pragma once

#include "Base.h"

BEGIN(Engine)

class RenderTargetManager final : public CBase
{
	DECLARE_SINGLETON(RenderTargetManager)
private:
	explicit RenderTargetManager();
	virtual ~RenderTargetManager() = default;

public:
	typedef struct tagRTDesc {
		_uint Width;
		_uint Height;
		DXGI_FORMAT Format;
		_float4		Color;
	}RTDESC;

#ifdef _DEBUG
	typedef struct tagDebugBufferDesc
	{
		_float X;
		_float Y;
		_float SizeX;
		_float SizeY;
	}DBDESC;
#endif // _DEBUG

public:
	ID3D11ShaderResourceView*			Get_SRV(const _tchar* _TargetTag);

public:
	HRESULT		AddRenderTarget(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext, const _tchar* _TargetTag, RTDESC _RTDesc);
	HRESULT		AddMRT(const _tchar* _MRTTag, const _tchar* _TargetTag);
	HRESULT		BeginMRT(ID3D11DeviceContext* _DeviceContext, const _tchar* _MRTTag);
	HRESULT		EndMRT(ID3D11DeviceContext* _DeviceContext);

private:
	class RenderTarget*		FindTarget(const _tchar* _TargetTag);
	list<class RenderTarget*>*		FindMRT(const _tchar* _MRTTag);

#ifdef _DEBUG
public:
	HRESULT		ReadyDebugBuffer(const _tchar* _TargetTag, DBDESC _DEDesc);
	HRESULT		RenderDebugBuffer(const _tchar* _MRTTag);
#endif // _DEBUG


private:
	ID3D11RenderTargetView*			m_OldRTV = nullptr;
	ID3D11DepthStencilView*				m_DSV = nullptr;

private:
	map<const _tchar*, class RenderTarget*>				m_Targets;
	typedef map<const _tchar*, class RenderTarget> TARGETS;

	map<const _tchar*, list<class RenderTarget*>>					m_MRTs;
	typedef map<const _tchar*, list<class RenderTarget*>>		MRTS;

private:
	virtual		void  Free() override;
};

END