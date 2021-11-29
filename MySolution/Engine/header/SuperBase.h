#pragma once

#include "EngineIncludeh.h"

BEGIN(Engine)

class ENGINE_DLL SuperBase abstract
{
protected:
	SuperBase();
	virtual ~SuperBase() = default;

public:
	unsigned long AddRef();
	unsigned long Release();

private:
	unsigned long	refCnt = 0;

public:
	virtual void Free() = 0;
};

END