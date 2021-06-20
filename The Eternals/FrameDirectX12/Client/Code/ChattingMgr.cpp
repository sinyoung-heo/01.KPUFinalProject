#include "stdafx.h"
#include "ChattingMgr.h"

IMPLEMENT_SINGLETON(CChattingMgr)

CChattingMgr::CChattingMgr()
{
}

void CChattingMgr::Set_ChattingInputString(wstring wstrChatting)
{
	if (wstrChatting.length() <= m_pChattingInput->Get_MaxStringLen())
		m_pChattingInput->Set_FontText(wstrChatting);
}

void CChattingMgr::Move_CursorPos(const CHATTING_CURSOR_MOVE& eMove)
{
	_tchar temp = Get_ChattingTextBack();

	_float fOffset = 0.0f;

	if (temp >= 0x41 && temp <= 0x5A)
		fOffset = ENGLISH_OFFSET;
	else
		fOffset = HANGUL_OFFSET;

	m_pChattingCursor->Move_CursorPos(eMove, fOffset);
}

void CChattingMgr::Free()
{
	m_pChattingCanvas = nullptr;
	m_pChattingInput  = nullptr;
	m_pChattingCursor = nullptr;
}
