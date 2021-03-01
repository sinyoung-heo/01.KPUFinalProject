#pragma once


// CTab2DUI 대화 상자
namespace Engine
{
	class CComponentMgr;
	class CObjectMgr;
	class CManagement;
	class CDescriptorHeapMgr;
}

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
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	HRESULT	Ready_Tab2DUI();

public:
	afx_msg void OnLbnSelchangeList2102_DataFileName();
	afx_msg void OnBnClickedCheck2100_RootUIIsAnimation();
	afx_msg void OnLbnSelchangeList2100_RootUITagSelect();
	afx_msg void OnBnClickedRadio2100_RootCreateMode();
	afx_msg void OnBnClickedRadio2101_RootModifyMode();
	afx_msg void OnBnClickedButton2104_RootUISAVE();
	afx_msg void OnBnClickedButton2105_RootUILOAD();

	Engine::CManagement*		m_pManagement		 = nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		 = nullptr;
	Engine::CComponentMgr*		m_pComponentMgr		 = nullptr;
	Engine::CDescriptorHeapMgr* m_pDescriptorHeapMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Control ]
	____________________________________________________________________________________________________________*/
	CListBox	m_ListBoxDataFileName;
	CButton		m_RadioRootCreateMode;
	CButton		m_RadioRootModifyMode;
	CListBox	m_ListBoxRootUI;
	CEdit		m_EditRootUITag;
	CEdit		m_EditDataFileName;
	CEdit		m_EditObjectTag;
	CEdit		m_EditRootPosX;
	CEdit		m_EditRootPosY;
	CEdit		m_EditRootScaleX;
	CEdit		m_EditRootScaleY;
	CEdit		m_EditRootUIDepth;
	CButton		m_CheckRootIsAnimation;
	CEdit		m_EditRootFrameSpeed;
	CEdit		m_EditRootRectPosOffsetX;
	CEdit		m_EditRootRectPosOffsetY;
	CEdit		m_EditRootRectScaleX;
	CEdit		m_EditRootRectScaleY;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_bool m_bIsRootCreateMode	   = false;
	_bool m_bIsRootModifyMode	   = false;

	CString m_wstrRootUITag        = L"";
	CString m_wstrRootDataFileName = L"";
	CString m_wstrRootObjectTag    = L"";
	float	m_fRootPosX            = 0.0f;
	float	m_fRootPosY            = 0.0f;
	float	m_fRootScaleX          = 100.0f;
	float	m_fRootScaleY          = 100.0f;
	long	m_RootUIDepth          = 1000;
	_bool	m_bIsRootAnimation     = false;
	float	m_fRootFrameSpeed	   = 0.0f;
	float	m_fRootRectPosOffsetX  = 0.0f;
	float	m_fRootRectPosOffsetY  = 0.0f;
	float	m_fRootRectScaleX      = 100.0f;
	float	m_fRootRectScaleY      = 100.0f;
};
