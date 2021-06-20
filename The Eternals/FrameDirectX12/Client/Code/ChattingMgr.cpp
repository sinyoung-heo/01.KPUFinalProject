#include "stdafx.h"
#include "ChattingMgr.h"

IMPLEMENT_SINGLETON(CChattingMgr)

CChattingMgr::CChattingMgr()
{
}

void CChattingMgr::Free()
{
	m_pChattingCanvas = nullptr;
	m_pChattingInput  = nullptr;
	m_pChattingCursor = nullptr;
}
