#pragma once


// CTab3DUI 대화 상자

class CTab3DUI : public CDialogEx
{
	DECLARE_DYNAMIC(CTab3DUI)

public:
	CTab3DUI(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTab3DUI();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabUI_3DUI};
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

public:


};
