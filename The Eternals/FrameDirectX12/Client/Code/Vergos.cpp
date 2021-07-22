#include "stdafx.h"
#include "Vergos.h"
#include "InstancePoolMgr.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "Font.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "CollisionTick.h"
#include "InstancePoolMgr.h"
#include "NormalMonsterHpGauge.h"
#include "BreathEffect.h"
#include "GameUIRoot.h"
#include "VergosHpGauge.h"
#include "BossDecal.h"
#include "DynamicCamera.h"
#include "FadeInOut.h"

CVergos::CVergos(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

HRESULT CVergos::Ready_GameObject(wstring wstrMeshTag, wstring wstrNaviMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag, wstrNaviMeshTag), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(vPos));

	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  _vec3(1200.0f),
											  _vec3(-120.0f, 40.f, 120.0f));

	Engine::FAILED_CHECK_RETURN(Create_Collider(), E_FAIL);

	m_wstrCollisionTag = L"Monster_MultiCollider";
	m_lstCollider.push_back(m_pBoundingSphereCom);

	m_pInfoCom->m_fSpeed = 1.f;
	m_bIsMoveStop = true;

	/*__________________________________________________________________________________________________________
	[ 局聪皋捞记 汲沥 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	m_iMonsterStatus = Vergos::A_WAIT;

	// Create HpGauge
	Engine::FAILED_CHECK_RETURN(Create_HpGauge(), E_FAIL);


	m_pHierarchyDesc[R_HAND]= &(m_pMeshCom->Find_HierarchyDesc("Bip01-R-Hand"));
	m_pHierarchyDesc[L_HAND] =& (m_pMeshCom->Find_HierarchyDesc("Bip01-L-Hand"));
	m_pHierarchyDesc[BREATH]= &(m_pMeshCom->Find_HierarchyDesc("FxShot"));
	m_pHierarchyDesc[HEAD]= &(m_pMeshCom->Find_HierarchyDesc("Bip01-Head"));
	return S_OK;
}

HRESULT CVergos::LateInit_GameObject()
{
	// DynamicCamera
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(12), E_FAIL);

	BazierPos[0] = _vec3(403.f, 0.f, 360.f);
	BazierPos[1] = _vec3(390.f, 0.f, 347.5f);
	BazierPos[2] = _vec3(350.f, 0.f, 362.f);

	return S_OK;
}

_int CVergos::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	EffectLoop(fTimeDelta);
	
	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		if (nullptr != m_pHpGaugeRoot)
		{
			m_pHpGaugeRoot->Set_IsActive(false);
			m_pHpGaugeRoot->Set_IsChildActive(false);
		}

		m_iSNum             = -1;
		m_bIsStartDissolve  = false;
		m_fDissolve         = -0.05f;
		m_bIsResetNaviMesh  = false;
		m_bIsCameraShaking  = false;
		m_bIsCameraShaking2 = false;
		m_bIsCameraShaking3 = false;
		m_bIsSpawn          = false;
		m_bIsSettingCamera  = false;
		m_bIsFadeInOut      =  false;

		CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
		if (nullptr != pDynamicCamera)
		{
			pDynamicCamera->Set_CameraAtParentMatrix(nullptr);
			pDynamicCamera->Set_Vergos(nullptr);
		}

		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterVergosPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (m_iSNum == -1)
	{
		m_bIsStartDissolve = false;
		m_bIsResetNaviMesh = false;
		m_fDissolve = -0.05f;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterVergosPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (!m_bIsResetNaviMesh)
	{
		m_bIsResetNaviMesh = true;
		m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
	}

	SetUp_Dissolve(fTimeDelta);

	// Angle Linear Interpolation
	SetUp_AngleInterpolation(fTimeDelta);
	
	/* Animation AI */
	Change_Animation(fTimeDelta);
	SetUp_CameraShaking();

	// Create CollisionTick
	if (m_pMeshCom->Is_BlendingComplete())
		SetUp_CollisionTick(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	if (!m_bIsStartDissolve)
	{
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pMeshCom->Play_Animation(fTimeDelta * TPS);
		m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
		m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());
	}

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	if (!g_bIsStartSkillCameraEffect && !g_bIsCinemaStart && m_bIsSpawn)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MINIMAP, this), -1);

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	if (!m_bIsStartDissolve && Vergos::A_DEATH != m_iMonsterStatus)
		m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	Engine::CGameObject::SetUp_MiniMapRandomY();

	return NO_EVENT;
}

_int CVergos::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	SetUp_HpGauge(fTimeDelta);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	return NO_EVENT;
}

void CVergos::Send_PacketToServer()
{
}

void CVergos::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTableMiniMap();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiMiniMapTexIdx, 
								   Engine::MATRIXID::TOP_VIEW);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CVergos::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	Render_HitEffect(fTimeDelta);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CVergos::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	if (!m_bIsStartDissolve)
	{
		m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
	}
}

HRESULT CVergos::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(7), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	// NaviMesh
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(wstrNaviMeshTag.c_str(), Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	return S_OK;
}

HRESULT CVergos::Create_Collider()
{
	/*__________________________________________________________________________________________________________
	[ Collider Bone Setting ]
	____________________________________________________________________________________________________________*/
	Engine::CColliderSphere*	pColliderSphereCom = nullptr;
	Engine::SKINNING_MATRIX*	pmatSkinning       = nullptr;
	_matrix* pmatParent = &(m_pTransCom->m_matWorld);

	// Head
	pColliderSphereCom = static_cast<Engine::CColliderSphere*>(m_pComponentMgr->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(pColliderSphereCom, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderSphere_Head", pColliderSphereCom);

	pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Head_Bone01");
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	pColliderSphereCom->Set_Scale(_vec3(450.f, 450.f, 450.f));	// Collider Scale
	pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);		// Collider Radius
	m_lstCollider.emplace_back(pColliderSphereCom);

	// R Hand
	pColliderSphereCom = static_cast<Engine::CColliderSphere*>(m_pComponentMgr->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(pColliderSphereCom, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderSphere_RHand", pColliderSphereCom);

	pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	pColliderSphereCom->Set_Scale(_vec3(450.f, 450.f, 450.f));	// Collider Scale
	pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);		// Collider Radius
	m_lstCollider.emplace_back(pColliderSphereCom);

	// L Hand
	pColliderSphereCom = static_cast<Engine::CColliderSphere*>(m_pComponentMgr->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(pColliderSphereCom, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderSphere_LHand", pColliderSphereCom);

	pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Bip01-L-Hand");
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	pColliderSphereCom->Set_Scale(_vec3(450.f, 450.f, 450.f));	// Collider Scale
	pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);		// Collider Radius
	m_lstCollider.emplace_back(pColliderSphereCom);

	return S_OK;
}

HRESULT CVergos::Create_HpGauge()
{
	{
		wifstream fin { L"../../Bin/ToolData/2DUIBossHpGaugeV2.2DUI" };
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
			pRootUI = CGameUIRoot::Create(m_pGraphicDevice, m_pCommandList,
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
			m_pHpGaugeRoot = static_cast<CGameUIRoot*>(pRootUI);

			// UIChild 积己.
			Engine::CGameObject* pChildUI = nullptr;
			for (_int i = 0; i < iChildUISize; ++i)
			{
				if (L"BossGaugeFrontV2" == vecObjectTag[i])
				{
					pChildUI = CVergosHpGauge::Create(m_pGraphicDevice, m_pCommandList,
													  wstrRootObjectTag,				// RootObjectTag
													  vecObjectTag[i],					// ObjectTag
													  vecDataFilePath[i],				// DataFilePath
													  vecPos[i],						// Pos
													  vecScale[i],						// Scane
													  (_bool)vecIsSpriteAnimation[i],	// Is Animation
													  vecFrameSpeed[i],					// FrameSpeed
													  vecRectPosOffset[i],				// RectPosOffset
													  vecRectScale[i],					// RectScaleOffset
													  vecUIDepth[i]);						// UI Depth
					m_pHpGauge = static_cast<CVergosHpGauge*>(pChildUI);
				}
				else
				{
					pChildUI = CGameUIChild::Create(m_pGraphicDevice, m_pCommandList,
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
				m_pObjectMgr->Add_GameObject(L"Layer_UI", vecObjectTag[i], pChildUI);
				static_cast<CGameUIRoot*>(pRootUI)->Add_ChildUI(pChildUI);
			}
		}
	}

	if (nullptr != m_pHpGaugeRoot)
	{
		m_pHpGaugeRoot->Set_IsActive(false);
		m_pHpGaugeRoot->Set_IsChildActive(false);
	}

	return S_OK;
}

void CVergos::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld       = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos      = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar  = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fDissolve      = m_fDissolve;
	tCB_ShaderMesh.vEmissiveColor = m_vEmissiveColor;
	tCB_ShaderMesh.fOffset6 = m_fRedColor;
	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CVergos::Set_ConstantTableShadowDepth()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_SHADOW tCB_ShaderShadow;
	ZeroMemory(&tCB_ShaderShadow, sizeof(Engine::CB_SHADER_SHADOW));
	tCB_ShaderShadow.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderShadow.matView  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderShadow.matProj  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderShadow.fProjFar = tShadowDesc.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

void CVergos::Set_ConstantTableMiniMap()
{
	m_pTransMiniMap->m_vPos.x = m_pTransCom->m_vPos.x + 8.5f;
	m_pTransMiniMap->m_vPos.z = m_pTransCom->m_vPos.z + -22.0f;
	m_pTransMiniMap->m_vAngle = _vec3(90.0f, 0.0f, 0.0f);
	m_pTransMiniMap->m_vScale = _vec3(20.0f, 20.0f, 20.0f);
	m_pTransMiniMap->Update_Component(0.16f);

	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_CAMERA_MATRIX tCB_CameraMatrix;
	ZeroMemory(&tCB_CameraMatrix, sizeof(Engine::CB_CAMERA_MATRIX));
	tCB_CameraMatrix.matView = Engine::CShader::Compute_MatrixTranspose(CShadowLightMgr::Get_Instance()->Get_MiniMapView());
	tCB_CameraMatrix.matProj = Engine::CShader::Compute_MatrixTranspose(CShadowLightMgr::Get_Instance()->Get_MiniMapProj());

	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransMiniMap->m_matWorld);
	tCB_ShaderTexture.fAlpha    = 1.0f;

	m_pShaderMiniMap->Get_UploadBuffer_CameraTopViewMatrix()->CopyData(0, tCB_CameraMatrix);
	m_pShaderMiniMap->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

void CVergos::SetUp_AngleInterpolation(const _float& fTimeDelta)
{
	if (m_tAngleInterpolationDesc.is_start_interpolation)
	{
		m_tAngleInterpolationDesc.linear_ratio += m_tAngleInterpolationDesc.interpolation_speed * fTimeDelta;

		m_pTransCom->m_vAngle.y = Engine::LinearInterpolation(m_tAngleInterpolationDesc.v1,
															  m_tAngleInterpolationDesc.v2,
															  m_tAngleInterpolationDesc.linear_ratio);

		if (m_tAngleInterpolationDesc.linear_ratio == Engine::MAX_LINEAR_RATIO)
		{
			m_tAngleInterpolationDesc.is_start_interpolation = false;
		}
	}
}

void CVergos::SetUp_PositionInterpolation(const _float& fTimeDelta)
{
	if (m_tPosInterpolationDesc.is_start_interpolation)
	{
		m_tPosInterpolationDesc.linear_ratio += m_tPosInterpolationDesc.interpolation_speed * fTimeDelta;

		m_pTransCom->m_vPos = Engine::LinearInterpolation(m_tPosInterpolationDesc.v1,
														  m_tPosInterpolationDesc.v2,
														  m_tPosInterpolationDesc.linear_ratio);

		if (m_tPosInterpolationDesc.linear_ratio == Engine::MAX_LINEAR_RATIO)
		{
			m_tPosInterpolationDesc.is_start_interpolation = false;
			m_bIsMoveStop = true;
		}
	}
}

void CVergos::SetUp_Dissolve(const _float& fTimeDelta)
{
	if (m_bIsStartDissolve)
	{
		m_fDissolve += fTimeDelta * 0.33f;

		if (!m_bIsFadeInOut)
		{
			m_bIsFadeInOut = true;

			Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
			if (nullptr != pGameObject)
			{
				static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::EVENT_CINEMATIC_VERGOS_DEATH);
				m_pObjectMgr->Add_GameObject(L"Layer_UI", L"FadeInOut", pGameObject);
			}
		}

		if (m_fDissolve >= 1.0f)
		{
			m_fDissolve = 1.0f;
			m_bIsReturn = true;
		}
	}
}

void CVergos::Active_Monster(const _float& fTimeDelta)
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	/* Monster MOVE */
	if (!m_bIsMoveStop && m_tPosInterpolationDesc.is_start_interpolation)
	{
		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
													 &m_pTransCom->m_vDir,
													 m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
	}
}

void CVergos::Change_Animation(const _float& fTimeDelta)
{
	if (m_pMeshCom->Is_BlendingComplete())
	{
		switch (m_iMonsterStatus)
		{

		case Vergos::A_WAIT:
		{
			if (nullptr != m_pHpGaugeRoot)
			{
				m_pHpGaugeRoot->Set_IsActive(true);
				m_pHpGaugeRoot->Set_IsChildActive(true);
			}

			m_bIsCreateCollisionTick                     = false;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
			m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

			m_bIsCameraShaking       = false;
			m_bIsCameraShaking2      = false;
			m_bIsCameraShaking3      = false;

			m_uiAnimIdx = Vergos::A_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		}
		break;

		case Vergos::A_SPAWN:
		{
			m_bIsSpawn = true;

			if (nullptr != m_pHpGaugeRoot)
			{
				m_pHpGaugeRoot->Set_IsActive(true);
				m_pHpGaugeRoot->Set_IsChildActive(true);
			}

			m_uiAnimIdx = Vergos::A_SPAWN;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus = Vergos::A_WAIT;
				m_uiAnimIdx      = Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_FINCH:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_FINCH;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_SWING_RIGHT:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_SWING_RIGHT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_SWING_LEFT:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_SWING_LEFT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_BLOW_LEFT:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_BLOW_LEFT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_BLOW_RIGHT:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_BLOW_RIGHT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_BLOW_HEAD:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_BLOW_HEAD;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_BLOW_ROTATION:
		{
			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_BLOW_ROTATION;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus	= Vergos::A_WAIT;
				m_uiAnimIdx			= Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_BREATH_FIRE:
		{
			m_bIsSpawn = true;

			m_uiAnimIdx = Vergos::A_BREATH_FIRE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus = Vergos::A_WAIT;
				m_uiAnimIdx = Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_FLY_START:
		{
			m_bIsSpawn = true;

			m_uiAnimIdx = Vergos::A_FLY_START;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus = Vergos::A_FLY_LOOP;
				m_uiAnimIdx = Vergos::A_FLY_LOOP;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_FLY_LOOP:
		{
			m_bIsSpawn = true;

			m_uiAnimIdx = Vergos::A_FLY_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus = Vergos::A_FLY_END;
				m_uiAnimIdx = Vergos::A_FLY_END;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_FLY_END:
		{
			m_bIsSpawn = true;

			m_uiAnimIdx = Vergos::A_FLY_END;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_bIsCreateCollisionTick                     = false;
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

				m_bIsCameraShaking       = false;
				m_bIsCameraShaking2      = false;
				m_bIsCameraShaking3      = false;

				m_iMonsterStatus = Vergos::A_WAIT;
				m_uiAnimIdx = Vergos::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case Vergos::A_DEATH:
		{
			if (!m_bIsSettingCamera)
			{
				m_bIsSettingCamera = true;

				g_bIsCinemaVergosDeath = true;

				m_bIsCameraShaking = false;
				m_bIsCameraShaking2 = false;
				m_bIsCameraShaking3 = false;

				m_pRenderer->Set_IsRenderAlphaGroup(false);

				CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
				if (nullptr != pDynamicCamera)
				{
					pDynamicCamera->Set_CameraState(CAMERA_STATE::CINEMATIC_VERGOS_DEATH);
					m_pDynamicCamera->SetUp_ThirdPersonViewOriginData();
				}
			}

			m_bIsSpawn = true;
			m_uiAnimIdx = Vergos::A_DEATH;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			
			if (nullptr != m_pHpGaugeRoot)
			{
				m_pHpGaugeRoot->Set_IsActive(false);
				m_pHpGaugeRoot->Set_IsChildActive(false);
			}

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta)) 
			{
				m_bIsStartDissolve = true;

				m_iMonsterStatus = Vergos::A_DEATH;
				m_uiAnimIdx = Vergos::A_DEATH;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;
		}
	}
}

void CVergos::SetUp_CollisionTick(const _float& fTimeDelta)
{
	if (Vergos::SWING_RIGHT == m_uiAnimIdx && m_ui3DMax_CurFrame >= VERGOS_SWING_CAMERA_SHAKING_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.fScaleOffset            = 35.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;

			m_strBone = "Bip01-R-Hand";
		}
	}
	else if (Vergos::SWING_LEFT == m_uiAnimIdx && m_ui3DMax_CurFrame >= VERGOS_SWING_CAMERA_SHAKING_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.fScaleOffset            = 35.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;

			m_strBone = "Bip01-L-Hand";
		}
	}
	else if (Vergos::BLOW_RIGHT == m_uiAnimIdx && m_ui3DMax_CurFrame >= VERGOS_BLOW_CAMERA_SHKAING_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.fScaleOffset            = 22.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;

			m_strBone = "Bip01-R-Hand";
		}
	}
	else if (Vergos::BLOW_LEFT == m_uiAnimIdx && m_ui3DMax_CurFrame >= VERGOS_BLOW_CAMERA_SHKAING_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.fScaleOffset            = 22.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;

			m_strBone = "Bip01-L-Hand";
		}
	}
	else if (Vergos::BREATH_FIRE == m_uiAnimIdx && m_ui3DMax_CurFrame >= VERGOS_BREATH_FRONT_CAMERA_SHAKING_TICK + 10)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 20.0f;
			m_tCollisionTickDesc.fScaleOffset            = 45.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 3.f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 4;

			m_strBone = "FxShot";
		}
	}

	// Create CollisionTick
	if (m_bIsCreateCollisionTick &&
		m_tCollisionTickDesc.bIsCreateCollisionTick &&
		m_tCollisionTickDesc.iCurCollisionTick < m_tCollisionTickDesc.iMaxCollisionTick)
	{
		m_tCollisionTickDesc.fCollisionTickTime += fTimeDelta;

		if (m_tCollisionTickDesc.fCollisionTickTime >= m_tCollisionTickDesc.fColisionTickUpdateTime)
		{
			m_tCollisionTickDesc.fCollisionTickTime = 0.0f;
			++m_tCollisionTickDesc.iCurCollisionTick;

			if (m_tCollisionTickDesc.iCurCollisionTick >= m_tCollisionTickDesc.iMaxCollisionTick)
			{
				m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
				m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
				m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;
			}

			// CollisionTick
			
			_matrix matBoneFinalTransform = INIT_MATRIX;
			_matrix matWorld = INIT_MATRIX;

			Engine::HIERARCHY_DESC* pHierarchyDesc = &(m_pMeshCom->Find_HierarchyDesc(m_strBone));
			if (nullptr != pHierarchyDesc)
			{
				matBoneFinalTransform = (pHierarchyDesc->matScale 
									  * pHierarchyDesc->matRotate 
									  * pHierarchyDesc->matTrans)
									  * pHierarchyDesc->matGlobalTransform;
				matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
			}
			_vec3 vDir = _vec3(matWorld._31, matWorld._32, matWorld._33);
			vDir.Normalize();
			
			_vec3 vPos = _vec3(matWorld._41, matWorld._42, matWorld._43) + vDir * m_tCollisionTickDesc.fPosOffset;
			vPos.y = 1.f;

			CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionTickPool()));
			if (nullptr != pCollisionTick)
			{
				pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
				pCollisionTick->Set_CollisionTag(L"CollisionTick_Monster");
				pCollisionTick->Set_Damage(m_pInfoCom->Get_RandomDamage());
				pCollisionTick->Set_LifeTime(0.3f);
				pCollisionTick->Get_Transform()->m_vScale = _vec3(1.0f) * m_tCollisionTickDesc.fScaleOffset;
				pCollisionTick->Get_Transform()->m_vPos   = vPos;
				pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);
				pCollisionTick->Set_ServerNumber(m_iSNum);
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_Monster", pCollisionTick);
			}
		}
	}
}

void CVergos::SetUp_HpGauge(const _float& fTimeDelta)
{
	if (nullptr != m_pHpGauge && Vergos::A_DEATH != m_iMonsterStatus)
	{
		m_pHpGauge->Set_Percent((_float)(m_pInfoCom->m_iHp) / (_float)(m_pInfoCom->m_iMaxHp), 
								m_pInfoCom->m_iHp, 
								m_pInfoCom->m_iMaxHp);
	}
}

void CVergos::EffectLoop(const _float& fTimeDelta)
{
	if (m_uiAnimIdx == Vergos::A_SPAWN && m_bisDecalEffect==false
		&& (m_ui3DMax_CurFrame >=149 && m_ui3DMax_CurFrame<=155))
	{
		m_bisDecalEffect = true;
		_matrix matBoneFinalTransform = (m_pHierarchyDesc[HEAD]->matScale * m_pHierarchyDesc[HEAD]->matRotate
			* m_pHierarchyDesc[HEAD]->matTrans) * m_pHierarchyDesc[HEAD]->matGlobalTransform;
		_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
		_vec3 Pos = _vec3(matWorld._41, matWorld._42, matWorld._43);
		Pos.y = 0.5f;
		CEffectMgr::Get_Instance()->Effect_Dust(Pos, 16.f, 10.f, 42.f, 36);
		Effect_BossStone(Pos);
		Engine::CSoundMgr::Get_Instance()->Play_Sound(L"Crack.ogg", SOUNDID::SOUND_EFFECT);
		int SoundNumber = rand() % 16;
		wstring SoundTag = L"RockImpact_HardCrumble_" + to_wstring(SoundNumber) + L".ogg";
		Engine::CSoundMgr::Get_Instance()->Play_Sound(SoundTag.c_str(), SOUNDID::SOUND_EFFECT);
	}

	if (m_uiAnimIdx == Vergos::SWING_LEFT || m_uiAnimIdx == Vergos::SWING_RIGHT)
	{
		string Bone;
		if (m_uiAnimIdx == Vergos::SWING_LEFT)
			Bone = "Bip01-L-Hand";
		else
			Bone = "Bip01-R-Hand";
		m_fSkillOffset += fTimeDelta;

		if (3.3f>m_fSkillOffset && m_fSkillOffset > 2.8f)
		{
			m_fParticleTime += fTimeDelta;
			if (m_fParticleTime > 0.01f)
			{
				m_fParticleTime = 0.f;
				Engine::HIERARCHY_DESC* pHierarchyDesc = &(m_pMeshCom->Find_HierarchyDesc(Bone));
				_matrix matBoneFinalTransform = (pHierarchyDesc->matScale * pHierarchyDesc->matRotate * pHierarchyDesc->matTrans)
					* pHierarchyDesc->matGlobalTransform;
				_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
				_vec3 Pos = _vec3(matWorld._41,0.2f, matWorld._43);

				CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
					_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);
			}
		}
		if (m_fSkillOffset > 3.f && m_bisDecalEffect == false)
		{
			
			m_bisDecalEffect = true;
			CGameObject* pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_BossDecal_Effect());
			if (m_uiAnimIdx == Vergos::SWING_RIGHT)
			{	
					static_cast<CBossDecal*>(pGameObj)->Set_CreateInfo(_vec3(0.5f, 0.f, 0.5f), _vec3(0.f, -180.f, 0.0f),
						_vec3(375.f, 0.4f, 371.f));
					Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"BossDecal", pGameObj), E_FAIL);
				
			}
			else
			{		
					static_cast<CBossDecal*>(pGameObj)->Set_CreateInfo(_vec3(0.5f, 0.f, 0.5f), _vec3(0.f, -180.f, 180.f),
						_vec3(375.f, 0.4f, 371.f));
					Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"BossDecal", pGameObj), E_FAIL);
			}	
			Engine::CSoundMgr::Get_Instance()->Play_Sound(L"Crash.ogg", SOUNDID::SOUND_EFFECT);
		}
	}
	else if (m_uiAnimIdx == Vergos::BLOW_LEFT || m_uiAnimIdx == Vergos::BLOW_RIGHT)
	{
		Engine::HIERARCHY_DESC* pHierarchyDesc;
		if (m_bisWarningEffect == false)
		{
			m_bisWarningEffect = true;
			if (m_uiAnimIdx == Vergos::BLOW_LEFT)
			{
				pHierarchyDesc = m_pHierarchyDesc[L_HAND];
				CEffectMgr::Get_Instance()->Effect_WarningGround(_vec3(390.4, 0.0f, 350.994f), 0.03f);
			}
			else
			{
				pHierarchyDesc = m_pHierarchyDesc[R_HAND];
				CEffectMgr::Get_Instance()->Effect_WarningGround(_vec3(385.4, 0.0f, 351.045f), 0.03f);
			}
		}
		m_fSkillOffset += fTimeDelta;
		if (m_fSkillOffset > 2.f && m_bisDecalEffect == false)
		{
			m_bisDecalEffect = true;
		
			if (m_uiAnimIdx == Vergos::BLOW_LEFT)
				pHierarchyDesc = m_pHierarchyDesc[L_HAND];
			else
				pHierarchyDesc = m_pHierarchyDesc[R_HAND];

			_matrix matBoneFinalTransform = (pHierarchyDesc->matScale * pHierarchyDesc->matRotate * pHierarchyDesc->matTrans)
				* pHierarchyDesc->matGlobalTransform;
			_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
			_vec3 Pos= _vec3(matWorld._41, matWorld._42, matWorld._43);
			CEffectMgr::Get_Instance()->Effect_Dust(Pos,12.f,10.f,42.f);
			Effect_BossStone(Pos, 0);
			Engine::CSoundMgr::Get_Instance()->Play_Sound(L"Crack.ogg", SOUNDID::SOUND_EFFECT);
			int SoundNumber = rand() % 16;
			wstring SoundTag = L"RockImpact_HardCrumble_" + to_wstring(SoundNumber)+L".ogg";
			Engine::CSoundMgr::Get_Instance()->Play_Sound(SoundTag.c_str(), SOUNDID::SOUND_EFFECT);

		}
	}
	else if (m_uiAnimIdx == Vergos::BLOW_HEAD)
	{
		m_fSkillOffset += fTimeDelta;

		if (m_fSkillOffset > 3.f && m_bisDecalEffect == false)
		{
			m_bisDecalEffect = true;
			_matrix matBoneFinalTransform = (m_pHierarchyDesc[HEAD]->matScale * m_pHierarchyDesc[HEAD]->matRotate 
				* m_pHierarchyDesc[HEAD]->matTrans)* m_pHierarchyDesc[HEAD]->matGlobalTransform;
			_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
			_vec3 Pos = _vec3(matWorld._41, matWorld._42, matWorld._43);
			Pos.y = 0.3f;
			_vec3 Dir = Pos - m_pTransCom->m_vPos;
			Dir.y = 0;
			Dir.Normalize();
			Pos += Dir * 10.f;
			Effect_BossStone(Pos, 0);
			CEffectMgr::Get_Instance()->Effect_RectDecal(_vec3(395.f,0.3f,232.f), m_pTransCom->m_vAngle.y);
			CEffectMgr::Get_Instance()->Effect_BossIceStorm(_vec3(400.f, 0.3f, 365.f), _vec3(0.1f, 0.f, -1.f), m_iSNum, m_pInfoCom->Get_RandomDamage());
			CEffectMgr::Get_Instance()->Effect_BossIceStorm(_vec3(378.f, 0.3f, 358.f), _vec3(-0.1f, 0.f, -1.f), m_iSNum, m_pInfoCom->Get_RandomDamage());
			int SoundNumber = rand() % 16;
			wstring SoundTag = L"RockImpact_HardCrumble_" + to_wstring(SoundNumber) + L".ogg";
			Engine::CSoundMgr::Get_Instance()->Play_Sound(SoundTag.c_str(), SOUNDID::SOUND_EFFECT);
		}
	}
	else if (m_uiAnimIdx == Vergos::A_BLOW_ROTATION)
	{
		m_fSkillOffset += fTimeDelta;
		if (m_fSkillOffset > 2.5f && m_bisBlow_Head_Effect[0] ==false)
		{
			m_bisBlow_Head_Effect[0] = true;
			_matrix matBoneFinalTransform = (m_pHierarchyDesc[R_HAND]->matScale * m_pHierarchyDesc[R_HAND]->matRotate 
				* m_pHierarchyDesc[R_HAND]->matTrans)* m_pHierarchyDesc[R_HAND]->matGlobalTransform;
			_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
			_vec3 Pos = _vec3(matWorld._41, matWorld._42, matWorld._43);
			CEffectMgr::Get_Instance()->Effect_Dust(Pos, 12.f, 10.f, 42.f);

			Pos.y = 0.2f;
			CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
				_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);
			CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
				_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);
			Engine::CSoundMgr::Get_Instance()->Play_Sound(L"Crack.ogg", SOUNDID::SOUND_EFFECT);
		}
		else if (m_fSkillOffset > 3.2f && m_bisBlow_Head_Effect[1] == false)
		{
			m_bisBlow_Head_Effect[1] = true;
			_matrix matBoneFinalTransform = (m_pHierarchyDesc[L_HAND]->matScale * m_pHierarchyDesc[L_HAND]->matRotate
				* m_pHierarchyDesc[L_HAND]->matTrans) * m_pHierarchyDesc[L_HAND]->matGlobalTransform;
			_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
			_vec3 Pos = _vec3(matWorld._41, matWorld._42, matWorld._43);
			CEffectMgr::Get_Instance()->Effect_Dust(Pos, 12.f, 10.f, 42.f);

			Pos.y = 0.2f;
			CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
				_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);
			CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
				_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);
			Engine::CSoundMgr::Get_Instance()->Play_Sound(L"Crack.ogg", SOUNDID::SOUND_EFFECT);
		}
		else if (m_fSkillOffset > 5.f && m_bisBlow_Head_Effect[2] == false)
		{
			m_bisBlow_Head_Effect[2] = true;
			_matrix matBoneFinalTransform = (m_pHierarchyDesc[HEAD]->matScale * m_pHierarchyDesc[HEAD]->matRotate
				* m_pHierarchyDesc[HEAD]->matTrans) * m_pHierarchyDesc[HEAD]->matGlobalTransform;
			_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
			_vec3 Pos = _vec3(matWorld._41, matWorld._42, matWorld._43);
			Pos.y = 0.5f;
			CEffectMgr::Get_Instance()->Effect_Dust(Pos, 16.f, 10.f, 42.f,36);
			Effect_BossStone(Pos);
			Engine::CSoundMgr::Get_Instance()->Play_Sound(L"Crack.ogg", SOUNDID::SOUND_EFFECT);
			int SoundNumber = rand() % 16;
			wstring SoundTag = L"RockImpact_HardCrumble_" + to_wstring(SoundNumber) + L".ogg";
			Engine::CSoundMgr::Get_Instance()->Play_Sound(SoundTag.c_str(), SOUNDID::SOUND_EFFECT);
		}
	}
	else if (m_uiAnimIdx == Vergos::A_BREATH_FIRE)
	{
		m_fSkillOffset += fTimeDelta;
		m_bisBreathDelta += fTimeDelta;
		m_fParticleTime += fTimeDelta;
		if (m_fSkillOffset >3.f)
		{
			m_fBreathTime += fTimeDelta * 0.5f;
			if (m_fParticleTime > 0.1f)
			{
				m_fParticleTime = 0.f;
				_matrix matBoneFinalTransform = (m_pHierarchyDesc[BREATH]->matScale * m_pHierarchyDesc[BREATH]->matRotate 
					* m_pHierarchyDesc[BREATH]->matTrans)* m_pHierarchyDesc[BREATH]->matGlobalTransform;
				_matrix matWorld = matBoneFinalTransform * m_pTransCom->m_matWorld;
				_vec3 Pos = _vec3(matWorld._41, matWorld._42, matWorld._43);
				BreathPos.x = powf((1.f - m_fBreathTime), 2.f) * BazierPos[0].x + 2 * m_fBreathTime * (1 - m_fBreathTime) * BazierPos[1].x
					+ powf(m_fBreathTime, 2.f) * BazierPos[2].x;
				BreathPos.y = 0.f;
				BreathPos.z = powf((1.f - m_fBreathTime), 2.f) * BazierPos[0].z + 2 * m_fBreathTime * (1 - m_fBreathTime) * BazierPos[1].z
					+ powf(m_fBreathTime, 2.f) * BazierPos[2].z;

				CEffectMgr::Get_Instance()->Effect_DirParticle(_vec3(1, 1, 0), _vec3(0.f), m_pTransCom->m_vPos, L"Lighting6", Pos,
					BreathPos, FRAME(1, 1, 1), 2, 20);
			}
			if (m_bisBreathEffect == false)
			{
				m_bisBreathEffect = true;
				m_bisBreathDelta = 0.f;
				CGameObject* pGameObj;
				pGameObj = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Breath_Effect());
				int SoundNumber = rand() % 2;
				wstring SoundTag = L"Breath_" + to_wstring(SoundNumber) + L".ogg";
				Engine::CSoundMgr::Get_Instance()->Play_Sound(SoundTag.c_str(), SOUNDID::SOUND_EFFECT);
				if (nullptr != pGameObj)
				{
					static_cast<CBreathEffect*>(pGameObj)->Set_CreateInfo(_vec3(0.f), _vec3(0.f), _vec3(-65.f, 2.2, -16.f));
					static_cast<CBreathEffect*>(pGameObj)->Set_HierchyDesc(m_pHierarchyDesc[BREATH]);
					static_cast<CBreathEffect*>(pGameObj)->Set_ParentMatrix(&m_pTransCom->m_matWorld);
					Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Breath", pGameObj), E_FAIL);
				}
			}
		}
	}

	else
	{
		m_bisBlow_Head_Effect[0] = false;
		m_bisBlow_Head_Effect[1] = false;
		m_bisBlow_Head_Effect[2] = false;
		m_bisBreathEffect = false;
		m_fBreathTime = 0.f;
		m_bisWarningEffect = false;
		m_bisDecalEffect = false;
		m_fSkillOffset = 0.f;
	}
}

void CVergos::SetUp_CameraShaking()
{
	if (!m_pMeshCom->Is_BlendingComplete())
		return; 

	switch (m_uiAnimIdx)
	{
	case Vergos::SPAWN:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_SPAWN_CAMERA_SHAKING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.75f;
			tCameraShakingDesc.vMin = _vec2(-75.0f, -75.0f);
			tCameraShakingDesc.vMax = _vec2(75.0f, 75.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Vergos::SWING_RIGHT:
	case Vergos::SWING_LEFT:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_SWING_CAMERA_SHAKING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.65f;
			tCameraShakingDesc.vMin = _vec2(-75.0f, -75.0f);
			tCameraShakingDesc.vMax = _vec2(75.0f, 75.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 6.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Vergos::BLOW_LEFT:
	case Vergos::BLOW_RIGHT:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_BLOW_CAMERA_SHKAING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.65f;
			tCameraShakingDesc.vMin = _vec2(-45.0f, -45.0f);
			tCameraShakingDesc.vMax = _vec2(45.0f, 45.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 6.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Vergos::BLOW_HEAD:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_BLOW_HEAD_CAMERA_SHAKING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.55f;
			tCameraShakingDesc.vMin = _vec2(-75.0f, -75.0f);
			tCameraShakingDesc.vMax = _vec2(75.0f, 75.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 6.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Vergos::BLOW_ROTATION:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_BLOW_ROTATION_CAMERA_SHAKING_TICK_1)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.4f;
			tCameraShakingDesc.vMin = _vec2(-40.0f, -40.0f);
			tCameraShakingDesc.vMax = _vec2(40.0f, 40.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}

		if (!m_bIsCameraShaking2 && m_ui3DMax_CurFrame >= VERGOS_BLOW_ROTATION_CAMERA_SHAKING_TICK_2)
		{
			m_bIsCameraShaking2 = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.4f;
			tCameraShakingDesc.vMin = _vec2(-40.0f, -40.0f);
			tCameraShakingDesc.vMax = _vec2(40.0f, 40.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}

		if (!m_bIsCameraShaking3 && m_ui3DMax_CurFrame >= VERGOS_BLOW_ROTATION_CAMERA_SHAKING_TICK_3)
		{
			m_bIsCameraShaking3 = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.65f;
			tCameraShakingDesc.vMin = _vec2(-80.0f, -80.0f);
			tCameraShakingDesc.vMax = _vec2(80.0f, 80.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}

	}
		break;

	case Vergos::BREATH_FIRE:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_BREATH_FRONT_CAMERA_SHAKING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 1.5f;
			tCameraShakingDesc.vMin = _vec2(-20.0f, -20.0f);
			tCameraShakingDesc.vMax = _vec2(20.0f, 20.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 12.5f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Vergos::FLY_END:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_FLY_CAMERA_SHAKING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.75f;
			tCameraShakingDesc.vMin = _vec2(-75.0f, -75.0f);
			tCameraShakingDesc.vMax = _vec2(75.0f, 75.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 8.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;

	case Vergos::DEATH:
	{
		if (!m_bIsCameraShaking && m_ui3DMax_CurFrame >= VERGOS_DEATH_HEAD_DOWN_CAMERA_SHAKING_TICK)
		{
			m_bIsCameraShaking = true;

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 0.8f;
			tCameraShakingDesc.vMin = _vec2(-150.0f, -150.0f);
			tCameraShakingDesc.vMax = _vec2(150.0f, 150.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 5.0f;
			m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}
		break;
	}
}

void CVergos::Effect_BossStone(_vec3 Pos,int Option)
{
	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.07f),
		_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(100, 10), 0, true);
	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.07f),
		_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(100, 10), 0, true);
	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.05f),
		_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(25, 6), 0, true);
	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.05f),
		_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(25, 6), 0, true);
	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
		_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);
	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
		_vec3(0.f), Pos, false, false, 5, 20, 0, 0, 0, _vec2(15, 2), 0, true);

}

Engine::CGameObject* CVergos::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, wstring wstrMeshTag, wstring wstrNaviMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	CVergos* pInstance = new CVergos(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CVergos** CVergos::Create_InstancePool(ID3D12Device* pGraphicDevice, 
												 ID3D12GraphicsCommandList* pCommandList,
												 const _uint& uiInstanceCnt)
{
	CVergos** ppInstance = new (CVergos * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CVergos(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"Vergos",				// MeshTag
										L"StageBeach_NaviMesh",	// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f),				// Angle
										_vec3(AWAY_FROM_STAGE));	// Pos
	}

	return ppInstance;
}

void CVergos::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);

	if (nullptr != m_pHpGaugeRoot)
		m_pHpGaugeRoot->Set_DeadGameObject();

	if (nullptr != m_pHpGauge)
		m_pHpGauge->Set_DeadGameObject();
}
