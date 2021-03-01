#include "stdafx.h"
#include "ObjPoolMgr.h"
#include "Player.h"
#include "Npc.h"
#include "Monster.h"

IMPLEMENT_SINGLETON(CObjPoolMgr)

CObjPoolMgr::CObjPoolMgr(void)
{
}

CObjPoolMgr::~CObjPoolMgr(void)
{
}

HRESULT CObjPoolMgr::Init_ObjPoolMgr(void)
{
	CObjectPool* pPlayerPool = new CObjectPool();
	pPlayerPool->Create_ObjectPool<CPlayer>(10);
	m_mapObjPool[L"PLAYER"] = pPlayerPool;

	CObjectPool* pNpcPool = new CObjectPool();
	pNpcPool->Create_ObjectPool<CNpc>(MAX_NPC);
	m_mapObjPool[L"NPC"] = pNpcPool;

	CObjectPool* pMonsterPool = new CObjectPool();
	pMonsterPool->Create_ObjectPool<CMonster>(MAX_MONSTER);
	m_mapObjPool[L"MONSTER"] = pMonsterPool;

	return S_OK;
}

CObj* CObjPoolMgr::use_Object(wstring wstrObjTag)
{
	/* 해당 Object Pool이 있는지 검사 */
	auto& iter_find = m_mapObjPool.find(wstrObjTag);

	if (iter_find == m_mapObjPool.end())
		return nullptr;

	/* 해당 Object Pool이 존재하면 Object 반환 */
	return iter_find->second->use_Object();
}

HRESULT CObjPoolMgr::return_Object(wstring wstrObjTag, CObj* Obj)
{
	/* 해당 Object Pool이 있는지 검사 */
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
