#include "..\header\SuperBase.h"

Engine::SuperBase::SuperBase()
	: refCnt(0)
{

}

unsigned long Engine::SuperBase::AddRef()
{
	return ++refCnt;
}

unsigned long Engine::SuperBase::Release()
{
	if (0 == refCnt)
	{
		Free();
		delete this;
		return 0;
	}
	else 
		return refCnt--;

	return 0;
}