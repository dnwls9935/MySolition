#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* 특정레벨의 원형객첻(게임오브젝트, 컴포넌트)르을 생성한다.  */
BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoader() = default;

public:
	CRITICAL_SECTION Get_CriticalSection() const {
		return m_CS;
	}

	LEVEL Get_NextLevelID() const {	
		return m_eNextLevel;
	} 

	_bool Get_Finished() const {
		return m_isFinished;
	}
	

public:
	HRESULT NativeConstruct(LEVEL eNextLevel);
	HRESULT Loading_ForLogo();
	HRESULT Loading_ForGamePlay();

public:
	void Print_LoaderText();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_CS;

	_tchar				m_szLoading[MAX_PATH] = TEXT("");
	_bool				m_isFinished = false;

private:
	LEVEL			m_eNextLevel = LEVEL_END;

private:
	

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END