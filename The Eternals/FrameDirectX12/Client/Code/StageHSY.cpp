#include "stdafx.h"
#include <fstream>
#include <random>
#include "StageHSY.h"
#include "ComponentMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "LightMgr.h"
#include "Font.h"
#include "DebugCamera.h"
#include "DynamicCamera.h"
#include "RectObject.h"
#include "CubeObject.h"
#include "TerrainObject.h"
#include "StaticMeshObject.h"
#include "Popori_F.h"
#include "TextureEffect.h"
#include "SkyBox.h"
#include "TexEffectInstance.h"
#include "TestCollisonObject.h"
#include "GameUIRoot.h"
#include "GameUIChild.h"
#include "CharacterHpGauge.h"
#include "CharacterMpGauge.h"
#include "MainMenuEquipment.h"
#include "MainMenuInventory.h"
#include "MainMenuLogout.h"
#include "MainMenuSetting.h"
#include "SampleNPC.h"
#include "PCGladiator.h"
#include "TerrainMeshObject.h"
#include "InstancePoolMgr.h"

CStageHSY::CStageHSY(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}



HRESULT CStageHSY::Ready_Scene()
{
#ifdef CLIENT_LOG
	COUT_STR("<< Ready Scene_Stage >>");
	COUT_STR("");
#endif

	Engine::FAILED_CHECK_RETURN(Ready_NaviMesh(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerCamera(L"Layer_Camera"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerEnvironment(L"Layer_Environment"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerGameObject(L"Layer_GameObject"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerUI(L"Layer_UI"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LayerFont(L"Layer_Font"), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);

	// Ready Instancing 
	Engine::CShaderShadowInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);
	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);
	Engine::CShaderLightingInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);
	Engine::CShaderColorInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);
	// Engine::CShaderTextureInstancing::Get_Instance()->SetUp_ConstantBuffer(Engine::INSTANCE::INSTANCE_DISTORTION ,m_pGraphicDevice);
	// Engine::CShaderTextureInstancing::Get_Instance()->SetUp_ConstantBuffer(Engine::INSTANCE::INSTANCE_ALPHA, m_pGraphicDevice);

	CMouseCursorMgr::Get_Instance()->Set_IsActiveMouse(false);
	CInstancePoolMgr::Get_Instance()->Ready_InstancePool(m_pGraphicDevice, m_pCommandList);

	g_bIsOnDebugCaemra = true;

	return S_OK;
}

void CStageHSY::Process_PacketFromServer()
{
	// CPacketMgr::Get_Instance()->recv_packet();
}


_int CStageHSY::Update_Scene(const _float & fTimeDelta)
{
	// MouseCursorMgr
	if (!m_bIsReadyMouseCursorMgr)
	{
		m_bIsReadyMouseCursorMgr = true;
		CMouseCursorMgr::Get_Instance()->Ready_MouseCursorMgr();
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CStageHSY::LateUpdate_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CStageHSY::Send_PacketToServer()
{
	Engine::CScene::Send_PacketToServer();
}

HRESULT CStageHSY::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

HRESULT CStageHSY::Ready_LayerCamera(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);
	
	/*__________________________________________________________________________________________________________
	[ Camera Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ DebugCamera ]
	____________________________________________________________________________________________________________*/
	pGameObj = CDebugCamera::Create(m_pGraphicDevice, m_pCommandList,
									Engine::CAMERA_DESC(_vec3(216.0f, 6.0f, 129.0f),	// Eye
														_vec3(197.0f, 13.0f, 116.0f),	// At
														_vec3(0.0f, 1.0f, 0.f)),		// Up
									Engine::PROJ_DESC(60.0f,							// FovY
													  _float(WINCX) / _float(WINCY),	// Aspect
													  0.1f,								// Near
													  1000.0f),							// Far
									Engine::ORTHO_DESC(WINCX,							// Viewport Width
													   WINCY,							// Viewport Height
													   0.0f,							// Near
													   1.0f));							// Far
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"DebugCamera", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ DynamicCamera ]
	____________________________________________________________________________________________________________*/
	pGameObj = CDynamicCamera::Create(m_pGraphicDevice, m_pCommandList,
									  Engine::CAMERA_DESC(_vec3(216.0f, 6.0f, 129.0f),	// Eye
									  					  _vec3(197.0f, 13.0f, 116.0f),	// At
									  					  _vec3(0.0f, 1.0f, 0.0f)),		// Up
									  Engine::PROJ_DESC(60.0f,							// FovY
									  					_float(WINCX) / _float(WINCY),	// Aspect
									  					0.1f,							// Near
									  					1000.0f),						// Far
									  Engine::ORTHO_DESC(WINCX,							// Viewport Width
									  					 WINCY,							// Viewport Height
									  					 0.0f,							// Near
									  					 1.0f));						// Far
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"DynamicCamera", pGameObj), E_FAIL);


	return S_OK;
}


HRESULT CStageHSY::Ready_LayerEnvironment(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Environment Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ SkyBox ]
	____________________________________________________________________________________________________________*/
	pGameObj = CSkyBox::Create(m_pGraphicDevice, m_pCommandList,
							   L"SkyBox",							// Texture Tag
							   _vec3(990.0f, 990.0f, 990.0f),		// Scale
							   _vec3(0.0f, 0.0f, 0.0f),				// Angle
							   _vec3(0.0f, 0.0f, 0.0f),				// Pos
							   1);									// Texture Idx
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ BumpTerrainMesh ]
	____________________________________________________________________________________________________________*/
	pGameObj = CTerrainMeshObject::Create(m_pGraphicDevice, m_pCommandList,
										  L"BumpTerrainMesh02",
										  _vec3(0.175f),
										  _vec3(0.0f, 0.0f ,0.0f),
										  _vec3(128.0f, -0.01f, 128.0f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_WINTER, L"BumpTerrainMesh02", pGameObj), E_FAIL);
	static_cast<CTerrainMeshObject*>(pGameObj)->Set_Wave(1.f);

	/*__________________________________________________________________________________________________________
	[ StaticMeshObject ]
	____________________________________________________________________________________________________________*/
	wifstream fin { L"../../Bin/ToolData/StageWendigo_StaticMesh.staticmesh" };
	if (fin.fail())
		return E_FAIL;

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
		fin >> wstrMeshTag 				// MeshTag
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
			>> bIsMousePicking;

		if (fin.eof())
			break;

		pGameObj = CStaticMeshObject::Create(m_pGraphicDevice, m_pCommandList,
											 wstrMeshTag,			// MeshTag
											 vScale,				// Scale
											 vAngle,				// Angle
											 vPos,					// Pos
											 bIsRenderShadow,		// Render Shadow
											 bIsCollision,			// Bounding Sphere
											 vBoundingSphereScale,	// Bounding Sphere Scale
											 vBoundingSpherePos);	// Bounding Sphere Pos
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_WINTER, wstrMeshTag, pGameObj), E_FAIL);
	}

	Engine::CObjectMgr::Get_Instance()->Set_CurrentStage(Engine::STAGEID::STAGE_WINTER);

	return S_OK;
}

HRESULT CStageHSY::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ TestCollisionObject ]
	____________________________________________________________________________________________________________*/
	//uniform_int_distribution<_int>	uid_x	{ 0, 256 };
	//uniform_int_distribution<_int>	uid_y	{ 0, 256 };
	//uniform_int_distribution<_int>	uid_z	{ 0, 256 };
	//random_device			rn;
	//default_random_engine	dre{ rn()};

	//for (_uint i = 0; i < 100; ++i)
	//{
	//	pGameObj = CTestCollisonObject::Create(m_pGraphicDevice, m_pCommandList,
	//										   _vec3(10.0f),
	//										   _vec3(0.0f),
	//										   _vec3((_float)uid_x(dre), (_float)uid_y(dre), (_float)uid_z(dre)));

	//	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"CollisionObject", pGameObj), E_FAIL);
	//}

#pragma region SAMPLE_NPC
	//pGameObj =	CSampleNPC::Create(m_pGraphicDevice, m_pCommandList,
	//							   L"Castanic_M_Lsmith",		// MeshTag
	//							   _vec3(1.0f, 1.0f, 1.0f),		// Scale
	//							   _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//							   _vec3(90.0f, 0.0f, 0.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SampleNPC", pGameObj), E_FAIL);

	//pGameObj =	CSampleNPC::Create(m_pGraphicDevice, m_pCommandList,
	//							   L"Human_girl",		// MeshTag
	//							   _vec3(1.0f, 1.0f, 1.0f),		// Scale
	//							   _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//							   _vec3(110.0f, 0.0f, 0.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SampleNPC", pGameObj), E_FAIL);

	//pGameObj =	CSampleNPC::Create(m_pGraphicDevice, m_pCommandList,
	//							   L"Highelf_girl",		// MeshTag
	//							   _vec3(1.0f, 1.0f, 1.0f),		// Scale
	//							   _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//							   _vec3(130.0f, 0.0f, 0.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SampleNPC", pGameObj), E_FAIL);

	//pGameObj =	CSampleNPC::Create(m_pGraphicDevice, m_pCommandList,
	//							   L"Popori_M_Merchant",		// MeshTag
	//							   _vec3(1.0f, 1.0f, 1.0f),		// Scale
	//							   _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//							   _vec3(150.0f, 0.0f, 0.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SampleNPC", pGameObj), E_FAIL);
#pragma endregion

	/*__________________________________________________________________________________________________________
	[ Texture Effect ]
	____________________________________________________________________________________________________________*/
	//// Fire
	//pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
	//								  L"Fire",						// TextureTag
	//								  _vec3(2.5f, 2.5f, 1.0f),		// Scale
	//								  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//								  _vec3(26.0f, 1.5f, 26.5f),	// Pos
	//								  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Fire", pGameObj), E_FAIL);

	//// Torch
	//pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
	//								  L"Torch",						// TextureTag
	//								  _vec3(2.5f, 5.0f, 1.0f),		// Scale
	//								  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//								  _vec3(28.0f, 2.0f, 27.0f),	// Pos
	//								  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Torch", pGameObj), E_FAIL);


	//// TestEffect
	//pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
	//								  L"Smoke1",						// TextureTag
	//								  _vec3(10.0f, 10.0f, 1.0f),		// Scale
	//								  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//								  _vec3(140.0f, 5.0f, 80.0f),	// Pos
	//								  FRAME(5, 6, 64.0f));			// Sprite Image Frame
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Torch", pGameObj), E_FAIL);

#pragma region TEXTURE_INSTANCING
	///*__________________________________________________________________________________________________________
	//[ Texture Effect Instancing ]
	//____________________________________________________________________________________________________________*/
	////_vec3 vStartPos = _vec3(0.0f, 15.f, 1.0f);
	////_vec3 vOffset   = _vec3(5.0f, 0.0f, 4.5f);

	////for (_int i = 0; i < 10; ++i)
	////{
	////	if (0 == i % 2)
	////		vStartPos.x = 0.0f;
	////	else
	////		vStartPos.x = 5.0f;

	////	for (_int j = 0; j < 10; ++j)
	////	{
	////		pGameObj = CTexEffectInstance::Create(m_pGraphicDevice, m_pCommandList,
	////									  L"Fire",						// TextureTag
	////									  _vec3(2.5f, 2.5f, 1.0f),		// Scale
	////									  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	////									  vStartPos,					// Pos
	////									  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	////		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Fire", pGameObj), E_FAIL);

	////		vStartPos.x += vOffset.x;
	////	}

	////	vStartPos.z += vOffset.z;
	////}

	////Engine::CShaderTextureInstancing::Get_Instance()->SetUp_Instancing(Engine::INSTANCE::INSTANCE_ALPHA, L"Fire");
	////Engine::CShaderTextureInstancing::Get_Instance()->SetUp_Instancing(Engine::INSTANCE::INSTANCE_DISTORTION, L"Fire");
	////


	////vStartPos = _vec3(0.0f, 10.0f, 0.0f);
	////vOffset   = _vec3(5.0f, 0.0f, 5.0f);

	////for (_int i = 0; i < 10; ++i)
	////{
	////	if (0 == i % 2)
	////		vStartPos.x = 0.0f;
	////	else
	////		vStartPos.x = 5.0f;

	////	for (_int j = 0; j < 10; ++j)
	////	{
	////		pGameObj = CTexEffectInstance::Create(m_pGraphicDevice, m_pCommandList,
	////									  L"Torch",						// TextureTag
	////									  _vec3(2.5f, 5.0f, 1.0f),		// Scale
	////									  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	////									  vStartPos,					// Pos
	////									  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	////		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Torch", pGameObj), E_FAIL);

	////		vStartPos.x += vOffset.x;
	////	}

	////	vStartPos.z += vOffset.z;
	////}

	////Engine::CShaderTextureInstancing::Get_Instance()->SetUp_Instancing(Engine::INSTANCE::INSTANCE_ALPHA, L"Torch");
	////Engine::CShaderTextureInstancing::Get_Instance()->SetUp_Instancing(Engine::INSTANCE::INSTANCE_DISTORTION, L"Torch");
#pragma endregion

	return S_OK;
}

HRESULT CStageHSY::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	Engine::CGameObject* pGameObj = nullptr;

	///*__________________________________________________________________________________________________________
	//[ CharacterClassFrame ]
	//____________________________________________________________________________________________________________*/
	//{
	//	wifstream fin { L"../../Bin/ToolData/2DUICharacterClassFrameArcher.2DUI" };
	//	if (fin.fail())
	//		return E_FAIL;

	//	// RootUI Data
	//	wstring wstrDataFilePath   = L"";			// DataFilePath
	//	wstring wstrRootObjectTag  = L"";			// ObjectTag
	//	_vec3	vPos               = _vec3(0.0f);	// Pos
	//	_vec3	vScale             = _vec3(1.0f);	// Scale
	//	_long	UIDepth            = 0;				// UIDepth
	//	_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
	//	_float	fFrameSpeed        = 0.0f;			// FrameSpeed
	//	_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
	//	_vec3	vRectScale         = _vec3(1.0f);	// RectScale
	//	_int	iChildUISize       = 0;				// ChildUI Size

	//	// ChildUI Data
	//	vector<wstring> vecDataFilePath;
	//	vector<wstring> vecObjectTag;
	//	vector<_vec3>	vecPos;
	//	vector<_vec3>	vecScale;
	//	vector<_long>	vecUIDepth;
	//	vector<_int>	vecIsSpriteAnimation;
	//	vector<_float>	vecFrameSpeed;
	//	vector<_vec3>	vecRectPosOffset;
	//	vector<_vec3>	vecRectScale;

	//	while (true)
	//	{
	//		fin >> wstrDataFilePath
	//			>> wstrRootObjectTag
	//			>> vPos.x
	//			>> vPos.y
	//			>> vScale.x
	//			>> vScale.y
	//			>> UIDepth
	//			>> bIsSpriteAnimation
	//			>> fFrameSpeed
	//			>> vRectPosOffset.x
	//			>> vRectPosOffset.y
	//			>> vRectScale.x
	//			>> vRectScale.y
	//			>> iChildUISize;

	//		vecDataFilePath.resize(iChildUISize);
	//		vecObjectTag.resize(iChildUISize);
	//		vecPos.resize(iChildUISize);
	//		vecScale.resize(iChildUISize);
	//		vecUIDepth.resize(iChildUISize);
	//		vecIsSpriteAnimation.resize(iChildUISize);
	//		vecFrameSpeed.resize(iChildUISize);
	//		vecRectPosOffset.resize(iChildUISize);
	//		vecRectScale.resize(iChildUISize);

	//		for (_int i = 0; i < iChildUISize; ++i)
	//		{
	//		fin >> vecDataFilePath[i]			// DataFilePath
	//			>> vecObjectTag[i]				// Object Tag
	//			>> vecPos[i].x					// Pos X
	//			>> vecPos[i].y					// Pos Y
	//			>> vecScale[i].x				// Scale X
	//			>> vecScale[i].y				// Scale Y
	//			>> vecUIDepth[i]				// UI Depth
	//			>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
	//			>> vecFrameSpeed[i]				// Frame Speed
	//			>> vecRectPosOffset[i].x		// RectPosOffset X
	//			>> vecRectPosOffset[i].y		// RectPosOffset Y
	//			>> vecRectScale[i].x			// RectScale X
	//			>> vecRectScale[i].y;			// RectScale Y
	//		}

	//		if (fin.eof())
	//			break;

	//		// UIRoot 积己.
	//		Engine::CGameObject* pRootUI = nullptr;
	//		pRootUI = CGameUIRoot::Create(m_pGraphicDevice, m_pCommandList,
	//									  wstrRootObjectTag,
	//									  wstrDataFilePath,
	//									  vPos,
	//									  vScale,
	//									  bIsSpriteAnimation,
	//									  fFrameSpeed,
	//									  vRectPosOffset,
	//									  vRectScale,
	//									  UIDepth);
	//		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);

	//		// UIChild 积己.
	//		Engine::CGameObject* pChildUI = nullptr;
	//		for (_int i = 0; i < iChildUISize; ++i)
	//		{
	//			if (L"ClassFrameHpFront" == vecObjectTag[i])
	//			{
	//				pChildUI = CCharacterHpGauge::Create(m_pGraphicDevice, m_pCommandList,
	//													 wstrRootObjectTag,				// RootObjectTag
	//													 vecObjectTag[i],				// ObjectTag
	//													 vecDataFilePath[i],			// DataFilePath
	//													 vecPos[i],						// Pos
	//													 vecScale[i],					// Scane
	//													 (_bool)vecIsSpriteAnimation[i],// Is Animation
	//													 vecFrameSpeed[i],				// FrameSpeed
	//													 vecRectPosOffset[i],			// RectPosOffset
	//													 vecRectScale[i],				// RectScaleOffset
	//													 vecUIDepth[i]);				// UI Depth
	//			}
	//			else if (L"ClassFrameMpFront" == vecObjectTag[i])
	//			{
	//				pChildUI = CCharacterMpGauge::Create(m_pGraphicDevice, m_pCommandList,
	//													 wstrRootObjectTag,				// RootObjectTag
	//													 vecObjectTag[i],				// ObjectTag
	//													 vecDataFilePath[i],			// DataFilePath
	//													 vecPos[i],						// Pos
	//													 vecScale[i],					// Scane
	//													 (_bool)vecIsSpriteAnimation[i],// Is Animation
	//													 vecFrameSpeed[i],				// FrameSpeed
	//													 vecRectPosOffset[i],			// RectPosOffset
	//													 vecRectScale[i],				// RectScaleOffset
	//													 vecUIDepth[i]);				// UI Depth
	//			}
	//			else
	//			{
	//				pChildUI = CGameUIChild::Create(m_pGraphicDevice, m_pCommandList,
	//												wstrRootObjectTag,				// RootObjectTag
	//												vecObjectTag[i],				// ObjectTag
	//												vecDataFilePath[i],				// DataFilePath
	//												vecPos[i],						// Pos
	//												vecScale[i],					// Scane
	//												(_bool)vecIsSpriteAnimation[i],	// Is Animation
	//												vecFrameSpeed[i],				// FrameSpeed
	//												vecRectPosOffset[i],			// RectPosOffset
	//												vecRectScale[i],				// RectScaleOffset
	//												vecUIDepth[i]);					// UI Depth
	//			}
	//			m_pObjectMgr->Add_GameObject(L"Layer_UI", vecObjectTag[i], pChildUI);
	//			static_cast<CGameUIRoot*>(pRootUI)->Add_ChildUI(pChildUI);
	//		}
	//	}
	//}

	///*__________________________________________________________________________________________________________
	//[ MainMenuLogout ]
	//____________________________________________________________________________________________________________*/
	//{
	//	wifstream fin { L"../../Bin/ToolData/2DUIMainMenuLogout_Normal.2DUI" };
	//	if (fin.fail())
	//		return E_FAIL;

	//	// RootUI Data
	//	wstring wstrDataFilePath   = L"";			// DataFilePath
	//	wstring wstrRootObjectTag  = L"";			// ObjectTag
	//	_vec3	vPos               = _vec3(0.0f);	// Pos
	//	_vec3	vScale             = _vec3(1.0f);	// Scale
	//	_long	UIDepth            = 0;				// UIDepth
	//	_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
	//	_float	fFrameSpeed        = 0.0f;			// FrameSpeed
	//	_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
	//	_vec3	vRectScale         = _vec3(1.0f);	// RectScale
	//	_int	iChildUISize       = 0;				// ChildUI Size

	//	while (true)
	//	{
	//		fin >> wstrDataFilePath
	//			>> wstrRootObjectTag
	//			>> vPos.x
	//			>> vPos.y
	//			>> vScale.x
	//			>> vScale.y
	//			>> UIDepth
	//			>> bIsSpriteAnimation
	//			>> fFrameSpeed
	//			>> vRectPosOffset.x
	//			>> vRectPosOffset.y
	//			>> vRectScale.x
	//			>> vRectScale.y
	//			>> iChildUISize;

	//		if (fin.eof())
	//			break;

	//		// UIRoot 积己.
	//		Engine::CGameObject* pRootUI = nullptr;
	//		pRootUI = CMainMenuLogout::Create(m_pGraphicDevice, m_pCommandList,
	//										  wstrRootObjectTag,
	//										  wstrDataFilePath,
	//										  vPos,
	//										  vScale,
	//										  bIsSpriteAnimation,
	//										  fFrameSpeed,
	//										  vRectPosOffset,
	//										  vRectScale,
	//										  UIDepth);
	//		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
	//	}
	//}

	///*__________________________________________________________________________________________________________
	//[ MainMenuSetting ]
	//____________________________________________________________________________________________________________*/
	//{
	//	wifstream fin { L"../../Bin/ToolData/2DUIMainMenuSetting_Normal.2DUI" };
	//	if (fin.fail())
	//		return E_FAIL;

	//	// RootUI Data
	//	wstring wstrDataFilePath   = L"";			// DataFilePath
	//	wstring wstrRootObjectTag  = L"";			// ObjectTag
	//	_vec3	vPos               = _vec3(0.0f);	// Pos
	//	_vec3	vScale             = _vec3(1.0f);	// Scale
	//	_long	UIDepth            = 0;				// UIDepth
	//	_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
	//	_float	fFrameSpeed        = 0.0f;			// FrameSpeed
	//	_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
	//	_vec3	vRectScale         = _vec3(1.0f);	// RectScale
	//	_int	iChildUISize       = 0;				// ChildUI Size

	//	while (true)
	//	{
	//		fin >> wstrDataFilePath
	//			>> wstrRootObjectTag
	//			>> vPos.x
	//			>> vPos.y
	//			>> vScale.x
	//			>> vScale.y
	//			>> UIDepth
	//			>> bIsSpriteAnimation
	//			>> fFrameSpeed
	//			>> vRectPosOffset.x
	//			>> vRectPosOffset.y
	//			>> vRectScale.x
	//			>> vRectScale.y
	//			>> iChildUISize;

	//		if (fin.eof())
	//			break;

	//		// UIRoot 积己.
	//		Engine::CGameObject* pRootUI = nullptr;
	//		pRootUI = CMainMenuSetting::Create(m_pGraphicDevice, m_pCommandList,
	//										   wstrRootObjectTag,
	//										   wstrDataFilePath,
	//										   vPos,
	//										   vScale,
	//										   bIsSpriteAnimation,
	//										   fFrameSpeed,
	//										   vRectPosOffset,
	//										   vRectScale,
	//										   UIDepth);
	//		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
	//	}
	//}

	///*__________________________________________________________________________________________________________
	//[ MainMenuInventory ]
	//____________________________________________________________________________________________________________*/
	//{
	//	wifstream fin { L"../../Bin/ToolData/2DUIMainMenuInventory_Normal.2DUI" };
	//	if (fin.fail())
	//		return E_FAIL;

	//	// RootUI Data
	//	wstring wstrDataFilePath   = L"";			// DataFilePath
	//	wstring wstrRootObjectTag  = L"";			// ObjectTag
	//	_vec3	vPos               = _vec3(0.0f);	// Pos
	//	_vec3	vScale             = _vec3(1.0f);	// Scale
	//	_long	UIDepth            = 0;				// UIDepth
	//	_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
	//	_float	fFrameSpeed        = 0.0f;			// FrameSpeed
	//	_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
	//	_vec3	vRectScale         = _vec3(1.0f);	// RectScale
	//	_int	iChildUISize       = 0;				// ChildUI Size

	//	while (true)
	//	{
	//		fin >> wstrDataFilePath
	//			>> wstrRootObjectTag
	//			>> vPos.x
	//			>> vPos.y
	//			>> vScale.x
	//			>> vScale.y
	//			>> UIDepth
	//			>> bIsSpriteAnimation
	//			>> fFrameSpeed
	//			>> vRectPosOffset.x
	//			>> vRectPosOffset.y
	//			>> vRectScale.x
	//			>> vRectScale.y
	//			>> iChildUISize;

	//		if (fin.eof())
	//			break;

	//		// UIRoot 积己.
	//		Engine::CGameObject* pRootUI = nullptr;
	//		pRootUI = CMainMenuInventory::Create(m_pGraphicDevice, m_pCommandList,
	//											 wstrRootObjectTag,
	//											 wstrDataFilePath,
	//											 vPos,
	//											 vScale,
	//											 bIsSpriteAnimation,
	//											 fFrameSpeed,
	//											 vRectPosOffset,
	//											 vRectScale,
	//											 UIDepth);
	//		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
	//	}
	//}

	///*__________________________________________________________________________________________________________
	//[ MainMenuEquipment ]
	//____________________________________________________________________________________________________________*/
	//{
	//	wifstream fin { L"../../Bin/ToolData/2DUIMainMenuEquipment_Normal.2DUI" };
	//	if (fin.fail())
	//		return E_FAIL;

	//	// RootUI Data
	//	wstring wstrDataFilePath   = L"";			// DataFilePath
	//	wstring wstrRootObjectTag  = L"";			// ObjectTag
	//	_vec3	vPos               = _vec3(0.0f);	// Pos
	//	_vec3	vScale             = _vec3(1.0f);	// Scale
	//	_long	UIDepth            = 0;				// UIDepth
	//	_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
	//	_float	fFrameSpeed        = 0.0f;			// FrameSpeed
	//	_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
	//	_vec3	vRectScale         = _vec3(1.0f);	// RectScale
	//	_int	iChildUISize       = 0;				// ChildUI Size

	//	while (true)
	//	{
	//		fin >> wstrDataFilePath
	//			>> wstrRootObjectTag
	//			>> vPos.x
	//			>> vPos.y
	//			>> vScale.x
	//			>> vScale.y
	//			>> UIDepth
	//			>> bIsSpriteAnimation
	//			>> fFrameSpeed
	//			>> vRectPosOffset.x
	//			>> vRectPosOffset.y
	//			>> vRectScale.x
	//			>> vRectScale.y
	//			>> iChildUISize;

	//		if (fin.eof())
	//			break;

	//		// UIRoot 积己.
	//		Engine::CGameObject* pRootUI = nullptr;
	//		pRootUI = CMainMenuEquipment::Create(m_pGraphicDevice, m_pCommandList,
	//											 wstrRootObjectTag,
	//											 wstrDataFilePath,
	//											 vPos,
	//											 vScale,
	//											 bIsSpriteAnimation,
	//											 fFrameSpeed,
	//											 vRectPosOffset,
	//											 vRectScale,
	//											 UIDepth);
	//		m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
	//	}
	//}

	return S_OK;
}

HRESULT CStageHSY::Ready_LayerFont(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CStageHSY::Ready_LightInfo()
{
	wifstream fin{ L"../../Bin/ToolData/StageVelika_DirectionAndShadow.lightinginfo" };
	if (fin.fail())
		return E_FAIL;

	while (true)
	{
		Engine::D3DLIGHT tDirLightInfo;
		_vec3	vLightAt;
		_float	fHeight;
		_float	fFovY;
		_float	fFar;

		// DirectionLight Data 阂矾坷扁.
		fin >> tDirLightInfo.Diffuse.x		// Diffuse
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

			// ShadowLight Data 阂矾坷扁.
			>> vLightAt.x					// ShadowLight At
			>> vLightAt.y
			>> vLightAt.z
			>> fHeight						// ShadowLight Height
			>> fFovY						// ShadowLight FovY
			>> fFar;						// ShadowLight Far

		if (fin.eof())
			break;

		Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice, m_pCommandList, 
																				 Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL,
																				 tDirLightInfo), E_FAIL);
		CShadowLightMgr::Get_Instance()->Set_LightAt(vLightAt);
		CShadowLightMgr::Get_Instance()->Set_LightHeight(fHeight);
		CShadowLightMgr::Get_Instance()->Set_LightFovY(fFovY);
		CShadowLightMgr::Get_Instance()->Set_LightFar(fFar);
		CShadowLightMgr::Get_Instance()->Update_ShadowLight();
	}


	wifstream fin2 { "../../Bin/ToolData/StageVelika_PointLight.lightinginfo" };
	if (fin2.fail())
		return E_FAIL;

	while (true)
	{
		// PointLight 沥焊 历厘.
		Engine::D3DLIGHT tLightInfo { };
		tLightInfo.Type = Engine::D3DLIGHT_POINT;

				// PointLight Data 阂矾坷扁.
		fin2	>> tLightInfo.Diffuse.x		// Diffuse
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
				>> tLightInfo.Position.x	// Position
				>> tLightInfo.Position.y
				>> tLightInfo.Position.z
				>> tLightInfo.Position.w
				>> tLightInfo.Range;		// Range

		if (fin2.eof())
			break;

		Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice, m_pCommandList,
																				 Engine::LIGHTTYPE::D3DLIGHT_POINT,
																				 tLightInfo), E_FAIL);
	}

	return S_OK;
}

HRESULT CStageHSY::Ready_NaviMesh()
{
	Engine::CNaviMesh* pNaviMesh = nullptr;

	pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice,  m_pCommandList, wstring(L"../../Bin/ToolData/TestNavigationCell.navimeshcellinfo"));
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"TestNaviMesh", Engine::ID_DYNAMIC, pNaviMesh), E_FAIL);

	pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice,  
										  m_pCommandList, 
										  wstring(L"../../Bin/ToolData/StageVelika_NaviMesh.navimeshcellinfo"),
										  _vec3(STAGE_VELIKA_OFFSET_X, 0.0f, STAGE_VELIKA_OFFSET_Z));
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC, pNaviMesh), E_FAIL);

	pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice,  
										  m_pCommandList, 
										  wstring(L"../../Bin/ToolData/StageBeach_NaviMesh3.navimeshcellinfo"),
										  _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z));
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"StageBeach_NaviMesh", Engine::ID_DYNAMIC, pNaviMesh), E_FAIL);

	pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice, m_pCommandList, wstring(L"../../Bin/ToolData/StageWendigo_NaviMesh.navimeshcellinfo"));
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"StageWendigo_NaviMesh", Engine::ID_DYNAMIC, pNaviMesh), E_FAIL);

	return S_OK;
}

CStageHSY * CStageHSY::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CStageHSY* pInstance = new CStageHSY(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CStageHSY::Free()
{
	Engine::CScene::Free();

	Engine::CShaderShadowInstancing::Get_Instance()->Reset_InstancingContainer();
	Engine::CShaderShadowInstancing::Get_Instance()->Reset_InstancingConstantBuffer();
	Engine::CShaderMeshInstancing::Get_Instance()->Reset_InstancingContainer();
	Engine::CShaderMeshInstancing::Get_Instance()->Reset_InstancingConstantBuffer();
	Engine::CShaderLightingInstancing::Get_Instance()->Reset_Instance();
	Engine::CShaderLightingInstancing::Get_Instance()->Reset_InstancingConstantBuffer();
	Engine::CShaderColorInstancing::Get_Instance()->Reset_Instance();
	Engine::CShaderColorInstancing::Get_Instance()->Reset_InstancingConstantBuffer();
}
