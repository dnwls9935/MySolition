#include "stdafx.h"
#include "..\public\UI.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

UI::UI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

UI::UI(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT UI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT UI::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(CGameObject::UIOBJDESC));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_UIDesc.m_LoadMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_UIDesc.m_LoadMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_UIDesc.m_LoadMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_UIDesc.m_LoadMatrix.r[3]);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_Camera = static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front());
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int UI::Tick(_double TimeDelta)
{

	return _int();
}

_int UI::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT UI::Render()
{
	if (TRUE == m_Camera->GetFocus())
		return S_OK;


	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_BarPercent", &m_BarPercent, sizeof(_float));
	
	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	if (CGameObject::UITYPE_ID::AMMO == m_UIDesc.m_UITypeID)
		m_pVIBufferCom->Render(3);
	else if (CGameObject::UITYPE_ID::PLAYER_HP == m_UIDesc.m_UITypeID ||
		CGameObject::UITYPE_ID::PLAYER_SHILED == m_UIDesc.m_UITypeID)
		m_pVIBufferCom->Render(2);
	else
		m_pVIBufferCom->Render(0);

	return S_OK;
}

void UI::SetLength(_float _Percent, _bool _LRCheck)
{
	m_BarPercent = _Percent;
	m_LRCheck = _LRCheck;
}

HRESULT UI::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, m_UIDesc.m_TextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

UI * UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	UI*	pInstance = new UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * UI::Clone(void * pArg)
{
	UI*	pInstance = new UI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void UI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
