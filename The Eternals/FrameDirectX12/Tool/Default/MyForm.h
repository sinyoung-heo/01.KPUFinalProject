#pragma once
#include "TabPathFinder.h"
#include "TabMap.h"
#include "TabEffect.h"
#include "TabUI.h"
#include "TabCollider.h"
#include "TabAnimation.h"
#include "TabCamera.h"


// CMyForm 폼 보기
#define SCROLL_POWER 75

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)

protected:
	CMyForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMyForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	// 재정의
	virtual void OnInitialUpdate();
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	CTabCtrl		m_Tab;

	CTabPathFinder	m_TabPathFinder;
	CTabMap			m_TabMap;
	CTabUI			m_TabUI;
	CTabEffect		m_TabEffect;
	CTabCollider	m_TabCollider;
	CTabAnimation	m_TabAnimation;
	CTabCamera		m_TabCamera;

	_bool m_bIsTabPathFinder	= false;
	_bool m_bIsTabMap			= false;
	_bool m_bIsTabUI			= false;
	_bool m_bIsTabEffect		= false;
	_bool m_bIsTabCollider		= false;
	_bool m_bIsTabAnimation		= false;
	_bool m_bIsTabCamera		= false;

	_long m_MaxBottom = 0;
};


