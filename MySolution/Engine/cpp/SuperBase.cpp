#include "..\header\SuperBase.h"


SuperBase::SuperBase()
	: dwRefCnt(0)
{
}

unsigned long SuperBase::AddRef()
{
	return ++dwRefCnt;
}

unsigned long SuperBase::Release()
{
	if (0 == dwRefCnt) {
		Free();
		delete this;
		return 0;
	}
	else
		return dwRefCnt--;
	return 0;
}