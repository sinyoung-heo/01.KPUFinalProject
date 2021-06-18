#include "stdafx.h"
#include "PartySystemMgr.h"

IMPLEMENT_SINGLETON(CPartySystemMgr)

CPartySystemMgr::CPartySystemMgr()
{
}

void CPartySystemMgr::Free()
{
	m_pSelectPlayer               = nullptr;
	m_pPartySuggestCanvas         = nullptr;
	m_pPartySuggestResponseCanvas = nullptr;
	m_pPartyLeaveCanvas           = nullptr;
	m_pPartySystemMessageCanvas   = nullptr;
}
