#include "..\public\Animation.h"

Animation::Animation()
{
}

HRESULT Animation::NativeConstruct(Animation::ANIMDESC _animDesc)
{
	m_AnimDesc = _animDesc;
	return S_OK;
}

Animation * Animation::Create(Animation::ANIMDESC _animDesc)
{
	Animation* pInstance = new Animation();
	if (FAILED(pInstance->NativeConstruct(_animDesc)))
	{
		MSGBOX("Faild to Create Animation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Animation::Free()
{
}