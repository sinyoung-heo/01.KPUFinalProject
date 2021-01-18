// TabObject.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabMap.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "ToolSceneStage.h"
#include "ToolTerrain.h"
#include "ToolSkyBox.h"
#include "ToolStaticMesh.h"
#include "GraphicDevice.h"
#include "LightMgr.h"
#include "Light.h"

// CTabMap 대화 상자

IMPLEMENT_DYNAMIC(CTabMap, CDialogEx)

CTabMap::CTabMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabObject, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
{

}

CTabMap::~CTabMap()
{
}

void CTabMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1001, m_TerrainRadio128);
	DDX_Control(pDX, IDC_RADIO1002, m_TerrainRadio256);
	DDX_Control(pDX, IDC_RADIO1003, m_TerrainRadio512);
	DDX_Control(pDX, IDC_CHECK1001, m_TerrainCheckBox_RenderWireFrame);
	DDX_Control(pDX, IDC_LIST1001, m_TerrainListBox_TexIndex);
	DDX_Control(pDX, IDC_EDIT1001, m_SkyBoxEdit_PosX);
	DDX_Control(pDX, IDC_EDIT1002, m_SkyBoxEdit_PosY);
	DDX_Control(pDX, IDC_EDIT1003, m_SkyBoxEdit_PosZ);
	DDX_Control(pDX, IDC_EDIT1004, m_SkyBoxEdit_Scale);
	DDX_Control(pDX, IDC_LIST1002, m_SkyBoxListBox_TexIndex);
	DDX_Control(pDX, IDC_CHECK1004, m_SkyBoxCheckBox_RenderOnOff);
	DDX_Text(pDX, IDC_EDIT1001, m_fSkyBox_PosX);
	DDX_Text(pDX, IDC_EDIT1002, m_fSkyBox_PosY);
	DDX_Text(pDX, IDC_EDIT1003, m_fSkyBox_PosZ);
	DDX_Text(pDX, IDC_EDIT1004, m_fSkyBox_Scale);
	DDX_Control(pDX, IDC_CHECK1005, m_EditCheck_StaticMesh);
	DDX_Control(pDX, IDC_CHECK1006, m_EditCheck_LightingInfo);
	DDX_Control(pDX, IDC_CHECK1007, m_EditCheck_NavigationMesh);
	DDX_Control(pDX, IDC_RADIO1005, m_StaticMeshRadio_CreateMode);
	DDX_Control(pDX, IDC_RADIO1006, m_StaticMeshRadio_ModifyMode);
	DDX_Control(pDX, IDC_TREE1001, m_StaticMeshTree_ResourceTree);
	DDX_Control(pDX, IDC_LIST1003, m_StaticMeshListBox_ObjectList);
	DDX_Control(pDX, IDC_BUTTON1001, m_StaticMeshButton_DeleteObject);
	DDX_Control(pDX, IDC_BUTTON1002, m_StaticMeshButton_AllDeleteObject);
	DDX_Control(pDX, IDC_EDIT1005, m_StaticMeshEdit_ScaleX);
	DDX_Control(pDX, IDC_EDIT1006, m_StaticMeshEdit_ScaleY);
	DDX_Control(pDX, IDC_EDIT1007, m_StaticMeshEdit_ScaleZ);
	DDX_Control(pDX, IDC_EDIT1008, m_StaticMeshEdit_AngleX);
	DDX_Control(pDX, IDC_EDIT1009, m_StaticMeshEdit_AngleY);
	DDX_Control(pDX, IDC_EDIT1010, m_StaticMeshEdit_AngleZ);
	DDX_Control(pDX, IDC_EDIT1011, m_StaticMeshEdit_PosX);
	DDX_Control(pDX, IDC_EDIT1012, m_StaticMeshEdit_PosY);
	DDX_Control(pDX, IDC_EDIT1013, m_StaticMeshEdit_PosZ);
	DDX_Control(pDX, IDC_CHECK1002, m_StaticMeshCheck_IsRenderShadow);
	DDX_Control(pDX, IDC_CHECK1003, m_StaticMeshCheck_IsCollision);
	DDX_Control(pDX, IDC_BUTTON1003, m_StaticMeshButton_Save);
	DDX_Control(pDX, IDC_BUTTON1004, m_StaticMeshButton_Load);
	DDX_Text(pDX, IDC_EDIT1005, m_fStaticMeshScaleX);
	DDX_Text(pDX, IDC_EDIT1006, m_fStaticMeshScaleY);
	DDX_Text(pDX, IDC_EDIT1007, m_fStaticMeshScaleZ);
	DDX_Text(pDX, IDC_EDIT1008, m_fStaticMeshAngleX);
	DDX_Text(pDX, IDC_EDIT1009, m_fStaticMeshAngleY);
	DDX_Text(pDX, IDC_EDIT1010, m_fStaticMeshAngleZ);
	DDX_Text(pDX, IDC_EDIT1011, m_fStaticMeshPosX);
	DDX_Text(pDX, IDC_EDIT1012, m_fStaticMeshPosY);
	DDX_Text(pDX, IDC_EDIT1013, m_fStaticMeshPosZ);
	DDX_Control(pDX, IDC_EDIT1057, m_StaticMeshEdit_ObjectSize);
	DDX_Text(pDX, IDC_EDIT1057, m_iStaticMeshObjectSize);
	DDX_Control(pDX, IDC_EDIT1058, m_StaticMeshEdit_SelectMesthTag);
	DDX_Control(pDX, IDC_EDIT1059, m_StaticMeshEdit_ColliderScale);
	DDX_Control(pDX, IDC_EDIT1060, m_StaticMeshEdit_ColliderPosX);
	DDX_Control(pDX, IDC_EDIT1061, m_StaticMeshEdit_ColliderPosY);
	DDX_Control(pDX, IDC_EDIT1062, m_StaticMeshEdit_ColliderPosZ);
	DDX_Text(pDX, IDC_EDIT1059, m_fStaticMeshColliderScale);
	DDX_Text(pDX, IDC_EDIT1060, m_fStaticMeshColliderPosX);
	DDX_Text(pDX, IDC_EDIT1061, m_fStaticMeshColliderPosY);
	DDX_Text(pDX, IDC_EDIT1062, m_fStaticMeshColliderPosZ);
	DDX_Control(pDX, IDC_EDIT1014, m_LightInfoEdit_DL_DiffuseR);
	DDX_Control(pDX, IDC_EDIT1015, m_LightInfoEdit_DL_DiffuseG);
	DDX_Control(pDX, IDC_EDIT1016, m_LightInfoEdit_DL_DiffuseB);
	DDX_Control(pDX, IDC_EDIT1017, m_LightInfoEdit_DL_DiffuseA);
	DDX_Control(pDX, IDC_EDIT1018, m_LightInfoEdit_DL_SpecularR);
	DDX_Control(pDX, IDC_EDIT1019, m_LightInfoEdit_DL_SpecularG);
	DDX_Control(pDX, IDC_EDIT1020, m_LightInfoEdit_DL_SpecularB);
	DDX_Control(pDX, IDC_EDIT1021, m_LightInfoEdit_DL_SpecularA);
	DDX_Control(pDX, IDC_EDIT1022, m_LightInfoEdit_DL_AmbientR);
	DDX_Control(pDX, IDC_EDIT1023, m_LightInfoEdit_DL_AmbientG);
	DDX_Control(pDX, IDC_EDIT1024, m_LightInfoEdit_DL_AmbientB);
	DDX_Control(pDX, IDC_EDIT1025, m_LightInfoEdit_DL_AmbientA);
	DDX_Control(pDX, IDC_EDIT1026, m_LightInfoEdit_DL_DirectionX);
	DDX_Control(pDX, IDC_EDIT1027, m_LightInfoEdit_DL_DirectionY);
	DDX_Control(pDX, IDC_EDIT1028, m_LightInfoEdit_DL_DirectionZ);
	DDX_Control(pDX, IDC_EDIT1030, m_LightInfoEdit_PL_DiffuseR);
	DDX_Control(pDX, IDC_EDIT1031, m_LightInfoEdit_PL_DiffuseG);
	DDX_Control(pDX, IDC_EDIT1032, m_LightInfoEdit_PL_DiffuseB);
	DDX_Control(pDX, IDC_EDIT1033, m_LightInfoEdit_PL_DiffuseA);
	DDX_Control(pDX, IDC_EDIT1034, m_LightInfoEdit_PL_SpecularR);
	DDX_Control(pDX, IDC_EDIT1035, m_LightInfoEdit_PL_SpecularG);
	DDX_Control(pDX, IDC_EDIT1036, m_LightInfoEdit_PL_SpecularB);
	DDX_Control(pDX, IDC_EDIT1037, m_LightInfoEdit_PL_SpecularA);
	DDX_Control(pDX, IDC_EDIT1038, m_LightInfoEdit_PL_AnbientR);
	DDX_Control(pDX, IDC_EDIT1039, m_LightInfoEdit_PL_AnbientG);
	DDX_Control(pDX, IDC_EDIT1040, m_LightInfoEdit_PL_AnbientB);
	DDX_Control(pDX, IDC_EDIT1041, m_LightInfoEdit_PL_AnbientA);
	DDX_Control(pDX, IDC_EDIT1029, m_LightInfoEdit_DL_DirectionW);
	DDX_Control(pDX, IDC_EDIT1042, m_LightInfoEdit_PL_PosX);
	DDX_Control(pDX, IDC_EDIT1043, m_LightInfoEdit_PL_PosY);
	DDX_Control(pDX, IDC_EDIT1044, m_LightInfoEdit_PL_PosZ);
	DDX_Control(pDX, IDC_EDIT1045, m_LightInfoEdit_PL_PosW);
	DDX_Control(pDX, IDC_EDIT1046, m_LightInfoEdit_PL_Range);
	DDX_Control(pDX, IDC_RADIO1008, m_LightInfoRadio_PL_CreateMode);
	DDX_Control(pDX, IDC_RADIO1009, m_LightInfoRadio_PL_ModifyMode);
	DDX_Control(pDX, IDC_LIST1004, m_LightInfoListBox_PL_List);
	DDX_Control(pDX, IDC_BUTTON1007, m_LightInfoButton_PL_DELETE);
	DDX_Control(pDX, IDC_BUTTON1008, m_LightInfoButton_PL_ALLDELETE);
	DDX_Control(pDX, IDC_BUTTON1009, m_LightInfoButton_PL_SAVE);
	DDX_Control(pDX, IDC_BUTTON1010, m_LightInfoButton_PL_LOAD);
	DDX_Control(pDX, IDC_EDIT1047, m_LightInfoEdit_SL_EyeX);
	DDX_Control(pDX, IDC_EDIT1048, m_LightInfoEdit_SL_EyeY);
	DDX_Control(pDX, IDC_EDIT1049, m_LightInfoEdit_SL_EyeZ);
	DDX_Control(pDX, IDC_EDIT1050, m_LightInfoEdit_SL_AtX);
	DDX_Control(pDX, IDC_EDIT1051, m_LightInfoEdit_SL_AtY);
	DDX_Control(pDX, IDC_EDIT1052, m_LightInfoEdit_SL_AtZ);
	DDX_Control(pDX, IDC_EDIT1056, m_LightInfoEdit_SL_Height);
	DDX_Control(pDX, IDC_EDIT1053, m_LightInfoEdit_SL_FovY);
	DDX_Control(pDX, IDC_EDIT1054, m_LightInfoEdit_SL_Near);
	DDX_Control(pDX, IDC_EDIT1055, m_LightInfoEdit_SL_Far);
	DDX_Control(pDX, IDC_BUTTON1011, m_LightInfoButton_SL_SAVE);
	DDX_Control(pDX, IDC_BUTTON1012, m_LightInfoButton_SL_LOAD);
	DDX_Text(pDX, IDC_EDIT1014, m_fLightInfo_DL_DiffuseR);
	DDX_Text(pDX, IDC_EDIT1015, m_fLightInfo_DL_DiffuseG);
	DDX_Text(pDX, IDC_EDIT1016, m_fLightInfo_DL_DiffuseB);
	DDX_Text(pDX, IDC_EDIT1017, m_fLightInfo_DL_DiffuseA);
	DDX_Text(pDX, IDC_EDIT1018, m_fLightInfo_DL_SpecularR);
	DDX_Text(pDX, IDC_EDIT1019, m_fLightInfo_DL_SpecularG);
	DDX_Text(pDX, IDC_EDIT1020, m_fLightInfo_DL_SpecularB);
	DDX_Text(pDX, IDC_EDIT1021, m_fLightInfo_DL_SpecularA);
	DDX_Text(pDX, IDC_EDIT1022, m_fLightInfo_DL_AmbientR);
	DDX_Text(pDX, IDC_EDIT1023, m_fLightInfo_DL_AmbientG);
	DDX_Text(pDX, IDC_EDIT1024, m_fLightInfo_DL_AmbientB);
	DDX_Text(pDX, IDC_EDIT1025, m_fLightInfo_DL_AmbientA);
	DDX_Text(pDX, IDC_EDIT1026, m_fLightInfo_DL_DirX);
	DDX_Text(pDX, IDC_EDIT1027, m_fLightInfo_DL_DirY);
	DDX_Text(pDX, IDC_EDIT1028, m_fLightInfo_DL_DirZ);
	DDX_Text(pDX, IDC_EDIT1029, m_fLightInfo_DL_DirW);
	DDX_Text(pDX, IDC_EDIT1030, m_fLightInfo_PL_DiffuseR);
	DDX_Text(pDX, IDC_EDIT1031, m_fLightInfo_PL_DiffuseG);
	DDX_Text(pDX, IDC_EDIT1032, m_fLightInfo_PL_DiffuseB);
	DDX_Text(pDX, IDC_EDIT1033, m_fLightInfo_PL_DiffuseA);
	DDX_Text(pDX, IDC_EDIT1034, m_fLightInfo_PL_SpecularR);
	DDX_Text(pDX, IDC_EDIT1035, m_fLightInfo_PL_SpecularG);
	DDX_Text(pDX, IDC_EDIT1036, m_fLightInfo_PL_SpecularB);
	DDX_Text(pDX, IDC_EDIT1037, m_fLightInfo_PL_SpecularA);
	DDX_Text(pDX, IDC_EDIT1038, m_fLightInfo_PL_AmbientR);
	DDX_Text(pDX, IDC_EDIT1039, m_fLightInfo_PL_AmbientG);
	DDX_Text(pDX, IDC_EDIT1040, m_fLightInfo_PL_AmbientB);
	DDX_Text(pDX, IDC_EDIT1041, m_fLightInfo_PL_AmbientA);
	DDX_Text(pDX, IDC_EDIT1042, m_fLightInfo_PL_PosX);
	DDX_Text(pDX, IDC_EDIT1043, m_fLightInfo_PL_PosY);
	DDX_Text(pDX, IDC_EDIT1044, m_fLightInfo_PL_PosZ);
	DDX_Text(pDX, IDC_EDIT1045, m_fLightInfo_PL_PosW);
	DDX_Text(pDX, IDC_EDIT1046, m_fLightInfo_PL_Range);
	DDX_Text(pDX, IDC_EDIT1047, m_fLightInfo_SL_EyeX);
	DDX_Text(pDX, IDC_EDIT1048, m_fLightInfo_SL_EyeY);
	DDX_Text(pDX, IDC_EDIT1049, m_fLightInfo_SL_EyeZ);
	DDX_Text(pDX, IDC_EDIT1050, m_fLightInfo_SL_AtX);
	DDX_Text(pDX, IDC_EDIT1051, m_fLightInfo_SL_AtY);
	DDX_Text(pDX, IDC_EDIT1052, m_fLightInfo_SL_AtZ);
	DDX_Text(pDX, IDC_EDIT1056, m_fLightInfo_SL_Height);
	DDX_Text(pDX, IDC_EDIT1053, m_fLightInfo_SL_FovY);
	DDX_Text(pDX, IDC_EDIT1054, m_fLightInfo_SL_Near);
	DDX_Text(pDX, IDC_EDIT1055, m_fLightInfo_SL_Far);
	DDX_Control(pDX, IDC_CHECK1008, m_StaticMeshCheck_IsMousePicking);
	DDX_Control(pDX, IDC_RADIO1011, m_NaviMeshRadio_CreateMode);
	DDX_Control(pDX, IDC_RADIO1012, m_NaviMeshRadio_ModifyMode);
	DDX_Control(pDX, IDC_CHECK1010, m_NaviMeshCheck_FindNearPoint);
	DDX_Control(pDX, IDC_LIST1005, m_NaviMeshListBox_CellList);
	DDX_Control(pDX, IDC_BUTTON1013, m_NaviMeshButton_Delete);
	DDX_Control(pDX, IDC_BUTTON1014, m_NaviMeshButton_AllDelete);
	DDX_Control(pDX, IDC_EDIT1063, m_NaviMeshEdit_PointA_X);
	DDX_Control(pDX, IDC_EDIT1064, m_NaviMeshEdit_PointA_Y);
	DDX_Control(pDX, IDC_EDIT1065, m_NaviMeshEdit_PointA_Z);
	DDX_Control(pDX, IDC_EDIT1066, m_NaviMeshEdit_PointB_X);
	DDX_Control(pDX, IDC_EDIT1067, m_NaviMeshEdit_PointB_Y);
	DDX_Control(pDX, IDC_EDIT1068, m_NaviMeshEdit_PointB_Z);
	DDX_Control(pDX, IDC_EDIT1069, m_NaviMeshEdit_PointC_X);
	DDX_Control(pDX, IDC_EDIT1070, m_NaviMeshEdit_PointC_Y);
	DDX_Control(pDX, IDC_EDIT1071, m_NaviMeshEdit_PointC_Z);
	DDX_Control(pDX, IDC_BUTTON1015, m_NaviMeshButton_SAVE);
	DDX_Control(pDX, IDC_BUTTON1016, m_NaviMeshButton_LOAD);
	DDX_Text(pDX, IDC_EDIT1063, m_fNaviMeshPointA_X);
	DDX_Text(pDX, IDC_EDIT1064, m_fNaviMeshPointA_Y);
	DDX_Text(pDX, IDC_EDIT1065, m_fNaviMeshPointA_Z);
	DDX_Text(pDX, IDC_EDIT1066, m_fNaviMeshPointB_X);
	DDX_Text(pDX, IDC_EDIT1067, m_fNaviMeshPointB_Y);
	DDX_Text(pDX, IDC_EDIT1068, m_fNaviMeshPointB_Z);
	DDX_Text(pDX, IDC_EDIT1069, m_fNaviMeshPointC_X);
	DDX_Text(pDX, IDC_EDIT1070, m_fNaviMeshPointC_Y);
	DDX_Text(pDX, IDC_EDIT1071, m_fNaviMeshPointC_Z);
}


BEGIN_MESSAGE_MAP(CTabMap, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1001, &CTabMap::OnBnClickedRadio1001_Terrain128)
	ON_BN_CLICKED(IDC_RADIO1002, &CTabMap::OnBnClickedRadio1002_Terrain256)
	ON_BN_CLICKED(IDC_RADIO1003, &CTabMap::OnBnClickedRadio1003_Terrain512)
	ON_BN_CLICKED(IDC_CHECK1001, &CTabMap::OnBnClickedCheck1001_TerrainRenderWireFrame)
	ON_LBN_SELCHANGE(IDC_LIST1001, &CTabMap::OnLbnSelchangeList1001_TerrainTexIndex)
	ON_LBN_SELCHANGE(IDC_LIST1002, &CTabMap::OnLbnSelchangeList1002_SkyBoxTexIndex)
	ON_BN_CLICKED(IDC_CHECK1004, &CTabMap::OnBnClickedCheck1004_SkyBoxRenderOnOff)
	ON_EN_CHANGE(IDC_EDIT1001, &CTabMap::OnEnChangeEdit1001_SkyBoxPosX)
	ON_EN_CHANGE(IDC_EDIT1002, &CTabMap::OnEnChangeEdit1002_SkyBoxPosY)
	ON_EN_CHANGE(IDC_EDIT1003, &CTabMap::OnEnChangeEdit1003_SkyBoxPosZ)
	ON_EN_CHANGE(IDC_EDIT1004, &CTabMap::OnEnChangeEdit1004__SkyBoxScale)
	ON_BN_CLICKED(IDC_CHECK1005, &CTabMap::OnBnClickedCheck1005_EditStaticMesh)
	ON_BN_CLICKED(IDC_CHECK1006, &CTabMap::OnBnClickedCheck1006_EditLightingInfo)
	ON_BN_CLICKED(IDC_CHECK1007, &CTabMap::OnBnClickedCheck1007_EditNavigationMesh)
	ON_NOTIFY(NM_CLICK, IDC_TREE1001, &CTabMap::OnNMClickTree1001_TreeMeshTag)
	ON_BN_CLICKED(IDC_RADIO1005, &CTabMap::OnBnClickedRadio1005_StaticMeshCreateMode)
	ON_BN_CLICKED(IDC_RADIO1006, &CTabMap::OnBnClickedRadio1006_StaticMeshModifyeMode)
	ON_EN_CHANGE(IDC_EDIT1005, &CTabMap::OnEnChangeEdit1005_StaticMeshScaleX)
	ON_EN_CHANGE(IDC_EDIT1006, &CTabMap::OnEnChangeEdit1006_StaticMeshScaleY)
	ON_EN_CHANGE(IDC_EDIT1007, &CTabMap::OnEnChangeEdit1007_StaticMeshScaleZ)
	ON_EN_CHANGE(IDC_EDIT1008, &CTabMap::OnEnChangeEdit1008_StaticMeshAngleX)
	ON_EN_CHANGE(IDC_EDIT1009, &CTabMap::OnEnChangeEdit1009_StaticMeshAngleY)
	ON_EN_CHANGE(IDC_EDIT1010, &CTabMap::OnEnChangeEdit1010_StaticMeshAngleZ)
	ON_EN_CHANGE(IDC_EDIT1011, &CTabMap::OnEnChangeEdit1011_StaticMeshPosX)
	ON_EN_CHANGE(IDC_EDIT1012, &CTabMap::OnEnChangeEdit1012_StaticMeshPosY)
	ON_EN_CHANGE(IDC_EDIT1013, &CTabMap::OnEnChangeEdit1013_StaticMeshPosZ)
	ON_BN_CLICKED(IDC_CHECK1002, &CTabMap::OnBnClickedCheck1002_StaticMeshRenderShadow)
	ON_BN_CLICKED(IDC_CHECK1003, &CTabMap::OnBnClickedCheck1003_StaticMeshIsCollision)
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_LBN_SELCHANGE(IDC_LIST1003, &CTabMap::OnLbnSelchangeList1003_StaticMeshObjectList)
	ON_BN_CLICKED(IDC_BUTTON1001, &CTabMap::OnBnClickedButton1001_StasticMeshDelete)
	ON_BN_CLICKED(IDC_BUTTON1002, &CTabMap::OnBnClickedButton1002_StaticMeshAllDelete)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON1003, &CTabMap::OnBnClickedButton1003_StaticMeshSAVE)
	ON_BN_CLICKED(IDC_BUTTON1004, &CTabMap::OnBnClickedButton1004_StaticMeshLOAD)
	ON_EN_CHANGE(IDC_EDIT1014, &CTabMap::OnEnChangeEdit1014_LightInfo_DL_DiffuseR)
	ON_EN_CHANGE(IDC_EDIT1015, &CTabMap::OnEnChangeEdit1015_LightInfo_DL_DiffuseG)
	ON_EN_CHANGE(IDC_EDIT1016, &CTabMap::OnEnChangeEdit1016_LightInfo_DL_DiffuseB)
	ON_EN_CHANGE(IDC_EDIT1017, &CTabMap::OnEnChangeEdit1017_LightInfo_DL_DiffuseA)
	ON_EN_CHANGE(IDC_EDIT1018, &CTabMap::OnEnChangeEdit1018_LightInfo_DL_SpecularR)
	ON_EN_CHANGE(IDC_EDIT1019, &CTabMap::OnEnChangeEdit1019_LightInfo_DL_SpecularG)
	ON_EN_CHANGE(IDC_EDIT1020, &CTabMap::OnEnChangeEdit1020_LightInfo_DL_SpecularB)
	ON_EN_CHANGE(IDC_EDIT1021, &CTabMap::OnEnChangeEdit1021_LightInfo_DL_SpecularA)
	ON_EN_CHANGE(IDC_EDIT1022, &CTabMap::OnEnChangeEdit1022_LightInfo_DL_AmbientR)
	ON_EN_CHANGE(IDC_EDIT1023, &CTabMap::OnEnChangeEdit1023_LightInfo_DL_AmbientG)
	ON_EN_CHANGE(IDC_EDIT1024, &CTabMap::OnEnChangeEdit1024_LightInfo_DL_AmbientB)
	ON_EN_CHANGE(IDC_EDIT1025, &CTabMap::OnEnChangeEdit1025_LightInfo_DL_AmbientA)
	ON_EN_CHANGE(IDC_EDIT1026, &CTabMap::OnEnChangeEdit1026_LightInfo_DL_DirX)
	ON_EN_CHANGE(IDC_EDIT1027, &CTabMap::OnEnChangeEdit1027_LightInfo_DL_DirY)
	ON_EN_CHANGE(IDC_EDIT1028, &CTabMap::OnEnChangeEdit1028_LightInfo_DL_DirZ)
	ON_EN_CHANGE(IDC_EDIT1029, &CTabMap::OnEnChangeEdit1029_LightInfo_DL_DirW)
	ON_EN_CHANGE(IDC_EDIT1030, &CTabMap::OnEnChangeEdit1030_LightInfo_PL_DiffuseR)
	ON_EN_CHANGE(IDC_EDIT1031, &CTabMap::OnEnChangeEdit1031_LightInfo_PL_DiffuseG)
	ON_EN_CHANGE(IDC_EDIT1032, &CTabMap::OnEnChangeEdit1032_LightInfo_PL_DiffuseB)
	ON_EN_CHANGE(IDC_EDIT1033, &CTabMap::OnEnChangeEdit1033_LightInfo_PL_DiffuseA)
	ON_EN_CHANGE(IDC_EDIT1034, &CTabMap::OnEnChangeEdit1034_LightInfo_PL_SpecularR)
	ON_EN_CHANGE(IDC_EDIT1035, &CTabMap::OnEnChangeEdit1035_LightInfo_PL_SpecularG)
	ON_EN_CHANGE(IDC_EDIT1036, &CTabMap::OnEnChangeEdit1036_LightInfo_PL_SpecularB)
	ON_EN_CHANGE(IDC_EDIT1037, &CTabMap::OnEnChangeEdit1037_LightInfo_PL_SpecularA)
	ON_EN_CHANGE(IDC_EDIT1038, &CTabMap::OnEnChangeEdit1038_LightInfo_PL_AmbientR)
	ON_EN_CHANGE(IDC_EDIT1039, &CTabMap::OnEnChangeEdit1039_LightInfo_PL_AmbientG)
	ON_EN_CHANGE(IDC_EDIT1040, &CTabMap::OnEnChangeEdit1040_LightInfo_PL_AmbientB)
	ON_EN_CHANGE(IDC_EDIT1041, &CTabMap::OnEnChangeEdit1041_LightInfo_PL_AmbientA)
	ON_EN_CHANGE(IDC_EDIT1042, &CTabMap::OnEnChangeEdit1042_LightInfo_PL_PosX)
	ON_EN_CHANGE(IDC_EDIT1043, &CTabMap::OnEnChangeEdit1043_LightInfo_PL_PosY)
	ON_EN_CHANGE(IDC_EDIT1044, &CTabMap::OnEnChangeEdit1044_LightInfo_PL_PosZ)
	ON_EN_CHANGE(IDC_EDIT1045, &CTabMap::OnEnChangeEdit1045_LightInfo_PL_PosW)
	ON_EN_CHANGE(IDC_EDIT1046, &CTabMap::OnEnChangeEdit1046_LightInfo_PL_Range)
	ON_BN_CLICKED(IDC_RADIO1008, &CTabMap::OnBnClickedRadio1008_LightInfo_PL_CreateMode)
	ON_BN_CLICKED(IDC_RADIO1009, &CTabMap::OnBnClickedRadio1009_LightInfo_PL_ModifyMode)
	ON_LBN_SELCHANGE(IDC_LIST1004, &CTabMap::OnLbnSelchangeList1004_LightInfo_PL_List)
	ON_BN_CLICKED(IDC_BUTTON1009, &CTabMap::OnBnClickedButton1009__LightInfo_PL_SAVE)
	ON_BN_CLICKED(IDC_BUTTON1010, &CTabMap::OnBnClickedButton1010__LightInfo_PL_LOAD)
	//ON_EN_CHANGE(IDC_EDIT1047, &CTabMap::OnEnChangeEdit1047_LightInfo_SL_EyeX)
	//ON_EN_CHANGE(IDC_EDIT1048, &CTabMap::OnEnChangeEdit1048_LightInfo_SL_EyeY)
	//ON_EN_CHANGE(IDC_EDIT1049, &CTabMap::OnEnChangeEdit1049_LightInfo_SL_EyeZ)
	ON_EN_CHANGE(IDC_EDIT1050, &CTabMap::OnEnChangeEdit1050_LightInfo_SL_AtX)
	ON_EN_CHANGE(IDC_EDIT1051, &CTabMap::OnEnChangeEdit1051_LightInfo_SL_AtY)
	ON_EN_CHANGE(IDC_EDIT1052, &CTabMap::OnEnChangeEdit1052_LightInfo_SL_AtZ)
	ON_EN_CHANGE(IDC_EDIT1056, &CTabMap::OnEnChangeEdit1056_LightInfo_SL_Height)
	ON_EN_CHANGE(IDC_EDIT1053, &CTabMap::OnEnChangeEdit1053_LightInfo_SL_FovY)
	ON_EN_CHANGE(IDC_EDIT1054, &CTabMap::OnEnChangeEdit1054_LightInfo_SL_Near)
	ON_EN_CHANGE(IDC_EDIT1055, &CTabMap::OnEnChangeEdit1055_LightInfo_SL_Far)
	ON_BN_CLICKED(IDC_BUTTON1011, &CTabMap::OnBnClickedButton1011__LightInfo_SL_SAVE)
	ON_BN_CLICKED(IDC_BUTTON1012, &CTabMap::OnBnClickedButton1012_LightInfo_SL_LOAD)
	//ON_BN_CLICKED(IDC_BUTTON1005, &CTabMap::OnBnClickedButton1005_LightInfo_DL_SAVE)
	//ON_BN_CLICKED(IDC_BUTTON1006, &CTabMap::OnBnClickedButton1006_LightInfo_DL_LOAD)
	ON_BN_CLICKED(IDC_CHECK1008, &CTabMap::OnBnClickedCheck1008_StaticMeshIsMousePicking)
	ON_BN_CLICKED(IDC_BUTTON1007, &CTabMap::OnBnClickedButton1007_LightInfo_PL_DELETE)
	ON_BN_CLICKED(IDC_BUTTON1008, &CTabMap::OnBnClickedButton1008_LightInfo_PL_ALLDELETE)
	ON_BN_CLICKED(IDC_RADIO1011, &CTabMap::OnBnClickedRadio1011_NaviMeshCreateMode)
	ON_BN_CLICKED(IDC_RADIO1012, &CTabMap::OnBnClickedRadio1012_NaviMeshModifyMode)
END_MESSAGE_MAP()


// CTabMap 메시지 처리기
BOOL CTabMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// Terrain 컨트롤 비활성화
	m_TerrainRadio128.EnableWindow(FALSE);
	m_TerrainRadio256.EnableWindow(FALSE);
	m_TerrainRadio512.EnableWindow(FALSE);
	m_TerrainCheckBox_RenderWireFrame.EnableWindow(FALSE);
	m_TerrainListBox_TexIndex.EnableWindow(FALSE);

	// SkyBox 컨트롤 비활성화
	m_SkyBoxEdit_PosX.EnableWindow(FALSE);
	m_SkyBoxEdit_PosY.EnableWindow(FALSE);
	m_SkyBoxEdit_PosZ.EnableWindow(FALSE);
	m_SkyBoxEdit_Scale.EnableWindow(FALSE);
	m_SkyBoxListBox_TexIndex.EnableWindow(FALSE);
	m_SkyBoxCheckBox_RenderOnOff.EnableWindow(FALSE);
	m_SkyBoxCheckBox_RenderOnOff.SetCheck(true);

	// EditCheck 컨트롤 비활성화.
	m_EditCheck_StaticMesh.EnableWindow(FALSE);
	m_EditCheck_LightingInfo.EnableWindow(FALSE);
	m_EditCheck_NavigationMesh.EnableWindow(FALSE);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(FALSE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_StaticMeshTree_ResourceTree.EnableWindow(FALSE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(FALSE);
	m_StaticMeshListBox_ObjectList.EnableWindow(FALSE);
	m_StaticMeshButton_DeleteObject.EnableWindow(FALSE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleX.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleY.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleX.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleY.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_PosX.EnableWindow(FALSE);
	m_StaticMeshEdit_PosY.EnableWindow(FALSE);
	m_StaticMeshEdit_PosZ.EnableWindow(FALSE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(FALSE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(FALSE);
	m_StaticMeshCheck_IsCollision.EnableWindow(FALSE);
	m_StaticMeshCheck_IsMousePicking.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(FALSE);
	m_StaticMeshButton_Load.EnableWindow(FALSE);

	// Lighting
	m_LightInfoEdit_DL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionX.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionY.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionZ.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionW.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosX.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosY.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosZ.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosW.EnableWindow(FALSE);
	m_LightInfoEdit_PL_Range.EnableWindow(FALSE);
	m_LightInfoRadio_PL_CreateMode.EnableWindow(FALSE);
	m_LightInfoRadio_PL_ModifyMode.EnableWindow(FALSE);
	m_LightInfoListBox_PL_List.EnableWindow(FALSE);
	m_LightInfoButton_PL_DELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_ALLDELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_PL_LOAD.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Height.EnableWindow(FALSE);
	m_LightInfoEdit_SL_FovY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Near.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Far.EnableWindow(FALSE);
	m_LightInfoButton_SL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_SL_LOAD.EnableWindow(FALSE);

	// NavigationMesh
	m_NaviMeshRadio_CreateMode.EnableWindow(FALSE);
	m_NaviMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_NaviMeshCheck_FindNearPoint.EnableWindow(FALSE);
	m_NaviMeshListBox_CellList.EnableWindow(FALSE);
	m_NaviMeshButton_Delete.EnableWindow(FALSE);
	m_NaviMeshButton_AllDelete.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Z.EnableWindow(FALSE);
	m_NaviMeshButton_SAVE.EnableWindow(FALSE);
	m_NaviMeshButton_LOAD.EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabMap::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CTabMap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);

	/*__________________________________________________________________________________________________________
	[ StaticMesh ]
	____________________________________________________________________________________________________________*/
	if (m_EditCheck_StaticMesh.GetCheck())
	{
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		RECT rcStaticMeshEdit[13] = { };
		m_StaticMeshEdit_ScaleX.GetWindowRect(&rcStaticMeshEdit[0]);
		m_StaticMeshEdit_ScaleY.GetWindowRect(&rcStaticMeshEdit[1]);
		m_StaticMeshEdit_ScaleZ.GetWindowRect(&rcStaticMeshEdit[2]);
		m_StaticMeshEdit_AngleX.GetWindowRect(&rcStaticMeshEdit[3]);
		m_StaticMeshEdit_AngleY.GetWindowRect(&rcStaticMeshEdit[4]);
		m_StaticMeshEdit_AngleZ.GetWindowRect(&rcStaticMeshEdit[5]);
		m_StaticMeshEdit_PosX.GetWindowRect(&rcStaticMeshEdit[6]);
		m_StaticMeshEdit_PosY.GetWindowRect(&rcStaticMeshEdit[7]);
		m_StaticMeshEdit_PosZ.GetWindowRect(&rcStaticMeshEdit[8]);
		m_StaticMeshEdit_ColliderScale.GetWindowRect(&rcStaticMeshEdit[9]);
		m_StaticMeshEdit_ColliderPosX.GetWindowRect(&rcStaticMeshEdit[10]);
		m_StaticMeshEdit_ColliderPosY.GetWindowRect(&rcStaticMeshEdit[11]);
		m_StaticMeshEdit_ColliderPosZ.GetWindowRect(&rcStaticMeshEdit[12]);

		if (PtInRect(&rcStaticMeshEdit[0], pt) ||	// Scale X
			PtInRect(&rcStaticMeshEdit[1], pt) ||	// Scale Y
			PtInRect(&rcStaticMeshEdit[2], pt))		// Scale X
		{
			if (zDelta > 0)
			{
				m_fStaticMeshScaleX += 0.01f;
				m_fStaticMeshScaleY += 0.01f;
				m_fStaticMeshScaleZ += 0.01f;
			}
			else if (zDelta < 0 && 
					 m_fStaticMeshScaleX > 0 &&
					 m_fStaticMeshScaleY > 0 &&
					 m_fStaticMeshScaleX > 0)
			{
				m_fStaticMeshScaleX -= 0.01f;
				m_fStaticMeshScaleY -= 0.01f;
				m_fStaticMeshScaleZ -= 0.01f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
			{
				pObject->Get_Transform()->m_vScale.x = m_fStaticMeshScaleX;
				pObject->Get_Transform()->m_vScale.y = m_fStaticMeshScaleY;
				pObject->Get_Transform()->m_vScale.z = m_fStaticMeshScaleZ;
			}
		}
		else if (PtInRect(&rcStaticMeshEdit[3], pt))	// Angle X
		{
			if (zDelta > 0)
				m_fStaticMeshAngleX += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshAngleX -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_Transform()->m_vAngle.x = m_fStaticMeshAngleX;
		}
		else if (PtInRect(&rcStaticMeshEdit[4], pt))	// Angle Y
		{
			if (zDelta > 0)
				m_fStaticMeshAngleY += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshAngleY -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_Transform()->m_vAngle.y = m_fStaticMeshAngleY;
		}
		else if (PtInRect(&rcStaticMeshEdit[5], pt))	// Angle Z
		{
			if (zDelta > 0)
				m_fStaticMeshAngleZ += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshAngleZ -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_Transform()->m_vAngle.z = m_fStaticMeshAngleZ;
		}
		else if (PtInRect(&rcStaticMeshEdit[6], pt))	// Pos X
		{
			if (zDelta > 0)
				m_fStaticMeshPosX += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshPosX -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_Transform()->m_vPos.x = m_fStaticMeshPosX;
		}
		else if (PtInRect(&rcStaticMeshEdit[7], pt))	// Pos Y
		{
			if (zDelta > 0)
				m_fStaticMeshPosY += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshPosY -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_Transform()->m_vPos.y= m_fStaticMeshPosY;
		}
		else if (PtInRect(&rcStaticMeshEdit[8], pt))	// Pos Z
		{
			if (zDelta > 0)
				m_fStaticMeshPosZ += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshPosZ -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_Transform()->m_vPos.z = m_fStaticMeshPosZ;
		}
		else if (PtInRect(&rcStaticMeshEdit[9], pt))	// ColliderScale
		{
			if (zDelta > 0)
				m_fStaticMeshColliderScale += (1.0f / m_fStaticMeshScaleX);
			else if (zDelta < 0 && m_fStaticMeshColliderScale > 0)
				m_fStaticMeshColliderScale -= (1.0f / m_fStaticMeshScaleX);

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_BoundingSphere()->Get_Transform()->m_vScale = _vec3(m_fStaticMeshColliderScale);
		}
		else if (PtInRect(&rcStaticMeshEdit[10], pt))	// ColliderPosX
		{
			if (zDelta > 0)
				m_fStaticMeshColliderPosX += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshColliderPosX -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_BoundingSphere()->Get_Transform()->m_vPos.x = m_fStaticMeshColliderPosX;
		}
		else if (PtInRect(&rcStaticMeshEdit[11], pt))	// ColliderPosY
		{
			if (zDelta > 0)
				m_fStaticMeshColliderPosY += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshColliderPosY -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_BoundingSphere()->Get_Transform()->m_vPos.y = m_fStaticMeshColliderPosY;
		}
		else if (PtInRect(&rcStaticMeshEdit[12], pt))	// ColliderPosZ
		{
			if (zDelta > 0)
				m_fStaticMeshColliderPosZ += 1.0f;
			else if (zDelta < 0)
				m_fStaticMeshColliderPosZ -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsModifyMode && nullptr != pObject)
				pObject->Get_BoundingSphere()->Get_Transform()->m_vPos.z = m_fStaticMeshColliderPosZ;
		}
	}

	/*__________________________________________________________________________________________________________
	[ Lighting Info ]
	____________________________________________________________________________________________________________*/
	if (m_EditCheck_LightingInfo.GetCheck())
	{
		RECT rcLightInfoEdit_DL_Edit[16] = { };
		m_LightInfoEdit_DL_DiffuseR.GetWindowRect(&rcLightInfoEdit_DL_Edit[0]);
		m_LightInfoEdit_DL_DiffuseG.GetWindowRect(&rcLightInfoEdit_DL_Edit[1]);
		m_LightInfoEdit_DL_DiffuseB.GetWindowRect(&rcLightInfoEdit_DL_Edit[2]);
		m_LightInfoEdit_DL_DiffuseA.GetWindowRect(&rcLightInfoEdit_DL_Edit[3]);
		m_LightInfoEdit_DL_SpecularR.GetWindowRect(&rcLightInfoEdit_DL_Edit[4]);
		m_LightInfoEdit_DL_SpecularG.GetWindowRect(&rcLightInfoEdit_DL_Edit[5]);
		m_LightInfoEdit_DL_SpecularB.GetWindowRect(&rcLightInfoEdit_DL_Edit[6]);
		m_LightInfoEdit_DL_SpecularA.GetWindowRect(&rcLightInfoEdit_DL_Edit[7]);
		m_LightInfoEdit_DL_AmbientR.GetWindowRect(&rcLightInfoEdit_DL_Edit[8]);
		m_LightInfoEdit_DL_AmbientG.GetWindowRect(&rcLightInfoEdit_DL_Edit[9]);
		m_LightInfoEdit_DL_AmbientB.GetWindowRect(&rcLightInfoEdit_DL_Edit[10]);
		m_LightInfoEdit_DL_AmbientA.GetWindowRect(&rcLightInfoEdit_DL_Edit[11]);
		m_LightInfoEdit_DL_DirectionX.GetWindowRect(&rcLightInfoEdit_DL_Edit[12]);
		m_LightInfoEdit_DL_DirectionY.GetWindowRect(&rcLightInfoEdit_DL_Edit[13]);
		m_LightInfoEdit_DL_DirectionZ.GetWindowRect(&rcLightInfoEdit_DL_Edit[14]);
		m_LightInfoEdit_DL_DirectionW.GetWindowRect(&rcLightInfoEdit_DL_Edit[15]);

		Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);
		
		if (PtInRect(&rcLightInfoEdit_DL_Edit[0], pt))			// DL_DiffuseR
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_DiffuseR += 0.1f;
				if (m_fLightInfo_DL_DiffuseR > 1.0f)
					m_fLightInfo_DL_DiffuseR = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_DiffuseR -= 0.1f;
				if (m_fLightInfo_DL_DiffuseR < 0.0f)
					m_fLightInfo_DL_DiffuseR = 0.0f;
			}

			tDirLightInfo.Diffuse.x = m_fLightInfo_DL_DiffuseR;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[1], pt))		// DL_DiffuseG
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_DiffuseG += 0.1f;
				if (m_fLightInfo_DL_DiffuseG > 1.0f)
					m_fLightInfo_DL_DiffuseG = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_DiffuseG -= 0.1f;
				if (m_fLightInfo_DL_DiffuseG < 0.0f)
					m_fLightInfo_DL_DiffuseG = 0.0f;
			}

			tDirLightInfo.Diffuse.y = m_fLightInfo_DL_DiffuseG;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[2], pt))		// DL_DiffuseB
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_DiffuseB += 0.1f;
				if (m_fLightInfo_DL_DiffuseB > 1.0f)
					m_fLightInfo_DL_DiffuseB = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_DiffuseB -= 0.1f;
				if (m_fLightInfo_DL_DiffuseB < 0.0f)
					m_fLightInfo_DL_DiffuseB = 0.0f;
			}

			tDirLightInfo.Diffuse.z = m_fLightInfo_DL_DiffuseB;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[3], pt))		// DL_DiffuseA
		{
			m_fLightInfo_DL_DiffuseA = 1.0f;

			tDirLightInfo.Diffuse.w = m_fLightInfo_DL_DiffuseA;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[4], pt))		// DL_SpecularR
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_SpecularR += 0.1f;
				if (m_fLightInfo_DL_SpecularR > 1.0f)
					m_fLightInfo_DL_SpecularR = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_SpecularR -= 0.1f;
				if (m_fLightInfo_DL_SpecularR < 0.0f)
					m_fLightInfo_DL_SpecularR = 0.0f;
			}


			tDirLightInfo.Specular.x = m_fLightInfo_DL_SpecularR;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[5], pt))		// DL_SpecularG
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_SpecularG += 0.1f;
				if (m_fLightInfo_DL_SpecularG > 1.0f)
					m_fLightInfo_DL_SpecularG = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_SpecularG -= 0.1f;
				if (m_fLightInfo_DL_SpecularG < 0.0f)
					m_fLightInfo_DL_SpecularG = 0.0f;
			}

			tDirLightInfo.Specular.y = m_fLightInfo_DL_SpecularG;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[6], pt))		// DL_SpecularB
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_SpecularB += 0.1f;
				if (m_fLightInfo_DL_SpecularB > 1.0f)
					m_fLightInfo_DL_SpecularB = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_SpecularB -= 0.1f;
				if (m_fLightInfo_DL_SpecularB < 0.0f)
					m_fLightInfo_DL_SpecularB = 0.0f;
			}

			tDirLightInfo.Specular.z = m_fLightInfo_DL_SpecularB;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
			
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[7], pt))		// DL_SpecularA
		{
			m_fLightInfo_DL_SpecularA = 1.0f;

			tDirLightInfo.Specular.w = m_fLightInfo_DL_SpecularA;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[8], pt))		// DL_AmbientR
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_AmbientR += 0.1f;
				if (m_fLightInfo_DL_AmbientR > 1.0f)
					m_fLightInfo_DL_AmbientR = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_AmbientR -= 0.1f;
				if (m_fLightInfo_DL_AmbientR < 0.0f)
					m_fLightInfo_DL_AmbientR = 0.0f;
			}

			tDirLightInfo.Ambient.x = m_fLightInfo_DL_AmbientR;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[9], pt))		// DL_AmbientG
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_AmbientG += 0.1f;
				if (m_fLightInfo_DL_AmbientG > 1.0f)
					m_fLightInfo_DL_AmbientG = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_AmbientG -= 0.1f;
				if (m_fLightInfo_DL_AmbientG < 0.0f)
					m_fLightInfo_DL_AmbientG = 0.0f;
			}

			tDirLightInfo.Ambient.y = m_fLightInfo_DL_AmbientG;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[10], pt))	// DL_AmbientB
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_AmbientB += 0.1f;
				if (m_fLightInfo_DL_AmbientB > 1.0f)
					m_fLightInfo_DL_AmbientB = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_AmbientB -= 0.1f;
				if (m_fLightInfo_DL_AmbientB < 0.0f)
					m_fLightInfo_DL_AmbientB = 0.0f;
			}
			
			tDirLightInfo.Ambient.z = m_fLightInfo_DL_AmbientB;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[11], pt))	// DL_AmbientA
		{
			m_fLightInfo_DL_AmbientA = 1.0f;

			tDirLightInfo.Ambient.w = m_fLightInfo_DL_AmbientA;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[12], pt))	// DL_DirectionX
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_DirX += 0.1f;
				if (m_fLightInfo_DL_DirX > 1.0f)
					m_fLightInfo_DL_DirX = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_DirX -= 0.1f;
				if (m_fLightInfo_DL_DirX < -1.0f)
					m_fLightInfo_DL_DirX = -1.0f;
			}

			tDirLightInfo.Direction.x = m_fLightInfo_DL_DirX;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
			m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
			m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
			m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[13], pt))	// DL_DirectionY
		{
			m_fLightInfo_DL_DirY = -1.0f;

			tDirLightInfo.Direction.y = m_fLightInfo_DL_DirY;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
			m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
			m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
			m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[14], pt))	// DL_DirectionZ
		{
			if (zDelta > 0)
			{
				m_fLightInfo_DL_DirZ += 0.1f;
				if (m_fLightInfo_DL_DirZ > 1.0f)
					m_fLightInfo_DL_DirZ = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_DL_DirZ -= 0.1f;
				if (m_fLightInfo_DL_DirZ < -1.0f)
					m_fLightInfo_DL_DirZ = -1.0f;
			}

			tDirLightInfo.Direction.z = m_fLightInfo_DL_DirZ;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
			m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
			m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
			m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

		}
		else if (PtInRect(&rcLightInfoEdit_DL_Edit[15], pt))	// DL_DirectionW
		{
			m_fLightInfo_DL_DirW = 0.0f;

			tDirLightInfo.Direction.w = m_fLightInfo_DL_DirW;
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
		}

		RECT rcLightInfoEdit_SL_Edit[10] = { };
		m_LightInfoEdit_SL_EyeX.GetWindowRect(&rcLightInfoEdit_SL_Edit[0]);
		m_LightInfoEdit_SL_EyeY.GetWindowRect(&rcLightInfoEdit_SL_Edit[1]);
		m_LightInfoEdit_SL_EyeZ.GetWindowRect(&rcLightInfoEdit_SL_Edit[2]);
		m_LightInfoEdit_SL_AtX.GetWindowRect(&rcLightInfoEdit_SL_Edit[3]);
		m_LightInfoEdit_SL_AtY.GetWindowRect(&rcLightInfoEdit_SL_Edit[4]);
		m_LightInfoEdit_SL_AtZ.GetWindowRect(&rcLightInfoEdit_SL_Edit[5]);
		m_LightInfoEdit_SL_Height.GetWindowRect(&rcLightInfoEdit_SL_Edit[6]);
		m_LightInfoEdit_SL_FovY.GetWindowRect(&rcLightInfoEdit_SL_Edit[7]);
		m_LightInfoEdit_SL_Near.GetWindowRect(&rcLightInfoEdit_SL_Edit[8]);
		m_LightInfoEdit_SL_Far.GetWindowRect(&rcLightInfoEdit_SL_Edit[9]);

		if (PtInRect(&rcLightInfoEdit_SL_Edit[0], pt))			// SL_EyeX
		{}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[1], pt))		// SL_EyeY
		{}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[2], pt))		// SL_EyeZ
		{}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[3], pt))		// SL_AtX
		{
			if (zDelta > 0)
				m_fLightInfo_SL_AtX += 1.f;
			else if (zDelta < 0)
				m_fLightInfo_SL_AtX -= 1.0f;

			CShadowLightMgr::Get_Instance()->m_vLightAt.x = m_fLightInfo_SL_AtX;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
		}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[4], pt))		// SL_AtY
		{

			m_fLightInfo_SL_AtY = 0.0f;

			CShadowLightMgr::Get_Instance()->m_vLightAt.x = m_fLightInfo_SL_AtY;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
		}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[5], pt))		// SL_AtZ
		{
			if (zDelta > 0)
				m_fLightInfo_SL_AtZ += 1.f;
			else if (zDelta < 0)
				m_fLightInfo_SL_AtZ -= 1.0f;

			CShadowLightMgr::Get_Instance()->m_vLightAt.x = m_fLightInfo_SL_AtZ;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
		}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[6], pt))		// SL_Height
		{
			if (zDelta > 0)
				m_fLightInfo_SL_Height += 100.f;
			else if (zDelta < 0)
			{
				m_fLightInfo_SL_Height -= 100.f;
				if (m_fLightInfo_SL_Height < 100.0f)
					m_fLightInfo_SL_Height = 100.0f;
			}

			CShadowLightMgr::Get_Instance()->m_fHeight = m_fLightInfo_SL_Height;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
			m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
			m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
			m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;
		}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[7], pt))		// SL_FovY
		{
			if (zDelta > 0)
				m_fLightInfo_SL_FovY += 1.f;
			else if (zDelta < 0)
			{
				m_fLightInfo_SL_FovY -= 1.f;

				if (m_fLightInfo_SL_FovY < 0.0f)
					m_fLightInfo_SL_FovY = 1.0f;
			}

			CShadowLightMgr::Get_Instance()->m_fFovY = m_fLightInfo_SL_FovY;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
		}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[8], pt))		// SL_Near
		{
			m_fLightInfo_SL_Near = 1.0f;

			CShadowLightMgr::Get_Instance()->m_fNear = m_fLightInfo_SL_Near;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
		}
		else if (PtInRect(&rcLightInfoEdit_SL_Edit[9], pt))		// SL_Far
		{
			if (zDelta > 0)
				m_fLightInfo_SL_Far += 100.f;
			else if (zDelta < 0)
			{
				m_fLightInfo_SL_Far -= 100.f;

				if (m_fLightInfo_SL_Far < 1000.0f)
					m_fLightInfo_SL_Far = 1000.0f;
			}

			CShadowLightMgr::Get_Instance()->m_fFar = m_fLightInfo_SL_Far;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();
		}


		RECT rcLightInfoEdit_PL_Edit[17] = { };
		m_LightInfoEdit_PL_DiffuseR.GetWindowRect(&rcLightInfoEdit_PL_Edit[0]);
		m_LightInfoEdit_PL_DiffuseG.GetWindowRect(&rcLightInfoEdit_PL_Edit[1]);
		m_LightInfoEdit_PL_DiffuseB.GetWindowRect(&rcLightInfoEdit_PL_Edit[2]);
		m_LightInfoEdit_PL_DiffuseA.GetWindowRect(&rcLightInfoEdit_PL_Edit[3]);
		m_LightInfoEdit_PL_SpecularR.GetWindowRect(&rcLightInfoEdit_PL_Edit[4]);
		m_LightInfoEdit_PL_SpecularG.GetWindowRect(&rcLightInfoEdit_PL_Edit[5]);
		m_LightInfoEdit_PL_SpecularB.GetWindowRect(&rcLightInfoEdit_PL_Edit[6]);
		m_LightInfoEdit_PL_SpecularA.GetWindowRect(&rcLightInfoEdit_PL_Edit[7]);
		m_LightInfoEdit_PL_AnbientR.GetWindowRect(&rcLightInfoEdit_PL_Edit[8]);
		m_LightInfoEdit_PL_AnbientG.GetWindowRect(&rcLightInfoEdit_PL_Edit[9]);
		m_LightInfoEdit_PL_AnbientB.GetWindowRect(&rcLightInfoEdit_PL_Edit[10]);
		m_LightInfoEdit_PL_AnbientA.GetWindowRect(&rcLightInfoEdit_PL_Edit[11]);
		m_LightInfoEdit_PL_PosX.GetWindowRect(&rcLightInfoEdit_PL_Edit[12]);
		m_LightInfoEdit_PL_PosY.GetWindowRect(&rcLightInfoEdit_PL_Edit[13]);
		m_LightInfoEdit_PL_PosZ.GetWindowRect(&rcLightInfoEdit_PL_Edit[14]);
		m_LightInfoEdit_PL_PosW.GetWindowRect(&rcLightInfoEdit_PL_Edit[15]);
		m_LightInfoEdit_PL_Range.GetWindowRect(&rcLightInfoEdit_PL_Edit[16]);

		Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;

		if (PtInRect(&rcLightInfoEdit_PL_Edit[0], pt))			// PL_DiffuseR
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_DiffuseR += 0.1f;
				if (m_fLightInfo_PL_DiffuseR > 1.0f)
					m_fLightInfo_PL_DiffuseR = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_DiffuseR -= 0.1f;
				if (m_fLightInfo_PL_DiffuseR < 0.0f)
					m_fLightInfo_PL_DiffuseR = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Diffuse.x = m_fLightInfo_PL_DiffuseR;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[1], pt))		// PL_DiffuseG
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_DiffuseG += 0.1f;
				if (m_fLightInfo_PL_DiffuseG > 1.0f)
					m_fLightInfo_PL_DiffuseG = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_DiffuseG -= 0.1f;
				if (m_fLightInfo_PL_DiffuseG < 0.0f)
					m_fLightInfo_PL_DiffuseG = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Diffuse.y = m_fLightInfo_PL_DiffuseG;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[2], pt))		// PL_DiffuseB
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_DiffuseB += 0.1f;
				if (m_fLightInfo_PL_DiffuseB > 1.0f)
					m_fLightInfo_PL_DiffuseB = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_DiffuseB -= 0.1f;
				if (m_fLightInfo_PL_DiffuseB < 0.0f)
					m_fLightInfo_PL_DiffuseB = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Diffuse.z = m_fLightInfo_PL_DiffuseB;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[3], pt))		// PL_DiffuseA
		{
			m_fLightInfo_PL_DiffuseA = 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Diffuse.w = m_fLightInfo_PL_DiffuseA;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[4], pt))		// PL_SpecularR
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_SpecularR += 0.1f;
				if (m_fLightInfo_PL_SpecularR > 1.0f)
					m_fLightInfo_PL_SpecularR = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_SpecularR -= 0.1f;
				if (m_fLightInfo_PL_SpecularR < 0.0f)
					m_fLightInfo_PL_SpecularR = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Specular.x = m_fLightInfo_PL_SpecularR;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[5], pt))		// PL_SpecularG
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_SpecularG += 0.1f;
				if (m_fLightInfo_PL_SpecularG > 1.0f)
					m_fLightInfo_PL_SpecularG = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_SpecularG -= 0.1f;
				if (m_fLightInfo_PL_SpecularG < 0.0f)
					m_fLightInfo_PL_SpecularG = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Specular.y = m_fLightInfo_PL_SpecularG;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[6], pt))		// PL_SpecularB
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_SpecularB += 0.1f;
				if (m_fLightInfo_PL_SpecularB > 1.0f)
					m_fLightInfo_PL_SpecularB = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_SpecularB -= 0.1f;
				if (m_fLightInfo_PL_SpecularB < 0.0f)
					m_fLightInfo_PL_SpecularB = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Specular.z = m_fLightInfo_PL_SpecularB;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[7], pt))		// PL_SpecularA
		{
			m_fLightInfo_PL_SpecularA = 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Specular.w = m_fLightInfo_PL_SpecularA;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[8], pt))		// PL_AmbientR
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_AmbientR += 0.1f;
				if (m_fLightInfo_PL_AmbientR > 1.0f)
					m_fLightInfo_PL_AmbientR = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_AmbientR -= 0.1f;
				if (m_fLightInfo_PL_AmbientR < 0.0f)
					m_fLightInfo_PL_AmbientR = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Ambient.x = m_fLightInfo_PL_AmbientR;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[9], pt))		// PL_AmbientG
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_AmbientG += 0.1f;
				if (m_fLightInfo_PL_AmbientG > 1.0f)
					m_fLightInfo_PL_AmbientG = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_AmbientG -= 0.1f;
				if (m_fLightInfo_PL_AmbientG < 0.0f)
					m_fLightInfo_PL_AmbientG = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Ambient.y = m_fLightInfo_PL_AmbientG;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[10], pt))	// PL_AmbientB
		{
			if (zDelta > 0)
			{
				m_fLightInfo_PL_AmbientB += 0.1f;
				if (m_fLightInfo_PL_AmbientB > 1.0f)
					m_fLightInfo_PL_AmbientB = 1.0f;
			}
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_AmbientB -= 0.1f;
				if (m_fLightInfo_PL_AmbientB < 0.0f)
					m_fLightInfo_PL_AmbientB = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Ambient.z = m_fLightInfo_PL_AmbientB;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[11], pt))	// PL_AmbientA
		{
			m_fLightInfo_PL_AmbientA = 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Ambient.w = m_fLightInfo_PL_AmbientA;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[12], pt))	// PL_PosX
		{
			if (zDelta > 0)
				m_fLightInfo_PL_PosX += 1.f;
			else if (zDelta < 0)
				m_fLightInfo_PL_PosX -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Position.x = m_fLightInfo_PL_PosX;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderPosition();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[13], pt))	// PL_PosY
		{
			if (zDelta > 0)
				m_fLightInfo_PL_PosY += 1.f;
			else if (zDelta < 0)
				m_fLightInfo_PL_PosY -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Position.y = m_fLightInfo_PL_PosY;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderPosition();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[14], pt))	// PL_PosZ
		{
			if (zDelta > 0)
				m_fLightInfo_PL_PosZ += 1.f;
			else if (zDelta < 0)
				m_fLightInfo_PL_PosZ -= 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Position.z = m_fLightInfo_PL_PosZ;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderPosition();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[15], pt))	// PL_PosW
		{
			m_fLightInfo_PL_PosW = 1.0f;

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Position.w = m_fLightInfo_PL_PosW;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderPosition();
				}
			}
		}
		else if (PtInRect(&rcLightInfoEdit_PL_Edit[16], pt))	// PL_Range
		{
			if (zDelta > 0)
				m_fLightInfo_PL_Range += 1.f;
			else if (zDelta < 0)
			{
				m_fLightInfo_PL_Range -= 1.0f;
				if (m_fLightInfo_PL_Range < 0.0f)
					m_fLightInfo_PL_Range = 0.0f;
			}

			// Modify Mode일 경우 변경된 값 반영.
			if (m_bIsLightingModifyMode)
			{
				if (nullptr != pSelectLight)
				{
					Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
					tLightInfo.Range = m_fLightInfo_PL_Range;
					pSelectLight->Set_LightInfo(tLightInfo);
					pSelectLight->Set_ColliderColorSelected();
				}
			}
		}

	}


	UpdateData(FALSE);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


HRESULT CTabMap::Ready_TerrainControl()
{
	/*__________________________________________________________________________________________________________
	[ TERRAIN ]
	____________________________________________________________________________________________________________*/
	UpdateData(TRUE);

	// 컨트롤 활성화
	m_TerrainRadio128.EnableWindow(TRUE);
	m_TerrainRadio256.EnableWindow(TRUE);
	m_TerrainRadio512.EnableWindow(TRUE);
	m_TerrainCheckBox_RenderWireFrame.EnableWindow(TRUE);
	m_TerrainListBox_TexIndex.EnableWindow(TRUE);

	// Terrain 128X128 활성화.
	m_TerrainRadio128.SetCheck(false);

	m_TerrainRadio256.SetCheck(true);
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	m_pTerrain256->m_bIsUpdate = true;

	m_TerrainRadio512.SetCheck(false);


	// Render WireFrame 활성화.
	m_TerrainCheckBox_RenderWireFrame.SetCheck(true);

	// ListBox 초기화.
	Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(L"Terrain", Engine::COMPONENTID::ID_STATIC));
	for (_uint i = 0; i < pTextureCom->Get_TexSize(); ++i)
	{
		_tchar m_szText[MAX_STR] = L"";
		wsprintf(m_szText, L"Index : %d", i);

		m_TerrainListBox_TexIndex.AddString(m_szText);
	}

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_SkyBoxControl()
{
	/*__________________________________________________________________________________________________________
	[ SkyBox ]
	____________________________________________________________________________________________________________*/
	UpdateData(TRUE);

	// 컨트롤 활성화.
	m_SkyBoxEdit_PosX.EnableWindow(TRUE);
	m_SkyBoxEdit_PosY.EnableWindow(TRUE);
	m_SkyBoxEdit_PosZ.EnableWindow(TRUE);
	m_SkyBoxEdit_Scale.EnableWindow(TRUE);
	m_SkyBoxListBox_TexIndex.EnableWindow(TRUE);
	m_SkyBoxCheckBox_RenderOnOff.EnableWindow(TRUE);

	// ListBox 초기화.
	Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(L"SkyBox", Engine::COMPONENTID::ID_STATIC));
	for (_uint i = 0; i < pTextureCom->Get_TexSize(); ++i)
	{
		_tchar m_szText[MAX_STR] = L"";
		wsprintf(m_szText, L"Index : %d", i);

		m_SkyBoxListBox_TexIndex.AddString(m_szText);
	}

	// EditControl 초기화.
	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	m_fSkyBox_PosX	= pSkyBox->Get_Transform()->m_vPos.x;
	m_fSkyBox_PosY	= pSkyBox->Get_Transform()->m_vPos.y;
	m_fSkyBox_PosZ	= pSkyBox->Get_Transform()->m_vPos.z;
	m_fSkyBox_Scale = pSkyBox->Get_Transform()->m_vScale.x;

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_EditControl()
{
	UpdateData(TRUE);

	// EditCheck 컨트롤 활성화.
	m_EditCheck_StaticMesh.EnableWindow(TRUE);
	m_EditCheck_LightingInfo.EnableWindow(TRUE);
	m_EditCheck_NavigationMesh.EnableWindow(TRUE);

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_StaticMeshControl()
{
	UpdateData(TRUE);

	// StaticMesh
	m_EditCheck_StaticMesh.SetCheck(true);
	m_StaticMeshRadio_CreateMode.EnableWindow(TRUE);
	m_StaticMeshRadio_CreateMode.SetCheck(true);
	m_StaticMeshRadio_ModifyMode.EnableWindow(TRUE);
	m_StaticMeshRadio_ModifyMode.SetCheck(false);
	m_bIsCreateMode = true;
	m_bIsModifyMode = false;

	m_StaticMeshTree_ResourceTree.EnableWindow(TRUE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(TRUE);
	m_StaticMeshListBox_ObjectList.EnableWindow(TRUE);
	m_StaticMeshButton_DeleteObject.EnableWindow(TRUE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleX.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleY.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleX.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleY.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_PosX.EnableWindow(TRUE);
	m_StaticMeshEdit_PosY.EnableWindow(TRUE);
	m_StaticMeshEdit_PosZ.EnableWindow(TRUE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(TRUE);
	
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(TRUE);
	m_StaticMeshCheck_IsCollision.EnableWindow(TRUE);
	m_StaticMeshCheck_IsMousePicking.EnableWindow(TRUE);
	m_StaticMeshCheck_IsMousePicking.SetCheck(true);
	m_bIsMousePicking = true;

	m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(TRUE);
	m_StaticMeshButton_Load.EnableWindow(TRUE);

	// Ready Mesh TreeControl.

	HTREEITEM h_DynamciMesh, h_DynamicMeshRoot;
	HTREEITEM h_StaticMesh, h_StaticMeshRoot;

	h_DynamciMesh	= m_StaticMeshTree_ResourceTree.InsertItem(L"DynamicMesh", NULL, NULL);
	h_StaticMesh	= m_StaticMeshTree_ResourceTree.InsertItem(L"StaticMesh", NULL, NULL);

	wifstream fin { L"../../Bin/ToolData/MeshTreeCtrlInfo.txt" };
	if (fin.fail())
		return E_FAIL;

	_tchar	szCurMeshType[MIN_STR]	= L"";
	_tchar	szCurMeshRoot[MIN_STR]	= L"";
	_tchar	szMeshTag[MAX_STR]		= L"";

	wstring wstrPreMeshType			= L"";
	wstring wstrPreMeshRoot			= L"";

	while (true)
	{
		fin.getline(szCurMeshType, MIN_STR, '|');
		fin.getline(szCurMeshRoot, MIN_STR, '|');
		fin.getline(szMeshTag, MIN_STR);

		if (fin.eof())
			break;

		wstring wstrCurMeshType = szCurMeshType;
		wstring wstrCurMeshRoot = szCurMeshRoot;

		// 이전의 MeshRootTag값이 현재의 MeshRootTag값과 다르면 TreeCtrl에 삽입.
		if (L"DynamicMesh" == wstrCurMeshType)
		{
			if (wstrPreMeshRoot != wstrCurMeshRoot)
			{
				wstrPreMeshRoot = wstrCurMeshRoot;
				h_DynamicMeshRoot = m_StaticMeshTree_ResourceTree.InsertItem(wstrPreMeshRoot.c_str(), h_DynamciMesh, NULL);
			}

			m_StaticMeshTree_ResourceTree.InsertItem(szMeshTag, h_DynamicMeshRoot, NULL);
		}

		if (L"StaticMesh" == wstrCurMeshType)
		{
			if (wstrPreMeshRoot != wstrCurMeshRoot)
			{
				wstrPreMeshRoot = wstrCurMeshRoot;
				h_StaticMeshRoot = m_StaticMeshTree_ResourceTree.InsertItem(wstrPreMeshRoot.c_str(), h_StaticMesh, NULL);
			}

			m_StaticMeshTree_ResourceTree.InsertItem(szMeshTag, h_StaticMeshRoot, NULL);
		}


	}

	fin.close();


	// 모든 트리의 노드를 펼친다.
	m_StaticMeshTree_ResourceTree.Expand(h_DynamciMesh, TVE_EXPAND);

	// DynamicMesh
	HTREEITEM h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_DynamciMesh, TVGN_CHILD);
	m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);

	while (h_Child != NULL)
	{
		h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_Child, TVGN_NEXT);
		m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);
	}

	// StaticMesh
	m_StaticMeshTree_ResourceTree.Expand(h_StaticMesh, TVE_EXPAND);

	h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_StaticMesh, TVGN_CHILD);
	m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);

	while (h_Child != NULL)
	{
		h_Child = m_StaticMeshTree_ResourceTree.GetNextItem(h_Child, TVGN_NEXT);
		m_StaticMeshTree_ResourceTree.Expand(h_Child, TVE_EXPAND);
	}

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_LightingInfoContorl()
{
	UpdateData(TRUE);

	m_EditCheck_LightingInfo.SetCheck(false);

	m_LightInfoEdit_DL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionX.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionY.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionZ.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionW.EnableWindow(FALSE);

	m_LightInfoEdit_PL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosX.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosY.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosZ.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosW.EnableWindow(FALSE);
	m_LightInfoEdit_PL_Range.EnableWindow(FALSE);

	m_LightInfoRadio_PL_CreateMode.EnableWindow(FALSE);
	m_LightInfoRadio_PL_ModifyMode.EnableWindow(FALSE);
	m_LightInfoRadio_PL_CreateMode.SetCheck(true);
	m_LightInfoRadio_PL_ModifyMode.SetCheck(false);
	m_bIsLightingCreateMode = true;
	m_bIsLightingModifyMode = false;
	m_LightInfoListBox_PL_List.EnableWindow(FALSE);
	m_LightInfoButton_PL_DELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_ALLDELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_PL_LOAD.EnableWindow(FALSE);

	m_LightInfoEdit_SL_EyeX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Height.EnableWindow(FALSE);
	m_LightInfoEdit_SL_FovY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Near.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Far.EnableWindow(FALSE);
	m_LightInfoButton_SL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_SL_LOAD.EnableWindow(FALSE);

	// Direction Light 정보 초기화.
	m_fLightInfo_DL_DiffuseR	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Diffuse.x;
	m_fLightInfo_DL_DiffuseG	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Diffuse.y;
	m_fLightInfo_DL_DiffuseB	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Diffuse.z;
	m_fLightInfo_DL_DiffuseA	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Diffuse.w;
	m_fLightInfo_DL_SpecularR	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Specular.x;
	m_fLightInfo_DL_SpecularG	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Specular.y;
	m_fLightInfo_DL_SpecularB	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Specular.z;
	m_fLightInfo_DL_SpecularA	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Specular.w;
	m_fLightInfo_DL_AmbientR	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Ambient.x;
	m_fLightInfo_DL_AmbientG	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Ambient.y;
	m_fLightInfo_DL_AmbientB	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Ambient.z;
	m_fLightInfo_DL_AmbientA	= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Ambient.w;
	m_fLightInfo_DL_DirX		= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Direction.x;
	m_fLightInfo_DL_DirY		= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Direction.y;
	m_fLightInfo_DL_DirZ		= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Direction.z;
	m_fLightInfo_DL_DirW		= Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0).Direction.w;

	// Shadow Light 정보 초기화.
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_fLightInfo_SL_EyeX	= CShadowLightMgr::Get_Instance()->m_vLightEye.x;
	m_fLightInfo_SL_EyeY	= CShadowLightMgr::Get_Instance()->m_vLightEye.y;
	m_fLightInfo_SL_EyeZ	= CShadowLightMgr::Get_Instance()->m_vLightEye.z;
	m_fLightInfo_SL_AtX		= CShadowLightMgr::Get_Instance()->m_vLightAt.x;
	m_fLightInfo_SL_AtY		= CShadowLightMgr::Get_Instance()->m_vLightAt.y;
	m_fLightInfo_SL_AtZ		= CShadowLightMgr::Get_Instance()->m_vLightAt.z;
	m_fLightInfo_SL_Height	= CShadowLightMgr::Get_Instance()->m_fHeight;
	m_fLightInfo_SL_FovY	= CShadowLightMgr::Get_Instance()->m_fFovY;
	m_fLightInfo_SL_Near	= CShadowLightMgr::Get_Instance()->m_fNear;
	m_fLightInfo_SL_Far		= CShadowLightMgr::Get_Instance()->m_fFar;

	UpdateData(FALSE);

	return S_OK;
}

HRESULT CTabMap::Ready_NavigationMeshControl()
{
	m_NaviMeshRadio_CreateMode.EnableWindow(FALSE);
	m_NaviMeshRadio_CreateMode.SetCheck(true);
	m_NaviMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_NaviMeshRadio_ModifyMode.SetCheck(false);
	m_bIsNaviCreateMode = true;
	m_bIsNaviModifyMode = false;

	m_NaviMeshRadio_CreateMode.EnableWindow(FALSE);
	m_NaviMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_NaviMeshCheck_FindNearPoint.EnableWindow(FALSE);
	m_NaviMeshListBox_CellList.EnableWindow(FALSE);
	m_NaviMeshButton_Delete.EnableWindow(FALSE);
	m_NaviMeshButton_AllDelete.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Z.EnableWindow(FALSE);
	m_NaviMeshButton_SAVE.EnableWindow(FALSE);
	m_NaviMeshButton_LOAD.EnableWindow(FALSE);

	m_fNaviMeshPointA_X = 0.0f;
	m_fNaviMeshPointA_Y = 0.0f;
	m_fNaviMeshPointA_Z = 0.0f;
	
	m_fNaviMeshPointB_X = 0.0f;
	m_fNaviMeshPointB_Y = 0.0f;
	m_fNaviMeshPointB_Z = 0.0f;
	
	m_fNaviMeshPointC_X = 0.0f;
	m_fNaviMeshPointC_Y = 0.0f;
	m_fNaviMeshPointC_Z = 0.0f;

	return S_OK;
}



void CTabMap::OnBnClickedRadio1001_Terrain128()
{
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));
	
	m_pTerrain128->m_bIsUpdate = true;
	m_pTerrain256->m_bIsUpdate = false;
	m_pTerrain512->m_bIsUpdate = false;

	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingTerrain = m_pTerrain128;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1002_Terrain256()
{
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	m_pTerrain128->m_bIsUpdate = false;
	m_pTerrain256->m_bIsUpdate = true;
	m_pTerrain512->m_bIsUpdate = false;

	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingTerrain = m_pTerrain256;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1003_Terrain512()
{
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	m_pTerrain128->m_bIsUpdate = false;
	m_pTerrain256->m_bIsUpdate = false;
	m_pTerrain512->m_bIsUpdate = true;

	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingTerrain = m_pTerrain512;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1001_TerrainRenderWireFrame()
{
	UpdateData(TRUE);

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	if (m_TerrainCheckBox_RenderWireFrame.GetCheck())
	{
		// WireFrame
		m_pTerrain128->m_pShaderCom->Set_PipelineStatePass(1);
		m_pTerrain256->m_pShaderCom->Set_PipelineStatePass(1);
		m_pTerrain512->m_pShaderCom->Set_PipelineStatePass(1);
	}
	else
	{
		// Solid
		m_pTerrain128->m_pShaderCom->Set_PipelineStatePass(0);
		m_pTerrain256->m_pShaderCom->Set_PipelineStatePass(0);
		m_pTerrain512->m_pShaderCom->Set_PipelineStatePass(0);
	}

	UpdateData(FALSE);
}


void CTabMap::OnLbnSelchangeList1001_TerrainTexIndex()
{
	UpdateData(TRUE);


	// 선택한 ListBox의 Index를 얻어온다.
	_int iNewTexIdx = m_TerrainListBox_TexIndex.GetCaretIndex();

	CToolTerrain* m_pTerrain128 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex128"));
	CToolTerrain* m_pTerrain256 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex256"));
	CToolTerrain* m_pTerrain512 = static_cast<CToolTerrain*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"TerrainTex512"));

	m_pTerrain128->m_uiTexIdx = iNewTexIdx;
	m_pTerrain256->m_uiTexIdx = iNewTexIdx;
	m_pTerrain512->m_uiTexIdx = iNewTexIdx;

	UpdateData(FALSE);

}


void CTabMap::OnLbnSelchangeList1002_SkyBoxTexIndex()
{
	UpdateData(TRUE);

	// 선택한 ListBox의 Index를 얻어온다.
	_int iNewTexIdx = m_SkyBoxListBox_TexIndex.GetCaretIndex();

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->m_uiTexIdx = iNewTexIdx;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1004_SkyBoxRenderOnOff()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	if (m_SkyBoxCheckBox_RenderOnOff.GetCheck())
		pSkyBox->m_bIsUpdate = true;
	else
		pSkyBox->m_bIsUpdate = false;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1001_SkyBoxPosX()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.x = m_fSkyBox_PosX;

	UpdateData(TRUE);
}


void CTabMap::OnEnChangeEdit1002_SkyBoxPosY()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.y = m_fSkyBox_PosY;

	UpdateData(FALSE);

}


void CTabMap::OnEnChangeEdit1003_SkyBoxPosZ()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vPos.z = m_fSkyBox_PosZ;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1004__SkyBoxScale()
{
	UpdateData(TRUE);

	CToolSkyBox* pSkyBox = static_cast<CToolSkyBox*>(m_pObjectMgr->Get_GameObject(L"Layer_Environment", L"SkyBox"));
	pSkyBox->Get_Transform()->m_vScale.x = m_fSkyBox_Scale;
	pSkyBox->Get_Transform()->m_vScale.y = m_fSkyBox_Scale;
	pSkyBox->Get_Transform()->m_vScale.z = m_fSkyBox_Scale;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1005_EditStaticMesh()
{
	UpdateData(TRUE);

	m_EditCheck_StaticMesh.SetCheck(true);
	m_EditCheck_LightingInfo.SetCheck(false);
	m_EditCheck_NavigationMesh.SetCheck(false);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(TRUE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(TRUE);
	m_StaticMeshTree_ResourceTree.EnableWindow(TRUE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(TRUE);
	m_StaticMeshListBox_ObjectList.EnableWindow(TRUE);
	m_StaticMeshButton_DeleteObject.EnableWindow(TRUE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleX.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleY.EnableWindow(TRUE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleX.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleY.EnableWindow(TRUE);
	m_StaticMeshEdit_AngleZ.EnableWindow(TRUE);
	m_StaticMeshEdit_PosX.EnableWindow(TRUE);
	m_StaticMeshEdit_PosY.EnableWindow(TRUE);
	m_StaticMeshEdit_PosZ.EnableWindow(TRUE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(TRUE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(TRUE);
	m_StaticMeshCheck_IsCollision.EnableWindow(TRUE);
	m_StaticMeshCheck_IsMousePicking.EnableWindow(TRUE);
	m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(TRUE);
	m_StaticMeshButton_Load.EnableWindow(TRUE);

	// Lighting
	m_LightInfoEdit_DL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionX.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionY.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionZ.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionW.EnableWindow(FALSE);

	m_LightInfoEdit_PL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosX.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosY.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosZ.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosW.EnableWindow(FALSE);
	m_LightInfoEdit_PL_Range.EnableWindow(FALSE);
	m_LightInfoRadio_PL_CreateMode.EnableWindow(FALSE);
	m_LightInfoRadio_PL_ModifyMode.EnableWindow(FALSE);
	m_LightInfoListBox_PL_List.EnableWindow(FALSE);
	m_LightInfoButton_PL_DELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_ALLDELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_PL_LOAD.EnableWindow(FALSE);

	m_LightInfoEdit_SL_EyeX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Height.EnableWindow(FALSE);
	m_LightInfoEdit_SL_FovY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Near.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Far.EnableWindow(FALSE);
	m_LightInfoButton_SL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_SL_LOAD.EnableWindow(FALSE);

	// NavigationMesh
	m_NaviMeshRadio_CreateMode.EnableWindow(FALSE);
	m_NaviMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_NaviMeshCheck_FindNearPoint.EnableWindow(FALSE);
	m_NaviMeshListBox_CellList.EnableWindow(FALSE);
	m_NaviMeshButton_Delete.EnableWindow(FALSE);
	m_NaviMeshButton_AllDelete.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Z.EnableWindow(FALSE);
	m_NaviMeshButton_SAVE.EnableWindow(FALSE);
	m_NaviMeshButton_LOAD.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1006_EditLightingInfo()
{

	UpdateData(TRUE);

	m_EditCheck_StaticMesh.SetCheck(false);
	m_EditCheck_LightingInfo.SetCheck(true);
	m_EditCheck_NavigationMesh.SetCheck(false);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(FALSE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_StaticMeshTree_ResourceTree.EnableWindow(FALSE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(FALSE);
	m_StaticMeshListBox_ObjectList.EnableWindow(FALSE);
	m_StaticMeshButton_DeleteObject.EnableWindow(FALSE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleX.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleY.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleX.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleY.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_PosX.EnableWindow(FALSE);
	m_StaticMeshEdit_PosY.EnableWindow(FALSE);
	m_StaticMeshEdit_PosZ.EnableWindow(FALSE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(FALSE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(FALSE);
	m_StaticMeshCheck_IsCollision.EnableWindow(FALSE);
	m_StaticMeshCheck_IsMousePicking.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(FALSE);
	m_StaticMeshButton_Load.EnableWindow(FALSE);

	// Lighting
	m_LightInfoEdit_DL_DiffuseR.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DiffuseG.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DiffuseB.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DiffuseA.EnableWindow(TRUE);
	m_LightInfoEdit_DL_SpecularR.EnableWindow(TRUE);
	m_LightInfoEdit_DL_SpecularG.EnableWindow(TRUE);
	m_LightInfoEdit_DL_SpecularB.EnableWindow(TRUE);
	m_LightInfoEdit_DL_SpecularA.EnableWindow(TRUE);
	m_LightInfoEdit_DL_AmbientR.EnableWindow(TRUE);
	m_LightInfoEdit_DL_AmbientG.EnableWindow(TRUE);
	m_LightInfoEdit_DL_AmbientB.EnableWindow(TRUE);
	m_LightInfoEdit_DL_AmbientA.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DirectionX.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DirectionY.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DirectionZ.EnableWindow(TRUE);
	m_LightInfoEdit_DL_DirectionW.EnableWindow(TRUE);

	m_LightInfoEdit_PL_DiffuseR.EnableWindow(TRUE);
	m_LightInfoEdit_PL_DiffuseG.EnableWindow(TRUE);
	m_LightInfoEdit_PL_DiffuseB.EnableWindow(TRUE);
	m_LightInfoEdit_PL_DiffuseA.EnableWindow(TRUE);
	m_LightInfoEdit_PL_SpecularR.EnableWindow(TRUE);
	m_LightInfoEdit_PL_SpecularG.EnableWindow(TRUE);
	m_LightInfoEdit_PL_SpecularB.EnableWindow(TRUE);
	m_LightInfoEdit_PL_SpecularA.EnableWindow(TRUE);
	m_LightInfoEdit_PL_AnbientR.EnableWindow(TRUE);
	m_LightInfoEdit_PL_AnbientG.EnableWindow(TRUE);
	m_LightInfoEdit_PL_AnbientB.EnableWindow(TRUE);
	m_LightInfoEdit_PL_AnbientA.EnableWindow(TRUE);
	m_LightInfoEdit_PL_PosX.EnableWindow(TRUE);
	m_LightInfoEdit_PL_PosY.EnableWindow(TRUE);
	m_LightInfoEdit_PL_PosZ.EnableWindow(TRUE);
	m_LightInfoEdit_PL_PosW.EnableWindow(TRUE);
	m_LightInfoEdit_PL_Range.EnableWindow(TRUE);
	m_LightInfoRadio_PL_CreateMode.EnableWindow(TRUE);
	m_LightInfoRadio_PL_ModifyMode.EnableWindow(TRUE);
	m_LightInfoRadio_PL_CreateMode.SetCheck(true);
	m_LightInfoRadio_PL_ModifyMode.SetCheck(false);
	m_bIsLightingCreateMode = true;
	m_bIsLightingModifyMode = false;

	m_LightInfoListBox_PL_List.EnableWindow(TRUE);
	m_LightInfoButton_PL_DELETE.EnableWindow(TRUE);
	m_LightInfoButton_PL_ALLDELETE.EnableWindow(TRUE);
	m_LightInfoButton_PL_SAVE.EnableWindow(TRUE);
	m_LightInfoButton_PL_LOAD.EnableWindow(TRUE);

	m_LightInfoEdit_SL_EyeX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtX.EnableWindow(TRUE);
	m_LightInfoEdit_SL_AtY.EnableWindow(TRUE);
	m_LightInfoEdit_SL_AtZ.EnableWindow(TRUE);
	m_LightInfoEdit_SL_Height.EnableWindow(TRUE);
	m_LightInfoEdit_SL_FovY.EnableWindow(TRUE);
	m_LightInfoEdit_SL_Near.EnableWindow(TRUE);
	m_LightInfoEdit_SL_Far.EnableWindow(TRUE);
	m_LightInfoButton_SL_SAVE.EnableWindow(TRUE);
	m_LightInfoButton_SL_LOAD.EnableWindow(TRUE);

	m_fLightInfo_PL_DiffuseR	= 1.0f;
	m_fLightInfo_PL_DiffuseG	= 1.0f;
	m_fLightInfo_PL_DiffuseB	= 1.0f;
	m_fLightInfo_PL_DiffuseA	= 1.0f;
	m_fLightInfo_PL_SpecularR	= 0.5f;
	m_fLightInfo_PL_SpecularG	= 0.5f;
	m_fLightInfo_PL_SpecularB	= 0.5f;
	m_fLightInfo_PL_SpecularA	= 1.0f;
	m_fLightInfo_PL_AmbientR	= 0.5f;
	m_fLightInfo_PL_AmbientG	= 0.5f;
	m_fLightInfo_PL_AmbientB	= 0.5f;
	m_fLightInfo_PL_AmbientA	= 1.0f;
	m_fLightInfo_PL_PosX		= 0.0f;
	m_fLightInfo_PL_PosY		= 0.0f;
	m_fLightInfo_PL_PosZ		= 0.0f;
	m_fLightInfo_PL_PosW		= 1.0f;
	m_fLightInfo_PL_Range		= 10.0f;


	// NavigationMesh
	m_NaviMeshRadio_CreateMode.EnableWindow(FALSE);
	m_NaviMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_NaviMeshCheck_FindNearPoint.EnableWindow(FALSE);
	m_NaviMeshListBox_CellList.EnableWindow(FALSE);
	m_NaviMeshButton_Delete.EnableWindow(FALSE);
	m_NaviMeshButton_AllDelete.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointA_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointB_Z.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_X.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Y.EnableWindow(FALSE);
	m_NaviMeshEdit_PointC_Z.EnableWindow(FALSE);
	m_NaviMeshButton_SAVE.EnableWindow(FALSE);
	m_NaviMeshButton_LOAD.EnableWindow(FALSE);

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1007_EditNavigationMesh()
{
	UpdateData(TRUE);

	m_EditCheck_StaticMesh.SetCheck(false);
	m_EditCheck_LightingInfo.SetCheck(false);
	m_EditCheck_NavigationMesh.SetCheck(true);

	// StaticMesh
	m_StaticMeshRadio_CreateMode.EnableWindow(FALSE);
	m_StaticMeshRadio_ModifyMode.EnableWindow(FALSE);
	m_StaticMeshTree_ResourceTree.EnableWindow(FALSE);
	m_StaticMeshEdit_SelectMesthTag.EnableWindow(FALSE);
	m_StaticMeshListBox_ObjectList.EnableWindow(FALSE);
	m_StaticMeshButton_DeleteObject.EnableWindow(FALSE);
	m_StaticMeshButton_AllDeleteObject.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleX.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleY.EnableWindow(FALSE);
	m_StaticMeshEdit_ScaleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleX.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleY.EnableWindow(FALSE);
	m_StaticMeshEdit_AngleZ.EnableWindow(FALSE);
	m_StaticMeshEdit_PosX.EnableWindow(FALSE);
	m_StaticMeshEdit_PosY.EnableWindow(FALSE);
	m_StaticMeshEdit_PosZ.EnableWindow(FALSE);
	m_StaticMeshEdit_ObjectSize.EnableWindow(FALSE);
	m_StaticMeshCheck_IsRenderShadow.EnableWindow(FALSE);
	m_StaticMeshCheck_IsCollision.EnableWindow(FALSE);
	m_StaticMeshCheck_IsMousePicking.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
	m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
	m_StaticMeshButton_Save.EnableWindow(FALSE);
	m_StaticMeshButton_Load.EnableWindow(FALSE);

	// Lighting
	m_LightInfoEdit_DL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientR.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientG.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientB.EnableWindow(FALSE);
	m_LightInfoEdit_DL_AmbientA.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionX.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionY.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionZ.EnableWindow(FALSE);
	m_LightInfoEdit_DL_DirectionW.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_DiffuseA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_SpecularA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientR.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientG.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientB.EnableWindow(FALSE);
	m_LightInfoEdit_PL_AnbientA.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosX.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosY.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosZ.EnableWindow(FALSE);
	m_LightInfoEdit_PL_PosW.EnableWindow(FALSE);
	m_LightInfoEdit_PL_Range.EnableWindow(FALSE);
	m_LightInfoRadio_PL_CreateMode.EnableWindow(FALSE);
	m_LightInfoRadio_PL_ModifyMode.EnableWindow(FALSE);
	m_LightInfoListBox_PL_List.EnableWindow(FALSE);
	m_LightInfoButton_PL_DELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_ALLDELETE.EnableWindow(FALSE);
	m_LightInfoButton_PL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_PL_LOAD.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_EyeZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtX.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_AtZ.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Height.EnableWindow(FALSE);
	m_LightInfoEdit_SL_FovY.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Near.EnableWindow(FALSE);
	m_LightInfoEdit_SL_Far.EnableWindow(FALSE);
	m_LightInfoButton_SL_SAVE.EnableWindow(FALSE);
	m_LightInfoButton_SL_LOAD.EnableWindow(FALSE);

	// NavigationMesh
	m_NaviMeshRadio_CreateMode.EnableWindow(TRUE);
	m_NaviMeshRadio_ModifyMode.EnableWindow(TRUE);
	m_NaviMeshCheck_FindNearPoint.EnableWindow(TRUE);
	m_NaviMeshListBox_CellList.EnableWindow(TRUE);
	m_NaviMeshButton_Delete.EnableWindow(TRUE);
	m_NaviMeshButton_AllDelete.EnableWindow(TRUE);
	m_NaviMeshEdit_PointA_X.EnableWindow(TRUE);
	m_NaviMeshEdit_PointA_Y.EnableWindow(TRUE);
	m_NaviMeshEdit_PointA_Z.EnableWindow(TRUE);
	m_NaviMeshEdit_PointB_X.EnableWindow(TRUE);
	m_NaviMeshEdit_PointB_Y.EnableWindow(TRUE);
	m_NaviMeshEdit_PointB_Z.EnableWindow(TRUE);
	m_NaviMeshEdit_PointC_X.EnableWindow(TRUE);
	m_NaviMeshEdit_PointC_Y.EnableWindow(TRUE);
	m_NaviMeshEdit_PointC_Z.EnableWindow(TRUE);
	m_NaviMeshButton_SAVE.EnableWindow(TRUE);
	m_NaviMeshButton_LOAD.EnableWindow(TRUE);

	UpdateData(FALSE);
}

void CTabMap::OnBnClickedRadio1005_StaticMeshCreateMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_StaticMeshRadio_CreateMode.SetCheck(true);
	m_StaticMeshRadio_ModifyMode.SetCheck(false);
	m_bIsCreateMode = true;
	m_bIsModifyMode = false;

	m_fStaticMeshScaleX = 0.01f;
	m_fStaticMeshScaleY = 0.01f;
	m_fStaticMeshScaleZ = 0.01f;
	m_fStaticMeshAngleX = 0.0f;
	m_fStaticMeshAngleY = 0.0f;
	m_fStaticMeshAngleZ = 0.0f;

	m_fStaticMeshColliderScale	= 0.0f;
	m_fStaticMeshColliderPosX	= 0.0f;
	m_fStaticMeshColliderPosY	= 0.0f;
	m_fStaticMeshColliderPosZ	= 0.0f;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1006_StaticMeshModifyeMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_StaticMeshRadio_CreateMode.SetCheck(false);
	m_StaticMeshRadio_ModifyMode.SetCheck(true);
	m_bIsCreateMode = false;
	m_bIsModifyMode = true;

	UpdateData(FALSE);
}


void CTabMap::OnNMClickTree1001_TreeMeshTag(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CPoint mouse;
	GetCursorPos(&mouse);
	m_StaticMeshTree_ResourceTree.ScreenToClient(&mouse);

	_uint iFlag = 0;
	HTREEITEM h_MeshTag = m_StaticMeshTree_ResourceTree.HitTest(mouse, &iFlag);

	// 클릭한 Tree의 문자열을 얻어온다.
	m_wstrTreeMeshTag = m_StaticMeshTree_ResourceTree.GetItemText(h_MeshTag);

	// 클릭한 문자열 설정.
	m_StaticMeshEdit_SelectMesthTag.SetWindowTextW(m_wstrTreeMeshTag.c_str());

	*pResult = 0;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1005_StaticMeshScaleX()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vScale.x = m_fStaticMeshScaleX;
		}
	}

	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1006_StaticMeshScaleY()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vScale.y = m_fStaticMeshScaleY;
		}
	}

	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1007_StaticMeshScaleZ()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vScale.z = m_fStaticMeshScaleZ;
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1008_StaticMeshAngleX()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vAngle.x = m_fStaticMeshAngleX;
		}
	}

	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1009_StaticMeshAngleY()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vAngle.y = m_fStaticMeshAngleY;
		}
	}

	UpdateData(FALSE);
}



void CTabMap::OnEnChangeEdit1010_StaticMeshAngleZ()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vAngle.z = m_fStaticMeshAngleZ;
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1011_StaticMeshPosX()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vPos.x = m_fStaticMeshPosX;
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1012_StaticMeshPosY()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vPos.y = m_fStaticMeshPosY;
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1013_StaticMeshPosZ()
{
	UpdateData(TRUE);

	// 현재 모드가 ModifyMode일 경우.
	if (m_bIsModifyMode)
	{
		// 선택한 Object가 nullptr이 아닐 경우 값 변경.
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (nullptr != pObject)
		{
			pObject->Get_Transform()->m_vPos.z = m_fStaticMeshPosZ;
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1002_StaticMeshRenderShadow()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_StaticMeshCheck_IsRenderShadow.GetCheck())
		m_bIsRenderShadow = true;
	else
		m_bIsRenderShadow = false;

	if (m_bIsModifyMode)
	{
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		if (pObject != nullptr)
		{
			pObject->Set_IsRenderShadow(m_bIsRenderShadow);
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedCheck1003_StaticMeshIsCollision()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);


	if (m_StaticMeshCheck_IsCollision.GetCheck())
	{
		m_bIsCollision = true;
		m_StaticMeshEdit_ColliderScale.EnableWindow(TRUE);
		m_StaticMeshEdit_ColliderPosX.EnableWindow(TRUE);
		m_StaticMeshEdit_ColliderPosY.EnableWindow(TRUE);
		m_StaticMeshEdit_ColliderPosZ.EnableWindow(TRUE);

		if (m_bIsModifyMode)
		{
			Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
			if (pObject != nullptr)
			{

				pObject->Set_IsCollision(m_bIsCollision);
				pObject->Get_BoundingSphere()->Get_Transform()->m_vScale	= _vec3(m_fStaticMeshColliderScale);
				pObject->Get_BoundingSphere()->Get_Transform()->m_vPos.x	= m_fStaticMeshColliderPosX;
				pObject->Get_BoundingSphere()->Get_Transform()->m_vPos.y	= m_fStaticMeshColliderPosY;
				pObject->Get_BoundingSphere()->Get_Transform()->m_vPos.z	= m_fStaticMeshColliderPosZ;
			}
		}
	}
	else
	{
		m_bIsCollision = false;
		m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
		m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
		m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
		m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
		m_fStaticMeshColliderScale	= 0.0f;
		m_fStaticMeshColliderPosX	= 0.0f;
		m_fStaticMeshColliderPosY	= 0.0f;
		m_fStaticMeshColliderPosZ	= 0.0f;

		if (m_bIsModifyMode)
		{
			Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
			if (pObject != nullptr)
			{
				pObject->Set_IsCollision(m_bIsCollision);
			}
		}
	}

	UpdateData(FALSE);

}

void CTabMap::OnBnClickedCheck1008_StaticMeshIsMousePicking()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_StaticMeshCheck_IsMousePicking.GetCheck())
	{
		m_bIsMousePicking = true;
		m_StaticMeshCheck_IsMousePicking.SetCheck(true);
	}
	else
	{
		m_bIsMousePicking = false;
		m_StaticMeshCheck_IsMousePicking.SetCheck(false);
	}
	
	if (m_bIsModifyMode)
	{
		Engine::CGameObject* pObject = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject;
		static_cast<CToolStaticMesh*>(pObject)->m_bIsMousePicking = m_bIsMousePicking;
	}
}



void CTabMap::OnLbnSelchangeList1003_StaticMeshObjectList()
{
	UpdateData(TRUE);

	Engine::OBJLIST* pStaticMeshList = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");
	// BoundingBox 색상 Green으로 변경.
	for (auto& pObject : *pStaticMeshList)
		pObject->Set_BoundingBoxColor(_rgba(0.0f, 1.0f, 0.0f, 1.0f));


	// 선택한 ListBox의 Index를 얻어온다.
	m_iStaticMeshSelectIdx = m_StaticMeshListBox_ObjectList.GetCaretIndex();

	// 선택한 Object의 색상을 Red로 변경.
	Engine::CGameObject* pSelectedObject = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"StaticMesh", m_iStaticMeshSelectIdx);
	if (nullptr != pSelectedObject)
	{
		pSelectedObject->Set_BoundingBoxColor(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

		_tchar szTemp[MIN_STR] = L"";

		m_fStaticMeshScaleX = pSelectedObject->Get_Transform()->m_vScale.x;
		m_fStaticMeshScaleY = pSelectedObject->Get_Transform()->m_vScale.y;
		m_fStaticMeshScaleZ = pSelectedObject->Get_Transform()->m_vScale.z;
		m_fStaticMeshAngleX = pSelectedObject->Get_Transform()->m_vAngle.x;
		m_fStaticMeshAngleY = pSelectedObject->Get_Transform()->m_vAngle.y;
		m_fStaticMeshAngleZ = pSelectedObject->Get_Transform()->m_vAngle.z;
		m_fStaticMeshPosX = pSelectedObject->Get_Transform()->m_vPos.x;
		m_fStaticMeshPosY = pSelectedObject->Get_Transform()->m_vPos.y;
		m_fStaticMeshPosZ = pSelectedObject->Get_Transform()->m_vPos.z;

		m_bIsRenderShadow = pSelectedObject->Get_IsRenderShadow();
		if (m_bIsRenderShadow)
			m_StaticMeshCheck_IsRenderShadow.SetCheck(true);
		else
			m_StaticMeshCheck_IsRenderShadow.SetCheck(false);


		m_bIsCollision = pSelectedObject->Get_IsCollision();
		if (m_bIsCollision)
		{
			m_StaticMeshEdit_ColliderScale.EnableWindow(TRUE);
			m_StaticMeshEdit_ColliderPosX.EnableWindow(TRUE);
			m_StaticMeshEdit_ColliderPosY.EnableWindow(TRUE);
			m_StaticMeshEdit_ColliderPosZ.EnableWindow(TRUE);
		}
		else
		{
			m_StaticMeshEdit_ColliderScale.EnableWindow(FALSE);
			m_StaticMeshEdit_ColliderPosX.EnableWindow(FALSE);
			m_StaticMeshEdit_ColliderPosY.EnableWindow(FALSE);
			m_StaticMeshEdit_ColliderPosZ.EnableWindow(FALSE);
		}
		m_fStaticMeshColliderScale = pSelectedObject->Get_BoundingSphere()->Get_Transform()->m_vScale.x;
		m_fStaticMeshColliderPosX = pSelectedObject->Get_BoundingSphere()->Get_Transform()->m_vPos.x;
		m_fStaticMeshColliderPosY = pSelectedObject->Get_BoundingSphere()->Get_Transform()->m_vPos.y;
		m_fStaticMeshColliderPosZ = pSelectedObject->Get_BoundingSphere()->Get_Transform()->m_vPos.z;

		m_bIsMousePicking = static_cast<CToolStaticMesh*>(pSelectedObject)->m_bIsMousePicking;

		if (m_bIsMousePicking)
			m_StaticMeshCheck_IsMousePicking.SetCheck(true);
		else
			m_StaticMeshCheck_IsMousePicking.SetCheck(false);

		static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject = pSelectedObject;
	}

	m_iStaticMeshSelectIdx = -1;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1001_StasticMeshDelete()
{
	UpdateData(TRUE);

	// 선택한 StaticMeshObject 삭제. (Picking)
	if (m_iStaticMeshSelectIdx == -1)
	{
		if (nullptr != static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject)
			static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingObject->Set_DeadGameObject();

		m_StaticMeshListBox_ObjectList.ResetContent();
		if (!m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->empty())
		{
			for (auto& pObject : *(m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")))
			{
				if (!pObject->Get_IsDead())
					m_StaticMeshListBox_ObjectList.AddString(static_cast<CToolStaticMesh*>(pObject)->m_wstrMeshTag.c_str());
			}
		}
	}

	// 선택한 StaticMeshObject 삭제. (LishtBox Select)
	else if (!m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->empty() &&
		m_iStaticMeshSelectIdx < m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->size())
	{
		m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"StaticMesh", m_iStaticMeshSelectIdx)->Set_DeadGameObject();

		// ListBox와 Edit컨트롤 수정.
		m_StaticMeshListBox_ObjectList.DeleteString(m_iStaticMeshSelectIdx);
	}

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1002_StaticMeshAllDelete()
{
	// 모든 StaticMeshObject 삭제.
	if (!m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh")->empty())
	{
		Engine::OBJLIST* lstStaticMeshObject = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");

		for (auto& pStaticMesh : *lstStaticMeshObject)
			pStaticMesh->Set_DeadGameObject();

		// ListBox와 Edit컨트롤 수정.
		m_StaticMeshListBox_ObjectList.ResetContent();
	}

}

void CTabMap::OnBnClickedButton1003_StaticMeshSAVE()
{
	UpdateData(TRUE);

	Engine::OBJLIST* lstStaticMeshObject = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");
	if (nullptr == lstStaticMeshObject ||
		lstStaticMeshObject->empty())
		return;

	CFileDialog Dlg(FALSE,
					L"staticmesh",
					L"*.staticmesh",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.staticmesh) | *.staticmesh ||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wofstream fout { Dlg.GetPathName().GetString() };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		for (auto& pObject : *lstStaticMeshObject)
		{
			wstring wstrMeshTag			= static_cast<CToolStaticMesh*>(pObject)->m_wstrMeshTag;
			_vec3 vScale				= pObject->Get_Transform()->m_vScale;
			_vec3 vAngle				= pObject->Get_Transform()->m_vAngle;
			_vec3 vPos					= pObject->Get_Transform()->m_vPos;
			_bool bIsRenderShadow		= pObject->Get_IsRenderShadow();
			_bool bIsCollision			= pObject->Get_IsCollision();
			_vec3 vBoundingSphereScale	= pObject->Get_BoundingSphere()->Get_Transform()->m_vScale;
			_vec3 vBoundingSpherePos	= pObject->Get_BoundingSphere()->Get_Transform()->m_vPos;
			_bool bIsMousePicking		= static_cast<CToolStaticMesh*>(pObject)->m_bIsMousePicking;

			// StaticMesh Data 저장
			fout	<< wstrMeshTag				<< L" "	// MeshTag
					<< vScale.x					<< L" " 
					<< vScale.y					<< L" "	
					<< vScale.z					<< L" "	// Scale
					<< vAngle.x					<< L" "	
					<< vAngle.y					<< L" " 
					<< vAngle.z					<< L" "	// Angle
					<< vPos.x					<< L" " 
					<< vPos.y					<< L" " 
					<< vPos.z					<< L" "	// Pos
					<< bIsRenderShadow			<< L" "	// Is Render Shadow
					<< bIsCollision				<< L" "	// Is Collision
					<< vBoundingSphereScale.x	<< L" " 
					<< vBoundingSphereScale.y	<< L" " 
					<< vBoundingSphereScale.z	<< L" "	// BoundingSphere Scale
					<< vBoundingSpherePos.x		<< L" " 
					<< vBoundingSpherePos.y		<< L" " 
					<< vBoundingSpherePos.z		<< L" "	// BoundingSphere Pos
					<< bIsMousePicking			<< L" ";// Is MousePicking
		}


		AfxMessageBox(L"Data Save Successed");
	}

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1004_StaticMeshLOAD()
{
	UpdateData(TRUE);

	// ListBox초기화.
	m_StaticMeshListBox_ObjectList.ResetContent();

	// ToolStaticMesh ObjectList 삭제.
	Engine::OBJLIST* pOBJLIST = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"StaticMesh");
	if (nullptr != pOBJLIST)
	{
		for (auto& pObject : *pOBJLIST)
			pObject->Set_DeadGameObject();
	}

	CFileDialog Dlg(TRUE,
					L"staticmesh",
					L"*.staticmesh",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.staticmesh)|*.staticmesh||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wifstream fin { Dlg.GetPathName().GetString() };
		if (fin.fail())
		{
			AfxMessageBox(L"Load is Failed");
			return;
		}

		wstring	wstrMeshTag				= L"";
		_vec3	vScale					= _vec3(0.0f);
		_vec3	vAngle					= _vec3(0.0f);
		_vec3	vPos					= _vec3(0.0f);
		_bool	bIsRenderShadow			= false;
		_bool	bIsCollision			= false;
		_vec3	vBoundingSphereScale	= _vec3(0.0f);
		_vec3	vBoundingSpherePos      = _vec3(0.0f);
		_bool	bIsMousePicking			= false;
		while (true)
		{

			fin		>> wstrMeshTag 				// MeshTag
					>> vScale.x	
					>> vScale.y 
					>> vScale.z					// Scale
					>> vAngle.x	
					>> vAngle.y 
					>> vAngle.z					// Angle
					>> vPos.x 
					>> vPos.y 
					>> vPos.z					// Pos
					>> bIsRenderShadow			// Is Render Shadow
					>> bIsCollision 			// Is Collision
					>> vBoundingSphereScale.x	// BoundingSphere Scale
					>> vBoundingSphereScale.y 
					>> vBoundingSphereScale.z	
					>> vBoundingSpherePos.x		// BoundingSphere Pos
					>> vBoundingSpherePos.y 
					>> vBoundingSpherePos.z
					>> bIsMousePicking;			// Is MousePicking
			if (fin.eof())
				break;

			// StasticMesh 생성.
			CToolStaticMesh* pStaticMesh = nullptr;
			pStaticMesh = CToolStaticMesh::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(), 
												  Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMDID::CMD_MAIN),
												  wstrMeshTag,			// MeshTag
												  vScale,				// Scale
												  vAngle,				// Angle
												  vPos,					// Pos
												  bIsRenderShadow,		// Render Shadow
												  true,					// Bounding Box
												  bIsCollision,			// Bounding Sphere
												  vBoundingSphereScale,	// Bounding Sphere Scale
												  vBoundingSpherePos,	// Bounding Sphere Pos
												  bIsMousePicking);		// Is Mouse Picking
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"StaticMesh", pStaticMesh), E_FAIL);
			// StaticMeshListBox에 삽입.
			m_StaticMeshListBox_ObjectList.AddString(wstrMeshTag.c_str());
		}

		AfxMessageBox(L"Data Load Successed");
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1014_LightInfo_DL_DiffuseR()
{
	UpdateData(TRUE);

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Diffuse.x = m_fLightInfo_DL_DiffuseR;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	
	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1015_LightInfo_DL_DiffuseG()
{
	UpdateData(TRUE);

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Diffuse.y = m_fLightInfo_DL_DiffuseG;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1016_LightInfo_DL_DiffuseB()
{
	UpdateData(TRUE);
	
	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Diffuse.z = m_fLightInfo_DL_DiffuseB;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);


	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1017_LightInfo_DL_DiffuseA()
{
	UpdateData(TRUE);

	m_fLightInfo_DL_DiffuseA = 1.0f;

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Diffuse.w = m_fLightInfo_DL_DiffuseA;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1018_LightInfo_DL_SpecularR()
{
	UpdateData(TRUE);
	
	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Specular.x = m_fLightInfo_DL_SpecularR;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1019_LightInfo_DL_SpecularG()
{
	UpdateData(TRUE);

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Specular.y = m_fLightInfo_DL_SpecularG;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1020_LightInfo_DL_SpecularB()
{
	UpdateData(TRUE);
	
	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Specular.z = m_fLightInfo_DL_SpecularB;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1021_LightInfo_DL_SpecularA()
{
	UpdateData(TRUE);

	m_fLightInfo_DL_SpecularA = 1.0f;

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Specular.w = m_fLightInfo_DL_SpecularA;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1022_LightInfo_DL_AmbientR()
{
	UpdateData(TRUE);

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Ambient.x = m_fLightInfo_DL_AmbientR;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1023_LightInfo_DL_AmbientG()
{
	UpdateData(TRUE);
	
	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Ambient.y = m_fLightInfo_DL_AmbientG;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1024_LightInfo_DL_AmbientB()
{
	UpdateData(TRUE);
	
	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Ambient.z = m_fLightInfo_DL_AmbientB;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1025_LightInfo_DL_AmbientA()
{
	UpdateData(TRUE);

	m_fLightInfo_DL_AmbientA = 1.0f;

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Ambient.w = m_fLightInfo_DL_AmbientA;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1026_LightInfo_DL_DirX()
{
	UpdateData(TRUE);

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Direction.x = m_fLightInfo_DL_DirX;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
	m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
	m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1027_LightInfo_DL_DirY()
{
	UpdateData(TRUE);

	m_fLightInfo_DL_DirY = -1.0f;

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Direction.y = m_fLightInfo_DL_DirY;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
	m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
	m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1028_LightInfo_DL_DirZ()
{
	UpdateData(TRUE);
	
	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Direction.z = m_fLightInfo_DL_DirZ;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
	m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
	m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1029_LightInfo_DL_DirW()
{
	UpdateData(TRUE);

	m_fLightInfo_DL_DirW = 0.0f;

	Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);

	tDirLightInfo.Direction.w = m_fLightInfo_DL_DirW;
	Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);

	UpdateData(FALSE);
}

void CTabMap::OnBnClickedRadio1008_LightInfo_PL_CreateMode()
{
	UpdateData(TRUE);

	m_LightInfoRadio_PL_CreateMode.SetCheck(true);
	m_LightInfoRadio_PL_ModifyMode.SetCheck(false);
	m_bIsLightingCreateMode = true;
	m_bIsLightingModifyMode = false;

	m_fLightInfo_PL_DiffuseR	= 1.0f;
	m_fLightInfo_PL_DiffuseG	= 1.0f;
	m_fLightInfo_PL_DiffuseB	= 1.0f;
	m_fLightInfo_PL_DiffuseA	= 1.0f;
	m_fLightInfo_PL_SpecularR	= 0.5f;
	m_fLightInfo_PL_SpecularG	= 0.5f;
	m_fLightInfo_PL_SpecularB	= 0.5f;
	m_fLightInfo_PL_SpecularA	= 1.0f;
	m_fLightInfo_PL_AmbientR	= 0.5f;
	m_fLightInfo_PL_AmbientG	= 0.5f;
	m_fLightInfo_PL_AmbientB	= 0.5f;
	m_fLightInfo_PL_AmbientA	= 1.0f;
	m_fLightInfo_PL_PosX		= 0.0f;
	m_fLightInfo_PL_PosY		= 0.0f;
	m_fLightInfo_PL_PosZ		= 0.0f;
	m_fLightInfo_PL_PosW		= 1.0f;
	m_fLightInfo_PL_Range		= 10.0f;

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1009_LightInfo_PL_ModifyMode()
{
	UpdateData(TRUE);

	m_LightInfoRadio_PL_CreateMode.SetCheck(false);
	m_LightInfoRadio_PL_ModifyMode.SetCheck(true);
	m_bIsLightingCreateMode = false;
	m_bIsLightingModifyMode = true;

	UpdateData(FALSE);
}

void CTabMap::OnEnChangeEdit1030_LightInfo_PL_DiffuseR()
{
	UpdateData(TRUE);


	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode && 
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Diffuse.x = m_fLightInfo_PL_DiffuseR;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1031_LightInfo_PL_DiffuseG()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Diffuse.y = m_fLightInfo_PL_DiffuseG;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1032_LightInfo_PL_DiffuseB()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Diffuse.z = m_fLightInfo_PL_DiffuseB;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1033_LightInfo_PL_DiffuseA()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Diffuse.w = m_fLightInfo_PL_DiffuseA;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1034_LightInfo_PL_SpecularR()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Specular.x = m_fLightInfo_PL_SpecularR;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1035_LightInfo_PL_SpecularG()
{
	UpdateData(TRUE);
	
	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Specular.y = m_fLightInfo_PL_SpecularG;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1036_LightInfo_PL_SpecularB()
{
	UpdateData(TRUE);
	
	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Specular.z = m_fLightInfo_PL_SpecularB;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1037_LightInfo_PL_SpecularA()
{
	UpdateData(TRUE);

	m_fLightInfo_PL_SpecularA = 1.0f;

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Specular.w = m_fLightInfo_PL_SpecularA;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1038_LightInfo_PL_AmbientR()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Ambient.x = m_fLightInfo_PL_AmbientR;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1039_LightInfo_PL_AmbientG()
{
	UpdateData(TRUE);
	
	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Ambient.y = m_fLightInfo_PL_AmbientG;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1040_LightInfo_PL_AmbientB()
{
	UpdateData(TRUE);
	
	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Ambient.z = m_fLightInfo_PL_AmbientB;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1041_LightInfo_PL_AmbientA()
{
	UpdateData(TRUE);

	m_fLightInfo_PL_AmbientA = 1.0f;

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Ambient.w = m_fLightInfo_PL_AmbientA;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1042_LightInfo_PL_PosX()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Position.x = m_fLightInfo_PL_PosX;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderPosition();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1043_LightInfo_PL_PosY()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Position.y = m_fLightInfo_PL_PosY;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderPosition();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1044_LightInfo_PL_PosZ()
{
	UpdateData(TRUE);
	
	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Position.z = m_fLightInfo_PL_PosZ;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderPosition();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1045_LightInfo_PL_PosW()
{
	UpdateData(TRUE);

	m_fLightInfo_PL_PosW = 1.0f;
	
	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Position.w = m_fLightInfo_PL_PosW;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderPosition();
	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1046_LightInfo_PL_Range()
{
	UpdateData(TRUE);

	Engine::CLight* pSelectLight = static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight;
	if (m_bIsLightingModifyMode &&
		nullptr != pSelectLight)
	{
		Engine::D3DLIGHT tLightInfo = pSelectLight->Get_LightInfo();
		tLightInfo.Range = m_fLightInfo_PL_Range;

		pSelectLight->Set_LightInfo(tLightInfo);
		pSelectLight->Set_ColliderColorSelected();
	}

	UpdateData(FALSE);
}


void CTabMap::OnLbnSelchangeList1004_LightInfo_PL_List()
{
	UpdateData(TRUE);

	// 선택한 ListBox의 Index를 얻어온다.
	m_iSelectPLIdx = m_LightInfoListBox_PL_List.GetCaretIndex();

	// 선택한 Light 설정.
	Engine::CLight* pSelectLight = Engine::CLightMgr::Get_Instance()->Get_Light(Engine::LIGHTTYPE::D3DLIGHT_POINT, m_iSelectPLIdx);
	static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight = pSelectLight;

	// 모든 Light의 Collider를 Diffuse 색상으로 변겅.
	for (auto& pPointLight : Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT))
		pPointLight->Set_ColliderColorDiffuse();

	// 선택한 Light의 Collider 색상 변경.
	pSelectLight->Set_ColliderColorSelected();

	// MFC Control 변수 값 설정.
	m_fLightInfo_PL_DiffuseR	= pSelectLight->Get_LightInfo().Diffuse.x;
	m_fLightInfo_PL_DiffuseG	= pSelectLight->Get_LightInfo().Diffuse.y;
	m_fLightInfo_PL_DiffuseB	= pSelectLight->Get_LightInfo().Diffuse.z;
	m_fLightInfo_PL_DiffuseA	= pSelectLight->Get_LightInfo().Diffuse.w;
	m_fLightInfo_PL_SpecularR	= pSelectLight->Get_LightInfo().Specular.x;
	m_fLightInfo_PL_SpecularG	= pSelectLight->Get_LightInfo().Specular.y;
	m_fLightInfo_PL_SpecularB	= pSelectLight->Get_LightInfo().Specular.z;
	m_fLightInfo_PL_SpecularA	= pSelectLight->Get_LightInfo().Specular.w;
	m_fLightInfo_PL_AmbientR	= pSelectLight->Get_LightInfo().Ambient.x;
	m_fLightInfo_PL_AmbientG	= pSelectLight->Get_LightInfo().Ambient.y;
	m_fLightInfo_PL_AmbientB	= pSelectLight->Get_LightInfo().Ambient.z;
	m_fLightInfo_PL_AmbientA	= pSelectLight->Get_LightInfo().Ambient.w;
	m_fLightInfo_PL_PosX		= pSelectLight->Get_LightInfo().Position.x;
	m_fLightInfo_PL_PosY		= pSelectLight->Get_LightInfo().Position.y;
	m_fLightInfo_PL_PosZ		= pSelectLight->Get_LightInfo().Position.z;
	m_fLightInfo_PL_PosW		= pSelectLight->Get_LightInfo().Position.w;
	m_fLightInfo_PL_Range		= pSelectLight->Get_LightInfo().Range;


	UpdateData(FALSE);
}

void CTabMap::OnBnClickedButton1007_LightInfo_PL_DELETE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// 선택한 PointLight 삭제. (Picking)
	if (m_iSelectPLIdx == -1)
	{
		if (nullptr != static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight)
			static_cast<CToolSceneStage*>(m_pManagement->Get_CurrentScene())->m_pPickingLight->Set_IsDead();

		// ListBox 수정.
		m_LightInfoListBox_PL_List.ResetContent();
		for (_uint i = 0; i < Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT).size() - 1; ++i)
		{
			_tchar szTemp[MIN_STR] = L"";
			wsprintf(szTemp, L"Index : %d", i);
			m_LightInfoListBox_PL_List.AddString(szTemp);
		}

	}

	// 선택한 PointLight 삭제. (ListBox Select)
	if (!Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT).empty() &&
		m_iSelectPLIdx < Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT).size())
	{
		Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT)[m_iSelectPLIdx]->Set_IsDead();

		// ListBox 수정.
		m_LightInfoListBox_PL_List.ResetContent();

		for (_uint i = 0; i < Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT).size() - 1; ++i)
		{
			_tchar szTemp[MIN_STR] = L"";
			wsprintf(szTemp, L"Index : %d", i);
			m_LightInfoListBox_PL_List.AddString(szTemp);
		}
	}

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1008_LightInfo_PL_ALLDELETE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	for (auto& pPointLight : Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT))
		pPointLight->Set_IsDead();

	// ListBox 수정.
	m_LightInfoListBox_PL_List.ResetContent();

	m_fLightInfo_PL_DiffuseR	= 1.0f;
	m_fLightInfo_PL_DiffuseG	= 1.0f;
	m_fLightInfo_PL_DiffuseB	= 1.0f;
	m_fLightInfo_PL_DiffuseA	= 1.0f;
	m_fLightInfo_PL_SpecularR	= 0.5f;
	m_fLightInfo_PL_SpecularG	= 0.5f;
	m_fLightInfo_PL_SpecularB	= 0.5f;
	m_fLightInfo_PL_SpecularA	= 1.0f;
	m_fLightInfo_PL_AmbientR	= 0.5f;
	m_fLightInfo_PL_AmbientG	= 0.5f;
	m_fLightInfo_PL_AmbientB	= 0.5f;
	m_fLightInfo_PL_AmbientA	= 1.0f;
	m_fLightInfo_PL_PosX		= 0.0f;
	m_fLightInfo_PL_PosY		= 0.0f;
	m_fLightInfo_PL_PosZ		= 0.0f;
	m_fLightInfo_PL_PosW		= 1.0f;
	m_fLightInfo_PL_Range		= 10.0f;


	UpdateData(FALSE);
}




void CTabMap::OnBnClickedButton1009__LightInfo_PL_SAVE()
{
	UpdateData(TRUE);


	CFileDialog Dlg(FALSE,
					L"lightinginfo",
					L"*.lightinginfo",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.lightinginfo) | *.lightinginfo ||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wofstream fout{ Dlg.GetPathName().GetString() };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		for (auto& pPointLight : Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::D3DLIGHT_POINT))
		{
			// PointLight 정보 저장.
			Engine::D3DLIGHT tLightInfo = pPointLight->Get_LightInfo();

			fout	<< tLightInfo.Diffuse.x		<< L" "		// Diffuse
					<< tLightInfo.Diffuse.y		<< L" " 
					<< tLightInfo.Diffuse.z		<< L" "	
					<< tLightInfo.Diffuse.w		<< L" "
					<< tLightInfo.Specular.x	<< L" "		// Specular
					<< tLightInfo.Specular.y	<< L" " 
					<< tLightInfo.Specular.z	<< L" "	
					<< tLightInfo.Specular.w	<< L" " 
					<< tLightInfo.Ambient.x		<< L" "		// Ambient
					<< tLightInfo.Ambient.y		<< L" "	
					<< tLightInfo.Ambient.z		<< L" "			
					<< tLightInfo.Ambient.w		<< L" "
					<< tLightInfo.Position.x	<< L" "		// Position
					<< tLightInfo.Position.y	<< L" " 
					<< tLightInfo.Position.z	<< L" "
					<< tLightInfo.Position.w	<< L" "
					<< tLightInfo.Range			<< L" ";	// Range
		}

	}

	AfxMessageBox(L"Data Save Successed");

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1010__LightInfo_PL_LOAD()
{
	UpdateData(TRUE);

	// 1. ListBox 초기화.
	m_LightInfoListBox_PL_List.ResetContent();

	// 2. 모든 PointLight 삭제.
	for (auto& pPointLight : Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::D3DLIGHT_POINT))
		pPointLight->Set_IsDead();

	CFileDialog Dlg(TRUE,
					L"lightinginfo",
					L"*.lightinginfo",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.lightinginfo)|*.lightinginfo||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wifstream fin{ Dlg.GetPathName().GetString() };
		if (fin.fail())
		{
			AfxMessageBox(L"Load is Failed");
			return;
		}

		while (true)
		{
			// PointLight 정보 저장.
			Engine::D3DLIGHT tLightInfo { };
			tLightInfo.Type = Engine::D3DLIGHT_POINT;

					// PointLight Data 불러오기.
			fin		>> tLightInfo.Diffuse.x		// Diffuse
					>> tLightInfo.Diffuse.y	
					>> tLightInfo.Diffuse.z	
					>> tLightInfo.Diffuse.w	
					>> tLightInfo.Specular.x	// Specular
					>> tLightInfo.Specular.y	
					>> tLightInfo.Specular.z	
					>> tLightInfo.Specular.w	
					>> tLightInfo.Ambient.x		// Ambient
					>> tLightInfo.Ambient.y		
					>> tLightInfo.Ambient.z			
					>> tLightInfo.Ambient.w	
					>> tLightInfo.Position.x	// Direction
					>> tLightInfo.Position.y
					>> tLightInfo.Position.z
					>> tLightInfo.Position.w
					>> tLightInfo.Range;		// Range

			if (fin.eof())
				break;

			Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(),
																					 Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMDID::CMD_MAIN),
																					 Engine::LIGHTTYPE::D3DLIGHT_POINT,
																					 tLightInfo), E_FAIL);

			// ListBox 추가.
			_uint iSize = (_uint)Engine::CLightMgr::Get_Instance()->Get_VecLightInfo(Engine::LIGHTTYPE::D3DLIGHT_POINT).size();
			_tchar szTemp[MIN_STR] = L"";
			wsprintf(szTemp, L"Index : %d", iSize - 1);
			m_LightInfoListBox_PL_List.AddString(szTemp);
		}

	}

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1050_LightInfo_SL_AtX()
{
	UpdateData(TRUE);
	
	CShadowLightMgr::Get_Instance()->m_vLightAt.x = m_fLightInfo_SL_AtX;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1051_LightInfo_SL_AtY()
{
	UpdateData(TRUE);
	
	m_fLightInfo_SL_AtY = 0.0f;
	CShadowLightMgr::Get_Instance()->m_vLightAt.y = m_fLightInfo_SL_AtY;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1052_LightInfo_SL_AtZ()
{
	UpdateData(TRUE);

	CShadowLightMgr::Get_Instance()->m_vLightAt.z = m_fLightInfo_SL_AtZ;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1056_LightInfo_SL_Height()
{
	UpdateData(TRUE);

	CShadowLightMgr::Get_Instance()->m_fHeight = m_fLightInfo_SL_Height;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	m_fLightInfo_SL_EyeX = CShadowLightMgr::Get_Instance()->m_vLightEye.x;
	m_fLightInfo_SL_EyeY = CShadowLightMgr::Get_Instance()->m_vLightEye.y;
	m_fLightInfo_SL_EyeZ = CShadowLightMgr::Get_Instance()->m_vLightEye.z;

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1053_LightInfo_SL_FovY()
{
	UpdateData(TRUE);
	
	CShadowLightMgr::Get_Instance()->m_fFovY = m_fLightInfo_SL_FovY;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1054_LightInfo_SL_Near()
{
	UpdateData(TRUE);
	
	m_fLightInfo_SL_Near = 1.0f;
	CShadowLightMgr::Get_Instance()->m_fNear = m_fLightInfo_SL_Near;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();

	UpdateData(FALSE);
}


void CTabMap::OnEnChangeEdit1055_LightInfo_SL_Far()
{
	UpdateData(TRUE);
	
	CShadowLightMgr::Get_Instance()->m_fFar = m_fLightInfo_SL_Far;
	CShadowLightMgr::Get_Instance()->Update_ShadowLight();

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1011__LightInfo_SL_SAVE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE,
					L"lightinginfo",
					L"*.lightinginfo",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.lightinginfo) | *.lightinginfo ||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wofstream fout{ Dlg.GetPathName().GetString() };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		// DirectionLight & ShadowLight Data 저장하기.
		Engine::D3DLIGHT tDirLightInfo = Engine::CLightMgr::Get_Instance()->Get_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0);
		_vec3	vLightAt	= CShadowLightMgr::Get_Instance()->m_vLightAt;
		_float	fHeight		= CShadowLightMgr::Get_Instance()->m_fHeight;
		_float	fFovY		= CShadowLightMgr::Get_Instance()->m_fFovY;
		_float	fFar		= CShadowLightMgr::Get_Instance()->m_fFar;

				// DirectionLight
		fout	<< tDirLightInfo.Diffuse.x		<< L" "	// Diffuse
				<< tDirLightInfo.Diffuse.y		<< L" " 
				<< tDirLightInfo.Diffuse.z		<< L" "	
				<< tDirLightInfo.Diffuse.w		<< L" "
				<< tDirLightInfo.Specular.x		<< L" "	// Specular
				<< tDirLightInfo.Specular.y		<< L" " 
				<< tDirLightInfo.Specular.z		<< L" "	
				<< tDirLightInfo.Specular.w		<< L" " 
				<< tDirLightInfo.Ambient.x		<< L" "	// Ambient
				<< tDirLightInfo.Ambient.y		<< L" "	
				<< tDirLightInfo.Ambient.z		<< L" "			
				<< tDirLightInfo.Ambient.w		<< L" "
				<< tDirLightInfo.Direction.x	<< L" "	// Direction
				<< tDirLightInfo.Direction.y	<< L" " 
				<< tDirLightInfo.Direction.z	<< L" "
				<< tDirLightInfo.Direction.w	<< L" "

				// ShadowLight Data 저장하기.
				<< vLightAt.x					<< L" "	// Light At.
				<< vLightAt.y					<< L" " 
				<< vLightAt.z					<< L" "	
				<< fHeight						<< L" "	// Light Height.
				<< fFovY						<< L" "	// Light FovY
				<< fFar							<< L" ";// Light Fawr
	}

	AfxMessageBox(L"Data Save Successed");

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedButton1012_LightInfo_SL_LOAD()
{
	UpdateData(TRUE);
	
	CFileDialog Dlg(TRUE,
					L"lightinginfo",
					L"*.lightinginfo",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.lightinginfo)|*.lightinginfo||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wifstream fin{ Dlg.GetPathName().GetString() };
		if (fin.fail())
		{
			AfxMessageBox(L"Load is Failed");
			return;
		}

		while (true)
		{
			Engine::D3DLIGHT tDirLightInfo;
			_vec3	vLightAt;
			_float	fHeight;
			_float	fFovY;
			_float	fFar;

					// DirectionLight Data 불러오기.
			fin		>> tDirLightInfo.Diffuse.x		// Diffuse
					>> tDirLightInfo.Diffuse.y	
					>> tDirLightInfo.Diffuse.z	
					>> tDirLightInfo.Diffuse.w	
					>> tDirLightInfo.Specular.x		// Specular
					>> tDirLightInfo.Specular.y	
					>> tDirLightInfo.Specular.z	
					>> tDirLightInfo.Specular.w	
					>> tDirLightInfo.Ambient.x		// Ambient
					>> tDirLightInfo.Ambient.y	
					>> tDirLightInfo.Ambient.z			
					>> tDirLightInfo.Ambient.w	
					>> tDirLightInfo.Direction.x	// Direction
					>> tDirLightInfo.Direction.y
					>> tDirLightInfo.Direction.z
					>> tDirLightInfo.Direction.w

					// ShadowLight Data 불러오기.
					>> vLightAt.x					// ShadowLight At
					>> vLightAt.y	
					>> vLightAt.z	
					>> fHeight						// ShadowLight Height
					>> fFovY						// ShadowLight FovY
					>> fFar;						// ShadowLight Far

			if (fin.eof())
				break;

			// Direction Light 정보 입력.
			Engine::CLightMgr::Get_Instance()->Set_LightInfo(Engine::D3DLIGHT_DIRECTIONAL, 0, tDirLightInfo);
			m_fLightInfo_DL_DiffuseR	= tDirLightInfo.Diffuse.x;
			m_fLightInfo_DL_DiffuseG	= tDirLightInfo.Diffuse.y;
			m_fLightInfo_DL_DiffuseB	= tDirLightInfo.Diffuse.z;
			m_fLightInfo_DL_DiffuseA	= tDirLightInfo.Diffuse.w;
			m_fLightInfo_DL_SpecularR	= tDirLightInfo.Specular.x;
			m_fLightInfo_DL_SpecularG	= tDirLightInfo.Specular.y;
			m_fLightInfo_DL_SpecularB	= tDirLightInfo.Specular.z;
			m_fLightInfo_DL_SpecularA	= tDirLightInfo.Specular.w;
			m_fLightInfo_DL_AmbientR	= tDirLightInfo.Ambient.x;
			m_fLightInfo_DL_AmbientG	= tDirLightInfo.Ambient.y;
			m_fLightInfo_DL_AmbientB	= tDirLightInfo.Ambient.z;
			m_fLightInfo_DL_AmbientA	= tDirLightInfo.Ambient.w;
			m_fLightInfo_DL_DirX		= tDirLightInfo.Direction.x;
			m_fLightInfo_DL_DirY		= tDirLightInfo.Direction.y;
			m_fLightInfo_DL_DirZ		= tDirLightInfo.Direction.z;
			m_fLightInfo_DL_DirW		= tDirLightInfo.Direction.w;

			// ShadowLight 정보 입력.
			CShadowLightMgr::Get_Instance()->m_vLightAt		= vLightAt;
			CShadowLightMgr::Get_Instance()->m_fHeight		= fHeight;
			CShadowLightMgr::Get_Instance()->m_fFovY		= fFovY;
			CShadowLightMgr::Get_Instance()->m_fFar			= fFar;
			CShadowLightMgr::Get_Instance()->Update_ShadowLight();

			m_fLightInfo_SL_EyeX	= CShadowLightMgr::Get_Instance()->m_vLightEye.x;
			m_fLightInfo_SL_EyeY	= CShadowLightMgr::Get_Instance()->m_vLightEye.y;
			m_fLightInfo_SL_EyeZ	= CShadowLightMgr::Get_Instance()->m_vLightEye.z;
			m_fLightInfo_SL_AtX		= CShadowLightMgr::Get_Instance()->m_vLightAt.x;
			m_fLightInfo_SL_AtY		= CShadowLightMgr::Get_Instance()->m_vLightAt.y;
			m_fLightInfo_SL_AtZ		= CShadowLightMgr::Get_Instance()->m_vLightAt.z;
			m_fLightInfo_SL_Height	= CShadowLightMgr::Get_Instance()->m_fHeight;
			m_fLightInfo_SL_FovY	= CShadowLightMgr::Get_Instance()->m_fFovY;
			m_fLightInfo_SL_Near	= CShadowLightMgr::Get_Instance()->m_fNear;
			m_fLightInfo_SL_Far		= CShadowLightMgr::Get_Instance()->m_fFar;
		}

	}

	AfxMessageBox(L"Data Load Successed");

	UpdateData(FALSE);
}


void CTabMap::OnBnClickedRadio1011_NaviMeshCreateMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bIsNaviCreateMode = true;
	m_bIsNaviModifyMode = false;

	m_NaviMeshRadio_CreateMode.SetCheck(m_bIsNaviCreateMode);
	m_NaviMeshRadio_ModifyMode.SetCheck(m_bIsNaviModifyMode);

	m_NaviMeshCheck_FindNearPoint.EnableWindow(TRUE);
	m_NaviMeshCheck_FindNearPoint.SetCheck(false);

	m_fNaviMeshPointA_X = 0.0f;
	m_fNaviMeshPointA_Y = 0.0f;
	m_fNaviMeshPointA_Z = 0.0f;
	m_fNaviMeshPointB_X = 0.0f;
	m_fNaviMeshPointB_Y = 0.0f;
	m_fNaviMeshPointB_Z = 0.0f;
	m_fNaviMeshPointC_X = 0.0f;
	m_fNaviMeshPointC_Y = 0.0f;
	m_fNaviMeshPointC_Z = 0.0f;
}


void CTabMap::OnBnClickedRadio1012_NaviMeshModifyMode()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bIsNaviCreateMode = false;
	m_bIsNaviModifyMode = true;

	m_NaviMeshRadio_CreateMode.SetCheck(m_bIsNaviCreateMode);
	m_NaviMeshRadio_ModifyMode.SetCheck(m_bIsNaviModifyMode);

	m_NaviMeshCheck_FindNearPoint.EnableWindow(FALSE);
	m_NaviMeshCheck_FindNearPoint.SetCheck(false);
}
