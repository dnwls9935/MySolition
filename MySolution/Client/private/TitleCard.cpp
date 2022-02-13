#include "stdafx.h"
#include "..\public\TitleCard.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "BossPrimeBeast.h"

#include <iostream>

TitleCard::TitleCard(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

TitleCard::TitleCard(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT TitleCard::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT TitleCard::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float2 TextureSize = m_pTitleCardCom->GetTextureInfo(0);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, g_iWinCX * 0.5f, g_iWinCY * 0.5f);

	m_X = TextureSize.x * 1.f;
	m_Y = TextureSize.y * 0.7f;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(m_X * 2.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, m_Y * 2.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,WindowPosition);

	list<CGameObject*> Enemys = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));

	for (auto& Enemy : Enemys)
	{
		if (CGameObject::OBJTYPE_ID::BOSS == Enemy->GetID())
		{
			m_Boss = static_cast<BossPrimeBeast*>(Enemy);
			Safe_AddRef(m_Boss);
		}
	}

	m_Camera = static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front());
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int TitleCard::Tick(_double TimeDelta)
{
	if (FALSE == m_Boss->GetFocus())
		return _int();

	m_pTransformCom->Set_Scale(XMVectorSet(m_X * m_Size, m_Y * m_Size, 0.f, 1.f));
	m_Size -= TimeDelta * 1.5f;

	if(1 >= m_Size)
		m_Size = 1;


	return _int();
}

_int TitleCard::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT TitleCard::Render()
{
	if (FALSE == m_Boss->GetFocus())
		return S_OK;

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTitleCardCom, 0);
	m_pVIBufferCom->Render(8);

	return S_OK;
}


HRESULT TitleCard::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_TCNames_Knuckledragger"), TEXT("Com_TitleCard"), (CComponent**)&m_pTitleCardCom)))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_TCBackgrounds_KnuckleDragger"), TEXT("Com_Background"), (CComponent**)&m_BackgroundCom)))
		return E_FAIL;

	return S_OK;
}

TitleCard * TitleCard::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	TitleCard*	pInstance = new TitleCard(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating TitleCard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * TitleCard::Clone(void * pArg)
{
	TitleCard*	pInstance = new TitleCard(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating TitleCard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void TitleCard::Free()
{
	__super::Free();

	Safe_Release(m_Boss);
	Safe_Release(m_pTitleCardCom);
	Safe_Release(m_BackgroundCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
