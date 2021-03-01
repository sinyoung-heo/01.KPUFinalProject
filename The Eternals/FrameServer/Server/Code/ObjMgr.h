#pragma once
#include "Obj.h"

typedef /*unordered_*/map <int,CObj*>		OBJLIST;
typedef OBJLIST::iterator				OBJITER;
typedef lock_guard<recursive_mutex>		objmgr_lock;

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
	bool Is_Player(int server_num = 0);
	bool Is_NPC(int server_num = NPC_NUM_START);
	bool Is_Monster(int server_num = MON_NUM_START);
	bool Is_Near(const CObj* me, const CObj* other);

public:
	HRESULT Add_GameObject(wstring wstrObjTag, CObj* pObj, int server_num);
	HRESULT Delete_GameObject(wstring wstrObjTag, CObj* pObj);
	HRESULT	Delete_OBJLIST(wstring wstrObjTag);

public:
	void Release();

private:
	map<wstring, OBJLIST>	m_mapObjList;
	recursive_mutex			m_mutex;
};

