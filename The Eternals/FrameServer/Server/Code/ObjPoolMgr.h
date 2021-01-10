#pragma once
#include "ObjPool.h"

class CObj;
typedef CObjPool<CObj>* OBJLIST;

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
	map<wstring, OBJLIST> m_mapObjPool;
};

