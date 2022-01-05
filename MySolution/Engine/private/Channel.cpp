#include "..\public\Channel.h"


Channel::Channel()
{
}

HRESULT Channel::NativeConstruct(const char* _name)
{
	strcpy_s(m_Name, _name);

	return S_OK;
}

Channel * Channel::Create(const char* _name)
{
	Channel* pInstance = new Channel();
	if (FAILED(pInstance->NativeConstruct(_name)))
	{
		MSGBOX("Failed to Create Channel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Channel::Free()
{
	/*for (auto& pKeyFrame : m_KeyFrames)
		Safe_Delete(pKeyFrame);*/

	m_KeyFrames.clear();
}

