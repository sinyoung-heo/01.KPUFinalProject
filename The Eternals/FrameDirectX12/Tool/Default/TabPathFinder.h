#pragma once


// CTabPathFinder 대화 상자

class CTabPathFinder : public CDialogEx
{
	DECLARE_DYNAMIC(CTabPathFinder)

public:
	CTabPathFinder(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabPathFinder();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabPathFinder };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
