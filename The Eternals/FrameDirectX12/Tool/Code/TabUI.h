#pragma once
#include "TabTexSpriteUV.h"
#include "Tab2DUI.h"
#include "Tab3DUI.h"

// CTabUI 대화 상자

class CTabUI : public CDialogEx
{
	DECLARE_DYNAMIC(CTabUI)

public:
	CTabUI(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabUI();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabUI };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);

public:
	Engine::CManagement*		m_pManagement		 = nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		 = nullptr;
	Engine::CComponentMgr*		m_pComponentMgr		 = nullptr;
	Engine::CDescriptorHeapMgr* m_pDescriptorHeapMgr = nullptr;

	CTabCtrl m_TabUI;

	CTabTexSpriteUV m_TabTexSpriteUV;
	CTab2DUI		m_Tab2DUI;
	CTab3DUI		m_Tab3DUI;

	_bool			m_bIsTabTexSpriteUV = false;
	_bool			m_bIsTab2DUI        = false;
	_bool			m_bIsTab3DUI        = false;
};
