#include "stdafx.h"
#include "PrionBerserkerBoss.h"
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
#include "CinemaMgr.h"
#include "DynamicCamera.h"
#include "SoundMgr.h"

CPrionBerserkerBoss::CPrionBerserkerBoss(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

HRESULT CPrionBerserkerBoss::Ready_GameObject(wstring wstrMeshTag, wstring wstrNaviMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
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
											  _vec3(60.0f),
											  _vec3(0.0f, 20.f, 7.0f));

	m_pInfoCom->m_fSpeed = 0.f;
	m_bIsMoveStop = true;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	m_iMonsterStatus = PrionBerserkerBoss::A_WAIT;

	m_mapIsPlaySound[PrionBerserkerBoss::ANGRY]   = false;
	m_mapIsPlaySound[PrionBerserkerBoss::COMMAND] = false;

	return S_OK;
}

HRESULT CPrionBerserkerBoss::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(3), E_FAIL);

	return S_OK;
}

_int CPrionBerserkerBoss::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (!g_bIsCinemaStart)
		return NO_EVENT;

	if (m_bIsDead)
		return DEAD_OBJ;
	
	if (m_bIsReturn)
	{
		m_iSNum = -1;
		m_bIsStartDissolve = false;
		m_fDissolve = -0.05f;
		m_bIsResetNaviMesh = false;

		m_mapIsPlaySound[PrionBerserkerBoss::ANGRY] = false;
		m_mapIsPlaySound[PrionBerserkerBoss::COMMAND] = false;

		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterPrionBerserkerBossPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (m_iSNum == -1)
	{
		m_bIsStartDissolve = false;
		m_bIsResetNaviMesh = false;
		m_fDissolve = -0.05f;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_MonsterPrionBerserkerBossPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (!m_bIsResetNaviMesh)
	{
		m_bIsResetNaviMesh = true;
		m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
	}

	// Angle Linear Interpolation
	SetUp_AngleInterpolation(fTimeDelta);
	
	/* Animation AI */
	Change_Animation(fTimeDelta);

	Active_Monster(fTimeDelta);

	SetUp_PlaySound(PrionBerserkerBoss::ANGRY, 44, L"PrionBerserkerBoss_Angry_Monster_Voice26.gpk_000018.wav", 1.0f);
	SetUp_PlaySound(PrionBerserkerBoss::COMMAND, 18, L"PrionBerserkerBoss_Angry_Monster_Voice26.gpk_000019.wav", 1.0f);

	// 시네마틱.
	if (m_bIsPrionBerserkerScreaming && m_ui3DMax_CurFrame > 50)
	{
		if (!m_bIsCameraShaking)
		{
			m_bIsCameraShaking = true;

			CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));

			CAMERA_SHAKING_DESC tCameraShakingDesc;
			tCameraShakingDesc.fUpdateShakingTime = 1.75f;
			tCameraShakingDesc.vMin = _vec2(-175.0f, -175.0f);
			tCameraShakingDesc.vMax = _vec2(175.0f, 175.0f);
			tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 4.0f;
			pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);
		}
	}

	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS * m_fAnimationSpeed);
	m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
	m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	if (m_pRenderer->Get_Frustum().Contains(m_pBoundingBoxCom->Get_BoundingInfo()) != DirectX::DISJOINT)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPrionBerserkerBoss::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	return NO_EVENT;
}

void CPrionBerserkerBoss::Send_PacketToServer()
{
}

void CPrionBerserkerBoss::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	Render_HitEffect(fTimeDelta);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPrionBerserkerBoss::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	if (!m_bIsStartDissolve)
	{
		m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
	}
}

HRESULT CPrionBerserkerBoss::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

void CPrionBerserkerBoss::Set_ConstantTable()
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

void CPrionBerserkerBoss::Set_ConstantTableShadowDepth()
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

void CPrionBerserkerBoss::Set_ConstantTableMiniMap()
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

void CPrionBerserkerBoss::SetUp_AngleInterpolation(const _float& fTimeDelta)
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

void CPrionBerserkerBoss::Active_Monster(const _float& fTimeDelta)
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	/* Monster MOVE */
	if (!m_bIsMoveStop && m_pMeshCom->Is_BlendingComplete())
	{
		m_pInfoCom->m_fSpeed += fTimeDelta * 10.0f;
		if (m_pInfoCom->m_fSpeed >= 6.0f)
			m_pInfoCom->m_fSpeed = 6.0f;

		if (m_pTransCom->m_vPos.z <= 375.f)
			m_bIsMoveStop = true;

		_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
													 &m_pTransCom->m_vDir,
													 m_pInfoCom->m_fSpeed * fTimeDelta);
		m_pTransCom->m_vPos = vPos;
	}
}

void CPrionBerserkerBoss::Change_Animation(const _float& fTimeDelta)
{
	if (m_pMeshCom->Is_BlendingComplete())
	{
		switch (m_iMonsterStatus)
		{

		case PrionBerserkerBoss::A_WAIT:
		{
			m_pInfoCom->m_fSpeed = 0.0f;
			m_fAnimationSpeed    = 0.8f;
			m_uiAnimIdx = PrionBerserkerBoss::A_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		}
		break;

		case PrionBerserkerBoss::A_ANGRY:
		{
			m_fAnimationSpeed = 1.0f;
			m_uiAnimIdx = PrionBerserkerBoss::A_ANGRY;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_iMonsterStatus = PrionBerserkerBoss::A_WAIT;
				m_uiAnimIdx = PrionBerserkerBoss::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// 시네마틱.
				if (m_bIsPrionBerserkerScreaming)
					CCinemaMgr::Get_Instance()->Scream_PrionBerserkers();

				m_mapIsPlaySound[PrionBerserkerBoss::ANGRY] = false;
			}
		}
		break;

		case PrionBerserkerBoss::A_COMMAND:
		{
			m_fAnimationSpeed = 0.5f;
			m_uiAnimIdx = PrionBerserkerBoss::A_COMMAND;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

			if (m_pMeshCom->Is_AnimationSetEnd(fTimeDelta) && m_pMeshCom->Is_BlendingComplete())
			{
				m_iMonsterStatus = PrionBerserkerBoss::A_WAIT;
				m_uiAnimIdx = PrionBerserkerBoss::A_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				m_mapIsPlaySound[PrionBerserkerBoss::COMMAND] = false;

				if (m_bIsPrionBerserkerScreaming)
				{
					m_bIsMoveStop = false;
					m_iMonsterStatus = PrionBerserkerBoss::A_RUN;
					CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
					pDynamicCamera->Set_CameraState(CAMERA_STATE::CINEMATIC_RUSH_PRIONBERSERKER);
					pDynamicCamera->Set_IsSettingCameraCinematicValue(false);
				}
			}
		}
		break;

		case PrionBerserkerBoss::A_RUN:
		{
			m_fAnimationSpeed = 0.75f;
			m_uiAnimIdx = PrionBerserkerBoss::A_RUN;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		}
		break;
		}
	}
}

void CPrionBerserkerBoss::SetUp_PlaySound(const _uint& uiAniIdx, 
										  const _uint& uiStartTick,
										  wstring wstrSoundTag, 
										  const _float& fVolume)
{
	if (uiAniIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
	{
		if (m_ui3DMax_CurFrame >= uiStartTick && !m_mapIsPlaySound[uiAniIdx])
		{
			m_mapIsPlaySound[uiAniIdx] = true;
			Engine::CSoundMgr::Get_Instance()->Play_Sound(wstrSoundTag.c_str(), SOUNDID::SOUND_MONSTER, fVolume);
		}
	}
}

Engine::CGameObject* CPrionBerserkerBoss::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, wstring wstrMeshTag, wstring wstrNaviMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	CPrionBerserkerBoss* pInstance = new CPrionBerserkerBoss(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CPrionBerserkerBoss** CPrionBerserkerBoss::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CPrionBerserkerBoss** ppInstance = new (CPrionBerserkerBoss * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CPrionBerserkerBoss(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"PrionBerserkerBoss",					// MeshTag
										L"StageWinter_NaviMesh",	// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f),				// Angle
										_vec3(AWAY_FROM_STAGE));	// Pos
	}

	return ppInstance;
}

void CPrionBerserkerBoss::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);
}
