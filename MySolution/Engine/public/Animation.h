#pragma once

#include "Base.h"

BEGIN(Engine)

class Animation final : public CBase
{

private:
	Animation();
	virtual ~Animation() = default;

public:
	typedef struct tagAnimationDesc {
		char			m_AnimationName[MAX_PATH] = "";
		_double		m_Duration = 0.0;
		_double		m_TrackPositionAcc = 0.0;
		_double		m_TrackPlaySpeed = 0.0;
	}ANIMDESC;

public:
	HRESULT	NativeConstruct(Animation::ANIMDESC _animDesc);

private:
	ANIMDESC		m_AnimDesc;
	
	vector<class Channel*>				m_Chennels;
	typedef vector<class Channel*>	CHANNELS;

public:
	static Animation* Create(Animation::ANIMDESC _animDesc);
	virtual void Free();
};

END