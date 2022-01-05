#pragma once

#include "Base.h"

BEGIN(Engine)

class Animation final : public CBase
{
private:		Animation();
private:		virtual ~Animation() = default;

public:		typedef struct tagAnimationDesc {
		char			m_AnimationName[MAX_PATH] = "";
		_double		m_Duration = 0.0;
		_double		m_TrackPositionAcc = 0.0;
		_double		m_TrackPlaySpeed = 0.0;
		_bool		m_IsFinished = FALSE;
	}ANIMDESC;

public:		HRESULT	NativeConstruct(Animation::ANIMDESC _animDesc);
public:		HRESULT	AddChannel(class Channel* _channel) { m_Channels.push_back(_channel); return S_OK; }
public:		HRESULT	UpdateTransformationMatrix(_double _timeDelta);


private:		ANIMDESC		m_AnimDesc;
private:		vector<class Channel*>				m_Channels;
private:		typedef vector<class Channel*>	CHANNELS;

public:		static Animation* Create(Animation::ANIMDESC _animDesc);
public:		virtual void Free();
};

END