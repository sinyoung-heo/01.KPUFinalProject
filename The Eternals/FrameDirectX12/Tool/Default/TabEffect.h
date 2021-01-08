#pragma once


// CTabEffect 대화 상자

class CTabEffect : public CDialogEx
{
	DECLARE_DYNAMIC(CTabEffect)

public:
	CTabEffect(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabEffect();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabEffect };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
