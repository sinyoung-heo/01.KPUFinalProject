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
	//CObj* Get_GameObject(int server_num);
	CObj* Get_GameObject(wstring wstrObjTag, int server_num = 0);
	//unordered_map<int, CObj*>& Get_mapObj() { return m_mapObj; }
	OBJLIST* Get_OBJLIST(wstring wstrObjTag);

public:
	//HRESULT Add_GameObject(int num, CObj* pObj);
	HRESULT Add_GameObject(wstring wstrObjTag, CObj* pObj, int server_num);
	//HRESULT Delete_GameObject(int server_num);
	HRESULT Delete_GameObject(wstring wstrObjTag, CObj* pObj);
	HRESULT	Delete_OBJLIST(wstring wstrObjTag);

public:
	void Release();

private:
	map<wstring, OBJLIST>	m_mapObjList;
};

