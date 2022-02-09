#include "stdafx.h"
#include "..\public\PrimeBeastRock.h"
#include "GameInstance.h"
#include "Player.h"
#include "HierarchyNode.h"
#include "PrimeBeast.h"
#include "Player.h"

#include <iostream>

PrimeBeastRock::PrimeBeastRock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

PrimeBeastRock::PrimeBeastRock(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT PrimeBeastRock::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT PrimeBeastRock::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	ZeroMemory(&m_PrbDesc, sizeof(PRBDESC));
	memcpy(&m_PrbDesc, (PRBDESC*)pArg, sizeof(PRBDESC));
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_Type = CGameObject::OBJTYPE_ID::ENEMY_OBJECT;

	if (TYPE_ROCK::SMALL == m_PrbDesc.m_TypeRock)
		m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 0.5f));

	return S_OK;
}

_int PrimeBeastRock::Tick(_double TimeDelta)
{
	if (FALSE == m_ChaseTarget)
	{
		if (TRUE == m_PrbDesc.m_PrimeBeast->GetDead()) {
			_vector MyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			MyPosition = XMVectorSetY(MyPosition, XMVectorGetY(MyPosition) - TimeDelta * 2.5f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, MyPosition);
		}
		else {
			CheckState(TimeDelta);
		}
	}
	else {
		_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector		vLook = m_PrbDesc.m_TargetPosition - vPosition;

		vPosition += XMVector3Normalize(vLook) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * (_float)TimeDelta;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	return _int();
}

_int PrimeBeastRock::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderCom);
#endif // !_DEBUG
	}

	if (0 >= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
		m_Dead = TRUE;

	if (m_ColliderCom->CollisionSphereToAABB(static_cast<CCollider*>(m_PrbDesc.m_Target->GetComponent(TEXT("Com_AABB"))))) {
		static_cast<CPlayer*>(m_PrbDesc.m_Target)->Hit(-100);
		m_Dead = TRUE;
	}

	return m_Dead;
}

HRESULT PrimeBeastRock::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_RockModel->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_RockModel->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_RockModel->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_RockModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_RockModel->Get_NumMeshContainer(); ++i)
	{
		m_RockModel->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_RockModel->Render(i, 0);
	}

	RELEASE_INSTANCE(CGameInstance);

#ifdef _DEBUG
	m_ColliderCom->Render();
#endif // _DEBUG


	return S_OK;
}

void PrimeBeastRock::CheckState(_double TimeDelta)
{
	CModel* ParentModel = static_cast<CModel*>(m_PrbDesc.m_PrimeBeast->GetComponent(TEXT("Com_Model")));
	if ((_uint)PrimeBeast::ANIMATION_STATE::ATT_TR_V1 == ParentModel->GetCurrentAnimation() &&
		38 == ParentModel->GetCurrentAnimationFrame()) {

		m_ChaseTarget = TRUE;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		_vector PlayerPosition = static_cast<CTransform*>(m_PrbDesc.m_Target->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
		m_PrbDesc.m_TargetPosition = PlayerPosition;

		RELEASE_INSTANCE(CGameInstance);
	}
	else {
		_matrix BoneMatrix = static_cast<PrimeBeast*>(m_PrbDesc.m_PrimeBeast)->SetRockPosition();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, BoneMatrix.r[3]);
	}
}

HRESULT PrimeBeastRock::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PrimeBeastBig"), TEXT("Com_RockModel"), (CComponent**)&m_RockModel)))
		return E_FAIL;

	/* Com_Collider*/
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(3.0f, 3.0f, 3.0f);
	CollisionDesc.Position = _float3(0.0f, 0.0f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

PrimeBeastRock * PrimeBeastRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	PrimeBeastRock*	pInstance = new PrimeBeastRock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PrimeBeastRock::Clone(void * pArg)
{
	PrimeBeastRock*	pInstance = new PrimeBeastRock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PrimeBeastRock::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_RockModel);
	Safe_Release(m_pRendererCom);
}
