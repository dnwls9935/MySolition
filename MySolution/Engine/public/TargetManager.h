#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);

public:
	HRESULT Add_RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag); /* 지정된 렌더타겟들을 장치에 순서대로 바인딩한다. */
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext); /* 바인딩되기 이전으로 돌린다. */


#ifdef _DEBUG
public:
	HRESULT Ready_Debug_Buffer(const _tchar* pTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug_Buffer(const _tchar* pMRTTag);
#endif // _DEBUG

private:
	map<const _tchar*, class CRenderTarget*>			m_Targets;
	typedef map<const _tchar*, class CRenderTarget*>	TARGETS;

private:
	/* MRT : MultiRenderTarget*/
	map<const _tchar*, list<class CRenderTarget*>>			m_MRTs;
	typedef map<const _tchar*, list<class CRenderTarget*>>	MRTS;

private:
	ID3D11RenderTargetView*				m_pOldView = nullptr;
	ID3D11DepthStencilView*				m_pDepthStencilView = nullptr;


private:
	class CRenderTarget* Find_Target(const _tchar* pTargetTag);
	list<class CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free() override;
};

END