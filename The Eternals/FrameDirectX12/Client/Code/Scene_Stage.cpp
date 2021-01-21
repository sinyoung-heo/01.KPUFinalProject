#include "stdafx.h"
#include "Scene_Stage.h"

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


CScene_Stage::CScene_Stage(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}



HRESULT CScene_Stage::Ready_Scene()
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

	return S_OK;
}

_int CScene_Stage::Update_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CScene_Stage::LateUpdate_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

HRESULT CScene_Stage::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

HRESULT CScene_Stage::Ready_LayerCamera(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);
	
	/*__________________________________________________________________________________________________________
	[ Camera Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	/*__________________________________________________________________________________________________________
	[ DebugCamera ]
	____________________________________________________________________________________________________________*/
	CDebugCamera* pDebugCamera = CDebugCamera::Create(m_pGraphicDevice, m_pCommandList,
													  Engine::CAMERA_DESC(_vec3(30.0f, 25.0f, 35.0f),	// Eye
													  					  _vec3(20.0f, 15.0f, 10.0f),	// At
													  					  _vec3(0.0f, 1.0f, 0.f)),		// Up
													  Engine::PROJ_DESC(60.0f,							// FovY
													  					_float(WINCX) / _float(WINCY),	// Aspect
													  					1.0f,							// Near
													  					1000.0f),						// Far
													  Engine::ORTHO_DESC(WINCX,							// Viewport Width
													  					 WINCY,							// Viewport Height
													  					 0.0f,							// Near
													  					 1.0f));						// Far
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"DebugCamera", pDebugCamera), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ DynamicCamera ]
	____________________________________________________________________________________________________________*/
	CDynamicCamera* pDynamicCamera = CDynamicCamera::Create(m_pGraphicDevice, m_pCommandList,
															Engine::CAMERA_DESC(_vec3(30.0f, 25.0f, -35.0f),// Eye
																				_vec3(20.0f, 15.0f, 10.0f),	// At
																				_vec3(0.0f, 1.0f, 0.0f)),	// Up

															Engine::PROJ_DESC(60.0f,						// FovY
																			  _float(WINCX) / _float(WINCY),// Aspect
																			  1.0f,							// Near
																			  1000.0f),						// Far

															Engine::ORTHO_DESC(WINCX,						// Viewport Width
																			   WINCY,						// Viewport Height
																			   0.0f,						// Near
																			   1.0f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"DynamicCamera", pDynamicCamera), E_FAIL);


	return S_OK;
}


HRESULT CScene_Stage::Ready_LayerEnvironment(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Environment Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);

	/*__________________________________________________________________________________________________________
	[ SkyBox ]
	____________________________________________________________________________________________________________*/
	CSkyBox* pSkyBox = CSkyBox::Create(m_pGraphicDevice, m_pCommandList,
									   L"SkyBox",							// Texture Tag
									   _vec3(512.f, 512.f, 512.f),			// Scale
									   _vec3(0.0f, 0.0f, 0.0f),				// Angle
									   _vec3(0.0f, 0.0f, 0.0f));			// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pSkyBox), E_FAIL);


	return S_OK;
}

HRESULT CScene_Stage::Ready_LayerGameObject(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ GameLogic Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	///*__________________________________________________________________________________________________________
	//[ TerrainObject ]
	//____________________________________________________________________________________________________________*/
	//CTerrainObject* pTerrainObject = CTerrainObject::Create(m_pGraphicDevice, m_pCommandList);
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TerrainObject", pTerrainObject), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ StaticMeshObject ]
	____________________________________________________________________________________________________________*/
	CStaticMeshObject* pStaticMeshObject = nullptr;

	//// LandMarkBoundary
	//pStaticMeshObject = CStaticMeshObject::Create(m_pGraphicDevice, m_pCommandList,
	//											  L"LandMarkBoundary",	// MeshTag 
	//											  _vec3(0.05f, 0.05f, 0.05f),				// Scale
	//											  _vec3(0.0f, 0.0f, 0.0f),					// Angle
	//											  _vec3(0.0f, 0.f, 0.f));					// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"StaticMeshObject", pStaticMeshObject), E_FAIL);


	// Status
	pStaticMeshObject = CStaticMeshObject::Create(m_pGraphicDevice, m_pCommandList,
												  L"Status",	// MeshTag 
												  _vec3(0.05f, 0.05f, 0.05f),	// Scale
												  _vec3(0.f, 0.0f, 0.0f),		// Angle
												  _vec3(-15.0f, 13.f, 0.f));	// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"StaticMeshObject", pStaticMeshObject), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ TerrainMeshObject ]
	____________________________________________________________________________________________________________*/
	//CTerrainMeshObject* pTerrainMeshObject = CTerrainMeshObject::Create(m_pGraphicDevice, m_pCommandList,
	//																	L"LandMarkPlane",	// MeshTag 
	//																	_vec3(0.05f, 0.05f, 0.05f),			// Scale
	//																	_vec3(0.0f, 0.0f, 0.0f),			// Angle
	//																	_vec3(0.0f, 0.f, 0.f));				// Pos
	//
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TerrainMeshObject", pTerrainMeshObject), E_FAIL);



	/*__________________________________________________________________________________________________________
	[ Popori_F ]
	____________________________________________________________________________________________________________*/
	CPopori_F* pPopori_F = nullptr;

	pPopori_F =	CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
								  L"PoporiR19",	// MeshTag
								  _vec3(0.25f, 0.25f, 0.25f),		// Scale
								  _vec3(0.0f, 0.0f, 0.0f),			// Angle
								  _vec3(58.0f, 0.f, 25.0f));		// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pPopori_F), E_FAIL);


	//pPopori_F = CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
	//							  L"PoporiH25",	// MeshTag
	//							  _vec3(0.25f, 0.25f, 0.25f),		// Scale
	//							  _vec3(0.0f, 0.0f, 0.0f),			// Angle
	//							  _vec3(0.0f, 0.f, 0.0f));			// Pos
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"Popori_F", pPopori_F), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ TexEffect ]
	____________________________________________________________________________________________________________*/
	CTextureEffect* pTexEffect = nullptr;

	// Fire
	pTexEffect = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
										L"Fire",							// TextureTag
										_vec3(5.0f, 5.0f, 1.0f),			// Scale
										_vec3(0.0f, 0.0f, 0.0f),			// Angle
										_vec3(6.0f, 2.5f, -10.0f),			// Pos
										FRAME(8, 8, 64.0f));				// Sprite Image Frame
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TexEffect", pTexEffect), E_FAIL);





	// Torch
	pTexEffect = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
										L"Torch",							// TextureTag
										_vec3(5.0f, 10.0f, 1.0f),			// Scale
										_vec3(0.0f, 0.0f, 0.0f),			// Angle
										_vec3(10.0f, 5.0f, -10.0f),			// Pos
										FRAME(8, 8, 64.0f));				// Sprite Image Frame
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TexEffect", pTexEffect), E_FAIL);


	return S_OK;
}

HRESULT CScene_Stage::Ready_LayerUI(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ UI Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CScene_Stage::Ready_LayerFont(wstring wstrLayerTag)
{
	Engine::NULL_CHECK_RETURN(m_pObjectMgr, E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Font Layer 持失 ]
	____________________________________________________________________________________________________________*/
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	Engine::NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_pObjectMgr->Add_Layer(wstrLayerTag, pLayer);


	return S_OK;
}

HRESULT CScene_Stage::Ready_LightInfo()
{
	Engine::D3DLIGHT tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(Engine::D3DLIGHT));

	// Direction
	tLightInfo.Type			= Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse		= _rgba(1.0f, 1.0f, 1.0f, 1.0f);
	tLightInfo.Specular		= _rgba(0.4f, 0.4f, 0.4f, 1.0f);
	tLightInfo.Ambient		= _rgba(0.0f, 0.0f, 0.0f, 1.0f);
	tLightInfo.Direction	= _vec4(1.0f, -1.0f, 1.0f, 0.0f);
	Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice, 
																			 m_pCommandList, 
																			 Engine::LIGHTTYPE::D3DLIGHT_DIRECTIONAL,
																			 tLightInfo), E_FAIL);

	// Point Light
	tLightInfo.Type			= Engine::LIGHTTYPE::D3DLIGHT_POINT;
	tLightInfo.Diffuse		= _rgba(0.2f, 0.2f, 1.0f, 1.0f);
	tLightInfo.Specular		= _rgba(0.3f, 0.3f, 0.3f, 1.0f);
	tLightInfo.Ambient		= _rgba(0.3f, 0.3f, 0.3f, 1.0f);
	tLightInfo.Position		= _vec4(5.0f, 50.0f, 0.0f, 1.0f);
	tLightInfo.Range		= 50.0f;
	Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice,
																			 m_pCommandList,
																			 Engine::LIGHTTYPE::D3DLIGHT_POINT,
																			 tLightInfo), E_FAIL);

	tLightInfo.Type			= Engine::LIGHTTYPE::D3DLIGHT_POINT;
	tLightInfo.Diffuse		= _rgba(1.0f, 0.2f, 0.2f, 1.0f);
	tLightInfo.Specular		= _rgba(0.3f, 0.3f, 0.3f, 1.0f);
	tLightInfo.Ambient		= _rgba(0.3f, 0.3f, 0.3f, 0.3f);
	tLightInfo.Position		= _vec4(10.0f, 50.0f, 0.0f, 1.0f);
	tLightInfo.Range		= 50.0f;
	Engine::FAILED_CHECK_RETURN(Engine::CLightMgr::Get_Instance()->Add_Light(m_pGraphicDevice, 
																			 m_pCommandList,
																			 Engine::LIGHTTYPE::D3DLIGHT_POINT,
																			 tLightInfo), E_FAIL);

	return S_OK;
}

HRESULT CScene_Stage::Ready_NaviMesh()
{
	Engine::CNaviMesh* pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice,
															 m_pCommandList,
															 wstring(L"../../Bin/ToolData/TestNaviMesh.dat"));

	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"TestNaviMesh",
																							  Engine::ID_DYNAMIC,
																							  pNaviMesh),
																							  E_FAIL);
	return S_OK;
}

CScene_Stage * CScene_Stage::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CScene_Stage* pInstance = new CScene_Stage(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CScene_Stage::Free()
{
	Engine::CScene::Free();
}
