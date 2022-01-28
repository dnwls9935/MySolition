#pragma once

#include "Base.h"

/* 애니메이션 하나의 동작. */

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT NativeConstruct(char* pName, _double Duration, _double PlaySpeed);
	HRESULT Add_Channel(class CChannel* pChannel);
	HRESULT Update_TransformationMatrix(_double TimeDelta);

public:
	_bool	GetFinished()const { return m_isFinished; };
	void			SetTrackPositionAcc(_double _TrackPositionAcc) {
		m_TrackPositionAcc = _TrackPositionAcc;
	}
	_double	GetTrackPositionAcc() const { 
		return m_TrackPositionAcc;
	};

private:
	char				m_szName[MAX_PATH] = "";
	_double				m_Duration = 0.0; /*하나의 애니메이션을 재생하는데 걸리는 시간(트랙의 위치와 동일) */
	_double				m_TrackPositionAcc = 0.0;
	_double				m_PlaySpeed = 0.0;
	_bool				m_isFinished = false;

private:
	/* 현재 애니메이션에 영향ㅇ르 주는 뼈대들의 정보. */
	vector<class CChannel*>			m_Channels;
	typedef vector<class CChannel*>	CHANNELS;
public:
	static CAnimation* Create(char* pName, _double Duration, _double PlaySpeed);
	virtual void Free();
};

END