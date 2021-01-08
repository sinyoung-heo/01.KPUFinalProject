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
		/* 1.map에 같은 server number값이 있는지 탐색 */
		auto& user = m_mapObj.find(num);

		/* 2.사용하지 않는 server number라면 새로운 유저에게 부여(탐색 실패) */
		if (user == m_mapObj.end())
		{
			m_mapObj[num] = pObj;
		}
	}
	return S_OK;
}

HRESULT CObjMgr::Delete_GameObject(int server_num)
{
	/* 1.server number값이 있는지 탐색 */
	auto& user = m_mapObj.find(server_num);

	/* 2.탐색 실패 */
	if (user == m_mapObj.end())
		return E_FAIL;

	/* 3.탐색 성공 -> 삭제 */
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
	/* 1.server number값이 있는지 탐색 */
	auto& user = m_mapObj.find(server_num);

	/* 2.탐색 실패 */
	if (user == m_mapObj.end())
		return nullptr;
	
	/* 3.탐색 성공 -> 객체 찾기 성공 */
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
