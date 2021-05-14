#include "stdafx.h"
#include "PCOthersGladiator.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "RenderTarget.h"

CPCOthersGladiator::CPCOthersGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pServerMath(CServerMath::Get_Instance())
{

}

void CPCOthersGladiator::Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack)
{
	m_uiAnimIdx = uiAniIdx;
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

	if (bIsStanceAttack)
	{
		m_eCurStance = Gladiator::STANCE_ATTACK;
		m_pWeapon->Set_DissolveInterpolation(-1.0f);
		m_pWeapon->Set_IsRenderShadow(true);
		SetUp_WeaponLHand();
	}
	else
	{
		m_eCurStance = Gladiator::STANCE_NONEATTACK;
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);
	}

	m_bIsCompleteStanceChange = false;
}

HRESULT CPCOthersGladiator::Ready_GameObject(wstring wstrMeshTag,
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
	m_chWeaponType        = chWeaponType;

	m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(vPos));

	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());

	m_pInfoCom->m_fSpeed     = Gladiator::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_chCurStageID = STAGE_VELIKA;
	m_chPreStageID = STAGE_VELIKA;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	m_ePreStance = Gladiator::STANCE_NONEATTACK;
	m_eCurStance = Gladiator::STANCE_NONEATTACK;

	/*__________________________________________________________________________________________________________
	[ 선형보간 설정 ]
	____________________________________________________________________________________________________________*/
	// Angle
	m_tAngleInterpolationDesc.interpolation_speed = 5.0f;

	// Move Speed
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Gladiator::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Gladiator::MAX_SPEED * Gladiator::OTHERS_OFFSET;

	m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tAttackMoveSpeedInterpolationDesc.v1           = Gladiator::MIN_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.v2           = Gladiator::MAX_SPEED * Gladiator::OTHERS_OFFSET;

	return S_OK;
}

HRESULT CPCOthersGladiator::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// Create Weapon
	Engine::FAILED_CHECK_RETURN(SetUp_PCWeapon(), E_FAIL);

	return S_OK;
}

_int CPCOthersGladiator::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	SetUp_StageID();
	Set_WeaponHierarchy();

	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	Set_AnimationSpeed();
	Set_BlendingSpeed();
	m_pMeshCom->Set_BlendingSpeed(m_fBlendingSpeed);
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * m_fAnimationSpeed);
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
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCOthersGladiator::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();
	return NO_EVENT;
}

void CPCOthersGladiator::Render_GameObject(const _float& fTimeDelta, 
										   ID3D12GraphicsCommandList* pCommandList, 
										   const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCOthersGladiator::Render_ShadowDepth(const _float& fTimeDelta, 
								   ID3D12GraphicsCommandList* pCommandList, 
								   const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPCOthersGladiator::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

	m_pVelikaNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pVelikaNaviMeshCom, E_FAIL);
	//m_pVelikaNaviMeshCom->AddRef();
	// m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pVelikaNaviMeshCom);

	m_pBeachNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageBeach_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pBeachNaviMeshCom, E_FAIL);
	//m_pBeachNaviMeshCom->AddRef();
	 //m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);
	return S_OK;
}

HRESULT CPCOthersGladiator::SetUp_PCWeapon()
{
	wstring wstrWeaponMeshTag = L"";

	if (m_chWeaponType == Twohand19_A_SM)
		wstrWeaponMeshTag = L"Twohand19_A_SM";

	else if (m_chWeaponType == TwoHand19_SM)
		wstrWeaponMeshTag = L"TwoHand19_SM";

	else if (m_chWeaponType == TwoHand27_SM)
		wstrWeaponMeshTag = L"TwoHand27_SM";

	else if (m_chWeaponType == TwoHand29_SM)
		wstrWeaponMeshTag = L"TwoHand29_SM";

	else if (m_chWeaponType == TwoHand31_SM)
		wstrWeaponMeshTag = L"TwoHand31_SM";

	else if (m_chWeaponType == TwoHand33_B_SM)
		wstrWeaponMeshTag = L"TwoHand33_B_SM";

	else if (m_chWeaponType == TwoHand33_SM)
		wstrWeaponMeshTag = L"TwoHand33_SM";

	m_pWeapon = CPCWeaponTwoHand::Create(m_pGraphicDevice, m_pCommandList,
										 wstrWeaponMeshTag,
										 _vec3(0.75f),
										 _vec3(0.0f, 0.0f, 180.0f),
										 _vec3(0.0f, 0.0f, 0.0f),
										 &(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")),
										 &m_pTransCom->m_matWorld,
										 _rgba(0.64f, 0.96f, 0.97f, 1.0f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"OthersWeaponTwoHand", m_pWeapon), E_FAIL);

	return S_OK;
}

void CPCOthersGladiator::SetUp_StageID()
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

		m_chPreStageID = m_chCurStageID;
	}
}

void CPCOthersGladiator::Set_WeaponHierarchy()
{
	if (m_uiAnimIdx == Gladiator::CUTTING_SLASH ||
		m_uiAnimIdx == Gladiator::JAW_BREAKER ||
		m_uiAnimIdx == Gladiator::CUT_HEAD ||
		m_uiAnimIdx == Gladiator::WIND_CUTTER1 || 
		m_uiAnimIdx == Gladiator::WIND_CUTTER2 || 
		m_uiAnimIdx == Gladiator::WIND_CUTTER3 ||
		m_uiAnimIdx == Gladiator::DRAW_SWORD_CHARGE ||
		m_uiAnimIdx == Gladiator::DRAW_SWORD_LOOP ||
		m_uiAnimIdx == Gladiator::DRAW_SWORD ||
		m_uiAnimIdx == Gladiator::DRAW_SWORD_END ||
		m_uiAnimIdx == Gladiator::ATTACK_RUN)
	{
		SetUp_WeaponRHand();
	}
	else
		SetUp_WeaponLHand();

}

void CPCOthersGladiator::Set_AnimationSpeed()
{
	if (m_uiAnimIdx == Gladiator::WIND_CUTTER1 ||
		m_uiAnimIdx == Gladiator::WIND_CUTTER2 ||
		m_uiAnimIdx == Gladiator::WIND_CUTTER3)
	{
		m_fAnimationSpeed = TPS * 0.75f;
	}
	else if (m_uiAnimIdx == Gladiator::DRAW_SWORD_CHARGE ||
			 m_uiAnimIdx == Gladiator::DRAW_SWORD_LOOP)
	{
		m_fAnimationSpeed = TPS * 2.f;
	}
	else if (m_uiAnimIdx == Gladiator::TUMBLING)
	{
		m_fAnimationSpeed = TPS * 1.35f;
	}
	else
		m_fAnimationSpeed = TPS;
}

void CPCOthersGladiator::Set_BlendingSpeed()
{
	if (m_uiAnimIdx == Gladiator::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Gladiator::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Gladiator::ATTACK_WAIT ||
		m_uiAnimIdx == Gladiator::ATTACK_RUN ||
		m_uiAnimIdx == Gladiator::GAIA_CRUSH1 ||
		m_uiAnimIdx == Gladiator::GAIA_CRUSH2 ||
		m_uiAnimIdx == Gladiator::GAIA_CRUSH3)
	{
		m_fBlendingSpeed = 0.001f;
	}
	else if (m_uiAnimIdx == Gladiator::TUMBLING)
	{
		m_fBlendingSpeed = 0.0075f;
	}
	else
		m_fBlendingSpeed = 0.005f;
}


void CPCOthersGladiator::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld      = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj  = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos     = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CPCOthersGladiator::Set_ConstantTableShadowDepth()
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

void CPCOthersGladiator::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsAttack)
		return;

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	SetUp_MoveSpeed(fTimeDelta);

	if (!m_bIsMoveStop || Gladiator::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		// NaviMesh 이동.		
		if (!m_pServerMath->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos, &m_pTransCom->m_vDir, m_pInfoCom->m_fSpeed * fTimeDelta);
			m_pTransCom->m_vPos = vPos;
		}
	}
}

void CPCOthersGladiator::SetUp_MoveSpeed(const _float& fTimeDelta)
{
	// Move On
	if (!m_bIsMoveStop)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Gladiator::OTHERS_OFFSET;

	// Move Off
	else
		m_tMoveSpeedInterpolationDesc.interpolation_speed = -Gladiator::MOVE_STOP_SPEED * Gladiator::OTHERS_OFFSET;

	m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);
}

void CPCOthersGladiator::SetUp_StanceChange(const _float& fTimeDelta)
{
	if ((m_ePreStance != m_eCurStance) && !m_bIsCompleteStanceChange)
	{
		// NONE_ATTACK -> ATTACK
		if (Gladiator::STANCE_ATTACK == m_eCurStance)
		{
			// NONE_ATTACK -> ATACK
			if (Gladiator::OUT_WEAPON == m_uiAnimIdx &&
				m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_uiAnimIdx = Gladiator::ATTACK_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				m_bIsCompleteStanceChange = true;
				m_ePreStance              = m_eCurStance;
			}
		}

		// ATTACK -> NONE_ATTACK
		else if (Gladiator::STANCE_NONEATTACK == m_eCurStance)
		{
			if (Gladiator::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
				m_bIsCompleteStanceChange = true;
				m_ePreStance              = m_eCurStance;
			}

			if (Gladiator::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
				SetUp_WeaponBack();
		}
	}
}

void CPCOthersGladiator::SetUp_OthersAttackMove(const _uint& uiAniIdx, 
												const _uint& uiStartTick,
												const _uint& uiStopTick, 
												const _float& fMoveSpeed,
												const _float& fStopSpeed)
{
	if (uiAniIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
	{
		// Move On
		if (m_ui3DMax_CurFrame >= uiStartTick && m_ui3DMax_CurFrame < uiStopTick)
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fMoveSpeed;

		// Move Off
		else
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fStopSpeed;
	}
}

void CPCOthersGladiator::SetUp_AttackTrail(const _uint& uiAniIdx, const _uint& uiStartTick, const _uint& uiStopTick)
{
	if (uiAniIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
	{
		// Trail On
		if (m_ui3DMax_CurFrame >= uiStartTick && m_ui3DMax_CurFrame < uiStopTick)
			m_pWeapon->Set_IsRenderTrail(true);

		// Trail Off
		else if (m_ui3DMax_CurFrame >= uiStopTick)
			m_pWeapon->Set_IsRenderTrail(false);

		else
			m_pWeapon->Set_IsRenderTrail(false);
	}
}

void CPCOthersGladiator::AttackMove_OnNaviMesh(const _float& fTimeDelta)
{
	// ComboAttack Move
	SetUp_OthersAttackMove(Gladiator::COMBO1, 0, Gladiator::COMBO1_MOVE_STOP, 1.0f, -3.0f);
	SetUp_OthersAttackMove(Gladiator::COMBO2, 0, Gladiator::COMBO2_MOVE_STOP, 1.0f, -3.5f);
	SetUp_OthersAttackMove(Gladiator::COMBO3, 0, Gladiator::COMBO3_MOVE_STOP, 1.0f, -3.5f);
	SetUp_OthersAttackMove(Gladiator::COMBO4, 0, Gladiator::COMBO4_MOVE_STOP, 0.9f, -0.95f);
	//// ComoboAttack Trail
	//SetUp_AttackTrail(Gladiator::COMBO1, Gladiator::COMBO1_TRAIL_START, Gladiator::COMBO1_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::COMBO2, Gladiator::COMBO2_TRAIL_START, Gladiator::COMBO2_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::COMBO3, Gladiator::COMBO3_TRAIL_START, Gladiator::COMBO3_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::COMBO4, Gladiator::COMBO4_TRAIL_START, Gladiator::COMBO4_TRAIL_STOP);

	// SkillAttack Move
	SetUp_OthersAttackMove(Gladiator::STINGER_BLADE, Gladiator::STINGER_BLADE_MOVE_START, Gladiator::STINGER_BLADE_MOVE_STOP, 3.0f, -3.0f);
	SetUp_OthersAttackMove(Gladiator::CUTTING_SLASH, Gladiator::CUTTING_SLASH_MOVE_START, Gladiator::CUTTING_SLASH_MOVE_STOP, 1.25f, -1.75f);
	SetUp_OthersAttackMove(Gladiator::JAW_BREAKER, Gladiator::JAW_BREAKER_MOVE_START, Gladiator::JAW_BREAKER_MOVE_STOP, 4.0f, -4.0f);
	SetUp_OthersAttackMove(Gladiator::CUT_HEAD, Gladiator::CUT_HEAD_MOVE_START, Gladiator::CUT_HEAD_MOVE_END, 1.5f, -8.0f);
	SetUp_OthersAttackMove(Gladiator::WIND_CUTTER1, Gladiator::WIND_CUTTER1_MOVE_START, Gladiator::WIND_CUTTER1_MOVE_STOP, 3.0f, -4.0f);
	SetUp_OthersAttackMove(Gladiator::WIND_CUTTER2, Gladiator::WIND_CUTTER2_MOVE_START, Gladiator::WIND_CUTTER2_MOVE_STOP, 3.0f, -4.0f);
	SetUp_OthersAttackMove(Gladiator::WIND_CUTTER3, Gladiator::WIND_CUTTER3_MOVE_START, Gladiator::WIND_CUTTER3_MOVE_STOP, 2.0f, -4.0f);
	SetUp_OthersAttackMove(Gladiator::GAIA_CRUSH1, Gladiator::GAIA_CRUSH1_MOVE_START, Gladiator::GAIA_CRUSH1_MOVE_STOP, 0.75f, 0.0f);
	SetUp_OthersAttackMove(Gladiator::GAIA_CRUSH2, Gladiator::GAIA_CRUSH2_MOVE_START, Gladiator::GAIA_CRUSH2_MOVE_STOP, 0.75f, -2.0f);
	SetUp_OthersAttackMove(Gladiator::DRAW_SWORD, Gladiator::DRAW_SWORD_MOVE_START, Gladiator::DRAW_SWORD_MOVE_STOP, 2.5f, -3.0f);
	SetUp_OthersAttackMove(Gladiator::TUMBLING, Gladiator::TUMBLING_MOVE_START, Gladiator::TUMBLING_MOVE_STOP, 5.0f, -5.0f);

	//// SkillAttck Trail
	//SetUp_AttackTrail(Gladiator::STINGER_BLADE, Gladiator::STINGER_BLADE_TRAIL_START, Gladiator::STINGER_BLADE_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::CUTTING_SLASH, Gladiator::CUTTING_SLASH_TRAIL_START, Gladiator::CUTTING_SLASH_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::JAW_BREAKER, Gladiator::JAW_BREAKER_TRAIL_START, Gladiator::JAW_BREAKER_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::CUT_HEAD, Gladiator::CUT_HEAD_TRAIL_START, Gladiator::CUT_HEAD_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::WIND_CUTTER1, Gladiator::WIND_CUTTER1_TRAIL_START, Gladiator::WIND_CUTTER1_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::WIND_CUTTER2, Gladiator::WIND_CUTTER2_TRAIL_START, Gladiator::WIND_CUTTER2_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::WIND_CUTTER3, Gladiator::WIND_CUTTER3_TRAIL_START, Gladiator::WIND_CUTTER3_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::GAIA_CRUSH1, Gladiator::GAIA_CRUSH1_TRAIL_START, Gladiator::GAIA_CRUSH1_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::GAIA_CRUSH2, Gladiator::GAIA_CRUSH2_TRAIL_START, Gladiator::GAIA_CRUSH2_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::DRAW_SWORD, Gladiator::DRAW_SWORD_TRAIL_START, Gladiator::DRAW_SWORD_TRAIL_STOP);
	//SetUp_AttackTrail(Gladiator::DRAW_SWORD_END, Gladiator::DRAW_SWORD_END_TRAIL_START, Gladiator::DRAW_SWORD_END_TRAIL_STOP);

	if (!m_bIsAttack)
		return;

	// Set Speed
	m_tAttackMoveSpeedInterpolationDesc.linear_ratio += m_tAttackMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tAttackMoveSpeedInterpolationDesc.v1, m_tAttackMoveSpeedInterpolationDesc.v2, m_tAttackMoveSpeedInterpolationDesc.linear_ratio);

	// NaviMesh 이동.
	if (!m_pServerMath->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
	{
		m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
		m_pTransCom->m_vDir.Normalize();
		m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos, &m_pTransCom->m_vDir, m_pInfoCom->m_fSpeed * fTimeDelta);
	}
}

void CPCOthersGladiator::SetUp_WeaponRHand()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 180.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 0.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("R_Sword")));
}

void CPCOthersGladiator::SetUp_WeaponLHand()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));
}

void CPCOthersGladiator::SetUp_WeaponBack()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

Engine::CGameObject* CPCOthersGladiator::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
												wstring wstrMeshTag, 
												wstring wstrNaviMeshTag, 
												const _vec3& vScale,
												const _vec3& vAngle,
												const _vec3& vPos,
												const char& chWeaponType)
{
	CPCOthersGladiator* pInstance = new CPCOthersGladiator(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos, chWeaponType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPCOthersGladiator::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	//Engine::Safe_Release(m_pNaviMeshCom);
	//Engine::Safe_Release(m_pVelikaNaviMeshCom);
	//Engine::Safe_Release(m_pBeachNaviMeshCom);
	m_pWeapon->Set_DeadGameObject();
}
