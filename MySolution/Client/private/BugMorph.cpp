#include "stdafx.h"
#include "..\public\BugMorph.h"
#include "GameInstance.h"
#include "Player.h"
#include "SMG.h"

BugMorph::BugMorph(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

BugMorph::BugMorph(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT BugMorph::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT BugMorph::NativeConstruct(void * pArg)
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

	m_HP = 3000;
	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPAWN_RUN);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_TargetObjectList = &pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int BugMorph::Tick(_double TimeDelta)
{
	Animation();
	

	if(TRUE == m_FrameStart)
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	else
		m_pModelCom->Update_CombinedTransformationMatrix(0.0);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	HitCheck();

	return _int();
}

_int BugMorph::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_pModelCom->GetAnimationFinished())
	{
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE_FLIGHT);
	}

	//m_ColliderCom->CollisionAABB((CCollider*)m_TargetObject->GetComponent(TEXT("Com_AABB")));

	if (0 >= m_HP)
	{
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::DEATH_CRITICAL);
		if (m_pModelCom->GetAnimationFinished())
		{
			m_Dead = TRUE;
			return m_Dead;
		}
	}

	return _int();
}

HRESULT BugMorph::Render()
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

#ifdef _DEBUG
	m_ColliderCom->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void BugMorph::HitCheck()
{
	auto& LayerPlayer = m_TargetObjectList->begin();
	std::advance(LayerPlayer, 1);
	if (TRUE == static_cast<SMG*>(*LayerPlayer)->GetFireFrame())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		Calculator::CALCDESC CalDesc;
		CalDesc._width = g_iWinCX;
		CalDesc._height = g_iWinCY;
		CalDesc._transformCom = m_pTransformCom;
		CalDesc._hWnd = g_hWnd;

		pGameInstance->CalcMousePos(&CalDesc);

		CalDesc._rayPos = XMVector3TransformCoord(CalDesc._rayPos, m_pTransformCom->Get_WorldMatrix());
		CalDesc._rayDir = XMVector3TransformNormal(CalDesc._rayDir, m_pTransformCom->Get_WorldMatrix());
		CalDesc._rayDir =  XMVector3Normalize(CalDesc._rayDir);
		
		_float	Distance;

		if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir, Distance))
		{
			m_HP--;
		}
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void BugMorph::Intro()
{
	_vector PlayerPosition = static_cast<CTransform*>(m_TargetObjectList->front()->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	_vector MyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector Distance = PlayerPosition - MyPosition;

	_float DistanceFloat = XMVectorGetX(XMVector3Length(Distance));

	if (10 >= DistanceFloat)
		m_FrameStart = TRUE;
}

void BugMorph::Animation()
{
	if(!m_FrameStart)
		Intro();
	else {

	}

}

HRESULT BugMorph::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BugMorph"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Model */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(0.3f, 1.3f, 0.3f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

BugMorph * BugMorph::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BugMorph*	pInstance = new BugMorph(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * BugMorph::Clone(void * pArg)
{
	BugMorph*	pInstance = new BugMorph(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void BugMorph::Free()
{
	__super::Free();

	Safe_Release(m_ColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
