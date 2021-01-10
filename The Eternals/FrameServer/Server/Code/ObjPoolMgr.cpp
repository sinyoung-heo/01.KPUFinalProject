#include "stdafx.h"
#include "ObjPoolMgr.h"

IMPLEMENT_SINGLETON(CObjPoolMgr)

CObjPoolMgr::CObjPoolMgr(void)
{
}

CObjPoolMgr::~CObjPoolMgr(void)
{
}

HRESULT CObjPoolMgr::Init_ObjPoolMgr(void)
{
	CObjectPool* pPlayerPool = new CObjectPool(100);
	m_mapObjPool[L"PLAYER"] = pPlayerPool;

	return S_OK;
}

CObj* CObjPoolMgr::use_Object(wstring wstrObjTag)
{
	/* �ش� Object Pool�� �ִ��� �˻� */
	auto& iter_find = m_mapObjPool.find(wstrObjTag);

	if (iter_find == m_mapObjPool.end())
		return nullptr;

	/* �ش� Object Pool�� �����ϸ� Object ��ȯ */
	return iter_find->second->use_Object();

	return nullptr;
}

HRESULT CObjPoolMgr::return_Object(wstring wstrObjTag, CObj* Obj)
{
	/* �ش� Object Pool�� �ִ��� �˻� */
	auto& iter_find = m_mapObjPool.find(wstrObjTag);

	if (iter_find == m_mapObjPool.end())
		return E_FAIL;

	iter_find->second->return_Obj(Obj);

	return S_OK;
}

void CObjPoolMgr::Release()
{
	for (auto& MyPair : m_mapObjPool)
	{
		delete MyPair.second;
		MyPair.second = nullptr;
	}

	m_mapObjPool.clear();
}
