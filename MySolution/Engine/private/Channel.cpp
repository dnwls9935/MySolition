#include "..\public\Channel.h"


Channel::Channel()
{
}

HRESULT Channel::NativeConstruct()
{
	return S_OK;
}

Channel * Channel::Create()
{
	Channel* pInstance = new Channel();
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Create Channel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Channel::Free()
{
}

