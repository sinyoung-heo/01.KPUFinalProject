#include "stdafx.h"
#include "ObjMgr.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr(void)
{
}

CObjMgr::~CObjMgr(void)
{
}

HRESULT CObjMgr::Init_ObjMgr()
{
	m_mapObjList[L"PLAYER"] = OBJLIST();
	m_mapObjList[L"NPC"] = OBJLIST();
	m_mapObjList[L"MONSTER"] = OBJLIST();

	return S_OK;
}

CObj* CObjMgr::Get_GameObject(wstring wstrObjTag, int server_num)
{
	objmgr_lock ol(m_mutex);

	/* map���� ã���� �ϴ� OBJLIST�� Key ���� ���� ã�� */
	auto& iter_find = m_mapObjList.find(wstrObjTag);

	/* �ش� OBJLIST�� ã�� ���Ͽ��ٸ� NULL ��ȯ */
	if (iter_find == m_mapObjList.end())
		return nullptr;

	/* server number���� �ִ��� Ž�� */
	auto& user = iter_find->second.find(server_num);

	/* Ž�� ���� */
	if (user == iter_find->second.end())
		return nullptr;

	/* Ž�� ���� -> ��ü ã�� ���� */
	return user->second;
}

OBJLIST* CObjMgr::Get_OBJLIST(wstring wstrObjTag)
{
	objmgr_lock ol(m_mutex);

	/* map���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
	auto& iter_find = m_mapObjList.find(wstrObjTag);

	/* �ش� OBJLIST�� ã�� ���Ͽ��ٸ� NULL ��ȯ */
	if (iter_find == m_mapObjList.end())
		return nullptr;

	return &(iter_find->second);
}

bool CObjMgr::Is_Player(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* PLAYER ObjList ���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
	auto& iter_find = m_mapObjList[L"PLAYER"].find(server_num);

	/* �ش� OBJLIST�� ã�� ���Ͽ��ٸ� NULL ��ȯ */
	if (iter_find == m_mapObjList[L"PLAYER"].end())
		return false;

	return true;
}

bool CObjMgr::Is_NPC(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* NPC ObjList ���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
	auto& iter_find = m_mapObjList[L"NPC"].find(server_num);

	/* �ش� OBJLIST�� ã�� ���Ͽ��ٸ� NULL ��ȯ */
	if (iter_find == m_mapObjList[L"NPC"].end())
		return false;

	return true;
}

bool CObjMgr::Is_Monster(int server_num)
{
	objmgr_lock ol(m_mutex);

	/* NPC ObjList ���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
	auto& iter_find = m_mapObjList[L"MONSTER"].find(server_num);

	/* �ش� OBJLIST�� ã�� ���Ͽ��ٸ� NULL ��ȯ */
	if (iter_find == m_mapObjList[L"MONSTER"].end())
		return false;

	return true;
}

bool CObjMgr::Is_Near(const CObj* me, const CObj* other)
{
	float dist = (other->m_vPos.x - me->m_vPos.x) * (other->m_vPos.x - me->m_vPos.x);
	dist += (other->m_vPos.y - me->m_vPos.y) * (other->m_vPos.y - me->m_vPos.y);
	dist += (other->m_vPos.z - me->m_vPos.z) * (other->m_vPos.z - me->m_vPos.z);

	return dist <= VIEW_LIMIT * VIEW_LIMIT;
}

bool CObjMgr::Is_Monster_Target(const CObj* me, const CObj* other)
{
	float dist = (other->m_vPos.x - me->m_vPos.x) * (other->m_vPos.x - me->m_vPos.x);
	dist += (other->m_vPos.y - me->m_vPos.y) * (other->m_vPos.y - me->m_vPos.y);
	dist += (other->m_vPos.z - me->m_vPos.z) * (other->m_vPos.z - me->m_vPos.z);

	return dist <= CHASE_RANGE * CHASE_RANGE;
}

bool CObjMgr::Is_Monster_AttackTarget(const CObj* me, const CObj* other, const int& range)
{
	float dist = (other->m_vPos.x - me->m_vPos.x) * (other->m_vPos.x - me->m_vPos.x);
	dist += (other->m_vPos.y - me->m_vPos.y) * (other->m_vPos.y - me->m_vPos.y);
	dist += (other->m_vPos.z - me->m_vPos.z) * (other->m_vPos.z - me->m_vPos.z);

	return dist <= range * range;
}

HRESULT CObjMgr::Add_GameObject(wstring wstrObjTag, CObj* pObj, int server_num)
{
	if (pObj != nullptr)
	{
		/* map���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
		auto& iter_find = m_mapObjList.find(wstrObjTag);

		/* �ش� OBJLIST�� ���ٸ�, �ӽ� OBJLIST ������ �� ������Ʈ ���� */
		if (iter_find == m_mapObjList.end())
			m_mapObjList[wstrObjTag] = OBJLIST();
		
		m_mapObjList[wstrObjTag][server_num] = pObj;
	}

	return S_OK;
}

HRESULT CObjMgr::Delete_GameObject(wstring wstrObjTag, CObj* pObj)
{
	if (pObj != nullptr)
	{
		/* map���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
		auto& iter_find = m_mapObjList.find(wstrObjTag);

		/* �ش� OBJLIST�� ���ٸ�, �ӽ� OBJLIST ������ �� ������Ʈ ���� */
		if (iter_find == m_mapObjList.end())
			return E_FAIL;

		/* server number ���� �ִ��� Ž�� */
		auto& user = iter_find->second.find(pObj->m_sNum);
		
		/* Ž�� ���� */
		if (user == iter_find->second.end())
			return E_FAIL;

		/* 3.Ž�� ���� -> ���� */
		if (user->second)
		{
			iter_find->second.erase(pObj->m_sNum);
			return S_OK;
		}
	}

	return S_OK;
}

HRESULT CObjMgr::Delete_OBJLIST(wstring wstrObjTag)
{
	m_mapObjList[wstrObjTag].clear();
	return S_OK;
}

void CObjMgr::Release()
{
	for (auto& o_list : m_mapObjList)
	{	
		o_list.second.clear();
	}
}
