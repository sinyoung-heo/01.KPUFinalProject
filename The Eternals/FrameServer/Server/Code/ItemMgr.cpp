#include "stdafx.h"
#include "ItemMgr.h"

IMPLEMENT_SINGLETON(CItemMgr)

CItemMgr::CItemMgr(void)
{
}

CItemMgr::~CItemMgr(void)
{
}

HRESULT CItemMgr::Ready_Item()
{
	if (CDBMgr::GetInstance()->Ready_GameItem(m_mapGameItem) == S_OK)
		return S_OK;

	return E_FAIL;
}

const int& CItemMgr::Find_ItemNumber(const char& type, const char& name)
{
	for (auto& i : m_mapGameItem)
	{
		if (i.second.cItemType == type && i.second.cItemName == name)
		{
			return i.first;
		}
	}

	return -1;
}

void CItemMgr::Release()
{
	m_mapGameItem.clear();
}
