#include "..\public\SoundManager.h"
#include <filesystem>

IMPLEMENT_SINGLETON(SoundManager)

SoundManager::SoundManager()
{

}

HRESULT Engine::SoundManager::Ready_SoundMgr()
{
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
		return E_FAIL;

	AUDIO_ENGINE_FLAGS flag = AudioEngine_Default;
#ifdef _DEBUG
	flag = flag | AudioEngine_Debug;
#endif // _DEBUG
	m_pAudioEngine = new AudioEngine(flag);

	if (FAILED(LoadSoundFile()))
		return E_FAIL;

	return S_OK;
}

_int Engine::SoundManager::Update_SoundMgr()
{
	if (!m_pAudioEngine->Update())
	{
		if (m_pAudioEngine->IsCriticalError())
		{
			return -1;
		}
	}

	return _int();
}

void Engine::SoundManager::PlayBGM(TCHAR* pSoundKey, _float _Volume, _int _BGMIdx)
{
	map<_tchar*, DirectX::SoundEffect*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	if (iter == m_mapSound.end())
		return;

	m_pBGM[_BGMIdx] = iter->second->CreateInstance();

	m_pBGM[_BGMIdx]->Play(true);
	m_pBGM[_BGMIdx]->SetVolume(_Volume);
}

void Engine::SoundManager::SoundPlay(_tchar * pSoundKey, _float _Volume)
{
	map<_tchar*, DirectX::SoundEffect*>::iterator iter;
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	if (iter == m_mapSound.end())
		return;

	iter->second->Play(_Volume, 0.f, 0.f);
}

namespace fs = std::experimental::filesystem;
HRESULT Engine::SoundManager::LoadSoundFile()
{
	fs::path p("../bin/Sound");

	for (auto& file : fs::recursive_directory_iterator(p))
	{
		string strPath = file.path().u8string();

		int iLength = strlen(strPath.c_str()) + 1;
		_tchar* pSoundPath = new _tchar[iLength];
		ZeroMemory(pSoundPath, sizeof(_tchar) * iLength);
		MultiByteToWideChar(CP_ACP, 0, strPath.c_str(), iLength, pSoundPath, iLength);

		SoundEffect* soundEffect = new SoundEffect(m_pAudioEngine, pSoundPath);

		char name[_MAX_FNAME];
		_splitpath_s(strPath.c_str(), nullptr, 0, nullptr, 0, name, _MAX_FNAME, nullptr, 0);

		iLength = strlen(name) + 1;
		_tchar* pSoundKey = new _tchar[iLength];
		ZeroMemory(pSoundKey, sizeof(_tchar) * iLength);
		MultiByteToWideChar(CP_ACP, 0, name, iLength, pSoundKey, iLength);

		m_mapSound.emplace(pSoundKey, soundEffect);

	}
	return S_OK;
}

void Engine::SoundManager::OnSuspending()
{
	m_pAudioEngine->Suspend();

}

void Engine::SoundManager::OnResuming()
{
	m_pAudioEngine->Resume();
}

void Engine::SoundManager::Free()
{
	if (m_pAudioEngine)
		m_pAudioEngine->Suspend();

	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
	}
	m_mapSound.clear();

}
