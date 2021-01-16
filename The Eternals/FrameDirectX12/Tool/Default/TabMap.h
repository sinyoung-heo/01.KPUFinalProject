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
	HRESULT Ready_LightingInfoContorl();

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
	afx_msg void OnBnClickedCheck1008_StaticMeshIsMousePicking();
	afx_msg void OnLbnSelchangeList1003_StaticMeshObjectList();
	afx_msg void OnBnClickedButton1001_StasticMeshDelete();
	afx_msg void OnBnClickedButton1002_StaticMeshAllDelete();
	afx_msg void OnBnClickedButton1003_StaticMeshSAVE();
	afx_msg void OnBnClickedButton1004_StaticMeshLOAD();

	// Control
	CButton			m_StaticMeshRadio_CreateMode;
	CButton			m_StaticMeshRadio_ModifyMode;
	CTreeCtrl		m_StaticMeshTree_ResourceTree;
	CEdit			m_StaticMeshEdit_SelectMesthTag;
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
	CEdit			m_StaticMeshEdit_ObjectSize;
	CEdit			m_StaticMeshEdit_ColliderScale;
	CEdit			m_StaticMeshEdit_ColliderPosX;
	CEdit			m_StaticMeshEdit_ColliderPosY;
	CEdit			m_StaticMeshEdit_ColliderPosZ;

	CButton			m_StaticMeshCheck_IsRenderShadow;
	CButton			m_StaticMeshCheck_IsCollision;
	CButton			m_StaticMeshCheck_IsMousePicking;

	CButton			m_StaticMeshButton_Save;
	CButton			m_StaticMeshButton_Load;

	// Value
	_bool	m_bIsCreateMode			= true;
	_bool	m_bIsModifyMode			= false;

	wstring	m_wstrTreeMeshTag		= L"";
	float	m_fStaticMeshScaleX		= 0.01f;
	float	m_fStaticMeshScaleY		= 0.01f;
	float	m_fStaticMeshScaleZ		= 0.01f;
	float	m_fStaticMeshAngleX		= 0.0f;
	float	m_fStaticMeshAngleY		= 0.0f;
	float	m_fStaticMeshAngleZ		= 0.0f;
	float	m_fStaticMeshPosX		= 0.0f;
	float	m_fStaticMeshPosY		= 0.0f;
	float	m_fStaticMeshPosZ		= 0.0f;
	int		m_iStaticMeshObjectSize = 0;
	_float	m_iObjectSize			= 0;

	_bool	m_bIsRenderShadow			= false;
	_bool	m_bIsCollision				= false;
	_bool	m_bIsMousePicking			= true;
	_int	m_iStaticMeshSelectIdx		= -1;
	float	m_fStaticMeshColliderScale	= 0.0f;
	float	m_fStaticMeshColliderPosX	= 0.0f;
	float	m_fStaticMeshColliderPosY	= 0.0f;
	float	m_fStaticMeshColliderPosZ	= 0.0f;


	/*__________________________________________________________________________________________________________
	[ LIGTING ]
	____________________________________________________________________________________________________________*/
	// Function
	afx_msg void OnEnChangeEdit1014_LightInfo_DL_DiffuseR();
	afx_msg void OnEnChangeEdit1015_LightInfo_DL_DiffuseG();
	afx_msg void OnEnChangeEdit1016_LightInfo_DL_DiffuseB();
	afx_msg void OnEnChangeEdit1017_LightInfo_DL_DiffuseA();
	afx_msg void OnEnChangeEdit1018_LightInfo_DL_SpecularR();
	afx_msg void OnEnChangeEdit1019_LightInfo_DL_SpecularG();
	afx_msg void OnEnChangeEdit1020_LightInfo_DL_SpecularB();
	afx_msg void OnEnChangeEdit1021_LightInfo_DL_SpecularA();
	afx_msg void OnEnChangeEdit1022_LightInfo_DL_AmbientR();
	afx_msg void OnEnChangeEdit1023_LightInfo_DL_AmbientG();
	afx_msg void OnEnChangeEdit1024_LightInfo_DL_AmbientB();
	afx_msg void OnEnChangeEdit1025_LightInfo_DL_AmbientA();
	afx_msg void OnEnChangeEdit1026_LightInfo_DL_DirX();
	afx_msg void OnEnChangeEdit1027_LightInfo_DL_DirY();
	afx_msg void OnEnChangeEdit1028_LightInfo_DL_DirZ();
	afx_msg void OnEnChangeEdit1029_LightInfo_DL_DirW();

	afx_msg void OnBnClickedRadio1008_LightInfo_PL_CreateMode();
	afx_msg void OnBnClickedRadio1009_LightInfo_PL_ModifyMode();
	afx_msg void OnEnChangeEdit1030_LightInfo_PL_DiffuseR();
	afx_msg void OnEnChangeEdit1031_LightInfo_PL_DiffuseG();
	afx_msg void OnEnChangeEdit1032_LightInfo_PL_DiffuseB();
	afx_msg void OnEnChangeEdit1033_LightInfo_PL_DiffuseA();
	afx_msg void OnEnChangeEdit1034_LightInfo_PL_SpecularR();
	afx_msg void OnEnChangeEdit1035_LightInfo_PL_SpecularG();
	afx_msg void OnEnChangeEdit1036_LightInfo_PL_SpecularB();
	afx_msg void OnEnChangeEdit1037_LightInfo_PL_SpecularA();
	afx_msg void OnEnChangeEdit1038_LightInfo_PL_AmbientR();
	afx_msg void OnEnChangeEdit1039_LightInfo_PL_AmbientG();
	afx_msg void OnEnChangeEdit1040_LightInfo_PL_AmbientB();
	afx_msg void OnEnChangeEdit1041_LightInfo_PL_AmbientA();
	afx_msg void OnEnChangeEdit1042_LightInfo_PL_PosX();
	afx_msg void OnEnChangeEdit1043_LightInfo_PL_PosY();
	afx_msg void OnEnChangeEdit1044_LightInfo_PL_PosZ();
	afx_msg void OnEnChangeEdit1045_LightInfo_PL_PosW();
	afx_msg void OnEnChangeEdit1046_LightInfo_PL_Range();
	afx_msg void OnLbnSelchangeList1004_LightInfo_PL_List();
	afx_msg void OnBnClickedButton1007_LightInfo_PL_DELETE();
	afx_msg void OnBnClickedButton1008_LightInfo_PL_ALLDELETE();
	afx_msg void OnBnClickedButton1009__LightInfo_PL_SAVE();
	afx_msg void OnBnClickedButton1010__LightInfo_PL_LOAD();

	afx_msg void OnEnChangeEdit1050_LightInfo_SL_AtX();
	afx_msg void OnEnChangeEdit1051_LightInfo_SL_AtY();
	afx_msg void OnEnChangeEdit1052_LightInfo_SL_AtZ();
	afx_msg void OnEnChangeEdit1056_LightInfo_SL_Height();
	afx_msg void OnEnChangeEdit1053_LightInfo_SL_FovY();
	afx_msg void OnEnChangeEdit1054_LightInfo_SL_Near();
	afx_msg void OnEnChangeEdit1055_LightInfo_SL_Far();
	afx_msg void OnBnClickedButton1011__LightInfo_SL_SAVE();
	afx_msg void OnBnClickedButton1012_LightInfo_SL_LOAD();

	// Contorl
	CEdit		m_LightInfoEdit_DL_DiffuseR;
	CEdit		m_LightInfoEdit_DL_DiffuseG;
	CEdit		m_LightInfoEdit_DL_DiffuseB;
	CEdit		m_LightInfoEdit_DL_DiffuseA;
	CEdit		m_LightInfoEdit_DL_SpecularR;
	CEdit		m_LightInfoEdit_DL_SpecularG;
	CEdit		m_LightInfoEdit_DL_SpecularB;
	CEdit		m_LightInfoEdit_DL_SpecularA;
	CEdit		m_LightInfoEdit_DL_AmbientR;
	CEdit		m_LightInfoEdit_DL_AmbientG;
	CEdit		m_LightInfoEdit_DL_AmbientB;
	CEdit		m_LightInfoEdit_DL_AmbientA;
	CEdit		m_LightInfoEdit_DL_DirectionX;
	CEdit		m_LightInfoEdit_DL_DirectionY;
	CEdit		m_LightInfoEdit_DL_DirectionZ;
	CEdit		m_LightInfoEdit_DL_DirectionW;

	CEdit		m_LightInfoEdit_SL_EyeX;
	CEdit		m_LightInfoEdit_SL_EyeY;
	CEdit		m_LightInfoEdit_SL_EyeZ;
	CEdit		m_LightInfoEdit_SL_AtX;
	CEdit		m_LightInfoEdit_SL_AtY;
	CEdit		m_LightInfoEdit_SL_AtZ;
	CEdit		m_LightInfoEdit_SL_Height;
	CEdit		m_LightInfoEdit_SL_FovY;
	CEdit		m_LightInfoEdit_SL_Near;
	CEdit		m_LightInfoEdit_SL_Far;
	CButton		m_LightInfoButton_SL_SAVE;
	CButton		m_LightInfoButton_SL_LOAD;


	CEdit		m_LightInfoEdit_PL_DiffuseR;
	CEdit		m_LightInfoEdit_PL_DiffuseG;
	CEdit		m_LightInfoEdit_PL_DiffuseB;
	CEdit		m_LightInfoEdit_PL_DiffuseA;
	CEdit		m_LightInfoEdit_PL_SpecularR;
	CEdit		m_LightInfoEdit_PL_SpecularG;
	CEdit		m_LightInfoEdit_PL_SpecularB;
	CEdit		m_LightInfoEdit_PL_SpecularA;
	CEdit		m_LightInfoEdit_PL_AnbientR;
	CEdit		m_LightInfoEdit_PL_AnbientG;
	CEdit		m_LightInfoEdit_PL_AnbientB;
	CEdit		m_LightInfoEdit_PL_AnbientA;
	CEdit		m_LightInfoEdit_PL_PosX;
	CEdit		m_LightInfoEdit_PL_PosY;
	CEdit		m_LightInfoEdit_PL_PosZ;
	CEdit		m_LightInfoEdit_PL_PosW;
	CEdit		m_LightInfoEdit_PL_Range;
	CButton		m_LightInfoRadio_PL_CreateMode;
	CButton		m_LightInfoRadio_PL_ModifyMode;
	CListBox	m_LightInfoListBox_PL_List;
	CButton		m_LightInfoButton_PL_DELETE;
	CButton		m_LightInfoButton_PL_ALLDELETE;
	CButton		m_LightInfoButton_PL_SAVE;
	CButton		m_LightInfoButton_PL_LOAD;

	// Value
	_bool	m_bIsLightingCreateMode = true;
	_bool	m_bIsLightingModifyMode = false;

	float m_fLightInfo_DL_DiffuseR	= 1.0f;
	float m_fLightInfo_DL_DiffuseG	= 1.0f;
	float m_fLightInfo_DL_DiffuseB	= 1.0f;
	float m_fLightInfo_DL_DiffuseA	= 1.0f;
	float m_fLightInfo_DL_SpecularR = 0.5f;
	float m_fLightInfo_DL_SpecularG = 0.5f;
	float m_fLightInfo_DL_SpecularB = 0.5f;
	float m_fLightInfo_DL_SpecularA = 1.0f;
	float m_fLightInfo_DL_AmbientR	= 0.5f;
	float m_fLightInfo_DL_AmbientG	= 0.5f;
	float m_fLightInfo_DL_AmbientB	= 0.5f;
	float m_fLightInfo_DL_AmbientA	= 1.0f;
	float m_fLightInfo_DL_DirX		= 1.0f;
	float m_fLightInfo_DL_DirY		= -1.0f;
	float m_fLightInfo_DL_DirZ		= 1.0f;
	float m_fLightInfo_DL_DirW		= 0.0f;

	float m_fLightInfo_PL_DiffuseR	= 1.0f;
	float m_fLightInfo_PL_DiffuseG	= 1.0f;
	float m_fLightInfo_PL_DiffuseB	= 1.0f;
	float m_fLightInfo_PL_DiffuseA	= 1.0f;
	float m_fLightInfo_PL_SpecularR	= 0.5f;
	float m_fLightInfo_PL_SpecularG	= 0.5f;
	float m_fLightInfo_PL_SpecularB	= 0.5f;
	float m_fLightInfo_PL_SpecularA	= 1.0f;
	float m_fLightInfo_PL_AmbientR	= 0.5f;
	float m_fLightInfo_PL_AmbientG	= 0.5f;
	float m_fLightInfo_PL_AmbientB	= 0.5f;
	float m_fLightInfo_PL_AmbientA	= 1.0f;
	float m_fLightInfo_PL_PosX		= 0.0f;
	float m_fLightInfo_PL_PosY		= 0.0f;
	float m_fLightInfo_PL_PosZ		= 0.0f;
	float m_fLightInfo_PL_PosW		= 1.0f;
	float m_fLightInfo_PL_Range		= 10.0f;
	_int m_iSelectPLIdx				= -1;

	float m_fLightInfo_SL_EyeX		= 0.0f;
	float m_fLightInfo_SL_EyeY		= 0.0f;
	float m_fLightInfo_SL_EyeZ		= 0.0f;
	float m_fLightInfo_SL_AtX		= 1.0f;
	float m_fLightInfo_SL_AtY		= 1.0f;
	float m_fLightInfo_SL_AtZ		= 1.0f;
	float m_fLightInfo_SL_Height	= 0.0f;
	float m_fLightInfo_SL_FovY		= 30.0f;
	float m_fLightInfo_SL_Near		= 1.0f;
	float m_fLightInfo_SL_Far		= 10'000.0f;
};
