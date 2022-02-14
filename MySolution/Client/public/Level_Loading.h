#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 특정레벨에 진입하기전에 사전에 합번 진입하는 레벨. */
/* 로딩씬을 담당한다. 폰트, 유아이같은 객체들을 생성해서 .... */
/* 로더객체를 생성하낟.(다음레벨정보를 넘기낟. ) : 스레드새성. ..로딩.  */
/* 로더에게 물어본다. 다 로딩했어?! 어! 레벨넘긴다.*/

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
public:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;
public:
	virtual HRESULT NativeConstruct(LEVEL eNextLevel);
	virtual _int Tick(_double TimeDelta);
	virtual HRESULT Render();

private:
	LEVEL			m_eNextLevel = LEVEL_END;
	class CLoader*	m_pLoader = nullptr;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT	 Ready_Layer_SkyBox(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);

private:
	HRESULT Open_Level();
	

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END 