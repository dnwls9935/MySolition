#include "stdafx.h"
#include "..\public\HitNum.h"

#include "GameInstance.h"

HitNum::HitNum(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

HitNum::HitNum(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT HitNum::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT HitNum::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, g_iWinCX * 0.5f, g_iWinCY * 0.5f);

	X = TextureSize.x * 0.015f;
	Y = TextureSize.y * 0.015f;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	if (nullptr != pArg)
		memcpy(&Num, pArg, sizeof(_int));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int HitNum::Tick(_double TimeDelta)
{
	Num100 = Num / 100;
	Num10 = Num / 10;
	Num1 = Num % 10;

	return _int();
}

_int HitNum::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT HitNum::Render()
{
	if (FAILED(Render100()))
		return E_FAIL;

	if (FAILED(Render10()))
		return E_FAIL;

	if (FAILED(Render1()))
		return E_FAIL;

	return S_OK;
}

HRESULT HitNum::Render100()
{

	return S_OK;
}

HRESULT HitNum::Render10()
{
	return S_OK;
}

HRESULT HitNum::Render1()
{
	return S_OK;
}

HRESULT HitNum::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer0"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transfom */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_Font"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HitNum * HitNum::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	HitNum*	pInstance = new HitNum(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Num");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * HitNum::Clone(void * pArg)
{
	HitNum*	pInstance = new HitNum(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating Num");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HitNum::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
