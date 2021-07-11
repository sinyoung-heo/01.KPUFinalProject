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
	CQuestRequestCanvas*	Get_QuestRequestCanvas()		{ return m_pCanvas; }
	CQuestButtonClose*		Get_QuestButtonClose()			{ return m_pButtonClose; }
	CQuestButtonAccept*		Get_QuestButtonACcept()			{ return m_pButtonAccept; }
	CGameUIRoot*			Get_SubQuestMiniCanvas()		{ return m_pSubQuestMiniCanvas; }
	CGameUIRoot*			Get_MainQuestMiniCanvas()		{ return m_pMainQuestMiniCanvas; }
	const _bool&			Get_IsAcceptQuest()				{ return m_bIsAcceptQuest; }
	const _bool&			Get_IsCompleteSubQuest()		{ return m_bIsCompleteSubQuest; }
	const _bool&			Get_IsCompleteMainQuest()		{ return m_bIsCompleteMainQuest; }
	void Set_CanvasClass(CQuestRequestCanvas* pUI)			{ m_pCanvas = pUI; }
	void Set_ButtonCloseClass(CQuestButtonClose* pUI)		{ m_pButtonClose = pUI; }
	void Set_ButtonAccpetClass(CQuestButtonAccept* pUI)		{ m_pButtonAccept = pUI; }
	void Set_SubQuestMiniCanvas(CGameUIRoot* pUI)			{ m_pSubQuestMiniCanvas = pUI; }
	void Set_MainQuestMiniCanvas(CGameUIRoot* pUI)			{ m_pMainQuestMiniCanvas = pUI; }
	void Set_IsAcceptQuest(const _bool& bIsAccept)			{ m_bIsAcceptQuest = bIsAccept; }
	void Set_IsCompleteSubQuest(const _bool& bIsComplete)	{ m_bIsCompleteSubQuest = bIsComplete; }
	void Set_IsCompleteMainQuest(const _bool& bIsComplete)	{ m_bIsCompleteMainQuest = bIsComplete; }

private:
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CQuestRequestCanvas*	m_pCanvas              = nullptr;
	CQuestButtonClose*		m_pButtonClose         = nullptr;
	CQuestButtonAccept*		m_pButtonAccept        = nullptr;
	CGameUIRoot*			m_pSubQuestMiniCanvas  = nullptr;
	CGameUIRoot*			m_pMainQuestMiniCanvas = nullptr;

	_bool m_bIsAcceptQuest       = false;	// 퀘스트 수락여부.
	_bool m_bIsCompleteSubQuest  = false;
	_bool m_bIsCompleteMainQuest = false;
private:
	virtual void Free();
};

