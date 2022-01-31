#include "..\public\RenderTargetManager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(RenderTargetManager)

RenderTargetManager::RenderTargetManager()
{
}

ID3D11ShaderResourceView * RenderTargetManager::Get_SRV(const _tchar * _TargetTag)
{
	RenderTarget*		RenderTarget = FindTarget(_TargetTag);
	if (nullptr == RenderTarget)
		return nullptr;

	return RenderTarget->Get_SRV();
}

HRESULT RenderTargetManager::AddRenderTarget(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext, const _tchar * _TargetTag, RTDESC _RTDesc)
{
	if (nullptr != FindTarget(_TargetTag))
		return E_FAIL;

	RenderTarget::RTDESC RTDesc;
	ZeroMemory(&RTDesc, sizeof(RenderTarget::RTDESC));
	RTDesc.Width = _RTDesc.Width;
	RTDesc.Height = _RTDesc.Height;
	RTDesc.Format = _RTDesc.Format;
	RTDesc.Color = _RTDesc.Color;

	RenderTarget*		pRenderTarget = RenderTarget::Create(_Device, _DeviceContext, RTDesc);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_Targets.emplace(_TargetTag, pRenderTarget);
	return S_OK;
}

HRESULT RenderTargetManager::AddMRT(const _tchar * _MRTTag, const _tchar * _TargetTag)
{
	RenderTarget*		pRenderTarget = FindTarget(_TargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;


	list<RenderTarget*>* ListRenderTargets = FindMRT(_MRTTag);
	if (nullptr == ListRenderTargets)
	{
		list<RenderTarget*> ListRTS;
		ListRTS.push_back(pRenderTarget);

		m_MRTs.emplace(_MRTTag, ListRTS);
	}
	else
		ListRenderTargets->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT RenderTargetManager::BeginMRT(ID3D11DeviceContext * _DeviceContext, const _tchar* _MRTTag)
{
	list<RenderTarget*>* ListMRT = FindMRT(_MRTTag);
	if (nullptr == ListMRT)
		return E_FAIL;

	ID3D11RenderTargetView*		RTVs[8] = {nullptr};
	
	_DeviceContext->OMGetRenderTargets(1, &m_OldRTV, &m_DSV);

	_uint NumView = m_MRTs.size();
	_uint Index = 0;

	for (auto& RenderTarget : *ListMRT)
	{
		RenderTarget->Clear();
		RTVs[Index++] = RenderTarget->Get_RTV();
	}

	_DeviceContext->OMSetRenderTargets(NumView, RTVs, m_DSV);

	return S_OK;
}

HRESULT RenderTargetManager::EndMRT(ID3D11DeviceContext * _DeviceContext)
{
	_DeviceContext->OMSetRenderTargets(1, &m_OldRTV, m_DSV);

	return S_OK;
}

RenderTarget * RenderTargetManager::FindTarget(const _tchar * _TargetTag)
{
	auto iter = find_if(m_Targets.begin(), m_Targets.end(), CTag_Finder(_TargetTag));
	if (m_Targets.end() == iter)
		return nullptr;

	return iter->second;
}

list<class RenderTarget*>* RenderTargetManager::FindMRT(const _tchar * _MRTTag)
{
	auto iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTag_Finder(_MRTTag));
	if (m_MRTs.end() == iter)
		return nullptr;

	return &iter->second;
}

#ifdef _DEBUG
HRESULT RenderTargetManager::ReadyDebugBuffer(const _tchar * _TargetTag, DBDESC _DEDesc)
{
	RenderTarget*		pRenderTarget = FindTarget(_TargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->ReadyDebugBuffer(_DEDesc.X, _DEDesc.Y, _DEDesc.SizeX, _DEDesc.SizeY);
}

HRESULT RenderTargetManager::RenderDebugBuffer(const _tchar * _MRTTag)
{
	list<RenderTarget*>*		ListMRT = FindMRT(_MRTTag);
	if (nullptr == ListMRT)
		return E_FAIL;

	for (auto& RT : *ListMRT)
		RT->RenderDebugBuffer();

	return S_OK;
}
#endif // _DEBUG

void RenderTargetManager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& RT : Pair.second)
			Safe_Release(RT);
		
		Pair.second.clear();
	}
	m_MRTs.clear();


	for (auto& Target : m_Targets)
		Safe_Release(Target.second);

	m_Targets.clear();
}
