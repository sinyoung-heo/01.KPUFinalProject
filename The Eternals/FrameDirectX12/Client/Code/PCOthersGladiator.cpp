#include "stdafx.h"
#include "PCOthersGladiator.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "RenderTarget.h"
#include "InstancePoolMgr.h"
#include "DirectInput.h"

CPCOthersGladiator::CPCOthersGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pServerMath(CServerMath::Get_Instance())
{

}

void CPCOthersGladiator::Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack)
{
	if (nullptr == m_pWeapon)
		return;

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

void CPCOthersGladiator::Set_OthersStance(const _bool& bIsStanceAttack)
{
	if (nullptr == m_pWeapon)
		return;

	if (bIsStanceAttack)
	{
		m_uiAnimIdx = Gladiator::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		m_eCurStance = Gladiator::STANCE_ATTACK;
		m_pWeapon->Set_DissolveInterpolation(-1.0f);
		m_pWeapon->Set_IsRenderShadow(true);
		SetUp_WeaponLHand();
	}
	else
	{
		m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		m_eCurStance = Gladiator::STANCE_NONEATTACK;
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);
		SetUp_WeaponBack();
	}
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
	m_chCurWeaponType     = chWeaponType;
	m_wstrMeshTag         = wstrMeshTag;

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
	
	// Create Weapon
	Engine::FAILED_CHECK_RETURN(SetUp_PCWeapon(), E_FAIL);

	return S_OK;
}

HRESULT CPCOthersGladiator::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pEdgeObjectShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(1), E_FAIL);

	return S_OK;
}

_int CPCOthersGladiator::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	if (m_bIsDead)
		return DEAD_OBJ;

	Effect_Loop(fTimeDelta);
	if (m_bIsReturn)
	{
		m_pWeapon->Set_IsReturnObject(true);
		m_pWeapon->Update_GameObject(fTimeDelta);

		m_bIsResetNaviMesh = false;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_PCOthersGladiatorPool(), m_uiInstanceIdx);

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

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCOthersGladiator::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPCOthersGladiator::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTableMiniMap();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiMiniMapTexIdx, 
								   Engine::MATRIXID::TOP_VIEW);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CPCOthersGladiator::Render_EdgeGameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_DynamicMesh(m_pEdgeObjectShaderCom);
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

	m_pVelikaNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pVelikaNaviMeshCom, E_FAIL);
	
	m_pBeachNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageBeach_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pBeachNaviMeshCom, E_FAIL);

	return S_OK;
}

HRESULT CPCOthersGladiator::SetUp_PCWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		m_pWeapon = static_cast<CPCWeaponTwoHand*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponTwoHand(m_chCurWeaponType)));
		m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
		m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
		m_pWeapon->Update_GameObject(0.016f);

		m_chPreWeaponType = m_chCurWeaponType;
	}

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

	if (!m_bIsResetNaviMesh)
	{
		m_bIsResetNaviMesh = true;
		m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
	}
}

void CPCOthersGladiator::Is_ChangeWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(0.016f);
		}

		m_pWeapon = static_cast<CPCWeaponTwoHand*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponTwoHand(m_chCurWeaponType)));
		m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
		m_pWeapon->Update_GameObject(0.016f);

		if (Gladiator::STANCE_ATTACK == m_eCurStance)
		{
			SetUp_WeaponLHand();
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
		}
		else if (Gladiator::STANCE_NONEATTACK == m_eCurStance)
		{
			SetUp_WeaponBack();
			m_pWeapon->Set_DissolveInterpolation(1.0f);
			m_pWeapon->Set_IsRenderShadow(false);
		}

		m_chPreWeaponType = m_chCurWeaponType;
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

void CPCOthersGladiator::Set_IsRepeatAnimation()
{
	if (m_uiAnimIdx == Gladiator::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Gladiator::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Gladiator::ATTACK_WAIT ||
		m_uiAnimIdx == Gladiator::ATTACK_RUN)
	{
		m_bIsRepeatAnimation = true;
	}
	else
		m_bIsRepeatAnimation = false;
}

void CPCOthersGladiator::Set_AnimationSpeed()
{
	if (m_uiAnimIdx == Gladiator::TUMBLING)
	{
		m_fAnimationSpeed = TPS * 1.60f;
	}
	else if (m_uiAnimIdx == Gladiator::COMBO1 || 
			 m_uiAnimIdx == Gladiator::COMBO1R || 
			 m_uiAnimIdx == Gladiator::COMBO2 || 
			 m_uiAnimIdx == Gladiator::COMBO2R || 
			 m_uiAnimIdx == Gladiator::COMBO3 || 
			 m_uiAnimIdx == Gladiator::COMBO3R || 
			 m_uiAnimIdx == Gladiator::COMBO4)
	{
		m_fAnimationSpeed = TPS * 1.25f;
	}
	else if (m_uiAnimIdx == Gladiator::STINGER_BLADE)
	{
		m_fAnimationSpeed = TPS * 1.45f;
	}
	else if (m_uiAnimIdx == Gladiator::CUTTING_SLASH)
	{
		m_fAnimationSpeed = TPS * 1.35f;
	}
	else if (m_uiAnimIdx == Gladiator::JAW_BREAKER)
	{
		m_fAnimationSpeed = TPS * 1.45f;
	}
	else if (m_uiAnimIdx == Gladiator::WIND_CUTTER1 ||
			 m_uiAnimIdx == Gladiator::WIND_CUTTER2 ||
			 m_uiAnimIdx == Gladiator::WIND_CUTTER3)
	{
		m_fAnimationSpeed = TPS * 0.85f;
	}
	else if (m_uiAnimIdx == Gladiator::GAIA_CRUSH1 ||
			 m_uiAnimIdx == Gladiator::GAIA_CRUSH3)
	{
		m_fAnimationSpeed = TPS * 1.4f;
	}
	else if (m_uiAnimIdx == Gladiator::DRAW_SWORD_CHARGE ||
			 m_uiAnimIdx == Gladiator::DRAW_SWORD_LOOP ||
			 m_uiAnimIdx == Gladiator::DRAW_SWORD_END)
	{
		m_fAnimationSpeed = TPS * 2.f;
	}
	else
		m_fAnimationSpeed = TPS;
}

void CPCOthersGladiator::Set_BlendingSpeed()
{
	if (m_uiAnimIdx == Gladiator::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Gladiator::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Gladiator::ATTACK_WAIT ||
		m_uiAnimIdx == Gladiator::ATTACK_RUN)
	{
		m_fBlendingSpeed = 0.001f;
	}
	else if (m_uiAnimIdx == Gladiator::WIND_CUTTER1 ||
			 m_uiAnimIdx == Gladiator::WIND_CUTTER2 ||
			 m_uiAnimIdx == Gladiator::WIND_CUTTER3)
	{
		m_fBlendingSpeed = 0.05f;
	}
	else if (m_uiAnimIdx == Gladiator::TUMBLING)
	{
		m_fBlendingSpeed = 1.5f;
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
	tCB_ShaderMesh.matWorld       = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj   = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos      = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar  = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.0f, 1.0f, 1.0f, 1.0f);

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	m_pEdgeObjectShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
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

void CPCOthersGladiator::Set_ConstantTableMiniMap()
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
			_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
														 &m_pTransCom->m_vDir,
														 m_pInfoCom->m_fSpeed * fTimeDelta);
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
	if (nullptr == m_pWeapon)
		return;

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
		m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos, 
															  &m_pTransCom->m_vDir, 
															  m_pInfoCom->m_fSpeed * fTimeDelta);
	}
}

void CPCOthersGladiator::SetUp_WeaponRHand()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vAngle.y = 180.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 0.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("R_Sword")));
}

void CPCOthersGladiator::SetUp_WeaponLHand()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));
}

void CPCOthersGladiator::SetUp_WeaponBack()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

void CPCOthersGladiator::Effect_Loop(const _float& fTimeDelta)
{
	
	if (m_uiAnimIdx == Gladiator::STINGER_BLADE)
	{
		m_fSwordsSkillOffset += fTimeDelta;
		if (m_fSwordsSkillOffset > 0.45f && m_bisSwordsEffect == false)
		{
			m_bisSwordsEffect = true;
			CEffectMgr::Get_Instance()->Effect_SwordEffect_s(m_pTransCom->m_vPos, m_pTransCom->m_vDir);
		}
	}
	else
	{
		m_bisSwordsEffect = false;
		m_fSwordsSkillOffset = 0.f;
	}
	if (m_uiAnimIdx == Gladiator::JAW_BREAKER)
	{
		m_fSwordsSkill3Offset += fTimeDelta;
		if (m_fSwordsSkill3Offset > 0.15f && m_bisIce_mEffect == false)
		{
			m_bisIce_mEffect = true;
			CEffectMgr::Get_Instance()->Effect_Straight_IceStorm(m_pTransCom->m_vPos, m_pTransCom->m_vDir);
		}
	}
	else
	{
		m_bisIce_mEffect = false;
		m_fSwordsSkill3Offset = 0.f;
	}
	if (m_uiAnimIdx == Gladiator::CUTTING_SLASH)
	{
		if (m_bisSwordEffect == false)
		{
			m_bisSwordEffect = true;
			CEffectMgr::Get_Instance()->Effect_SwordEffect(m_pTransCom->m_vPos, m_pTransCom->m_vDir);
		}
	}
	else
	{
		m_bisSwordEffect = false;
	}
	if (m_uiAnimIdx == Gladiator::DRAW_SWORD_CHARGE && m_bisDustEffect == false)
	{
		_vec3 newPos = m_pTransCom->m_vPos;
		newPos.y = 0.2f;
		CEffectMgr::Get_Instance()->Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f),
			newPos, 20, 20, 2, true, true, m_pTransCom, true, 0.015, 2.f);

		m_bisDustEffect = true;
		CEffectMgr::Get_Instance()->Effect_Dust(m_pTransCom->m_vPos, 3.f);
		Engine::HIERARCHY_DESC* pHierarchyDesc = &(m_pMeshCom->Find_HierarchyDesc("L_Sword"));
		_vec3 Pos = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer")->Get_Transform()->Get_PositionVector();
		Pos.y += 2.f;

		/*	CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Lighting2", _vec3(1.f), _vec3(0.0f), Pos, FRAME(5, 16, 25.0f)
				, false, false
				, _vec4(0.0f), true, pHierarchyDesc, m_pTransCom);*/
				/*CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Lighting4", _vec3(0.8f), _vec3(0.0f), Pos, FRAME(8, 8, 40.0f), false, false
					, _vec4(0.7f, 0.1f, 0, 0),true, pHierarchyDesc, m_pTransCom);*/
	}
	else if (m_uiAnimIdx == Gladiator::DRAW_SWORD)
	{

		if (m_bisFireEffect == false)
		{
			m_fSkillOffSet += fTimeDelta;
			if (m_fSkillOffSet > 0.2f)
			{
				m_bisFireEffect = true;
				CEffectMgr::Get_Instance()->Effect_FireCone(m_pTransCom->m_vPos, m_pTransCom->m_vAngle.y);
			}
		}
	}
	else if (m_uiAnimIdx == Gladiator::DRAW_SWORD_LOOP || m_uiAnimIdx == Gladiator::DRAW_SWORD_CHARGE)
	{
		m_fParticleDeltaTime += fTimeDelta;
		if (m_fParticleDeltaTime > 0.05f)
		{
			m_fParticleDeltaTime = 0.f;
			CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, 3, L"Lighting6", _vec3(0.4f));
		}
	}
	else if (m_uiAnimIdx == Gladiator::DRAW_SWORD_END)
	{
		m_fParticleDeltaTime = 0.f;
		m_fSkillOffSet = 0.f;
		m_bisFireEffect = false;
		m_bisDustEffect = false;
	}
	if (m_uiAnimIdx == Gladiator::GAIA_CRUSH2 && m_bisIceEffect == false)
	{
		m_fSkillOffSet += fTimeDelta;
		if (m_fSkillOffSet > 0.45f)
		{
			m_bisIceEffect = true;
			CEffectMgr::Get_Instance()->Effect_IceStorm(m_pTransCom->m_vPos, 36, 5.f);
			_vec3 Temp = m_pTransCom->m_vPos;
			Temp.y += 0.2f;
			CEffectMgr::Get_Instance()->Effect_IceDecal(Temp);
		}
	}
	else if (m_uiAnimIdx == Gladiator::GAIA_CRUSH3)
	{
		m_fSkillOffSet = 0.f;
		m_bisIceEffect = false;
	}

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

CPCOthersGladiator** CPCOthersGladiator::Create_InstancePool(ID3D12Device* pGraphicDevice, 
															 ID3D12GraphicsCommandList* pCommandList, 
															 const _uint& uiInstanceCnt)
{
	CPCOthersGladiator** ppInstance = new (CPCOthersGladiator*[uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CPCOthersGladiator(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"PoporiR27Gladiator",		// MeshTag
										L"StageVelika_NaviMesh",	// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f, 0.0f, 0.0f),	// Angle
										_vec3(AWAY_FROM_STAGE),		// Pos
										Twohand19_A_SM);			// Pos
	}

	return ppInstance;
}

void CPCOthersGladiator::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pEdgeObjectShaderCom);
	Engine::Safe_Release(m_pNaviMeshCom);
	Engine::Safe_Release(m_pVelikaNaviMeshCom);
	Engine::Safe_Release(m_pBeachNaviMeshCom);
	Engine::Safe_Release(m_pWeapon);
}
