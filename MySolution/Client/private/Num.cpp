#include "stdafx.h"
#include "..\public\Num.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Player.h"
#include "SMG.h"

Num::Num(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

Num::Num(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT Num::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT Num::NativeConstruct(void * pArg)
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

	m_Camera = static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front());

	list<CGameObject*> TargetObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	auto& iter = TargetObjectList.begin();
	advance(iter, 0);
	m_Player = *iter;
	if (nullptr == m_Player)
		return E_FAIL;

	advance(iter, 1);
	m_SMG = *iter;
	if (nullptr == m_SMG)
		return E_FAIL;

	ShieldPos = {70, 605};
	HpPos = { 70, 645 };
	AmmoPos = {1130 , 640};
	Gap = 15.f;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int Num::Tick(_double TimeDelta)
{
	TickHP();
	TickShield();
	TickAmmo();
	TickMagazine();
	
	return _int();
}

_int Num::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT Num::Render()
{
	if (TRUE == m_Camera->GetFocus())
		return S_OK;

	if (FAILED(RenderHP()))
		return S_OK;

	if (FAILED(RenderShield()))
		return S_OK;

	if (FAILED(RenderAmmo()))
		return S_OK;

	if (FAILED(RenderSlash()))
		return S_OK;

	if (FAILED(RenderMagazine()))
		return S_OK;

	return S_OK;
}

void Num::TickHP()
{
	_int HP = m_Player->GetHP();

	H1 = HP / 1000;

	H2 = HP / 100;
	H2 = H2 % 10;

	H3 = HP / 10;
	H3 = H3 % 10;

	H4 = HP % 10;
}

void Num::TickShield()
{
	_int Shield = static_cast<CPlayer*>(m_Player)->GetShield();

	S1 = Shield / 100;

	S2 = Shield / 10;
	S2 = S2 % 10;

	S3 = Shield % 10;
}

void Num::TickAmmo()
{
	_int Ammo = static_cast<SMG*>(m_SMG)->GetAmmo();

	A1 = Ammo / 10;
	A2 = Ammo % 10;
}

void Num::TickMagazine()
{
	_int Magazine = static_cast<SMG*>(m_SMG)->GetMazine();

	M1 = Magazine / 100;

	M2 = Magazine / 10;
	M2 = M2 % 10;

	M3 = Magazine % 10;
}

HRESULT Num::RenderHP()
{
	if (FAILED(RenderHP0()))
		return E_FAIL;
	if (FAILED(RenderHP1()))
		return E_FAIL;
	if (FAILED(RenderHP2()))
		return E_FAIL;
	if (FAILED(RenderHP3()))
		return E_FAIL;

	return S_OK;
}

HRESULT Num::RenderHP0()
{
	_float X = 0, Y = 0;
	if (5 <= H1) {
		X = H1 - 5;
		Y = 1;
	}
	else {
		X = H1;
		Y = 0;
	}
	if (0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, HpPos.x, HpPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderHP1()
{
	_float X = 0, Y = 0;
	if (5 <= H2) {
		X = H2 - 5;
		Y = 1;
	}
	else {
		X = H2;
		Y = 0;
	}

	if(0 == H1 && 0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, HpPos.x + Gap, HpPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderHP2()
{
	_float X = 0, Y = 0;
	if (5 <= H3) {
		X = H3 - 5;
		Y = 1;
	}
	else {
		X = H3;
		Y = 0;
	}

	if (0 == H1 && 0 == H2 && 0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, HpPos.x + Gap * 2, HpPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderHP3()
{
	_float X = 0, Y = 0;
	if (5 <= H4) {
		X = H4 - 5;
		Y = 1;
	}
	else {
		X = H4;
		Y = 0;
	}

	if (0 == H1 && 0 == H2 && 0 == H3 && 0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, HpPos.x + Gap * 3, HpPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderShield()
{
	if (FAILED(RenderShield0()))
		return E_FAIL;
	if (FAILED(RenderShield1()))
		return E_FAIL;
	if (FAILED(RenderShield2()))
		return E_FAIL;

	return S_OK;
}

HRESULT Num::RenderShield0()
{
	_float X = 0, Y = 0;
	if (5 <= S1) {
		X = S1 - 5;
		Y = 1;
	}
	else {
		X = S1;
		Y = 0;
	}

	if (0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, ShieldPos.x, ShieldPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderShield1()
{
	_float X = 0, Y = 0;
	if (5 <= S2) {
		X = S2 - 5;
		Y = 1;
	}
	else {
		X = S2;
		Y = 0;
	}

	if (0 == S1 && 0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, ShieldPos.x + Gap, ShieldPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderShield2()
{
	_float X = 0, Y = 0;
	if (5 <= S3) {
		X = S3 - 5;
		Y = 1;
	}
	else {
		X = S3;
		Y = 0;
	}

	if (0 == S1 && 0 == S2 && 0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, ShieldPos.x + Gap * 2, ShieldPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderAmmo()
{
	if (FAILED(RenderAmmo0()))
		return E_FAIL;
	if (FAILED(RenderAmmo1()))
		return E_FAIL;

	return S_OK;
}

HRESULT Num::RenderAmmo0()
{
	_float X = 0, Y = 0;
	if (5 <= A1) {
		X = A1 - 5;
		Y = 1;
	}
	else {
		X = A1;
		Y = 0;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, AmmoPos.x, AmmoPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderAmmo1()
{
	_float X = 0, Y = 0;
	if (5 <= A2) {
		X = A2 - 5;
		Y = 1;
	}
	else {
		X = A2;
		Y = 0;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, AmmoPos.x + Gap, AmmoPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderSlash()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, AmmoPos.x + Gap + 15, AmmoPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pSlashCom, 0);

	m_pVIBufferCom->Render(10);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderMagazine()
{
	if (FAILED(RenderMagazine0()))
		return S_OK;
	if (FAILED(RenderMagazine1()))
		return S_OK;
	if (FAILED(RenderMagazine2()))
		return S_OK;

	return S_OK;
}

HRESULT Num::RenderMagazine0()
{
	_float X = 0, Y = 0;
	if (5 <= M1) {
		X = M1 - 5;
		Y = 1;
	}
	else {
		X = M1;
		Y = 0;
	}

	if (0 == X && 0 == Y)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, AmmoPos.x + Gap + 35, AmmoPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderMagazine1()
{
	_float X = 0, Y = 0;
	if (5 <= M2) {
		X = M2 - 5;
		Y = 1;
	}
	else {
		X = M2;
		Y = 0;
	}
	if (0 == M1 && 0 == X && 0 == Y)
		return S_OK;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, AmmoPos.x + Gap *2 + 35, AmmoPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::RenderMagazine2()
{
	_float X = 0, Y = 0;
	if (5 <= M3) {
		X = M3 - 5;
		Y = 1;
	}
	else {
		X = M3;
		Y = 0;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, AmmoPos.x + Gap * 3 + 35, AmmoPos.y);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Num::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_Font_Slash"), TEXT("Com_Texture1"), (CComponent**)&m_pSlashCom)))
		return E_FAIL;

	return S_OK;
}

Num * Num::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Num*	pInstance = new Num(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Num");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Num::Clone(void * pArg)
{
	Num*	pInstance = new Num(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating Num");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Num::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);

	Safe_Release(m_pVIBufferCom);

	Safe_Release(m_pRendererCom);
}
