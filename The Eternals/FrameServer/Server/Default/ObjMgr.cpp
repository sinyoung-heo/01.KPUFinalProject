#include "stdafx.h"
#include "ObjMgr.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr(void)
{
}

CObjMgr::~CObjMgr(void)
{
}

HRESULT CObjMgr::Add_GameObject(int num, CObj* pObj)
{
	if (pObj != nullptr)
	{
		/* 1.map�� ���� server number���� �ִ��� Ž�� */
		auto& user = m_mapObj.find(num);

		/* 2.������� �ʴ� server number��� ���ο� �������� �ο�(Ž�� ����) */
		if (user == m_mapObj.end())
		{
			m_mapObj[num] = pObj;
		}
	}
	return S_OK;
}

HRESULT CObjMgr::Delete_GameObject(int server_num)
{
	/* 1.server number���� �ִ��� Ž�� */
	auto& user = m_mapObj.find(server_num);

	/* 2.Ž�� ���� */
	if (user == m_mapObj.end())
		return E_FAIL;

	/* 3.Ž�� ���� -> ���� */
	if (user->second)
	{
		delete user->second;
		user->second = nullptr;

		m_mapObj.erase(server_num);

		return S_OK;
	}	

	return E_FAIL;
}

CObj* CObjMgr::Get_GameObject(int server_num)
{
	/* 1.server number���� �ִ��� Ž�� */
	auto& user = m_mapObj.find(server_num);

	/* 2.Ž�� ���� */
	if (user == m_mapObj.end())
		return nullptr;
	
	/* 3.Ž�� ���� -> ��ü ã�� ���� */
	return user->second;
}

void CObjMgr::Release()
{
	for (auto& MyPair : m_mapObj)
	{
		Safe_Release(MyPair.second);
	}

	m_mapObj.clear();
}
