#include "stdafx.h"
#include "..\public\HyperionChest.h"
#include "GameInstance.h"
#include "Player.h"

HyperionChest::HyperionChest(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

HyperionChest::HyperionChest(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT HyperionChest::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT HyperionChest::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameObject::TOOLOBJDESC		ToolObjDesc;
	ZeroMemory(&ToolObjDesc, sizeof(CGameObject::TOOLOBJDESC));
	memcpy(&ToolObjDesc, (CGameObject::TOOLOBJDESC*)pArg, sizeof(CGameObject::TOOLOBJDESC));
	
	_matrix TransformMatrix = XMLoadFloat4x4(&ToolObjDesc.m_pTransformMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, TransformMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, TransformMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, TransformMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::CLOSED_IDLE);

	return S_OK;
}

_int HyperionChest::Tick(_double TimeDelta)
{
	m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if ((_uint)ANIMATION_STATE::CLOSED_IDLE == m_pModelCom->GetCurrentAnimation())
		Picking();

	return _int();
}

_int HyperionChest::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderCom);
#endif // _DEBUG
	}

	if (TRUE == m_pModelCom->GetAnimationFinished())
	{
		if ((_uint)ANIMATION_STATE::OPEN == m_pModelCom->GetCurrentAnimation())
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::OPENED_IDLE);
	}
	return _int();
}

HRESULT HyperionChest::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshContainer(); ++i)
	{
		m_pModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 1);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void HyperionChest::Picking()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* Player = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front();
	RELEASE_INSTANCE(CGameInstance);

	if (FALSE == static_cast<CPlayer*>(Player)->GetInteractPick())
		return;

	Calculator::CALCDESC CalDesc;
	CalDesc._width = g_iWinCX;
	CalDesc._height = g_iWinCY;
	CalDesc._transformCom = m_pTransformCom;
	CalDesc._hWnd = g_hWnd;

	pGameInstance->CalcMousePos(&CalDesc);

	CalDesc._rayPos = XMVector3TransformCoord(CalDesc._rayPos, m_pTransformCom->Get_WorldMatrix());
	CalDesc._rayDir = XMVector3TransformNormal(CalDesc._rayDir, m_pTransformCom->Get_WorldMatrix());
	CalDesc._rayDir = XMVector3Normalize(CalDesc._rayDir);

	_float	Distance;

	if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir, Distance))
	{
		if (2.5 >= Distance)
		{
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::OPEN);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

}

HRESULT HyperionChest::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HyperionChest"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Model */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.5f, 1.0f, 1.5);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

HyperionChest * HyperionChest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	HyperionChest*	pInstance = new HyperionChest(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating HyperionChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * HyperionChest::Clone(void * pArg)
{
	HyperionChest*	pInstance = new HyperionChest(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating HyperionChest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HyperionChest::Free()
{
	__super::Free();

	Safe_Release(m_ColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
