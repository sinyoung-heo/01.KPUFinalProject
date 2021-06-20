#pragma once
#include "Include.h"
#include "Base.h"
#include "ChattingCanvas.h"
#include "ChattingInput.h"
#include "ChattingCursor.h"

class CChattingMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CChattingMgr)

private:
	explicit CChattingMgr();
	virtual ~CChattingMgr() = default;
public:
	CChattingCanvas*	Get_ChattingCanvasClass()	{ return m_pChattingCanvas; }
	CChattingInput*		Get_ChattingInputClass()	{ return m_pChattingInput; }
	CChattingCursor*	Get_ChattingCursorClass()	{ return m_pChattingCursor; }
	void Set_ChattingCanvasClass(CChattingCanvas* pUI)	{ m_pChattingCanvas = pUI; }
	void Set_ChattingInputClass(CChattingInput* pUI)	{ m_pChattingInput = pUI; }
	void Set_ChattingCursorClass(CChattingCursor* pUI)	{ m_pChattingCursor = pUI; }

private:
	CChattingCanvas*	m_pChattingCanvas = nullptr;
	CChattingInput*		m_pChattingInput  = nullptr;
	CChattingCursor*	m_pChattingCursor = nullptr;
private:
	virtual void Free();
};

