#pragma once


// CTab2DUI 대화 상자

class CTab2DUI : public CDialogEx
{
	DECLARE_DYNAMIC(CTab2DUI)

public:
	CTab2DUI(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTab2DUI();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabUI_2DUI };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
