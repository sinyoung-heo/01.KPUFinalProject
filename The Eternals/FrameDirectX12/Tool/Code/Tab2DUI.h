#pragma once


// CTab2DUI 대화 상자
namespace Engine
{
	class CComponentMgr;
	class CObjectMgr;
	class CManagement;
	class CDescriptorHeapMgr;
}

class CToolUIChild;

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
	Engine::CManagement*		m_pManagement		 = nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		 = nullptr;
	Engine::CComponentMgr*		m_pComponentMgr		 = nullptr;
	Engine::CDescriptorHeapMgr* m_pDescriptorHeapMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ RootUI Method ]
	____________________________________________________________________________________________________________*/
	afx_msg void OnLbnSelchangeList2102_DataFileName();
	afx_msg void OnBnClickedCheck2100_RootUIIsAnimation();
	afx_msg void OnLbnSelchangeList2100_RootUITagSelect();
	afx_msg void OnBnClickedRadio2100_RootCreateMode();
	afx_msg void OnBnClickedRadio2101_RootModifyMode();
	afx_msg void OnBnClickedButton2104_RootUISAVE();
	afx_msg void OnBnClickedButton2105_RootUILOAD();
	afx_msg void OnBnClickedButton2101_DeleteRootUI();
	afx_msg void OnEnChangeEdit2103_RootUIPosX();
	afx_msg void OnEnChangeEdit2104_RootUIPosY();
	afx_msg void OnEnChangeEdit2106_RootUIScaleX();
	afx_msg void OnEnChangeEdit2107_RootUIScaleY();
	afx_msg void OnEnChangeEdit2105_RootUIUIDepth();
	afx_msg void OnEnChangeEdit2121_RootUIFrameSpeed();
	afx_msg void OnEnChangeEdit2111_RootUIRectPosOffsetX();
	afx_msg void OnEnChangeEdit2112_RootUIRectPosOffsetY();
	afx_msg void OnEnChangeEdit2113_RootUIRectScaleOffsetX();
	afx_msg void OnEnChangeEdit2114_RootUIRectScaleOffsetY();

	/*__________________________________________________________________________________________________________
	[ RootUI Control ]
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
	[ RootUI Value ]
	____________________________________________________________________________________________________________*/
	_bool	m_bIsRootCreateMode	   = false;
	_bool	m_bIsRootModifyMode	   = false;
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
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*__________________________________________________________________________________________________________
	[ ChildUI Method ]
	____________________________________________________________________________________________________________*/
	afx_msg void OnBnClickedRadio2103_ChildUICreateMode();
	afx_msg void OnBnClickedRadio2104_ChildUIModifyMode();
	afx_msg void OnBnClickedCheck2101_CheckChildIsSpriteAnimation();
	afx_msg void OnBnClickedButton2103_ChildUICreate();
	afx_msg void OnBnClickedButton2102_ChildUIDelete();
	afx_msg void OnLbnSelchangeList2101_SelectChildUI();
	afx_msg void OnEnChangeEdit2116_ChildUIPosOffsetX();
	afx_msg void OnEnChangeEdit2117_ChildUIPosOffsetY();
	afx_msg void OnEnChangeEdit2119_ChildUIScaleOffsetX();
	afx_msg void OnEnChangeEdit2120_ChildUIScaleOffsetY();
	afx_msg void OnEnChangeEdit2118_ChildUIUIDepth();
	afx_msg void OnEnChangeEdit2126_ChildUIFrameSpeed();
	afx_msg void OnEnChangeEdit2122_ChildUIRectPosOffsetX();
	afx_msg void OnEnChangeEdit2123_ChildUIRectPosOffsetY();
	afx_msg void OnEnChangeEdit2124_ChildUIRectScaleOffsetX();
	afx_msg void OnEnChangeEdit2125_ChildUIRectScaleOffsetY();

	/*__________________________________________________________________________________________________________
	[ ChildUI Control ]
	____________________________________________________________________________________________________________*/
	CButton		m_RadioChildUICreateMode;
	CButton		m_RadioChildUIModifyMode;
	CListBox	m_ListBoxChildUI;
	CButton		m_ButtonCreateChild;
	CButton		m_ButtonDeleteChild;
	CEdit		m_EditChildRootObjectTag;
	CEdit		m_EditChildObjectTag;
	CEdit		m_EditChildPosOffsetX;
	CEdit		m_EditChildPosOffsetY;
	CEdit		m_EditChildScaleOffsetX;
	CEdit		m_EditChildScaleOffsetY;
	CEdit		m_EditChildUIDepth;
	CButton		m_CheckChildIsAnimation;
	CEdit		m_EditChildFrameSpeed;
	CEdit		m_EditChildRectPosOffsetX;
	CEdit		m_EditChildRectPosOffsetY;
	CEdit		m_EditChildRectScaleOffsetX;
	CEdit		m_EditChildRectScaleOffsetY;
	CButton		m_ButtonChildSAVE;
	CButton		m_ButtonChildLOAD;

	/*__________________________________________________________________________________________________________
	[ ChildUI Value ]
	____________________________________________________________________________________________________________*/
	_bool	m_bIsChildCreateMode     = false;
	_bool	m_bIsChildModifyMode     = false;
	CString m_wstrChildRootObjectTag = L"";
	CString m_wstrChildObjectTag     = L"";
	float	m_fChildPosOffsetX       = 0.0f;
	float	m_fChildPosOffsetY       = 0.0f;
	float	m_fChildScaleOffsetX     = 1.0f;
	float	m_fChildScaleOffsetY     = 1.0f;
	_bool	m_bIsChildAnimation      = false;
	long	m_ChildUIDepth           = 900;
	float	m_fChildFrameSpeed       = 0.0f;
	float	m_fChildRectPosOffsetX   = 0.0f;
	float	m_fChildRectPosOffsetY   = 0.0f;
	float	m_fChildRectScaleOffsetX = 1.0f;
	float	m_fChildRectScaleOffsetY = 1.0f;

	_int			m_iChildUISelectIdx = 0;
	CToolUIChild*	m_pChildUISelected  = nullptr;
};
