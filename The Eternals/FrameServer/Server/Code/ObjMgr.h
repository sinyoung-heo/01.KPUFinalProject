#pragma once
#include "Obj.h"

class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr)

private:
	explicit CObjMgr(void);
	virtual ~CObjMgr(void);

public:
	HRESULT Add_GameObject(int num, CObj* pObj);
	HRESULT Delete_GameObject(int server_num);
public:
	CObj* Get_GameObject(int server_num);

public:
	void Release();

private:
	unordered_map<int, CObj*>	m_mapObj;
};

