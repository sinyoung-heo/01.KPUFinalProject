#pragma once


// CTabTexSpriteUV 대화 상자

class CTabTexSpriteUV : public CDialogEx
{
	DECLARE_DYNAMIC(CTabTexSpriteUV)

public:
	CTabTexSpriteUV(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabTexSpriteUV();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabUI_TexSpriteUV };
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
