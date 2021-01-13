#pragma once
#include "TabPathFinder.h"
#include "TabMap.h"
#include "TabCollider.h"
#include "TabAnimation.h"
#include "TabCamera.h"
#include "TabEffect.h"

// CMyForm 폼 보기

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

public:
	CTabCtrl		m_Tab;

	CTabPathFinder	m_TabPathFinder;
	CTabMap			m_TabMap;
	CTabCollider	m_TabCollider;
	CTabAnimation	m_TabAnimation;
	CTabCamera		m_TabCamera;
	CTabEffect		m_TabEffect;

	_bool m_bIsTabPathFinder	= false;
	_bool m_bIsTabMap			= false;
	_bool m_bIsTabCollider		= false;
	_bool m_bIsTabAnimation		= false;
	_bool m_bIsTabCamera		= false;
	_bool m_bIsTabEffect		= false;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


