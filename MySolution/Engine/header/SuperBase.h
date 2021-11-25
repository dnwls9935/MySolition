#pragma once

#include "EngineInclude.h"

BEGIN(Engine)

class DLL_MODE SuperBase abstract
{
protected:
	SuperBase();
	virtual ~SuperBase() = default;

public:
	unsigned long AddRef();
	unsigned long Release();

private:
	unsigned long dwRefCnt = 0;

public:
	virtual void Free() = 0;
};

END