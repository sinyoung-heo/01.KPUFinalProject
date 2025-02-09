#include "stdafx.h"
#include "PCOthersArcher.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "RenderTarget.h"
#include "InstancePoolMgr.h"
#include "DirectInput.h"
#include "CollisionArrow.h"

CPCOthersArcher::CPCOthersArcher(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

void CPCOthersArcher::Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack)
{
	if (nullptr == m_pWeapon)
		return;

	m_uiAnimIdx = uiAniIdx;
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

	if (bIsStanceAttack)
	{
		m_eCurStance = Archer::STANCE_ATTACK;
		m_pWeapon->Set_DissolveInterpolation(-1.0f);
		m_pWeapon->Set_IsRenderShadow(true);
		SetUp_WeaponLHand();
	}
	else
	{
		m_eCurStance = Archer::STANCE_NONEATTACK;
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);
	}

	m_bIsCompleteStanceChange = false;
}

void CPCOthersArcher::Set_OthersStance(const _bool& bIsStanceAttack)
{
	if (nullptr == m_pWeapon)
		return;

	if (bIsStanceAttack)
	{
		m_uiAnimIdx = Archer::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		m_eCurStance = Archer::STANCE_ATTACK;
		m_pWeapon->Set_DissolveInterpolation(-1.0f);
		m_pWeapon->Set_IsRenderShadow(true);
		SetUp_WeaponLHand();
	}
	else
	{
		m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		m_eCurStance = Archer::STANCE_NONEATTACK;
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);
		SetUp_WeaponBack();
	}
}

void CPCOthersArcher::Reset_CollisionArrowDesc()
{
	m_bIsSetCollisionTick                        = false;
	m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
	m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
	m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;
}

HRESULT CPCOthersArcher::Ready_GameObject(wstring wstrMeshTag, 
										  wstring wstrNaviMeshTag, 
										  const _vec3& vScale, 
										  const _vec3& vAngle, 
										  const _vec3& vPos, 
										  const char& chWeaponType)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag, wstrNaviMeshTag), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos   = vPos;
	m_chCurWeaponType     = chWeaponType;
	m_wstrMeshTag         = wstrMeshTag;

	m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(vPos));

	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());

	m_pInfoCom->m_fSpeed     = Archer::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_chCurStageID = STAGE_VELIKA;
	m_chPreStageID = STAGE_VELIKA;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	m_ePreStance = Archer::STANCE_NONEATTACK;
	m_eCurStance = Archer::STANCE_NONEATTACK;

	/*__________________________________________________________________________________________________________
	[ 선형보간 설정 ]
	____________________________________________________________________________________________________________*/
	// Angle
	m_tAngleInterpolationDesc.interpolation_speed = 5.0f;

	// Move Speed
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Archer::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Archer::MAX_SPEED * Archer::OTHERS_OFFSET;

	m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tAttackMoveSpeedInterpolationDesc.v1           = Archer::MIN_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.v2           = Archer::MAX_ATTACK_SPEED * Archer::OTHERS_OFFSET;
	
	// Create Weapon
	Engine::FAILED_CHECK_RETURN(SetUp_PCWeapon(), E_FAIL);

	return S_OK;
}

HRESULT CPCOthersArcher::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pEdgeObjectShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(1), E_FAIL);

	return S_OK;
}

_int CPCOthersArcher::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	Effect_Loop(fTimeDelta);
	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(fTimeDelta);
			m_pWeapon = nullptr;
		}

		for (auto& pEffect : m_pEffectAxe)
		{
			if (nullptr != pEffect)
			{
				pEffect->Set_IsReturnObject(true);
				pEffect = nullptr;
			}
		}

		for (auto& pEffect : m_pEffectShield)
		{
			if (nullptr != pEffect)
			{
				pEffect->Set_IsReturnObject(true);
				pEffect = nullptr;
			}
		}

		m_chPreWeaponType = -1;
		m_chCurWeaponType = -1;
		m_bIsResetNaviMesh = false;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_PCOthersArcherPool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	SetUp_StageID();
	Is_ChangeWeapon();
	Set_WeaponHierarchy();

	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	Set_IsRepeatAnimation();
	Set_AnimationSpeed();
	Set_BlendingSpeed();
	m_pMeshCom->Set_BlendingSpeed(m_fBlendingSpeed);
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * m_fAnimationSpeed, m_bIsRepeatAnimation);
	m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
	m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());

	SetUp_StanceChange(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Move_OnNaviMesh(fTimeDelta);
	AttackMove_OnNaviMesh(fTimeDelta);

	// Linear Interpolation
	Engine::SetUp_LinearInterpolation(fTimeDelta, m_pTransCom->m_vPos, m_tPosInterpolationDesc);
	Engine::SetUp_LinearInterpolation(fTimeDelta, m_pTransCom->m_vAngle.y, m_tAngleInterpolationDesc);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	if (!g_bIsCinemaVergosDeath)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
	
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MINIMAP, this), -1);
	
	if (m_bIsThisPlayerPartyMember)
	{
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_EDGE, this), -1);
		m_uiMiniMapTexIdx = 2;
	}
	else
		m_uiMiniMapTexIdx = 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);
	Engine::CGameObject::SetUp_MiniMapRandomY();

	// Create CollisionTick
	if (m_pMeshCom->Is_BlendingComplete() && nullptr != m_pWeapon)
		SetUp_CollisionArrow(fTimeDelta);

	// Weapon Update
	if (m_pWeapon != nullptr && !g_bIsCinemaVergosDeath)
		m_pWeapon->Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCOthersArcher::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPCOthersArcher::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTableMiniMap();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiMiniMapTexIdx, 
								   Engine::MATRIXID::TOP_VIEW);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CPCOthersArcher::Render_EdgeGameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_DynamicMesh(m_pEdgeObjectShaderCom);
}

void CPCOthersArcher::Render_GameObject(const _float& fTimeDelta, 
										ID3D12GraphicsCommandList* pCommandList, 
										const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCOthersArcher::Render_ShadowDepth(const _float& fTimeDelta, 
										 ID3D12GraphicsCommandList* pCommandList, 
										 const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPCOthersArcher::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);


	// Shader
	m_pEdgeObjectShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pEdgeObjectShaderCom, E_FAIL);
	m_pEdgeObjectShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pEdgeObjectShaderCom->Set_PipelineStatePass(4), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pEdgeObjectShaderCom);

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	// NaviMesh
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(wstrNaviMeshTag, Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_pNaviMeshCom->Set_CurrentCellIndex(0);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	m_pOriginNaviMeshCom = m_pNaviMeshCom;

	m_pVelikaNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pVelikaNaviMeshCom, E_FAIL);

	m_pBeachNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageBeach_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pBeachNaviMeshCom, E_FAIL);

	m_pWinterNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageWinter_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pWinterNaviMeshCom, E_FAIL);

	return S_OK;
}

HRESULT CPCOthersArcher::SetUp_PCWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		m_pWeapon = static_cast<CPCWeaponBow*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponBow(m_chCurWeaponType)));
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
			m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
			m_pWeapon->Update_GameObject(0.016f);
		}

		m_chPreWeaponType = m_chCurWeaponType;
	}

	return S_OK;
}

void CPCOthersArcher::SetUp_StageID()
{
	if (m_chCurStageID != m_chPreStageID)
	{
		if (STAGE_VELIKA == m_chCurStageID)
		{
			m_pNaviMeshCom = m_pVelikaNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}
		else if (STAGE_BEACH == m_chCurStageID)
		{
			m_pNaviMeshCom = m_pBeachNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}
		else if (STAGE_WINTER == m_chCurStageID)
		{
			m_pNaviMeshCom = m_pWinterNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}

		m_chPreStageID = m_chCurStageID;
	}

	if (!m_bIsResetNaviMesh)
	{
		m_bIsResetNaviMesh = true;
		m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
	}
}

void CPCOthersArcher::Is_ChangeWeapon()
{
	if (m_chCurWeaponType == -1)
		return;
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(0.016f);
		}

		m_pWeapon = static_cast<CPCWeaponBow*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponBow(m_chCurWeaponType)));
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
			m_pWeapon->Update_GameObject(0.016f);

			if (Archer::STANCE_ATTACK == m_eCurStance)
			{
				SetUp_WeaponLHand();
				m_pWeapon->Set_DissolveInterpolation(-1.0f);
				m_pWeapon->Set_IsRenderShadow(true);
			}
			else if (Archer::STANCE_NONEATTACK == m_eCurStance)
			{
				SetUp_WeaponBack();
				m_pWeapon->Set_DissolveInterpolation(1.0f);
				m_pWeapon->Set_IsRenderShadow(false);
			}

			m_chPreWeaponType = m_chCurWeaponType;
		}
	}
}

void CPCOthersArcher::Set_WeaponHierarchy()
{
	if (m_uiAnimIdx == Archer::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Archer::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Archer::IN_WEAPON)
	{
		SetUp_WeaponBack();
	}
	else
		SetUp_WeaponLHand();
}

void CPCOthersArcher::Set_IsRepeatAnimation()
{
	if (m_uiAnimIdx == Archer::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Archer::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Archer::ATTACK_WAIT ||
		m_uiAnimIdx == Archer::ATTACK_RUN)
	{
		m_bIsRepeatAnimation = true;
	}
	else
		m_bIsRepeatAnimation = false;
}

void CPCOthersArcher::Set_AnimationSpeed()
{
	if (m_uiAnimIdx == Archer::ATTACK_ARROW)
	{
		m_fAnimationSpeed = TPS * 1.6f;
	}
	else if (m_uiAnimIdx == Archer::ATTACK_RUN)
	{
		m_fAnimationSpeed = TPS * 1.35f;
	}
	else if (m_uiAnimIdx == Archer::BACK_DASH)
	{
		m_fAnimationSpeed = TPS * 1.25f;
	}
	else if (m_uiAnimIdx == Archer::RAPID_SHOT1 ||
			 m_uiAnimIdx == Archer::RAPID_SHOT2)
	{
		m_fAnimationSpeed = TPS * 1.5f;
	}
	else if (m_uiAnimIdx == Archer::ESCAPING_BOMB)
	{
		m_fAnimationSpeed = TPS * 1.25f;
	}
	else if (m_uiAnimIdx == Archer::ARROW_SHOWER_START ||
			 m_uiAnimIdx == Archer::ARROW_SHOWER_LOOP ||
			 m_uiAnimIdx == Archer::ARROW_SHOWER_SHOT)
	{
		m_fAnimationSpeed = TPS * 1.75f;
	}
	else if (m_uiAnimIdx == Archer::ARROW_FALL_START ||
			 m_uiAnimIdx == Archer::ARROW_FALL_LOOP ||
			 m_uiAnimIdx == Archer::ARROW_FALL_SHOT)
	{
		m_fAnimationSpeed = TPS * 1.75f;
	}
	else
		m_fAnimationSpeed = TPS;
}

void CPCOthersArcher::Set_BlendingSpeed()
{
	if (m_uiAnimIdx == Archer::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Archer::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Archer::ATTACK_WAIT ||
		m_uiAnimIdx == Archer::ATTACK_RUN)
	{
		m_fBlendingSpeed = 0.001f;
	}
	else
		m_fBlendingSpeed = 0.005f;
}

void CPCOthersArcher::Set_ConstantTable()
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
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.0f, 1.0f, 1.0f, 1.0f);

	if (nullptr != m_pShaderCom->Get_UploadBuffer_ShaderMesh())
	{
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
		m_pEdgeObjectShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	}
}

void CPCOthersArcher::Set_ConstantTableShadowDepth()
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

	if (nullptr != m_pShadowCom->Get_UploadBuffer_ShaderShadow())
		m_pShadowCom->Get_UploadBuffer_ShaderShadow()->CopyData(0, tCB_ShaderShadow);
}

void CPCOthersArcher::Set_ConstantTableMiniMap()
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

void CPCOthersArcher::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsAttack)
		return;

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	SetUp_MoveSpeed(fTimeDelta);

	if (!m_bIsMoveStop || Archer::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		// NaviMesh 이동.		
		if (!m_pServerMath->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
														 &m_pTransCom->m_vDir,
														 m_pInfoCom->m_fSpeed * fTimeDelta);
			m_pTransCom->m_vPos = vPos;
		}
	}
}

void CPCOthersArcher::SetUp_MoveSpeed(const _float& fTimeDelta)
{
	// Move On
	if (!m_bIsMoveStop)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Archer::OTHERS_OFFSET;

	// Move Off
	else
		m_tMoveSpeedInterpolationDesc.interpolation_speed = -Archer::MOVE_STOP_SPEED * Archer::OTHERS_OFFSET;

	m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);
}

void CPCOthersArcher::SetUp_StanceChange(const _float& fTimeDelta)
{
	if ((m_ePreStance != m_eCurStance) && !m_bIsCompleteStanceChange)
	{
		// NONE_ATTACK -> ATTACK
		if (Archer::STANCE_ATTACK == m_eCurStance)
		{
			// NONE_ATTACK -> ATACK
			if (Archer::OUT_WEAPON == m_uiAnimIdx &&
				m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_uiAnimIdx = Archer::ATTACK_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				m_bIsCompleteStanceChange = true;
				m_ePreStance              = m_eCurStance;
			}
		}

		// ATTACK -> NONE_ATTACK
		else if (Archer::STANCE_NONEATTACK == m_eCurStance)
		{
			if (Archer::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
				m_bIsCompleteStanceChange = true;
				m_ePreStance              = m_eCurStance;
			}

			if (Archer::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
				SetUp_WeaponBack();
		}
	}
}

void CPCOthersArcher::SetUp_OthersAttackMove(const _uint& uiAniIdx,
											 const _uint& uiStartTick, 
											 const _uint& uiStopTick, 
											 const _float& fMoveSpeed,
											 const _float& fStopSpeed)
{
	if (uiAniIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
	{
		// Move On
		if (m_ui3DMax_CurFrame >= uiStartTick && m_ui3DMax_CurFrame < uiStopTick)
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fMoveSpeed * Archer::OTHERS_OFFSET;

		// Move Off
		else
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fStopSpeed * Archer::OTHERS_OFFSET;
	}
}

void CPCOthersArcher::AttackMove_OnNaviMesh(const _float& fTimeDelta)
{
	SetUp_OthersAttackMove(Archer::ATTACK_ARROW, 0, 0, 0.0f, 0.0f);
	SetUp_OthersAttackMove(Archer::BACK_DASH, Archer::BACK_DASH_MOVE_START, Archer::BACK_DASH_MOVE_STOP, 8.0f, -5.0f);
	SetUp_OthersAttackMove(Archer::ESCAPING_BOMB, Archer::ESCAPING_BOMB_MOVE_START, Archer::ESCAPING_BOMB_MOVE_STOP, 7.5f, -5.0f);
	SetUp_OthersAttackMove(Archer::CHARGE_ARROW_SHOT, Archer::CHARGE_ARROW_MOVE_START, Archer::CHARGE_ARROW_MOVE_STOP, 4.0f, -5.0f);

	if (!m_bIsAttack)
		return;

	// Set Speed
	m_tAttackMoveSpeedInterpolationDesc.linear_ratio += m_tAttackMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tAttackMoveSpeedInterpolationDesc.v1, m_tAttackMoveSpeedInterpolationDesc.v2, m_tAttackMoveSpeedInterpolationDesc.linear_ratio);

	// NaviMesh 이동.
	if (!m_pServerMath->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
	{
		_float fDir = 1.0f;
		if (m_uiAnimIdx == Archer::BACK_DASH ||
			m_uiAnimIdx == Archer::ESCAPING_BOMB ||
			m_uiAnimIdx == Archer::CHARGE_ARROW_SHOT)
		{
			fDir = -1.0f;
		}

		m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
		m_pTransCom->m_vDir.Normalize();
		m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos, 
															  &(m_pTransCom->m_vDir * fDir), 
															  m_pInfoCom->m_fSpeed * fTimeDelta);
	}
}

void CPCOthersArcher::SetUp_WeaponLHand()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vAngle.x = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.y = -160.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 210.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));
}

void CPCOthersArcher::SetUp_WeaponBack()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

void CPCOthersArcher::SetUp_CollisionArrow(const _float& fTimeDelta)
{
	_float fTempTimeDelta = fTimeDelta;
	if (fTempTimeDelta > 0.1f)
		fTempTimeDelta = 0.016f;

	// ATTACK_ARROW
	if (Archer::ATTACK_ARROW == m_uiAnimIdx && m_ui3DMax_CurFrame >= Archer::ATTACK_ARROW_COLLISIONARROW_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 1.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	// ATTACK_ARROW
	else if (Archer::RAPID_SHOT1 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Archer::RAPID_SHOT_COLLISIONARROW_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 9.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 5;
		}
	}
	else if (Archer::ESCAPING_BOMB == m_uiAnimIdx)
	{
		if (!m_bIsSetCollisionTick)
		{
			CEffectMgr::Get_Instance()->Effect_Explosion(m_pTransCom->m_vPos);
			m_bIsSetCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 9.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 3;
		}
	}
	// ARROW_SHOWER
	else if (Archer::ARROW_SHOWER_START == m_uiAnimIdx && m_ui3DMax_CurFrame >= Archer::ARROW_SHOWER_COLLISIONARROW_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 6.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 4;
		}
	}
	// ARROW_FALL
	else if (Archer::ARROW_FALL_LOOP == m_uiAnimIdx && m_ui3DMax_CurFrame >= Archer::ARROW_FALL_COLLISIONARROW_START)
	{
		if (!m_bIsStartArrowFall)
		{
			m_bIsStartArrowFall = true;

			m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
			m_pTransCom->m_vDir.Normalize();
			m_vArrowFallPos = m_pTransCom->m_vPos + m_pTransCom->m_vDir * Archer::ARROW_FALL_DIST;
			CEffectMgr::Get_Instance()->Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f), 
				m_vArrowFallPos, 34, 34, 2, true, true, nullptr, false);

			m_vArrowFallPos.y = 20.f;
		}
	}
	else if (Archer::CHARGE_ARROW_SHOT == m_uiAnimIdx && m_ui3DMax_CurFrame >= Archer::CHARGE_ARROW_COLLISIONARROW_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;

			m_tCollisionTickDesc.fPosOffset              = 0.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 1.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;

			m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
			m_pTransCom->m_vDir.Normalize();

			m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));
		}
	}

	// Create CollisionArrow
	if (m_bIsSetCollisionTick && m_tCollisionTickDesc.bIsCreateCollisionTick && m_tCollisionTickDesc.iCurCollisionTick < m_tCollisionTickDesc.iMaxCollisionTick)
	{
		m_tCollisionTickDesc.fCollisionTickTime += fTempTimeDelta;

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
			if (m_uiAnimIdx == Archer::ARROW_SHOWER_START || m_uiAnimIdx == Archer::ARROW_SHOWER_LOOP || m_uiAnimIdx == Archer::ARROW_SHOWER_SHOT)
			{
				_vec3 vPos = m_pWeapon->Get_Transform()->Get_PositionVector();
				vPos.y += 0.3f;

				_vec3	vDir        = _vec3(0.0f);
				_float	fAngle      = m_pTransCom->m_vAngle.y - (45.0f / 2.0f);
				_matrix matRotation = INIT_MATRIX;

				for (_int i = 0; i < Archer::ARROW_SHOWER_CNT; ++i)
				{
					matRotation = XMMatrixRotationY(XMConvertToRadians(fAngle));
					vDir.TransformNormal(_vec3(0.0f, 0.0f, 1.0f), matRotation);

					CCollisionArrow* pCollisionArrow = static_cast<CCollisionArrow*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionArrowPool(ARROW_POOL_TYPE::ARROW_POOL_ICE)));
					if (nullptr != pCollisionArrow)
					{
						pCollisionArrow->Set_ArrowType(ARROW_TYPE::ARROW_DEFAULT);
						pCollisionArrow->Set_Speed(45.0f);
						pCollisionArrow->Set_CollisionTag(L"NoneCollisionArrow");
						pCollisionArrow->Set_Damage(m_pInfoCom->Get_RandomDamage());
						pCollisionArrow->Set_LifeTime(5.0f);
						pCollisionArrow->Set_OriginPos(vPos);
						pCollisionArrow->Get_Transform()->m_vPos     = vPos;
						pCollisionArrow->Get_Transform()->m_vAngle.y = fAngle;
						pCollisionArrow->Get_Transform()->m_vDir     = vDir;
						pCollisionArrow->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
						pCollisionArrow->Get_BoundingSphere()->Set_Radius(pCollisionArrow->Get_Transform()->m_vScale);

						m_pObjectMgr->Add_GameObject(L"Layer_GameObject", pCollisionArrow->Get_MeshTag(), pCollisionArrow);
					}

					fAngle += (45.0f / (_float)Archer::ARROW_SHOWER_CNT);
				}
			}
			else if (m_uiAnimIdx == Archer::CHARGE_ARROW_SHOT)
			{
				m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
				m_pTransCom->m_vDir.Normalize();
				_vec3 vPos = m_pWeapon->Get_Transform()->Get_PositionVector() + m_pTransCom->m_vDir * 3.5f;
				vPos.y += 0.3f;

				CCollisionArrow* pCollisionArrow = static_cast<CCollisionArrow*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionArrowPool(ARROW_POOL_TYPE::ARROW_POOL_LIGHTNING)));
				if (nullptr != pCollisionArrow)
				{
					pCollisionArrow->Set_ArrowType(ARROW_TYPE::ARROW_CHARGE);
					pCollisionArrow->Set_Speed(90.0f);
					pCollisionArrow->Set_CollisionTag(L"NoneCollisionArrow");
					pCollisionArrow->Set_Damage(m_pInfoCom->Get_RandomDamage());
					pCollisionArrow->Set_LifeTime(5.0f);
					pCollisionArrow->Set_OriginPos(vPos);
					pCollisionArrow->Get_Transform()->m_vAngle.y = m_pTransCom->m_vAngle.y;
					pCollisionArrow->Get_Transform()->m_vScale	 = _vec3(0.35f, 0.35f, 0.35f);
					pCollisionArrow->Get_Transform()->m_vPos     = vPos;
					pCollisionArrow->Get_Transform()->m_vDir     = m_pTransCom->m_vDir;
					pCollisionArrow->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
					pCollisionArrow->Get_BoundingSphere()->Set_Radius(pCollisionArrow->Get_Transform()->m_vScale);

					m_pObjectMgr->Add_GameObject(L"Layer_GameObject", pCollisionArrow->Get_MeshTag(), pCollisionArrow);
				}
			}
			else if (m_uiAnimIdx == Archer::ESCAPING_BOMB)
			{

			}
			else
			{
				m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
				m_pTransCom->m_vDir.Normalize();
				_vec3 vPos = m_pWeapon->Get_Transform()->Get_PositionVector();
				vPos.y += 0.3f;

				CCollisionArrow* pCollisionArrow = static_cast<CCollisionArrow*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionArrowPool(ARROW_POOL_TYPE::ARROW_POOL_ICE)));
				if (nullptr != pCollisionArrow)
				{
					pCollisionArrow->Set_ArrowType(ARROW_TYPE::ARROW_DEFAULT);
					pCollisionArrow->Set_Speed(45.0f);
					pCollisionArrow->Set_CollisionTag(L"NoneCollisionArrow");
					pCollisionArrow->Set_Damage(m_pInfoCom->Get_RandomDamage());
					pCollisionArrow->Set_LifeTime(5.0f);
					pCollisionArrow->Set_OriginPos(vPos);
					pCollisionArrow->Get_Transform()->m_vPos     = vPos;
					pCollisionArrow->Get_Transform()->m_vAngle.y = m_pTransCom->m_vAngle.y;
					pCollisionArrow->Get_Transform()->m_vDir     = m_pTransCom->m_vDir;
					pCollisionArrow->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
					pCollisionArrow->Get_BoundingSphere()->Set_Radius(pCollisionArrow->Get_Transform()->m_vScale);

					m_pObjectMgr->Add_GameObject(L"Layer_GameObject", pCollisionArrow->Get_MeshTag(), pCollisionArrow);
				}
			}
		}
	}

	// ARROW_FALL
	if (m_bIsStartArrowFall)
	{
		m_fArrowFallTime += fTempTimeDelta;

		if (m_fArrowFallTime >= m_fArrowFallUpdateTime)
		{
			m_fArrowFallTime = 0.0f;
			++m_iCurArrowFallCnt;

			CCollisionArrow* pCollisionArrow = nullptr;
			pCollisionArrow = static_cast<CCollisionArrow*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionArrowPool(ARROW_POOL_TYPE::ARROW_POOL_ICE)));

			if (nullptr != pCollisionArrow)
			{
				pCollisionArrow->Set_ArrowType(ARROW_TYPE::ARROW_FALL);
				pCollisionArrow->Set_Speed(50.0f);
				pCollisionArrow->Set_IsReadyArrowFall(false);
				pCollisionArrow->Set_CollisionTag(L"NoneCollisionArrow");
				pCollisionArrow->Set_Damage(m_pInfoCom->Get_RandomDamage());
				pCollisionArrow->Set_LifeTime(5.0f);
				pCollisionArrow->Get_Transform()->m_vScale = _vec3(0.08f);
				pCollisionArrow->Get_Transform()->m_vAngle = _vec3(90.0f, 0.0f ,0.0f);
				pCollisionArrow->Get_Transform()->m_vPos   = m_vArrowFallPos;
				pCollisionArrow->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
				pCollisionArrow->Get_BoundingSphere()->Set_Radius(pCollisionArrow->Get_Transform()->m_vScale);

				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", pCollisionArrow->Get_MeshTag(), pCollisionArrow);
			}
		}

		if (m_iCurArrowFallCnt >= m_iMaxArrowFallCnt)
		{
			m_iCurArrowFallCnt  = 0;
			m_bIsStartArrowFall = false;
		}
	}

	//JunO
	if (m_uiAnimIdx == Archer::CHARGE_ARROW_START && m_bisHandEffect == false)
	{
		m_bisHandEffect = true;
		Engine::HIERARCHY_DESC* pHierarchyDesc = &(m_pMeshCom->Find_HierarchyDesc("L_Sword"));
		_vec3 Pos = m_pTransCom->Get_PositionVector();
		Pos.y += 2.f;
		CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Lighting2", _vec3(1.f), _vec3(0.0f), Pos, FRAME(5, 16, 25.0f)
			, false, false,0.f,0
			, _vec4(0.0f), true, pHierarchyDesc, m_pTransCom);
		
	}
	if (m_uiAnimIdx == Archer::CHARGE_ARROW_SHOT)
	{
		m_bisHandEffect = false;
	}
	if ( m_uiAnimIdx == Archer::ARROW_FALL_LOOP )
	{
		static float Time = 0.f;
		Time += fTempTimeDelta;
		static int Temp = 10;
		CGameObject* pGameObj = nullptr;
		_vec3 Pos = m_pTransCom->Get_PositionVector();
		if (Temp > 0 && Time > 0.03f)
		{
			Time = 0.f;
			Temp--;
			if (Temp == 0)
				Temp = 10;
			Pos.y += 2.f;
		}
	}
}

void CPCOthersArcher::Effect_Loop(const _float& fTimeDelta)
{
	//
	if (m_bisSkillEffect[0] == true && m_bisUseShieldEffect == false)
	{
		m_bisSkillEffect[0] = false;
		m_bisUseShieldEffect = true;
		CEffectMgr::Get_Instance()->Effect_TargetShield(this);
	}
	if (m_bisUseShieldEffect)
	{
		m_fUseShieldDelta += fTimeDelta;//쉴드를 쓰는동안 사용시간올라감
		if (m_fUseShieldDelta > 60.f)
		{
			m_fUseShieldDelta = 0.f;
			m_bisUseShieldEffect = false;
		}
	}

	if (m_bisSkillEffect[1] == true && m_bisUseAxeEffect == false)
	{
		m_bisSkillEffect[1] = false;
		m_bisUseAxeEffect = true;
		CEffectMgr::Get_Instance()->Effect_TargetAxe(this);
	}
	if (m_bisUseAxeEffect)
	{
		m_fUseAxeDelta += fTimeDelta;//쉴드를 쓰는동안 사용시간올라감
		if (m_fUseAxeDelta > 60.f)
		{
			m_fUseAxeDelta = 0.f;
			m_bisUseAxeEffect = false;
		}
	}
}

Engine::CGameObject* CPCOthersArcher::Create(ID3D12Device* pGraphicDevice, 
											 ID3D12GraphicsCommandList* pCommandList, 
											 wstring wstrMeshTag,
											 wstring wstrNaviMeshTag,
											 const _vec3& vScale, 
											 const _vec3& vAngle, 
											 const _vec3& vPos, 
											 const char& chWeaponType)
{
	CPCOthersArcher* pInstance = new CPCOthersArcher(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos, chWeaponType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CPCOthersArcher** CPCOthersArcher::Create_InstancePool(ID3D12Device* pGraphicDevice, 
													   ID3D12GraphicsCommandList* pCommandList, 
													   const _uint& uiInstanceCnt)
{
	CPCOthersArcher** ppInstance = new (CPCOthersArcher *[uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CPCOthersArcher(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"HumanPCEvent27Archer",	// MeshTag
										L"StageVelika_NaviMesh",	// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f, 0.0f, 0.0f),	// Angle
										_vec3(AWAY_FROM_STAGE),		// Pos
										Bow23_SM);					// Pos
	}

	return ppInstance;
}

void CPCOthersArcher::Free()
{
	Engine::CGameObject::Free();

	m_pNaviMeshCom = m_pOriginNaviMeshCom;
	m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pOriginNaviMeshCom;

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pEdgeObjectShaderCom);
	Engine::Safe_Release(m_pNaviMeshCom);
	Engine::Safe_Release(m_pVelikaNaviMeshCom);
	Engine::Safe_Release(m_pBeachNaviMeshCom);
	// Engine::Safe_Release(m_pWeapon);
	m_pWeapon = nullptr;
}
