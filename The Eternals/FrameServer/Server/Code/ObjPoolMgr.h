#pragma once
#include "ObjectPool.h"

class CObjPoolMgr
{
	DECLARE_SINGLETON(CObjPoolMgr)

private:
	explicit CObjPoolMgr(void);
	virtual ~CObjPoolMgr(void);
	
public:
	HRESULT Init_ObjPoolMgr(void);

public:
	CObj* use_Object(wstring wstrObjTag);
	HRESULT return_Object(wstring wstrObjTag, CObj* Obj);
public:
	void Release();

private:
	map<wstring, CObjectPool*> m_mapObjPool;
};

