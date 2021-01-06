#pragma once


// CTabCamera 대화 상자

class CTabCamera : public CDialogEx
{
	DECLARE_DYNAMIC(CTabCamera)

public:
	CTabCamera(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabCamera();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabCamera };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
