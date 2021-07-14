#include "stdafx.h"
#include "PCOthersPriest.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "RenderTarget.h"
#include "InstancePoolMgr.h"
#include "DirectInput.h"

CPCOthersPriest::CPCOthersPriest(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pServerMath(CServerMath::Get_Instance())
{
}

void CPCOthersPriest::Set_StanceChange(const _uint& uiAniIdx, const _bool& bIsStanceAttack)
{
	if (nullptr == m_pWeapon)
		return;

	m_uiAnimIdx = uiAniIdx;
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

	if (bIsStanceAttack)
	{
		m_eCurStance = Priest::STANCE_ATTACK;
		m_pWeapon->Set_DissolveInterpolation(-1.0f);
		m_pWeapon->Set_IsRenderShadow(true);
		SetUp_WeaponRHand();
	}
	else
	{
		m_eCurStance = Priest::STANCE_NONEATTACK;
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);
	}

	m_bIsCompleteStanceChange = false;
}

void CPCOthersPriest::Set_OthersStance(const _bool& bIsStanceAttack)
{
	if (nullptr == m_pWeapon)
		return;

	if (bIsStanceAttack)
	{
		m_uiAnimIdx = Priest::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		m_eCurStance = Priest::STANCE_ATTACK;
		m_pWeapon->Set_DissolveInterpolation(-1.0f);
		m_pWeapon->Set_IsRenderShadow(true);
		SetUp_WeaponRHand();
	}
	else
	{
		m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		m_eCurStance = Priest::STANCE_NONEATTACK;
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);
		SetUp_WeaponBack();
	}
}

HRESULT CPCOthersPriest::Ready_GameObject(wstring wstrMeshTag, 
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

	m_pInfoCom->m_fSpeed     = Priest::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_chCurStageID = STAGE_VELIKA;
	m_chPreStageID = STAGE_VELIKA;

	/*__________________________________________________________________________________________________________
	[ �ִϸ��̼� ���� ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	m_ePreStance = Priest::STANCE_NONEATTACK;
	m_eCurStance = Priest::STANCE_NONEATTACK;

	/*__________________________________________________________________________________________________________
	[ �������� ���� ]
	____________________________________________________________________________________________________________*/
	// Angle
	m_tAngleInterpolationDesc.interpolation_speed = 5.0f;

	// Move Speed
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Priest::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Priest::MAX_SPEED * Priest::OTHERS_OFFSET;

	// Create Weapon
	Engine::FAILED_CHECK_RETURN(SetUp_PCWeapon(), E_FAIL);

	return S_OK;
}

HRESULT CPCOthersPriest::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pEdgeObjectShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(1), E_FAIL);

	return S_OK;
}

_int CPCOthersPriest::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	if (m_bIsDead)
		return DEAD_OBJ;

	Effect_Loop(fTimeDelta);
	if (m_bIsReturn)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(fTimeDelta);
		}

		m_bIsResetNaviMesh = false;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_PCOthersPriestPool(), m_uiInstanceIdx);

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

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCOthersPriest::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPCOthersPriest::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTableMiniMap();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiMiniMapTexIdx, 
								   Engine::MATRIXID::TOP_VIEW);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CPCOthersPriest::Render_EdgeGameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_DynamicMesh(m_pEdgeObjectShaderCom);
}

void CPCOthersPriest::Render_GameObject(const _float& fTimeDelta, 
										ID3D12GraphicsCommandList* pCommandList, 
										const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCOthersPriest::Render_ShadowDepth(const _float& fTimeDelta, 
										 ID3D12GraphicsCommandList* pCommandList, 
										 const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPCOthersPriest::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

	m_pWinterNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageWinter_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pWinterNaviMeshCom, E_FAIL);

	return S_OK;
}

HRESULT CPCOthersPriest::SetUp_PCWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		m_pWeapon = static_cast<CPCWeaponRod*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponRod(m_chCurWeaponType)));
		m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
		m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
		m_pWeapon->Update_GameObject(0.016f);

		m_chPreWeaponType = m_chCurWeaponType;
	}

	return S_OK;
}

void CPCOthersPriest::SetUp_StageID()
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

void CPCOthersPriest::Is_ChangeWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(0.016f);
		}

		m_pWeapon = static_cast<CPCWeaponRod*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponRod(m_chCurWeaponType)));
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
			m_pWeapon->Update_GameObject(0.016f);

			if (Priest::STANCE_ATTACK == m_eCurStance)
			{
				SetUp_WeaponRHand();
				m_pWeapon->Set_DissolveInterpolation(-1.0f);
				m_pWeapon->Set_IsRenderShadow(true);
			}
			else if (Priest::STANCE_NONEATTACK == m_eCurStance)
			{
				SetUp_WeaponBack();
				m_pWeapon->Set_DissolveInterpolation(1.0f);
				m_pWeapon->Set_IsRenderShadow(false);
			}

			m_chPreWeaponType = m_chCurWeaponType;
		}
	}
}

void CPCOthersPriest::Set_WeaponHierarchy()
{
	if (m_uiAnimIdx == Priest::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Priest::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Priest::IN_WEAPON)
	{
		SetUp_WeaponBack();
	}
	else
		SetUp_WeaponRHand();
}

void CPCOthersPriest::Set_IsRepeatAnimation()
{
	if (m_uiAnimIdx == Priest::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Priest::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Priest::ATTACK_WAIT ||
		m_uiAnimIdx == Priest::ATTACK_RUN)
	{
		m_bIsRepeatAnimation = true;
	}
	else
		m_bIsRepeatAnimation = false;
}

void CPCOthersPriest::Set_AnimationSpeed()
{
	if (m_uiAnimIdx == Priest::NONE_ATTACK_WALK)
	{
		m_fAnimationSpeed = TPS * 1.45f;
	}
	else if (m_uiAnimIdx == Priest::NONE_ATTACK_IDLE)
	{
		m_fAnimationSpeed = TPS * 0.6f;
	}
	else if (m_uiAnimIdx == Priest::ATTACK_WAIT)
	{
		m_fAnimationSpeed = TPS * 0.8f;
	}
	else if (m_uiAnimIdx == Priest::HASTE)
	{
		m_fAnimationSpeed = TPS * 1.35f;
	}
	else
		m_fAnimationSpeed = TPS;
}

void CPCOthersPriest::Set_BlendingSpeed()
{
	if (m_uiAnimIdx == Priest::NONE_ATTACK_IDLE ||
		m_uiAnimIdx == Priest::NONE_ATTACK_WALK ||
		m_uiAnimIdx == Priest::ATTACK_WAIT ||
		m_uiAnimIdx == Priest::ATTACK_RUN)
	{
		m_fBlendingSpeed = 0.001f;
	}
	else if (m_uiAnimIdx == Priest::HASTE ||
			 m_uiAnimIdx == Priest::AURA_ON ||
			 m_uiAnimIdx == Priest::PURIFY ||
			 m_uiAnimIdx == Priest::HEAL_SHOT ||
			 m_uiAnimIdx == Priest::MP_CHARGE_END)
	{
		m_fBlendingSpeed = 100.0f;
	}
	else
		m_fBlendingSpeed = 0.005f;
}

void CPCOthersPriest::Set_ConstantTable()
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

void CPCOthersPriest::Set_ConstantTableShadowDepth()
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

void CPCOthersPriest::Set_ConstantTableMiniMap()
{
	m_pTransMiniMap->m_vPos.x = m_pTransCom->m_vPos.x;
	m_pTransMiniMap->m_vPos.y = 1.1f;
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

void CPCOthersPriest::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsAttack)
		return;

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	SetUp_MoveSpeed(fTimeDelta);

	if (!m_bIsMoveStop || Priest::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		// NaviMesh �̵�.		
		if (!m_pServerMath->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
														 &m_pTransCom->m_vDir,
														 m_pInfoCom->m_fSpeed * fTimeDelta);
			m_pTransCom->m_vPos = vPos;
		}
	}
}

void CPCOthersPriest::SetUp_MoveSpeed(const _float& fTimeDelta)
{
	// Move On
	if (!m_bIsMoveStop)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f * Priest::OTHERS_OFFSET;

	// Move Off
	else
		m_tMoveSpeedInterpolationDesc.interpolation_speed = -Priest::MOVE_STOP_SPEED * Priest::OTHERS_OFFSET;

	m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, m_tMoveSpeedInterpolationDesc.v2, m_tMoveSpeedInterpolationDesc.linear_ratio);
}

void CPCOthersPriest::SetUp_StanceChange(const _float& fTimeDelta)
{
	if ((m_ePreStance != m_eCurStance) && !m_bIsCompleteStanceChange)
	{
		// NONE_ATTACK -> ATTACK
		if (Priest::STANCE_ATTACK == m_eCurStance)
		{
			// NONE_ATTACK -> ATACK
			if (Priest::OUT_WEAPON == m_uiAnimIdx &&
				m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_uiAnimIdx = Priest::ATTACK_WAIT;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				m_bIsCompleteStanceChange = true;
				m_ePreStance              = m_eCurStance;
			}
		}

		// ATTACK -> NONE_ATTACK
		else if (Priest::STANCE_NONEATTACK == m_eCurStance)
		{
			if (Priest::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
			{
				m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;
				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
				m_bIsCompleteStanceChange = true;
				m_ePreStance              = m_eCurStance;
			}

			if (Priest::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
				SetUp_WeaponBack();
		}
	}
}

void CPCOthersPriest::SetUp_WeaponRHand()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vPos.x = -2.0f;
	m_pWeapon->Get_Transform()->m_vPos.y = 5.0f;
	m_pWeapon->Get_Transform()->m_vPos.z = 1.0f;

	m_pWeapon->Get_Transform()->m_vAngle.x = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 0.0f;

	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("R_Sword")));
}

void CPCOthersPriest::SetUp_WeaponBack()
{
	if (nullptr == m_pWeapon)
		return;

	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

void CPCOthersPriest::Effect_Loop(const _float& fTimeDelta)
{
	if (m_uiAnimIdx == Priest::AURA_ON)
	{
		if (m_bisEffect == false)
		{
			m_bisEffect = true;
			CEffectMgr::Get_Instance()->Effect_Shield(m_pTransCom->m_vPos, m_pTransCom);
		}
	}
	else if (m_uiAnimIdx == Priest::PURIFY)
	{
		if (m_bisEffect == false)
		{
			m_bisEffect = true;
			CEffectMgr::Get_Instance()->Effect_Axe(m_pTransCom->m_vPos, m_pTransCom);
		}
	}
	else if (m_uiAnimIdx == Priest::HEAL_LOOP)
	{
		if (m_bisEffect == false)
		{
			m_bisEffect = true;
			CEffectMgr::Get_Instance()->Effect_GridShieldEffect(m_pTransCom->m_vPos, 1, m_pTransCom);
		}
	}
	else if (m_uiAnimIdx == Priest::MP_CHARGE_LOOP)
	{
		if (m_bisEffect == false)
		{
			m_bisEffect = true;
			CEffectMgr::Get_Instance()->Effect_GridShieldEffect(m_pTransCom->m_vPos, 0, m_pTransCom);
		}
	}
	else
	{
		m_bisEffect = false;
	}
}

Engine::CGameObject* CPCOthersPriest::Create(ID3D12Device* pGraphicDevice, 
											 ID3D12GraphicsCommandList* pCommandList,
											 wstring wstrMeshTag, 
											 wstring wstrNaviMeshTag, 
											 const _vec3& vScale, 
											 const _vec3& vAngle,
											 const _vec3& vPos, 
											 const char& chWeaponType)
{
	CPCOthersPriest* pInstance = new CPCOthersPriest(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos, chWeaponType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CPCOthersPriest** CPCOthersPriest::Create_InstancePool(ID3D12Device* pGraphicDevice, 
													   ID3D12GraphicsCommandList* pCommandList,
													   const _uint& uiInstanceCnt)
{
	CPCOthersPriest** ppInstance = new (CPCOthersPriest* [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CPCOthersPriest(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"PoporiMR25Priest",		// MeshTag
										L"StageVelika_NaviMesh",	// NaviMeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f, 0.0f, 0.0f),	// Angle
										_vec3(AWAY_FROM_STAGE),		// Pos
										Event_Wit_Rod_01);	// Pos
	}

	return ppInstance;
}

void CPCOthersPriest::Free()
{
	Engine::CGameObject::Free();

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
