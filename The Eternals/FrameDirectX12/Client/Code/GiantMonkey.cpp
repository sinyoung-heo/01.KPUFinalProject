#include "stdafx.h"
#include "GiantMonkey.h"
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

CGiantMonkey::CGiantMonkey(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

HRESULT CGiantMonkey::Ready_GameObject(wstring wstrMeshTag, wstring wstrNaviMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
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
											  _vec3(128.0f),
											  _vec3(2.0f, 37.f, 0.0f));
	m_wstrCollisionTag = L"Monster_SingleCollider";
	m_lstCollider.push_back(m_pBoundingSphereCom);

	m_pInfoCom->m_fSpeed = 1.f;
	m_bIsMoveStop = true;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	m_iMonsterStatus = GiantMonkey::A_WAIT;

	// Create HpGauge
	m_pHpGauge = static_cast<CNormalMonsterHpGauge*>(CNormalMonsterHpGauge::Create(m_pGraphicDevice, 
																				   m_pCommandList,
																				   _vec3(0.0f),
																				   _vec3(2.0f, 0.075f, 1.0f)));
	Engine::NULL_CHECK_RETURN(m_pHpGauge, E_FAIL);

	return S_OK;
}

HRESULT CGiantMonkey::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(3), E_FAIL);

	return S_OK;
}

_int CGiantMonkey::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		m_iSNum = -1;
		m_bIsStartDissolve = false;
		m_fDissolve = -0.05f;
		m_bIsResetNaviMesh = false;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterGiantMonkeyPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (m_iSNum == -1)
	{
		m_bIsStartDissolve = false;
		m_bIsResetNaviMesh = false;
		m_fDissolve = -0.05f;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterGiantMonkeyPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (!m_bIsResetNaviMesh)
	{
		m_bIsResetNaviMesh = true;
		m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
	}

	// Create CollisionTick
	if (m_pMeshCom->Is_BlendingComplete())
		SetUp_CollisionTick(fTimeDelta);

	SetUp_Dissolve(fTimeDelta);

	// Angle Linear Interpolation
	SetUp_AngleInterpolation(fTimeDelta);
	
	/* Animation AI */
	Change_Animation(fTimeDelta);

	if (m_uiAnimIdx == GiantMonkey::DOWN && m_ui3DMax_CurFrame >= GiantMonkey::A_DOWN_START_TICK)
		SetUp_PositionInterpolation(fTimeDelta);

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
	if (!g_bIsStartSkillCameraEffect)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MINIMAP, this), -1);

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	if (!m_bIsStartDissolve && GiantMonkey::A_DEATH != m_iMonsterStatus)
		m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	Engine::CGameObject::SetUp_MiniMapRandomY();

	return NO_EVENT;
}

_int CGiantMonkey::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	SetUp_HpGauge(fTimeDelta);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	return NO_EVENT;
}

void CGiantMonkey::Send_PacketToServer()
{
}

void CGiantMonkey::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTableMiniMap();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiMiniMapTexIdx, 
								   Engine::MATRIXID::TOP_VIEW);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CGiantMonkey::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	Render_HitEffect(fTimeDelta);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CGiantMonkey::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	if (!m_bIsStartDissolve)
	{
		m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
	}
}

HRESULT CGiantMonkey::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

void CGiantMonkey::Set_ConstantTable()
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

void CGiantMonkey::Set_ConstantTableShadowDepth()
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

void CGiantMonkey::Set_ConstantTableMiniMap()
{
	m_pTransMiniMap->m_vPos.x = m_pTransCom->m_vPos.x;
	m_pTransMiniMap->m_vPos.z = m_pTransCom->m_vPos.z;
	m_pTransMiniMap->m_vAngle = _vec3(90.0f, 0.0f, 0.0f);
	m_pTransMiniMap->m_vScale = _vec3(6.0f, 6.0f, 6.0f);
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

void CGiantMonkey::SetUp_AngleInterpolation(const _float& fTimeDelta)
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

void CGiantMonkey::SetUp_PositionInterpolation(const _float& fTimeDelta)
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

void CGiantMonkey::SetUp_Dissolve(const _float& fTimeDelta)
{
	if (m_bIsStartDissolve)
	{
		m_fDissolve += fTimeDelta * 0.33f;

		if (m_fDissolve >= 1.0f)
		{
			m_fDissolve = 1.0f;
			m_bIsReturn = true;
		}
	}
}

void CGiantMonkey::Active_Monster(const _float& fTimeDelta)
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

void CGiantMonkey::Change_Animation(const _float& fTimeDelta)
{
	if (m_pMeshCom->Is_BlendingComplete())
	{
		switch (m_iMonsterStatus)
		{

		case GiantMonkey::A_WAIT:
		{
			m_bIsCreateCollisionTick = false;
			m_uiAnimIdx = GiantMonkey::A_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		}
		break;

		case GiantMonkey::A_WALK:
		{
			m_bIsCreateCollisionTick = false;
			m_uiAnimIdx = GiantMonkey::A_WALK;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		}
		break;

		case GiantMonkey::A_ATTACK_RIGHT:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_RIGHT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_ATTACK_LEFT:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_LEFT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_ATTACK_STAMP:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_STAMP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_ATTACK_HOOK:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_HOOK;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_ATTACK_JUMPING:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_JUMPING;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_ATTACK_FLYSTAMP:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_FLYSTAMP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_ATTACK_COMBO:
		{
			m_uiAnimIdx = GiantMonkey::A_ATTACK_COMBO;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus	= GiantMonkey::A_WAIT;

				m_uiAnimIdx			= GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_DEATH:
		{
			m_uiAnimIdx = GiantMonkey::A_DEATH;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta)) 
			{
				m_bIsStartDissolve = true;
			}
		}
		break;

		case GiantMonkey::A_FINCH:
		{
			m_bIsCreateCollisionTick = false;

			m_uiAnimIdx = GiantMonkey::A_FINCH;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus = GiantMonkey::A_WAIT;

				m_uiAnimIdx = GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_GROGGY:
		{
			m_bIsCreateCollisionTick = false;

			m_uiAnimIdx = GiantMonkey::A_GROGGY;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus = GiantMonkey::A_WAIT;

				m_uiAnimIdx = GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		case GiantMonkey::A_DOWN:
		{
			m_bIsCreateCollisionTick = false;

			m_uiAnimIdx = GiantMonkey::A_DOWN;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus = GiantMonkey::A_WAIT;

				m_uiAnimIdx = GiantMonkey::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			}
		}
		break;

		}
	}

}

void CGiantMonkey::SetUp_CollisionTick(const _float& fTimeDelta)
{
	if (GiantMonkey::A_ATTACK_RIGHT == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_RIGHT_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 2.5f;
			m_tCollisionTickDesc.fScaleOffset			 = 4.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (GiantMonkey::A_ATTACK_LEFT == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_LEFT_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 2.5f;
			m_tCollisionTickDesc.fScaleOffset			 = 4.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (GiantMonkey::A_ATTACK_STAMP == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_STAMP_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 3.0f;
			m_tCollisionTickDesc.fScaleOffset			 = 4.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (GiantMonkey::A_ATTACK_HOOK == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_HOOK_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 2.5f;
			m_tCollisionTickDesc.fScaleOffset			 = 3.5f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (GiantMonkey::A_ATTACK_JUMPING == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_JUMPING_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 3.0f;
			m_tCollisionTickDesc.fScaleOffset			 = 4.5f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (GiantMonkey::A_ATTACK_FLYSTAMP == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_FLYSTAMP_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.fScaleOffset			 = 12.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (GiantMonkey::A_ATTACK_COMBO == m_uiAnimIdx && m_ui3DMax_CurFrame >= GiantMonkey::A_ATTACK_COMBO_START_TICK)
	{
		if (!m_bIsCreateCollisionTick)
		{
			m_bIsCreateCollisionTick                     = true;
			m_tCollisionTickDesc.fPosOffset              = 4.0f;
			m_tCollisionTickDesc.fScaleOffset			 = 4.0f; 
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 2.f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 5;
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
			m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
			m_pTransCom->m_vDir.Normalize();
			_vec3 vPos = m_pTransCom->m_vPos + m_pTransCom->m_vDir * m_tCollisionTickDesc.fPosOffset;
			vPos.y = 1.f;

			CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionTickPool()));
			if (nullptr != pCollisionTick)
			{
				pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
				pCollisionTick->Set_CollisionTag(L"CollisionTick_Monster");
				pCollisionTick->Set_Damage(m_pInfoCom->Get_RandomDamage());
				pCollisionTick->Set_LifeTime(0.25f);
				pCollisionTick->Get_Transform()->m_vScale = _vec3(1.0f) * m_tCollisionTickDesc.fScaleOffset;
				pCollisionTick->Get_Transform()->m_vPos   = vPos;
				pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);
				pCollisionTick->Set_ServerNumber(m_iSNum);
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_Monster", pCollisionTick);
			}
		}
	}
}

void CGiantMonkey::SetUp_HpGauge(const _float& fTimeDelta)
{
	if (nullptr != m_pHpGauge && GiantMonkey::A_DEATH != m_iMonsterStatus)
	{
		_vec3 vPos = m_pTransCom->m_vPos;
		vPos.y += 4.75f;
		m_pHpGauge->Get_Transform()->m_vPos = vPos;
		m_pHpGauge->Set_Percent((_float)m_pInfoCom->m_iHp / (_float)m_pInfoCom->m_iMaxHp);

		m_pHpGauge->Update_GameObject(fTimeDelta);
		m_pHpGauge->LateUpdate_GameObject(fTimeDelta);
	}
}

Engine::CGameObject* CGiantMonkey::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, wstring wstrMeshTag, wstring wstrNaviMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	CGiantMonkey* pInstance = new CGiantMonkey(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CGiantMonkey** CGiantMonkey::Create_InstancePool(ID3D12Device* pGraphicDevice, 
												 ID3D12GraphicsCommandList* pCommandList,
												 const _uint& uiInstanceCnt)
{
	CGiantMonkey** ppInstance = new (CGiantMonkey * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CGiantMonkey(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"GiantMonkey",				// MeshTag
										L"StageWinter_NaviMesh",	// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f),				// Angle
										_vec3(AWAY_FROM_STAGE));	// Pos
	}

	return ppInstance;
}

void CGiantMonkey::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);

	Engine::Safe_Release(m_pHpGauge);
}
