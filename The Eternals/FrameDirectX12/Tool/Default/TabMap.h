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
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	Engine::CManagement*		m_pManagement		= nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		= nullptr;
	Engine::CComponentMgr*		m_pComponentMgr		= nullptr;
	HRESULT	Ready_TerrainControl();
	HRESULT	Ready_SkyBoxControl();
	HRESULT Ready_EditControl();
	HRESULT Ready_StaticMeshControl();

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
	[ SKYBOX ]
	____________________________________________________________________________________________________________*/
	// Function
	afx_msg void OnLbnSelchangeList1002_SkyBoxTexIndex();
	afx_msg void OnBnClickedCheck1004_SkyBoxRenderOnOff();
	afx_msg void OnEnChangeEdit1001_SkyBoxPosX();
	afx_msg void OnEnChangeEdit1002_SkyBoxPosY();
	afx_msg void OnEnChangeEdit1003_SkyBoxPosZ();
	afx_msg void OnEnChangeEdit1004__SkyBoxScale();

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

	/*__________________________________________________________________________________________________________
	[ EDIT CHECK CONTROL ]
	____________________________________________________________________________________________________________*/
	// Function
	afx_msg void OnBnClickedCheck1005_EditStaticMesh();
	afx_msg void OnBnClickedCheck1006_EditLightingInfo();
	afx_msg void OnBnClickedCheck1007_EditNavigationMesh();

	// Control
	CButton m_EditCheck_StaticMesh;
	CButton m_EditCheck_LightingInfo;
	CButton m_EditCheck_NavigationMesh;

	/*__________________________________________________________________________________________________________
	[ STATIC MESH ]
	____________________________________________________________________________________________________________*/
	// Function
	afx_msg void OnBnClickedRadio1005_StaticMeshCreateMode();
	afx_msg void OnBnClickedRadio1006_StaticMeshModifyeMode();
	afx_msg void OnNMClickTree1001_TreeMeshTag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEdit1005_StaticMeshScaleX();
	afx_msg void OnEnChangeEdit1006_StaticMeshScaleY();
	afx_msg void OnEnChangeEdit1007_StaticMeshScaleZ();
	afx_msg void OnEnChangeEdit1008_StaticMeshAngleX();
	afx_msg void OnEnChangeEdit1009_StaticMeshAngleY();
	afx_msg void OnEnChangeEdit1010_StaticMeshAngleZ();
	afx_msg void OnEnChangeEdit1011_StaticMeshPosX();
	afx_msg void OnEnChangeEdit1012_StaticMeshPosY();
	afx_msg void OnEnChangeEdit1013_StaticMeshPosZ();
	afx_msg void OnBnClickedCheck1002_StaticMeshRenderShadow();
	afx_msg void OnBnClickedCheck1003_StaticMeshIsCollision();

	// Control
	CButton			m_StaticMeshRadio_CreateMode;
	CButton			m_StaticMeshRadio_ModifyMode;
	CTreeCtrl		m_StaticMeshTree_ResourceTree;
	CListBox		m_StaticMeshListBox_ObjectList;
	CButton			m_StaticMeshButton_DeleteObject;
	CButton			m_StaticMeshButton_AllDeleteObject;

	CEdit			m_StaticMeshEdit_ScaleX;
	CEdit			m_StaticMeshEdit_ScaleY;
	CEdit			m_StaticMeshEdit_ScaleZ;
	CEdit			m_StaticMeshEdit_AngleX;
	CEdit			m_StaticMeshEdit_AngleY;
	CEdit			m_StaticMeshEdit_AngleZ;
	CEdit			m_StaticMeshEdit_PosX;
	CEdit			m_StaticMeshEdit_PosY;
	CEdit			m_StaticMeshEdit_PosZ;

	CButton			m_StaticMeshCheck_IsRenderShadow;
	CButton			m_StaticMeshCheck_IsCollision;

	CButton			m_StaticMeshButton_Save;
	CButton			m_StaticMeshButton_Load;

	// Value
	_bool	m_bIsCreateMode = true;
	_bool	m_bIsModifyMode = false;

	wstring	m_wstrTreeMeshTag	= L"";
	float m_fStaticMeshScaleX	= 0.05f;
	float m_fStaticMeshScaleY	= 0.05f;
	float m_fStaticMeshScaleZ	= 0.05f;
	float m_fStaticMeshAngleX	= 0.0f;
	float m_fStaticMeshAngleY	= 0.0f;
	float m_fStaticMeshAngleZ	= 0.0f;
	float m_fStaticMeshPosX		= 0.0f;
	float m_fStaticMeshPosY		= 0.0f;
	float m_fStaticMeshPosZ		= 0.0f;
	_bool m_bIsRenderShadow		= false;
	_bool m_bIsCollision		= false;
};
