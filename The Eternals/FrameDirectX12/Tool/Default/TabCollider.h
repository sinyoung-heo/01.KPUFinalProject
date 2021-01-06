#pragma once


// CTabCollider 대화 상자

class CTabCollider : public CDialogEx
{
	DECLARE_DYNAMIC(CTabCollider)

public:
	CTabCollider(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabCollider();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabCollider };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
