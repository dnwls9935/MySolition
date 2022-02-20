#pragma once
#include "Base.h"

BEGIN(Engine)
class SoundManager final :public CBase
{
	DECLARE_SINGLETON(SoundManager)

private:
	SoundManager();
	virtual ~SoundManager() = default;

public:
	HRESULT Ready_SoundMgr();
	_int Update_SoundMgr();

public:
	void OnSuspending(); //종료
	void OnResuming(); //재개 

	void SoundPlay(_tchar* pSoundKey, _float _Volume);
	void PlayBGM(TCHAR* pSoundKey, _float _Volume, _int _BGMIdx = 0);

private:
	HRESULT LoadSoundFile();


public:
	virtual void Free();

private:
	DirectX::AudioEngine* m_pAudioEngine;
	map<_tchar*, DirectX::SoundEffect*> m_mapSound;
	unique_ptr<DirectX::SoundEffectInstance> m_pBGM[2] = {nullptr};
};


END