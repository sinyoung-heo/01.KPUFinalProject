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

	HRESULT	 Ready_TerrainControl();
	HRESULT	 Ready_SkyBoxControl();

	/*__________________________________________________________________________________________________________
	[ TERRAIN ]
	____________________________________________________________________________________________________________*/
	// Function
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


	/*__________________________________________________________________________________________________________
	[ SkyBox ]
	____________________________________________________________________________________________________________*/
	// Function
	afx_msg void OnLbnSelchangeList1002_SkyBoxTexIndex();
	afx_msg void OnBnClickedCheck1004_SkyBoxRenderOnOff();

	// Control
	CEdit		m_SkyBoxEdit_PosX;
	CEdit		m_SkyBoxEdit_PosY;
	CEdit		m_SkyBoxEdit_PosZ;
	CEdit		m_SkyBoxEdit_Scale;
	CListBox	m_SkyBoxListBox_TexIndex;
	CButton		m_SkyBoxCheckBox_RenderOnOff;

	// Value
	float		m_fSkyBox_PosX	= 0.0f;
	float		m_fSkyBox_PosY	= 0.0f;
	float		m_fSkyBox_PosZ	= 0.0f;
	float		m_fSkyBox_Scale = 512.0f;

	afx_msg void OnEnChangeEdit1001_SkyBoxPosX();
	afx_msg void OnEnChangeEdit1002_SkyBoxPosY();
	afx_msg void OnEnChangeEdit1003_SkyBoxPosZ();
	afx_msg void OnEnChangeEdit1004__SkyBoxScale();
};
