#include "stdafx.h"
#include <fstream>
#include "Scene_MainStage.h"
#include "ComponentMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "LightMgr.h"
#include "InstancePoolMgr.h"
#include "Font.h"
#include "DebugCamera.h"
#include "DynamicCamera.h"
#include "CubeObject.h"
#include "StaticMeshObject.h"
#include "TerrainMeshObject.h"
#include "WaterMeshObject.h"
#include "TextureEffect.h"
#include "SkyBox.h"
#include "GameUIRoot.h"
#include "GameUIChild.h"
#include "CharacterHpGauge.h"
#include "CharacterMpGauge.h"
#include "MainMenuEquipment.h"
#include "MainMenuInventory.h"
#include "MainMenuLogout.h"
#include "MainMenuSetting.h"
#include "PCGladiator.h"
#include "SampleNPC.h"
#include "FadeInOut.h"
#include "Portal.h"
#include "WaterFall.h"
#include "MainMenuSettingCanvas.h"
#include "SettingButtonClose.h"
#include "SettingButtonGameExit.h"
#include "SettingButtonShaderCheckBox.h"

CScene_MainStage::CScene_MainStage(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}



HRESULT CScene_MainStage::Ready_Scene()
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


	CMouseCursorMgr::Get_Instance()->Set_IsActiveMouse(false);
	CInstancePoolMgr::Get_Instance()->Ready_InstancePool(m_pGraphicDevice, m_pCommandList);

	// Server
	Engine::FAILED_CHECK_RETURN(CPacketMgr::Get_Instance()->Ready_Server(m_pGraphicDevice, m_pCommandList), E_FAIL);
	Engine::FAILED_CHECK_RETURN(CPacketMgr::Get_Instance()->Connect_Server(), E_FAIL);

	return S_OK;
}

void CScene_MainStage::Process_PacketFromServer()
{
	CPacketMgr::Get_Instance()->recv_packet();
}

_int CScene_MainStage::Update_Scene(const _float & fTimeDelta)
{
	// MouseCursorMgr
	if (!m_bIsReadyMouseCursorMgr)
	{
		m_bIsReadyMouseCursorMgr = true;
		CMouseCursorMgr::Get_Instance()->Ready_MouseCursorMgr();
	}

	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CScene_MainStage::LateUpdate_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

void CScene_MainStage::Send_PacketToServer()
{
	Engine::CScene::Send_PacketToServer();
}

HRESULT CScene_MainStage::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

HRESULT CScene_MainStage::Ready_LayerCamera(wstring wstrLayerTag)
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
									Engine::CAMERA_DESC(_vec3(41.0f, 79.0f, -124.0f),	// Eye
														_vec3(43.0f, 79.0f, -115.0f),	// At
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
									  Engine::CAMERA_DESC(_vec3(41.0f, 79.0f, -124.0f),	// Eye
									  					  _vec3(43.0f, 79.0f, -115.0f),	// At
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


HRESULT CScene_MainStage::Ready_LayerEnvironment(wstring wstrLayerTag)
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
							   L"SkyBox",						// Texture Tag
							   _vec3(990.0f, 990.0f, 990.0f),	// Scale
							   _vec3(0.0f, 0.0f, 0.0f),			// Angle
							   _vec3(0.0f, 0.0f, 0.0f),			// Pos
							   1);								// Tex Index
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Portal ]
	____________________________________________________________________________________________________________*/
	pGameObj = CPortal::Create(m_pGraphicDevice, m_pCommandList, 
							   L"Portal_VelikaToBeach", 
							   _vec3(20.0f),
							   _vec3(PORTAL_FROM_VELIKA_TO_BEACH_X, 0.0f, PORTAL_FROM_VELIKA_TO_BEACH_Z));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Portal", pGameObj), E_FAIL);

	pGameObj = CPortal::Create(m_pGraphicDevice, m_pCommandList, 
							   L"Portal_BeachToVelika", 
							   _vec3(20.0f),
							   _vec3(PORTAL_FROM_BEACH_TO_VELIKA_X, 0.0f, PORTAL_FROM_BEACH_TO_VELIKA_Z));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Portal", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ StaticMeshObject ]
	____________________________________________________________________________________________________________*/
	wstring	wstrMeshTag				= L"";
	_vec3	vScale					= _vec3(0.0f);
	_vec3	vAngle					= _vec3(0.0f);
	_vec3	vPos					= _vec3(0.0f);
	_bool	bIsRenderShadow			= false;
	_bool	bIsCollision			= false;
	_vec3	vBoundingSphereScale	= _vec3(0.0f);
	_vec3	vBoundingSpherePos      = _vec3(0.0f);
	_bool	bIsMousePicking			= false;

	// Velika
	wifstream fin_velika { L"../../Bin/ToolData/StageVelika_StaticMesh.staticmesh" };
	if (fin_velika.fail())
		return E_FAIL;

	while (true)
	{
		fin_velika	>> wstrMeshTag 				// MeshTag
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

		if (fin_velika.eof())
			break;

		pGameObj = CStaticMeshObject::Create(m_pGraphicDevice, m_pCommandList,
											 wstrMeshTag,			// MeshTag
											 vScale,				// Scale
											 vAngle,				// Angle
											 vPos,					// Pos
											 bIsRenderShadow,		// Render Shadow
											 bIsCollision,			// Bounding Sphere
											 vBoundingSphereScale,	// Bounding Sphere Scale
											 vBoundingSpherePos,	// Bounding Sphere Pos
											 _vec3(STAGE_VELIKA_OFFSET_X, 0.0f, STAGE_VELIKA_OFFSET_Z));

		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_VELIKA, wstrMeshTag, pGameObj), E_FAIL);
	}
	pGameObj = CTerrainMeshObject::Create(m_pGraphicDevice, m_pCommandList,
										  L"BumpTerrainMesh00",
										  _vec3(0.075f),
										  _vec3(90.0f, 0.0f ,0.0f),
										  _vec3(128.0f, -0.0f, 128.0f),
										  _vec3(STAGE_VELIKA_OFFSET_X, 0.0f, STAGE_VELIKA_OFFSET_Z));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_VELIKA, L"BumpTerrainMesh00", pGameObj), E_FAIL);

	static_cast<CTerrainMeshObject*>(pGameObj)->Set_Wave(1.f);
	// Beach
	wifstream fin_beach { L"../../Bin/ToolData/StageBeach_StaticMesh.staticmesh" };
	if (fin_beach.fail())
		return E_FAIL;

	while (true)
	{
		fin_beach	>> wstrMeshTag 				// MeshTag
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

		if (fin_beach.eof())
			break;

		if (wstrMeshTag.find(L"WaterFall") != wstring::npos)
		 {
		    pGameObj = CWaterFall::Create(m_pGraphicDevice, m_pCommandList,
										  wstrMeshTag,         // MeshTag
										  vScale,				// Scale
										  vAngle,				// Angle
										  vPos,
										  _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z));

		 }
		 else
		 {
		    pGameObj = CStaticMeshObject::Create(m_pGraphicDevice, m_pCommandList,
												 wstrMeshTag,				// MeshTag
												 vScale,					// Scale
												 vAngle,					// Angle
												 vPos,
												 bIsRenderShadow,			// Render Shadow
												 bIsCollision,				// Bounding Sphere
												 vBoundingSphereScale,		// Bounding Sphere Scale
												 vBoundingSpherePos,		// Bounding Sphere Pos
												 _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z));
		 }

		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_BEACH, wstrMeshTag, pGameObj), E_FAIL);
	}
	pGameObj = CTerrainMeshObject::Create(m_pGraphicDevice, m_pCommandList,
										  L"BumpDesertMesh00",
										  _vec3(0.145f),
										  _vec3(90.0f, 40.0f, 0.0f),
										  _vec3(128.0f, 0.01f, 128.0f),
										  _vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_BEACH, L"BumpDesertMesh00", pGameObj), E_FAIL);
	static_cast<CTerrainMeshObject*>(pGameObj)->Set_Wave(0.8f);
	pGameObj = CWaterMeshObject::Create(m_pGraphicDevice, m_pCommandList,
										L"BumpWaterMesh00",
										_vec3(0.145f),
										_vec3(90.f, 40.0f, 0.0f),
										_vec3(256.0f, 0.7f, 276.0f),
										_vec3(STAGE_BEACH_OFFSET_X, 0.0f, STAGE_BEACH_OFFSET_Z));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_BEACH, L"BumpWaterMesh00", pGameObj), E_FAIL);

	pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
		L"Torch",
		_vec3(3.f),
		_vec3(0.f),
		_vec3(80.6f +256.f, 2.f, 157.75f)
	,FRAME(8.f,8.f,64.f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_BEACH, L"Torch", pGameObj), E_FAIL);
	pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
		L"Torch",
		_vec3(2.f),
		_vec3(0.f),
		_vec3(82.9f + 256.f, 0.7f, 149.72f ), FRAME(8.f, 8.f, 64.f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(Engine::STAGEID::STAGE_BEACH, L"Torch", pGameObj), E_FAIL);
	/*__________________________________________________________________________________________________________
	[ Sector Grid ]
	____________________________________________________________________________________________________________*/
	_int world_width	= WORLD_WIDTH;
	_int world_height	= WORLD_HEIGHT;
	_int sector_size	= SECTOR_SIZE;

	_vec3 vOffset(_float(sector_size), 0.0f, _float(sector_size));
	_vec3 vCount((_float)(world_width / sector_size), 0.0f, _float(world_height / sector_size));

	vPos = _vec3(0.0f, 0.2f, (_float)world_height / 2);
	for (_int i = 0; i < vCount.x + 1; ++i)
	{
		pGameObj = CCubeObject::Create(m_pGraphicDevice, m_pCommandList,
									   _vec3(0.1f, 0.1f, (_float)world_height), // Scale
									   _vec3(0.0f),								 // Angle
									   vPos);									 // Pos
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Grid_Width", pGameObj), E_FAIL);

		vPos.x += vOffset.x;
	}

	vPos = _vec3((_float)world_width / 2, 0.2f, 0.0f);
	for (_int i = 0; i < vCount.z + 1; ++i)
	{
		pGameObj = CCubeObject::Create(m_pGraphicDevice, m_pCommandList,
									   _vec3((_float)world_width, 0.1f, 0.1f),	// Scale
									   _vec3(0.0f),								// Angle
									   vPos);									// Pos
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Grid_Height", pGameObj), E_FAIL);

		vPos.z += vOffset.z;
	}

	return S_OK;
}

HRESULT CScene_MainStage::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	Engine::CGameObject* pGameObj = nullptr;


	return S_OK;
}

HRESULT CScene_MainStage::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 积己 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ FadeInOut ]
	____________________________________________________________________________________________________________*/
	pGameObj = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::FADE_IN);
	Engine::NULL_CHECK_RETURN(pGameObj, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"FadeInOut", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ MainMenuLogout ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuLogout_Normal.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			if (fin.eof())
				break;

			// UIRoot 积己.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CMainMenuLogout::Create(m_pGraphicDevice, m_pCommandList,
											  wstrRootObjectTag,
											  wstrDataFilePath,
											  vPos,
											  vScale,
											  bIsSpriteAnimation,
											  fFrameSpeed,
											  vRectPosOffset,
											  vRectScale,
											  UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
		}
	}

	/*__________________________________________________________________________________________________________
	[ MainMenuSetting ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuSetting_Normal.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			if (fin.eof())
				break;

			// UIRoot 积己.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CMainMenuSetting::Create(m_pGraphicDevice, m_pCommandList,
											   wstrRootObjectTag,
											   wstrDataFilePath,
											   vPos,
											   vScale,
											   bIsSpriteAnimation,
											   fFrameSpeed,
											   vRectPosOffset,
											   vRectScale,
											   UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
		}
	}

	// MainMenuSettingCanvas
	CSettingButtonClose* pSettingButtonXMouseNormal  = nullptr;
	CSettingButtonClose* pSettingButtonXMouseOn      = nullptr;
	CSettingButtonClose* pSettingButtonXMouseClicked = nullptr;

	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuSettingCanvas.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		// ChildUI Data
		vector<wstring> vecDataFilePath;
		vector<wstring> vecObjectTag;
		vector<_vec3>	vecPos;
		vector<_vec3>	vecScale;
		vector<_long>	vecUIDepth;
		vector<_int>	vecIsSpriteAnimation;
		vector<_float>	vecFrameSpeed;
		vector<_vec3>	vecRectPosOffset;
		vector<_vec3>	vecRectScale;

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			vecDataFilePath.resize(iChildUISize);
			vecObjectTag.resize(iChildUISize);
			vecPos.resize(iChildUISize);
			vecScale.resize(iChildUISize);
			vecUIDepth.resize(iChildUISize);
			vecIsSpriteAnimation.resize(iChildUISize);
			vecFrameSpeed.resize(iChildUISize);
			vecRectPosOffset.resize(iChildUISize);
			vecRectScale.resize(iChildUISize);

			for (_int i = 0; i < iChildUISize; ++i)
			{
				fin >> vecDataFilePath[i]			// DataFilePath
					>> vecObjectTag[i]				// Object Tag
					>> vecPos[i].x					// Pos X
					>> vecPos[i].y					// Pos Y
					>> vecScale[i].x				// Scale X
					>> vecScale[i].y				// Scale Y
					>> vecUIDepth[i]				// UI Depth
					>> vecIsSpriteAnimation[i]		// Is SpriteAnimation
					>> vecFrameSpeed[i]				// Frame Speed
					>> vecRectPosOffset[i].x		// RectPosOffset X
					>> vecRectPosOffset[i].y		// RectPosOffset Y
					>> vecRectScale[i].x			// RectScale X
					>> vecRectScale[i].y;			// RectScale Y
			}

			if (fin.eof())
				break;

			// UIRoot 积己.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CMainMenuSettingCanvas::Create(m_pGraphicDevice, m_pCommandList,
													 wstrRootObjectTag,
													 wstrDataFilePath,
													 vPos,
													 vScale,
													 bIsSpriteAnimation,
													 fFrameSpeed,
													 vRectPosOffset,
													 vRectScale,
													 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);

			// UIChild 积己.
			for (_int i = 0; i < iChildUISize; ++i)
			{
				Engine::CGameObject* pChildUI = nullptr;

				if (L"ButtonExitGame" == vecObjectTag[i])
				{
					pChildUI = CSettingButtonGameExit::Create(m_pGraphicDevice, m_pCommandList,
															  wstrRootObjectTag,				// RootObjectTag
															  vecObjectTag[i],					// ObjectTag
															  vecDataFilePath[i],				// DataFilePath
															  vecPos[i],						// Pos
															  vecScale[i],						// Scane
															  (_bool)vecIsSpriteAnimation[i],	// Is Animation
															  vecFrameSpeed[i],					// FrameSpeed
															  vecRectPosOffset[i],				// RectPosOffset
															  vecRectScale[i],					// RectScaleOffset
															  vecUIDepth[i]);					// UI Depth
				}
				else if (L"SystemSettingButtonX_Normal" == vecObjectTag[i] ||
						 L"SystemSettingButtonX_MouseOn" == vecObjectTag[i] ||
						 L"SystemSettingButtonX_MouseClicked" == vecObjectTag[i])
				{
					pChildUI = CSettingButtonClose::Create(m_pGraphicDevice, m_pCommandList,
															  wstrRootObjectTag,				// RootObjectTag
															  vecObjectTag[i],					// ObjectTag
															  vecDataFilePath[i],				// DataFilePath
															  vecPos[i],						// Pos
															  vecScale[i],						// Scane
															  (_bool)vecIsSpriteAnimation[i],	// Is Animation
															  vecFrameSpeed[i],					// FrameSpeed
															  vecRectPosOffset[i],				// RectPosOffset
															  vecRectScale[i],					// RectScaleOffset
															  vecUIDepth[i]);					// UI Depth

					if (L"SystemSettingButtonX_MouseOn" == vecObjectTag[i])
						pSettingButtonXMouseOn = static_cast<CSettingButtonClose*>(pChildUI);
					else if (L"SystemSettingButtonX_MouseClicked" == vecObjectTag[i])
						pSettingButtonXMouseClicked = static_cast<CSettingButtonClose*>(pChildUI);
					else
						pSettingButtonXMouseNormal = static_cast<CSettingButtonClose*>(pChildUI);

				}
				else if (L"SystemSettingCheckBox_Shade" == vecObjectTag[i] ||
						 L"SystemSettingCheckBox_Specular" == vecObjectTag[i] || 
						 L"SystemSettingCheckBox_SSAO" == vecObjectTag[i] || 
						 L"SystemSettingCheckBox_HDRTone" == vecObjectTag[i] || 
						 L"SystemSettingCheckBox_DOF" == vecObjectTag[i] || 
						 L"SystemSettingCheckBox_Shadow" == vecObjectTag[i])
				{
					pChildUI = CSettingButtonShaderCheckBox::Create(m_pGraphicDevice, m_pCommandList,
																	wstrRootObjectTag,				// RootObjectTag
																	vecObjectTag[i],				// ObjectTag
																	vecDataFilePath[i],				// DataFilePath
																	vecPos[i],						// Pos
																	vecScale[i],					// Scane
																	(_bool)vecIsSpriteAnimation[i],	// Is Animation
																	vecFrameSpeed[i],				// FrameSpeed
																	vecRectPosOffset[i],			// RectPosOffset
																	vecRectScale[i],				// RectScaleOffset
																	vecUIDepth[i]);					// UI Depth
				}

				if (nullptr != pChildUI &&
					(L"SystemSettingButtonX_MouseOn" != vecObjectTag[i] && 
					 L"SystemSettingButtonX_MouseClicked" != vecObjectTag[i]))
				{
					m_pObjectMgr->Add_GameObject(L"Layer_UI", vecObjectTag[i], pChildUI);
					static_cast<CGameUIRoot*>(pRootUI)->Add_ChildUI(pChildUI);
				}
			}
		}

		UI_CHILD_STATE tState;

		tState.tFrame         = pSettingButtonXMouseOn->Get_Frame();
		tState.vPos           = pSettingButtonXMouseOn->Get_Transform()->m_vPos;
		tState.vScale         = pSettingButtonXMouseOn->Get_Transform()->m_vScale;
		tState.vRectPosOffset = pSettingButtonXMouseOn->Get_RectOffset();
		tState.vRectScale     = pSettingButtonXMouseOn->Get_TransformColor()->m_vScale;
		pSettingButtonXMouseNormal->SetUp_MainMenuState(L"MouseOn", tState);

		tState.tFrame         = pSettingButtonXMouseClicked->Get_Frame();
		tState.vPos           = pSettingButtonXMouseClicked->Get_Transform()->m_vPos;
		tState.vScale         = pSettingButtonXMouseClicked->Get_Transform()->m_vScale;
		tState.vRectPosOffset = pSettingButtonXMouseClicked->Get_RectOffset();
		tState.vRectScale     = pSettingButtonXMouseClicked->Get_TransformColor()->m_vScale;
		pSettingButtonXMouseNormal->SetUp_MainMenuState(L"MouseClicked", tState);

		pSettingButtonXMouseNormal	= nullptr;
		Engine::Safe_Release(pSettingButtonXMouseOn);
		Engine::Safe_Release(pSettingButtonXMouseClicked);
	}

	/*__________________________________________________________________________________________________________
	[ MainMenuInventory ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuInventory_Normal.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			if (fin.eof())
				break;

			// UIRoot 积己.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CMainMenuInventory::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
		}
	}

	/*__________________________________________________________________________________________________________
	[ MainMenuEquipment ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUIMainMenuEquipment_Normal.2DUI" };
		if (fin.fail())
			return E_FAIL;

		// RootUI Data
		wstring wstrDataFilePath   = L"";			// DataFilePath
		wstring wstrRootObjectTag  = L"";			// ObjectTag
		_vec3	vPos               = _vec3(0.0f);	// Pos
		_vec3	vScale             = _vec3(1.0f);	// Scale
		_long	UIDepth            = 0;				// UIDepth
		_bool	bIsSpriteAnimation = false;			// IsSpriteAnimation
		_float	fFrameSpeed        = 0.0f;			// FrameSpeed
		_vec3	vRectPosOffset     = _vec3(0.0f);	// RectPosOffset
		_vec3	vRectScale         = _vec3(1.0f);	// RectScale
		_int	iChildUISize       = 0;				// ChildUI Size

		while (true)
		{
			fin >> wstrDataFilePath
				>> wstrRootObjectTag
				>> vPos.x
				>> vPos.y
				>> vScale.x
				>> vScale.y
				>> UIDepth
				>> bIsSpriteAnimation
				>> fFrameSpeed
				>> vRectPosOffset.x
				>> vRectPosOffset.y
				>> vRectScale.x
				>> vRectScale.y
				>> iChildUISize;

			if (fin.eof())
				break;

			// UIRoot 积己.
			Engine::CGameObject* pRootUI = nullptr;
			pRootUI = CMainMenuEquipment::Create(m_pGraphicDevice, m_pCommandList,
												 wstrRootObjectTag,
												 wstrDataFilePath,
												 vPos,
												 vScale,
												 bIsSpriteAnimation,
												 fFrameSpeed,
												 vRectPosOffset,
												 vRectScale,
												 UIDepth);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", wstrRootObjectTag, pRootUI);
		}
	}

	return S_OK;
}

HRESULT CScene_MainStage::Ready_LayerFont(wstring wstrLayerTag)
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

HRESULT CScene_MainStage::Ready_LightInfo()
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

HRESULT CScene_MainStage::Ready_NaviMesh()
{
	Engine::CNaviMesh* pNaviMesh = nullptr;

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


	return S_OK;
}

CScene_MainStage * CScene_MainStage::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CScene_MainStage* pInstance = new CScene_MainStage(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CScene_MainStage::Free()
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
