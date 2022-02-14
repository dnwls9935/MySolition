#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* Ư�������� �����ϱ����� ������ �չ� �����ϴ� ����. */
/* �ε����� ����Ѵ�. ��Ʈ, �����̰��� ��ü���� �����ؼ� .... */
/* �δ���ü�� �����ϳ�.(�������������� �ѱⳮ. ) : ���������. ..�ε�.  */
/* �δ����� �����. �� �ε��߾�?! ��! �����ѱ��.*/

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