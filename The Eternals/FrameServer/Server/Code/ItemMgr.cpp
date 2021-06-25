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

void CItemMgr::Release()
{
    m_mapGameItem.clear();
}
