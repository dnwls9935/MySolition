#pragma once

#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "Collider.h"
#include "Navigation.h"
#include "Model.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_TriangleToLine.h"
#include "VIBuffer_PointInstance.h"
#include "VIBuffer_PointInstance_Dust.h"
#include "VIBuffer_PointInstance_CollisionDust.h"
#include "HP.h"
#include "ThreeWaySystem.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;	
public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	
private:	
	unordered_map<const _tchar*, class CComponent*>*			m_pComponents = nullptr;
	typedef unordered_map<const _tchar*, class CComponent*>		COMPONENTS;

	_uint			m_iNumLevels = 0;
private:
	CComponent* Find_Prototype_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);
public:
	virtual void Free() override;
};

END