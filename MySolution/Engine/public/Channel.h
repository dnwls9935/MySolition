#pragma once

#include "Base.h"

BEGIN(Engine)

class Channel final : public CBase
{
private:
	Channel();
	virtual ~Channel() = default;

public:
	typedef struct tagKeyFrame {
		_double		m_Time;
		_float3		m_Scale;
		_float4		m_Rotation;
		_float3		m_Position;
	}KEYFRAME;

public:
	HRESULT NativeConstruct();

private:
	vector<KEYFRAME>					m_KeyFrames;
	typedef vector<KEYFRAME>		KEYFRAMES;

public:
	static Channel* Create();
	virtual void Free() override;
};

END