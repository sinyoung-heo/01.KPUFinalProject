#include "stdafx.h"
#include <fstream>
#include "StageLDH.h"
#include "ComponentMgr.h"
#include "GraphicDevice.h"
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
#include "TerrainMeshObject.h"

#include "Chicken.h"
#include "Cat.h"
#include "Aman_boy.h"
#include "Human_boy.h"
#include "Human_girl.h"
#include "NPC_Villagers.h"
#include "Baraka_M_Extractor.h"
#include "Baraka_M_Merchant.h"
#include "Baraka_M_Mystellium.h"

CStageLDH::CStageLDH(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}



HRESULT CStageLDH::Ready_Scene()
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

	Engine::CShaderShadowInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);
	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_ConstantBuffer(m_pGraphicDevice);

#ifdef SERVER
	Engine::FAILED_CHECK_RETURN(CPacketMgr::Get_Instance()->Ready_Server(m_pGraphicDevice,m_pCommandList), E_FAIL);
	Engine::FAILED_CHECK_RETURN(CPacketMgr::Get_Instance()->Connect_Server(), E_FAIL);
#endif

	return S_OK;
}

_int CStageLDH::Update_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CStageLDH::LateUpdate_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

HRESULT CStageLDH::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

#ifdef SERVER
	CPacketMgr::Get_Instance()->recv_packet();
#endif

	return S_OK;
}

HRESULT CStageLDH::Ready_LayerCamera(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);
	
	/*__________________________________________________________________________________________________________
	[ Camera Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ DebugCamera ]
	____________________________________________________________________________________________________________*/
	pGameObj = CDebugCamera::Create(m_pGraphicDevice, m_pCommandList,
									Engine::CAMERA_DESC(_vec3(30.0f, 30.0f, 35.0f),	// Eye
														_vec3(20.0f, 15.0f, 10.0f),	// At
														_vec3(0.0f, 1.0f, 0.f)),	// Up
									Engine::PROJ_DESC(60.0f,						// FovY
													  _float(WINCX) / _float(WINCY),// Aspect
													  0.1f,							// Near
													  1000.0f),						// Far
									Engine::ORTHO_DESC(WINCX,						// Viewport Width
													   WINCY,						// Viewport Height
													   0.0f,						// Near
													   1.0f));						// Far
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"DebugCamera", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ DynamicCamera ]
	____________________________________________________________________________________________________________*/
	pGameObj = CDynamicCamera::Create(m_pGraphicDevice, m_pCommandList,
									  Engine::CAMERA_DESC(_vec3(30.0f, 25.0f, -35.0f),	// Eye
									  					  _vec3(20.0f, 15.0f, 10.0f),	// At
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


HRESULT CStageLDH::Ready_LayerEnvironment(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Environment Layer 생성 ]
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
							   _vec3(512.f, 512.f, 512.f),			// Scale
							   _vec3(0.0f, 0.0f, 0.0f),				// Angle
							   _vec3(0.0f, 0.0f, 0.0f));			// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Sector Grid ]
	____________________________________________________________________________________________________________*/
	_int world_width	= 1000;
	_int world_height	= 1000;
	_int sector_size	= 40;

	_vec3 vOffset(_float(sector_size), 0.0f, _float(sector_size));
	_vec3 vCount((_float)(world_width / sector_size), 0.0f, _float(world_height / sector_size));

	_vec3 vPos = _vec3(0.0f, 0.0f, (_float)world_height / 2);
	for (_int i = 0; i < vCount.x; ++i)
	{
		pGameObj = CCubeObject::Create(m_pGraphicDevice, m_pCommandList,
									   _vec3(0.25f, 1.0f, (_float)world_height),	// Scale
									   _vec3(0.0f),								// Angle
									   vPos);									// Pos
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Grid_Width", pGameObj), E_FAIL);

		vPos.x += vOffset.x;
	}

	vPos = _vec3((_float)world_width / 2, 0.0f, 0.0f);
	for (_int i = 0; i < vCount.z; ++i)
	{
		pGameObj = CCubeObject::Create(m_pGraphicDevice, m_pCommandList,
									   _vec3((_float)world_width, 1.0f, 0.25f),	// Scale
									   _vec3(0.0f),								// Angle
									   vPos);									// Pos
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Grid_Height", pGameObj), E_FAIL);

		vPos.z += vOffset.z;
	}

	return S_OK;
}

HRESULT CStageLDH::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	Engine::CGameObject* pGameObj = nullptr;

	/*__________________________________________________________________________________________________________
	[ StaticMeshObject ]
	____________________________________________________________________________________________________________*/
	wifstream fin { L"../../Bin/ToolData/TestStaticMesh.staticmesh" };
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
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, wstrMeshTag, pGameObj), E_FAIL);

		Engine::CShaderShadowInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
		Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	}

#ifndef SERVER
	/*__________________________________________________________________________________________________________
	[ Popori_F ]
	____________________________________________________________________________________________________________*/
	pGameObj = CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
		L"PoporiR19",					// MeshTag
		_vec3(0.05f, 0.05f, 0.05f),	// Scale
		_vec3(0.0f, 0.0f, 0.0f),		// Angle
		_vec3(25.0f, 0.f, 20.0f));	// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pGameObj), E_FAIL);


	pGameObj = CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
		L"PoporiH25",					// MeshTag
		_vec3(0.05f, 0.05f, 0.05f),	// Scale
		_vec3(0.0f, 0.0f, 0.0f),		// Angle
		_vec3(26.5f, 0.f, 20.0f));	// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pGameObj), E_FAIL);

#endif // !SERVER

	//pGameObj = CChicken::Create(m_pGraphicDevice, m_pCommandList,
	//							L"Chicken",					// MeshTag
	//							L"TestNaviMesh",    // NaviMeshTag
	//							_vec3(0.05f, 0.05f, 0.05f),	// Scale
	//							_vec3(0.0f, 0.0f, 0.0f),	// Angle
	//							_vec3(25.0f, 0.f, 20.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"NPC", pGameObj), E_FAIL);

	//pGameObj = CCat::Create(m_pGraphicDevice, m_pCommandList,
	//						L"Cat",					// MeshTag
	//						L"TestNaviMesh",    // NaviMeshTag
	//						_vec3(0.05f, 0.05f, 0.05f),	// Scale
	//						_vec3(0.0f, 0.0f, 0.0f),	// Angle
	//						_vec3(27.0f, 0.f, 20.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"NPC", pGameObj), E_FAIL);

	//pGameObj = CAman_boy::Create(m_pGraphicDevice, m_pCommandList,
	//							 L"Aman_boy",					// MeshTag
	//							 L"TestNaviMesh",    // NaviMeshTag
	//							 _vec3(0.05f, 0.05f, 0.05f),	// Scale
	//							 _vec3(0.0f, 0.0f, 0.0f),	// Angle
	//							 _vec3(29.0f, 0.f, 20.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"NPC", pGameObj), E_FAIL);

	//pGameObj = CHuman_boy::Create(m_pGraphicDevice, m_pCommandList,
	//							  L"Human_boy",					// MeshTag
	//							  L"TestNaviMesh",    // NaviMeshTag
	//							  _vec3(0.05f, 0.05f, 0.05f),	// Scale
	//							  _vec3(0.0f, 0.0f, 0.0f),	// Angle
	//							  _vec3(31.0f, 0.f, 20.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"NPC", pGameObj), E_FAIL);

	//
	//pGameObj = CNPC_Villagers::Create(m_pGraphicDevice, m_pCommandList,
	//								  L"NPC_Villagers",					// MeshTag
	//								  L"TestNaviMesh",    // NaviMeshTag
	//								  _vec3(0.05f, 0.05f, 0.05f),	// Scale
	//								  _vec3(0.0f, 0.0f, 0.0f),	// Angle
	//								  _vec3(19.0f, 0.f, 20.0f));	// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"NPC", pGameObj), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ TexEffect ]
	____________________________________________________________________________________________________________*/
	// Fire
	pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
									  L"Fire",						// TextureTag
									  _vec3(2.5f, 2.5f, 1.0f),		// Scale
									  _vec3(0.0f, 0.0f, 0.0f),		// Angle
									  _vec3(26.0f, 1.5f, 26.5f),	// Pos
									  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Fire", pGameObj), E_FAIL);

	// Torch
	pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
									  L"Torch",						// TextureTag
									  _vec3(2.5f, 5.0f, 1.0f),		// Scale
									  _vec3(0.0f, 0.0f, 0.0f),		// Angle
									  _vec3(28.0f, 2.0f, 27.0f),	// Pos
									  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Torch", pGameObj), E_FAIL);


	return S_OK;
}

HRESULT CStageLDH::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CStageLDH::Ready_LayerFont(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font Layer 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CStageLDH::Ready_LightInfo()
{
	wifstream fin{ L"../../Bin/ToolData/TestLightInfo_DirectionAndShadow.lightinginfo" };
	if (fin.fail())
		return E_FAIL;

	while (true)
	{
		Engine::D3DLIGHT tDirLightInfo;
		_vec3	vLightAt;
		_float	fHeight;
		_float	fFovY;
		_float	fFar;

		// DirectionLight Data 불러오기.
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

			// ShadowLight Data 불러오기.
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


	wifstream fin2 { "../../Bin/ToolData/TestLightInfo_PointLight.lightinginfo" };
	if (fin2.fail())
		return E_FAIL;

	while (true)
	{
		// PointLight 정보 저장.
		Engine::D3DLIGHT tLightInfo { };
		tLightInfo.Type = Engine::D3DLIGHT_POINT;

				// PointLight Data 불러오기.
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

HRESULT CStageLDH::Ready_NaviMesh()
{
	Engine::CNaviMesh* pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice,  m_pCommandList,
															 wstring(L"../../Bin/ToolData/TestNavigationCell.navimeshcellinfo"));
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"TestNaviMesh", Engine::ID_DYNAMIC, pNaviMesh), E_FAIL);

	return S_OK;
}

CStageLDH * CStageLDH::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CStageLDH* pInstance = new CStageLDH(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CStageLDH::Free()
{
	Engine::CScene::Free();
}
