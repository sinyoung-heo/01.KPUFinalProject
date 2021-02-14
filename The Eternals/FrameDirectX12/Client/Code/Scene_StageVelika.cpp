#include "stdafx.h"
#include <fstream>
#include "Scene_StageVelika.h"
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


CScene_StageVelika::CScene_StageVelika(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CScene(pGraphicDevice, pCommandList)
{
}



HRESULT CScene_StageVelika::Ready_Scene()
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

	return S_OK;
}

_int CScene_StageVelika::Update_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::Update_Scene(fTimeDelta);
}

_int CScene_StageVelika::LateUpdate_Scene(const _float & fTimeDelta)
{
	return Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

HRESULT CScene_StageVelika::Render_Scene(const _float & fTimeDelta, const Engine::RENDERID& eID)
{
	Engine::FAILED_CHECK_RETURN(CScene::Render_Scene(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

HRESULT CScene_StageVelika::Ready_LayerCamera(wstring wstrLayerTag)
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
									Engine::CAMERA_DESC(_vec3(95.0f, 45.0f, -20.0f),	// Eye
														_vec3(95.0f, 40.0f, 15.0f),		// At
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
									  Engine::CAMERA_DESC(_vec3(95.0f, 45.0f, -20.0f),	// Eye
									  					  _vec3(95.0f, 40.0f, 15.0f),	// At
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


HRESULT CScene_StageVelika::Ready_LayerEnvironment(wstring wstrLayerTag)
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
							   L"SkyBox",					// Texture Tag
							   _vec3(512.f, 512.f, 512.f),	// Scale
							   _vec3(0.0f, 0.0f, 0.0f),		// Angle
							   _vec3(0.0f, 0.0f, 0.0f),		// Pos
							   1);							// Tex Index
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"SkyBox", pGameObj), E_FAIL);

	return S_OK;
}

HRESULT CScene_StageVelika::Ready_LayerGameObject(wstring wstrLayerTag)
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
	[ BumpTerrainMesh ]
	____________________________________________________________________________________________________________*/
	_vec3 vStartPos = _vec3(0.0f, -0.1f, 0.5f);
	_vec3 vOffset   = _vec3(13.83f, 0.0f, 11.98f);

	for (_int i = 0; i < 21; ++i)
	{
		if (0 == i % 2)
			vStartPos.x = 0.0f;
		else
			vStartPos.x = 6.915f;

		for (_int j = 0; j < 21; ++j)
		{
			pGameObj = CStaticMeshObject::Create(m_pGraphicDevice, m_pCommandList,
												 L"BumpTerrainMesh01",		// MeshTag
												 _vec3(0.003f),				// Scale
												 _vec3(90.0f, 0.0f, 0.0f),	// Angle
												 vStartPos,					// Pos
												 true,						// Render Shadow
												 false,						// Is Collision
												 _vec3(0.0f),				// Bounding Sphere Scale
												 _vec3(0.0f));				// Bounding Sphere Pos
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"BumpTerrainMesh01", pGameObj), E_FAIL);
			vStartPos.x += vOffset.x;
		}

		vStartPos.z += vOffset.z;
	}

	Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(L"BumpTerrainMesh01");
	Engine::CShaderShadowInstancing::Get_Instance()->SetUp_Instancing(L"BumpTerrainMesh01");
	

	/*__________________________________________________________________________________________________________
	[ StaticMeshObject ]
	____________________________________________________________________________________________________________*/
	wifstream fin { L"../../Bin/ToolData/StageVelika_StaticMesh.staticmesh" };
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

		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", wstrMeshTag, pGameObj), E_FAIL);

		Engine::CShaderMeshInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
		Engine::CShaderShadowInstancing::Get_Instance()->SetUp_Instancing(wstrMeshTag);
	}

	/*__________________________________________________________________________________________________________
	[ Popori_F ]
	____________________________________________________________________________________________________________*/
	pGameObj =	CPopori_F::Create(m_pGraphicDevice, m_pCommandList,
								  L"PoporiR19",					// MeshTag
								  L"StageVelika_NaviMesh",		// NaviMeshTag
								  _vec3(0.05f, 0.05f, 0.05f),	// Scale
								  _vec3(0.0f, 0.0f, 0.0f),		// Angle
								  _vec3(120.0f, 0.f, 75.0f));	// Pos
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"PoporiR19", pGameObj), E_FAIL);


	/*__________________________________________________________________________________________________________
	[ TexEffect ]
	____________________________________________________________________________________________________________*/
	//// Fire
	//pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
	//								  L"Fire",						// TextureTag
	//								  _vec3(2.5f, 2.5f, 1.0f),		// Scale
	//								  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//								  _vec3(26.0f, 1.5f, 26.5f),	// Pos
	//								  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TexEffect", pGameObj), E_FAIL);

	//// Torch
	//pGameObj = CTextureEffect::Create(m_pGraphicDevice, m_pCommandList,
	//								  L"Torch",						// TextureTag
	//								  _vec3(2.5f, 5.0f, 1.0f),		// Scale
	//								  _vec3(0.0f, 0.0f, 0.0f),		// Angle
	//								  _vec3(28.0f, 2.0f, 27.0f),	// Pos
	//								  FRAME(8, 8, 64.0f));			// Sprite Image Frame
	//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(wstrLayerTag, L"TexEffect", pGameObj), E_FAIL);


	return S_OK;
}

HRESULT CScene_StageVelika::Ready_LayerUI(wstring wstrLayerTag)
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

HRESULT CScene_StageVelika::Ready_LayerFont(wstring wstrLayerTag)
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

HRESULT CScene_StageVelika::Ready_LightInfo()
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


	wifstream fin2 { "../../Bin/ToolData/StageVelika_PointLight.lightinginfo" };
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

HRESULT CScene_StageVelika::Ready_NaviMesh()
{
	Engine::CNaviMesh* pNaviMesh = Engine::CNaviMesh::Create(m_pGraphicDevice,  m_pCommandList,
															 wstring(L"../../Bin/ToolData/StageVelika_NaviMesh.navimeshcellinfo"));
	Engine::FAILED_CHECK_RETURN(Engine::CComponentMgr::Get_Instance()->Add_ComponentPrototype(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC, pNaviMesh), E_FAIL);

	return S_OK;
}

CScene_StageVelika * CScene_StageVelika::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CScene_StageVelika* pInstance = new CScene_StageVelika(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Scene()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CScene_StageVelika::Free()
{
	Engine::CScene::Free();
}
