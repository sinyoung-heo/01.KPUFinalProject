﻿
// Tool.h: Tool 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


// CToolApp:
// 이 클래스의 구현에 대해서는 Tool.cpp을(를) 참조하세요.
//

class CMainFrame;
class CToolView;

class CToolApp : public CWinApp
{
public:
	CToolApp() noexcept;


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);

public:
	CMainFrame* m_pMainFrame	= nullptr;
	CToolView*	m_pMainView		= nullptr;
	_bool		m_bIsReady		= false;


	_tchar	m_szFPS[32] = L"";
	_int	m_iFPS		= 0;
	_float	m_fFPSTime	= 1.0f;
};

extern CToolApp theApp;
