#include "stdafx.h"
#include "ObjMgr.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr(void)
{
}

CObjMgr::~CObjMgr(void)
{
}

CObj* CObjMgr::Get_GameObject(wstring wstrObjTag, int server_num)
{
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
	/* map���� ã���� �ϴ� OBJLIST�� key ���� ���� ã�� */
	auto& iter_find = m_mapObjList.find(wstrObjTag);

	/* �ش� OBJLIST�� ã�� ���Ͽ��ٸ� NULL ��ȯ */
	if (iter_find == m_mapObjList.end())
		return nullptr;

	return &(iter_find->second);
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

//HRESULT CObjMgr::Add_GameObject(int num, CObj* pObj)
//{
//	if (pObj != nullptr)
//	{
//		/* 1.map�� ���� server number���� �ִ��� Ž�� */
//		auto& user = m_mapObj.find(num);
//
//		/* 2.������� �ʴ� server number��� ���ο� �������� �ο�(Ž�� ����) */
//		if (user == m_mapObj.end())
//		{
//			m_mapObj[num] = pObj;
//		}
//	}
//	return S_OK;
//}
//
//HRESULT CObjMgr::Delete_GameObject(int server_num)
//{
//	/* 1.server number���� �ִ��� Ž�� */
//	auto& user = m_mapObj.find(server_num);
//
//	/* 2.Ž�� ���� */
//	if (user == m_mapObj.end())
//		return E_FAIL;
//
//	/* 3.Ž�� ���� -> ���� */
//	if (user->second)
//	{
//		m_mapObj.erase(server_num);
//
//		return S_OK;
//	}	
//
//	return E_FAIL;
//}
//
//CObj* CObjMgr::Get_GameObject(int server_num)
//{
//	/* 1.server number���� �ִ��� Ž�� */
//	auto& user = m_mapObj.find(server_num);
//
//	/* 2.Ž�� ���� */
//	if (user == m_mapObj.end())
//		return nullptr;
//	
//	/* 3.Ž�� ���� -> ��ü ã�� ���� */
//	return user->second;
//}

void CObjMgr::Release()
{
	for (auto& o_list : m_mapObjList)
	{
		o_list.second.clear();
	}
}
