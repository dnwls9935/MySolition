#include "stdafx.h"
#include "..\public\GlacialFlowWall.h"

#include "GameInstance.h"

GlcialFlowWall::GlcialFlowWall(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

GlcialFlowWall::GlcialFlowWall(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT GlcialFlowWall::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT GlcialFlowWall::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	CGameObject::TOOLOBJDESC ToolObjDesc;
	ZeroMemory(&ToolObjDesc, sizeof(ToolObjDesc));
	memcpy(&ToolObjDesc, (CGameObject::TOOLOBJDESC*)pArg, sizeof(CGameObject::TOOLOBJDESC));

	m_Type = ToolObjDesc.m_Type;
	m_ObjectTag = ToolObjDesc.m_BufferTag;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix Transform = XMLoadFloat4x4(&ToolObjDesc.m_pTransformMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Transform.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, Transform.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, Transform.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Transform.r[3]);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_Terrain = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Terrain")).front();
	RELEASE_INSTANCE(CGameInstance);


	return S_OK;
}


_int GlcialFlowWall::Tick(_double TimeDelta)
{
	m_ColliderAABB->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_Terrain)
	{
		if (TRUE == static_cast<CCollider*>(m_Terrain->GetComponent(TEXT("Com_BossColliderCom")))->GetIsCollision())
		{
			m_Show = TRUE;
		}
	}
	
	return _int();
}

_int GlcialFlowWall::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderAABB);
#endif // _DEBUG
	}
	return _int();
}

HRESULT GlcialFlowWall::Render()
{
	if (FALSE == m_Show)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_ModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_ModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_ModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_ModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_ModelCom->Get_NumMeshContainer(); ++i)
	{
		m_ModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_ModelCom->Render(i, 0);
	}

#ifdef _DEBUG
	if (TRUE == m_Show)
	{
		if (FAILED(m_ColliderAABB->Render()))
			return E_FAIL;
	}
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT GlcialFlowWall::SetUp_Components()
{
	/* Com_Transform */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, m_ObjectTag, TEXT("Com_Model"), (CComponent**)&m_ModelCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Collider */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(20.f, 5.f, 10.f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_ColliderAABB, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

GlcialFlowWall * GlcialFlowWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	GlcialFlowWall*	pInstance = new GlcialFlowWall(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * GlcialFlowWall::Clone(void * pArg)
{
	GlcialFlowWall*	pInstance = new GlcialFlowWall(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void GlcialFlowWall::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_ModelCom);
	Safe_Release(m_pRendererCom);
}
