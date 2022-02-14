#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
public:
	explicit CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Logo() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT		Ready_LightDesc();
	HRESULT		Ready_Layer_Object(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_Sky(const _tchar* pLayerTag);
	HRESULT		Ready_Layer_UI(const _tchar* pLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END 