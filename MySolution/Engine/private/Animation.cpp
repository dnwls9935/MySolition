#include "..\public\Animation.h"

#include "Channel.h"

Animation::Animation()
{
}

HRESULT Animation::NativeConstruct(Animation::ANIMDESC _animDesc)
{
	m_AnimDesc = _animDesc;
	return S_OK;
}

HRESULT Animation::UpdateTransformationMatrix(_double _timeDelta)
{
	m_AnimDesc.m_TrackPositionAcc += m_AnimDesc.m_TrackPlaySpeed * _timeDelta;

	if (m_AnimDesc.m_TrackPositionAcc >= m_AnimDesc.m_Duration)
	{
		m_AnimDesc.m_IsFinished = TRUE;
		m_AnimDesc.m_TrackPositionAcc = 0.0;
	}
	else {
		m_AnimDesc.m_IsFinished = FALSE;
	}

	_uint		pNumChannels = (_uint)m_Channels.size();

	for (_uint i = 0; i < pNumChannels; i++)
	{
		vector<Channel::KEYFRAME>		pKeyFrame = m_Channels[i]->GetKeyFrame();

		_uint pCurrentKeyFrameIndex = m_Channels[i]->GetCurrentKeyFrameIndex();

		_vector pScale, pRotation, pPosition;
		pScale = XMVectorZero();
		pRotation = XMVectorZero();
		pPosition = XMVectorZero();


		if (TRUE == m_AnimDesc.m_IsFinished)
		{
			pCurrentKeyFrameIndex = 0;
			m_Channels[i]->SetCurrentKeyFrameIndex(0);
		}

		_uint		pNumKeyFrame = (_uint)pKeyFrame.size();

		/* 애니메이션 시작전 */
		if (m_AnimDesc.m_TrackPositionAcc < pKeyFrame[0].m_Time)
		{
			pScale = XMLoadFloat3(&pKeyFrame[0].m_Scale);
			pRotation = XMLoadFloat4(&pKeyFrame[0].m_Rotation);
			pPosition = XMLoadFloat3(&pKeyFrame[0].m_Position);
			pPosition = XMVectorSetW(pPosition, 1.f);
		}
		/* 애니메이션 끝남 */
		else if (m_AnimDesc.m_TrackPositionAcc > pKeyFrame[pNumKeyFrame-1].m_Time) {
			pScale = XMLoadFloat3(&pKeyFrame[pNumKeyFrame-1].m_Scale);
			pRotation = XMLoadFloat4(&pKeyFrame[pNumKeyFrame-1].m_Rotation);
			pPosition = XMLoadFloat3(&pKeyFrame[pNumKeyFrame-1].m_Position);
			pPosition = XMVectorSetW(pPosition, 1.f);
		}
		/* 아직 애니메이션이 돌고있다 -> 특정 키프레임과 그 다음 키프레임 사이 어딘가에 있다 */
		else {
			if (m_AnimDesc.m_TrackPositionAcc >= pKeyFrame[pCurrentKeyFrameIndex + 1].m_Time)
				m_Channels[i]->SetCurrentKeyFrameIndex(++pCurrentKeyFrameIndex);

			_float	pRatio = (_float)((m_AnimDesc.m_TrackPositionAcc - pKeyFrame[pCurrentKeyFrameIndex].m_Time) /
				(pKeyFrame[pCurrentKeyFrameIndex + 1].m_Time - pKeyFrame[pCurrentKeyFrameIndex].m_Time));

			_vector pSourScale, pSourRotation, pSourPosition,
						pDestScale, pDestRotation, pDestPosition;

			pSourScale = XMLoadFloat3(&pKeyFrame[pCurrentKeyFrameIndex].m_Scale);
			pSourRotation = XMLoadFloat4(&pKeyFrame[pCurrentKeyFrameIndex].m_Rotation);
			pSourPosition = XMLoadFloat3(&pKeyFrame[pCurrentKeyFrameIndex].m_Position);

			pDestScale = XMLoadFloat3(&pKeyFrame[pCurrentKeyFrameIndex].m_Scale);
			pDestRotation = XMLoadFloat4(&pKeyFrame[pCurrentKeyFrameIndex].m_Rotation);
			pDestPosition = XMLoadFloat3(&pKeyFrame[pCurrentKeyFrameIndex].m_Position);

			pScale = XMVectorLerp(pSourScale, pDestScale, pRatio);
			pRotation = XMQuaternionSlerp(pSourRotation, pDestRotation, pRatio);
			pPosition = XMVectorLerp(pSourPosition, pDestPosition, pRatio);

			_matrix pTransformationMatrix = XMMatrixAffineTransformation(pScale, XMVectorSet(0.f, 0.f, 0.f, 0.f) ,pRotation, pPosition);

			m_Channels[i]->SetTrasnformationMatrix(pTransformationMatrix);
		}
	}

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
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}