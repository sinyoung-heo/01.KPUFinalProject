#pragma once


// CTabMap 대화 상자
namespace Engine
{
	class CComponentMgr;
	class CObjectMgr;
	class CManagement;
}


class CTabMap : public CDialogEx
{
	DECLARE_DYNAMIC(CTabMap)

public:
	CTabMap(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabMap();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabObject };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	Engine::CManagement*		m_pManagement		= nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		= nullptr;
	Engine::CComponentMgr*		m_pComponentMgr		= nullptr;

	/*__________________________________________________________________________________________________________
	[ TERRAIN ]
	____________________________________________________________________________________________________________*/
	// Function
	HRESULT	 Ready_TerrainControl();
	afx_msg void OnBnClickedRadio1001_Terrain128();
	afx_msg void OnBnClickedRadio1002_Terrain256();
	afx_msg void OnBnClickedRadio1003_Terrain512();
	afx_msg void OnBnClickedCheck1001_TerrainRenderWireFrame();
	afx_msg void OnLbnSelchangeList1001_TerrainTexIndex();

	// Control
	CButton		m_TerrainRadio128;
	CButton		m_TerrainRadio256;
	CButton		m_TerrainRadio512;
	CButton		m_TerrainCheckBox_RenderWireFrame;
	CListBox	m_TerrainListBox_TexIndex;

	// Value

};
