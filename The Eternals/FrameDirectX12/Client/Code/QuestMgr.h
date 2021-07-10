#pragma once
#include "Include.h"
#include "Base.h"
#include "QuestRequestCanvas.h"
#include "QuestButtonClose.h"
#include "QuestButtonAccept.h"
#include "QuestComplete.h"

class CQuestMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CQuestMgr)

private:
	explicit CQuestMgr();
	virtual ~CQuestMgr() = default;
public:
	CQuestRequestCanvas* Get_QuestRequestCanvas()		{ return m_pCanvas; }
	CQuestButtonClose*	 Get_QuestButtonClose()			{ return m_pButtonClose; }
	CQuestButtonAccept*	 Get_QuestButtonACcept()		{ return m_pButtonAccept; }
	void Set_CanvasClass(CQuestRequestCanvas* pUI)		{ m_pCanvas = pUI; }
	void Set_ButtonCloseClass(CQuestButtonClose* pUI)	{ m_pButtonClose = pUI; }
	void Set_ButtonAccpetClass(CQuestButtonAccept* pUI) { m_pButtonAccept = pUI; }
	void Set_SubQuestMiniCanvas(CGameUIRoot* pUI)		{ m_pSubQuestMiniCanvas = pUI; }
	void Set_MainQuestMiniCanvas(CGameUIRoot* pUI)		{ m_pMainQuestMiniCanvas = pUI; }

private:
	CQuestRequestCanvas*	m_pCanvas       = nullptr;
	CQuestButtonClose*		m_pButtonClose  = nullptr;
	CQuestButtonAccept*		m_pButtonAccept = nullptr;

	CGameUIRoot*			m_pSubQuestMiniCanvas  = nullptr;
	CGameUIRoot*			m_pMainQuestMiniCanvas = nullptr;
private:
	virtual void Free();
};

