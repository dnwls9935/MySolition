#include "..\public\Base.h"

CBase::CBase()
	: m_dwRefCnt(0)
{
}

unsigned long CBase::AddRef()
{
	return ++m_dwRefCnt;	
}

unsigned long CBase::Release()
{
	/* 감소하지 못할때 삭제한다. */
	if ( 0 == m_dwRefCnt)
	{
		Free();

		delete this;

		return 0;
	}
	/* 감소한다. */
	else
		return m_dwRefCnt--;

	return 0;
}

