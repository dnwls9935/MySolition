#pragma once

#include "Base.h"

BEGIN(Engine)

class Channel final : public CBase
{
private:		Channel();
private:		virtual ~Channel() = default;

public:	typedef struct tagKeyFrame {
	_double		m_Time;
	_float3		m_Scale;
	_float4		m_Rotation;
	_float3		m_Position;
}KEYFRAME;

public:		HRESULT	NativeConstruct(const char* _name);
public:		HRESULT	AddKeyFrame(KEYFRAME _keyFrame) { m_KeyFrames.push_back(_keyFrame); return S_OK; };

public:		vector<KEYFRAME>&	GetKeyFrame() { return m_KeyFrames; };

public:		_uint		GetCurrentKeyFrameIndex() const { return m_CurrentKeyFrameIndex; }
public:		void		SetCurrentKeyFrameIndex(_uint _currentKeyFrameIndex) { m_CurrentKeyFrameIndex = _currentKeyFrameIndex; }

public:		_matrix		GetTransformationMatrix() const { return XMLoadFloat4x4(&m_TransformationMatrix); };
public:		void			SetTrasnformationMatrix(_fmatrix _transformationMatrix) { XMStoreFloat4x4(&m_TransformationMatrix, _transformationMatrix); }

private:		vector<KEYFRAME>					m_KeyFrames;
private:		typedef	 vector<KEYFRAME>	KEYFRAMES;

private:		char				m_Name[MAX_PATH] = "";
private:		_uint				m_CurrentKeyFrameIndex = 0;
private:		_float4x4		m_TransformationMatrix;

public:		static Channel* Create(const char* _name);
public:		virtual void Free() override;
};

END