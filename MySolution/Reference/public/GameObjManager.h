#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class GameObjManager : public SuperBase
{
	DECLARE_SINGLETON(GameObjManager)
private:
	GameObjManager() {};
	virtual ~GameObjManager() = default;

public:
	HRESULT		ReserveManager(_uint _lvlNum);
	HRESULT		Add_ProtoObj(const _tchar* _tag, class GameObj* _obj);
	HRESULT		Add_GameObj(_uint _lvl, const _tchar* _layerTag, const _tchar* _protoTag, void* _arg);
	_int		Tick(_double _timeDelta);
	_int		LateTick(_double _timeDelta);
	
private:
	class GameObj* FindProto(const _tchar* _protoTag);

private:
	unordered_map<const _tchar*, class GameObj*> umapProtoContainer;
	typedef unordered_map<const _tchar*, class GameObj*> PROTOTYPE;

	unordered_map<const _tchar*, class Layer*>* umapGameObjContainer = nullptr;
	typedef unordered_map<const _tchar*, class Layer*> LAYERS;

	_uint levelNum = 0;

public:
	virtual void Free();
};

END