#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
public:
	explicit CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_GamePlay() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT		Ready_LightDesc();
	HRESULT		Ready_Layer_Enemy(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_UI(const _tchar* pLayerTag);

	HRESULT LoadData(HANDLE& hFile);
	HRESULT LoadTerrain(HANDLE& hFile);
	HRESULT LoadNavigation(HANDLE& hFile);
	HRESULT LoadEnvironment(HANDLE& hFile);

public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END 