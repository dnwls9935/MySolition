#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"
#include "Terrain.h"
#include "Camera_Dynamic.h"
#include "Camera_Static.h"
#include "Sky.h"
#include "LogoSky.h"
#include "Player.h"
#include "SMG.h"
#include "BugMorph.h"
#include "PrimeBeast.h"
#include "BossPrimeBeast.h"
#include "PrimeBeastRock.h"
#include "BossPrimeBeastRock.h"
#include "EnvironmentObject.h"
#include "DahlWeaponLocker.h"
#include "DahlEpicCrate.h"
#include "HyperionChest.h"
#include "HitBullet.h"
#include "GlacialFlowWall.h"
#include "UI.h"
#include "CrossSight.h"
#include "HP.h"
#include "Muzzle.h"
#include "PickUps.h"
#include "HITUI.h"
#include "Snow.h"
#include "SnowScreen.h"
#include "TitleCard.h"
#include "ClapTrap.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	EnterCriticalSection(&pLoader->Get_CriticalSection());

	HRESULT			hr = 0;

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		hr = pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		hr = pLoader->Loading_ForGamePlay();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoader->Get_CriticalSection());
	CoUninitialize();
	return 0;
}


HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CS);

	/* 스레드를 생성한다. -> 진입점함수를 정의한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
	/* 컴포넌트 원형을 생성한다. */
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl")))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("모델을 생성한다. "));
	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(-130.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO	, TEXT("Prototype_Component_Model_ClapTrap"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/ClapTrap/", "ClapTrap.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;


	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Cube"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/ColdNight.dds")))))
		return E_FAIL;


	wsprintf(m_szLoading, TEXT("객체원형을 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_CrapTrap"), ClapTrap::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraStatic"), CCamera_Static::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LogoCube"), LogoSky::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));
	m_isFinished = true;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* 컴포넌트 원형 생성한다. */
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), 200, 200))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Effect_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_RectEffect.hlsl")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstace"), CVIBuffer_PointInstance::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_PointInstance.hlsl"), 40000))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Cube"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/ColdNight.dds")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Snow_256_Dif.tga")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HitBullet"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Blood_Flipbook_Dif.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ammo"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/Ammo.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GrenadeBar"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/GrenadeBar.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HeartCross"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HeartCross.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I2CD"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I2CD.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I2D3"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I2D3.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I3AD"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I3AD.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I3B3"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I3B3.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I3B9"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I3B9.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I15D"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I15D.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I101"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I101.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_I316"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I316.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_IB8"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_IB8.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_IB8_B"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_IB8_B.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD_IF8"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_IF8.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Large"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/Large.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Large_Reverse"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/Large_Reverse.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sheld"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/Sheld.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Small"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/Small.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hit"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/Hit%d.png"), 3))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hyperion_Sight_SMG"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Textures/Tex_Hyperion_Sight_SMG.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/MuzzleEffect/Muzzle%d.png"),4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MuzzleComp"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/MuzzleEffect/MuzzleComp%d.png"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IceScreen"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Snow/Ice_Screen_Dif.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IceScreenNrm"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Snow/Ice_Screen_Nrm.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SnowDustDif"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Snow/Snow_Dust_Dif.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SnowFlakesDif"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Snow/Snow_Flakes_Dif.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SnowFlakesScreenDif"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Snow/Snow_FlakesScreen_Dif.png")))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_Crit"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Critical/Tex_Crit%d.png"), 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UseMeAnimGrad"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Useme/UseMe_AnimGrad.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_TCBackgrounds_KnuckleDragger"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/TitleCard/Tex_TCBackgrounds_KnuckleDragger.png")))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_TCNames_Knuckledragger"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/TitleCard/Tex_TCNames_Knuckledragger.png")))))
		return E_FAIL;


	wsprintf(m_szLoading, TEXT("콜라이더를 생성한다. "));
	/* For.Prototype_Component_Collider*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE::TYPE_AABB))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE::TYPE_SPHERE))))
		return E_FAIL;
	wsprintf(m_szLoading, TEXT("네비게이션을 생성한다. "));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), Navigation::Create(m_pDevice, m_pDeviceContext))))  
		return E_FAIL;
	wsprintf(m_szLoading, TEXT("HP바를 생성한다. "));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_UI_HP"), HP::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("모델불러오는중.... "));
	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Zero/", "ZeroHand.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.013f, 0.013f, 0.013f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BugMorph"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/BugMorph/", "BugMorph.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.025f, 0.025f, 0.025f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossPrimeBeast"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/PrimeBeast/", "PrimeBeast.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;
/*
	PivotMatrix = XMMatrixScaling(1.0f, 1.0, 1.0f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossPrimeBeastRock"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/PrimeBeast/", "BigRock.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
*/
	PivotMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PrimeBeast"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/PrimeBeast/", "PrimeBeast.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PrimeBeastBig"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/PrimeBeast/", "BigRock.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SMG"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Weapon/", "SMG.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(-90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zero"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Zero/", "Zero.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Dahl_WeaponLocker"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/ItemBox/", "Dahl_WeaponLocker.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DahlEpicCrate"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/ItemBox/", "DahlEpicCrate.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HyperionChest"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/ItemBox/", "HyperionChest.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HandsomeJack"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "HandsomeJack.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HandsomeJackGoldenStatue"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "HandsomeJackGoldlStatue.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowDrift"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "GlacialTest.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowPile0"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Snowpile00.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowPile1"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Snowpile01.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowPile2"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Snowpile02.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowPile3"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Snowpile03.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowPile4"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Snowpile04.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GlacialFlow_Straight0"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "GlacialFlow_Straight0.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GlacialFlow_Straight1"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "GlacialFlow_Straight1.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ArenaSign"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "ArenaSign.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AutoJunk"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "AutoJunk.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AutoJunk2"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "AutoJunk2.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BanditFlag"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BanditFlag.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BanditHouse"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BanditHouse.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BanditHouseDestory"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BanditHouseDestory.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCrapTrapA"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BrokenCrapTrapA.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCrapTrapB"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BrokenCrapTrapB.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCrapTrapC"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BrokenCrapTrapC.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCrapTrapD"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BrokenCrapTrapD.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCrapTrapE"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BrokenCrapTrapE.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BrokenCrapTrapF"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "BrokenCrapTrapF.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DoorWayLR"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "DoorWayLR.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlagPole"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "FlagPole.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FlagPoleOptimizer"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "FlagPoleOptimizer.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Furace"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Furace.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_IceBlockLarge"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "IceBlockLarge.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkyBoxMountain"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "SkyBoxMountain.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SnowDriftStatic"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "SnowDrift.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Tire"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Tire.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TurbinBase"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "TurbinBase.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_TurbinFan"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "TurbinFan.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ComShip"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "ComShip.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ComShipBrace"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "ComShipBrace.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DeadMaleVillagerBody"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "DeadMaleVillagerBody.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DeadMaleVillagerBody1"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "DeadMaleVillagerBody1.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DeathBandit0"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "DeathBandit0.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DeathBandit1"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "DeathBandit1.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FrozenWaterFall"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "FrozenWaterFall.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GlacialFlowWall"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "GlacialFlowWall.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_IceFalls"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "IceFalls.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wire0"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Wire0.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wire1"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Wire1.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wire2"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Wire2.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wire3"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/", "Wire3.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(100.f, 100.f, 100.f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BoosterSheld"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/PickUps/", "BoosterSheld.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HealthViral"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/PickUps/", "HealthViral.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(200.f, 200.f, 200.f);
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ShotgunAmmoPickUp"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/PickUps/", "ShotgunAmmoPickUp.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SMGAmmoPickUp"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Static/PickUps/", "SMGAmmoPickUp.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;



	/* 객체원형을 생성한다. */
	wsprintf(m_szLoading, TEXT("객체를 생성한다. "));	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CameraDynamic"), CCamera_Dynamic::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"), Sky::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Player"), CPlayer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BugMorph"), BugMorph::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_PrimeBeast"), PrimeBeast::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BossPrimeBeast"), BossPrimeBeast::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_PrimeBeastRock"), PrimeBeastRock::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BossPrimeBeastRock"), BossPrimeBeastRock::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SMG"), SMG::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Dahl_WeaponLocker"), DahlWeaponLocker::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DahlEpicCrate"), DahlEpicCrate::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HyperionChest"), HyperionChest::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_EnvironmentObject"), EnvrionmentObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_GlacialFlowWall"), GlcialFlowWall::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_PickUps"), PickUps::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	/* UI들 */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"), UI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_CrossSight"), CrossSight::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HIT"), HITUI::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TitleCard"), TitleCard::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// 이팩트들
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_HitBullet"), HitBullet::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Muzzle"), Muzzle::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Snow"), Snow::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_SnowScreen"), SnowScreen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));

	m_isFinished = true;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CLoader::Print_LoaderText()
{
	SetWindowText(g_hWnd, m_szLoading);
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSGBOX("Failed to Creating CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);	
	DeleteCriticalSection(&m_CS);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);		

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
