#include "stdafx.h"
#include "..\public\CrossSight.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"

CrossSight::CrossSight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CrossSight::CrossSight(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CrossSight::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT CrossSight::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, g_iWinCX * 0.5f, g_iWinCY * 0.5f);

	X = TextureSize.x;
	Y = TextureSize.y;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,WindowPosition);

	ShowCursor(FALSE);

	m_EnemyObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
	m_Camera = static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front());
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int CrossSight::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_IsCollision = FALSE;

	for (auto& Enemy : m_EnemyObjectList)
	{
		if (FALSE == Enemy->GetIntroEnd())
			continue;

		if (TRUE == Enemy->Picked())
		{
			m_IsCollision = TRUE;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CrossSight::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CrossSight::Render()
{
	if (TRUE == m_Camera->GetFocus())
		return S_OK;

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));
	m_pVIBufferCom->SetUp_ValueOnShader("g_CrossCollision", &m_IsCollision, sizeof(_bool));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);
	m_pVIBufferCom->Render(4);

	return S_OK;
}


HRESULT CrossSight::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transfom */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hyperion_Sight_SMG"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CrossSight * CrossSight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CrossSight*	pInstance = new CrossSight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CrossSight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CrossSight::Clone(void * pArg)
{
	CrossSight*	pInstance = new CrossSight(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CrossSight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CrossSight::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
