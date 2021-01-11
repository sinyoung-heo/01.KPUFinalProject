#pragma once
#include "Obj.h"

typedef unordered_map <int,CObj*>		OBJLIST;
typedef OBJLIST::iterator				OBJITER;

class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr)

private:
	explicit CObjMgr(void);
	virtual ~CObjMgr(void);

public:
	HRESULT Init_ObjMgr();
public:
	CObj* Get_GameObject(wstring wstrObjTag, int server_num = 0);
	OBJLIST* Get_OBJLIST(wstring wstrObjTag);

public:
	HRESULT Add_GameObject(wstring wstrObjTag, CObj* pObj, int server_num);
	HRESULT Delete_GameObject(wstring wstrObjTag, CObj* pObj);
	HRESULT	Delete_OBJLIST(wstring wstrObjTag);

public:
	void Release();

private:
	map<wstring, OBJLIST>	m_mapObjList;
};

