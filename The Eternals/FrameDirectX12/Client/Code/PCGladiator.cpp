#include "stdafx.h"
#include "PCGladiator.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "Font.h"
#include "RenderTarget.h"
#include "DynamicCamera.h"
#include "TimeMgr.h"

CPCGladiator::CPCGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
{
}

HRESULT CPCGladiator::Ready_GameObject(wstring wstrMeshTag, 
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
										   m_pMeshCom->Get_MaxVector(),
										   1.0f,
										   _vec3(0.0f, 5.0f, 0.0f));

	m_pInfoCom->m_fSpeed     = Gladiator::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_eKeyState   = MVKEY::K_END;
	m_bIsKeyDown  = false;
	m_bIsSameDir  = false;
	m_fPreAngle   = m_pTransCom->m_vAngle.y;

	/*__________________________________________________________________________________________________________
	[ 선형보간 설정 ]
	____________________________________________________________________________________________________________*/
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Gladiator::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Gladiator::MAX_SPEED;

	m_tAttackMoveSpeedInterpolationDesc.linear_ratio        = 0.0f;
	m_tAttackMoveSpeedInterpolationDesc.v1                  = Gladiator::MIN_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.v2                  = Gladiator::MAX_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = 1.0f;
	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_eStance   = Gladiator::STANCE_NONEATTACK;
	m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;

	/*__________________________________________________________________________________________________________
	[ Collider Bone Setting ]
	____________________________________________________________________________________________________________*/
	Engine::SKINNING_MATRIX* pmatSkinning = nullptr;
	_matrix* pmatParent = nullptr;

	//// ColliderSphere
	//pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	//pmatParent = &(m_pTransCom->m_matWorld);
	//Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	//m_pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	//m_pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	//m_pColliderSphereCom->Set_Scale(_vec3(3.f, 3.f, 3.f));		// Collider Scale
	//m_pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);	// Collider Radius

	//// ColliderBox
	//pmatSkinning = m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	//pmatParent = &(m_pTransCom->m_matWorld);
	//Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);
	//m_pColliderBoxCom->Set_SkinningMatrix(pmatSkinning);	// Bone Matrix
	//m_pColliderBoxCom->Set_ParentMatrix(pmatParent);		// Parent Matrix
	//m_pColliderBoxCom->Set_Scale(_vec3(3.f, 3.f, 3.f));		// Collider Scale
	//m_pColliderBoxCom->Set_Extents(m_pTransCom->m_vScale);	// Box Offset From Center

	/*__________________________________________________________________________________________________________
	[ Font 생성 ]
	____________________________________________________________________________________________________________*/
	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont->Ready_GameObject(L"", _vec2(900.f, 0.f), D2D1::ColorF::Yellow), E_FAIL);


	return S_OK;
}

HRESULT CPCGladiator::LateInit_GameObject()
{
	//DynamicCamera ]
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// Create Weapon
	Engine::FAILED_CHECK_RETURN(SetUp_PCWeapon(), E_FAIL);

	return S_OK;
}

_int CPCGladiator::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if (!g_bIsOnDebugCaemra)
	{
		Key_Input(fTimeDelta);
	}

	// NaviMesh 이동.
	SetUp_RunMoveSpeed(fTimeDelta);
	Move_OnNaviMesh(fTimeDelta);

	// Angle Linear Interpolation
	SetUp_AngleInterpolation(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);
	m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
	m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCGladiator::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Font Update ]
	____________________________________________________________________________________________________________*/
	if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"DebugFont"))
	{
		m_wstrText = wstring(L"[ Character Info ] \n") +
					 wstring(L"Pos\t %d, %d, %d\n") +
					 wstring(L"AngleY\t %d\n") +
					 wstring(L"AniIndex \t %d\n") +
					 wstring(L"MaxFrame \t %d\n") +
					 wstring(L"CurrentFrame \t%d");

		wsprintf(m_szText, m_wstrText.c_str(),
				 (_int)m_pTransCom->m_vPos.x, (_int)m_pTransCom->m_vPos.y, (_int)m_pTransCom->m_vPos.z,
				 (_int)m_pTransCom->m_vAngle.y,
				 m_uiAnimIdx,
				 m_ui3DMax_NumFrame,
				 m_ui3DMax_CurFrame);

		m_pFont->Update_GameObject(fTimeDelta);
		m_pFont->Set_Text(wstring(m_szText));
	}

	return NO_EVENT;
}

void CPCGladiator::Send_PacketToServer()
{
	// 움직이고 있는 중일 경우
	if (m_bIsKeyDown)
	{
		if (m_pPacketMgr->change_MoveKey(m_eKeyState) || m_bIsSameDir == true)
		{
			Send_Player_Move();
			m_bIsSameDir = false;
		}

		if (Is_Change_CamDirection())
		{
			Send_Player_Move();
		}
	}
}

void CPCGladiator::Render_GameObject(const _float& fTimeDelta, 
									 ID3D12GraphicsCommandList* pCommandList, 
									 const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCGladiator::Render_ShadowDepth(const _float& fTimeDelta, 
									  ID3D12GraphicsCommandList* pCommandList, 
									  const _int& iContextIdx)
{
	Set_ConstantTableShadowDepth();
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPCGladiator::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

	//// Collider - Sphere
	//m_pColliderSphereCom = static_cast<Engine::CColliderSphere*>(m_pComponentMgr->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
	//Engine::NULL_CHECK_RETURN(m_pColliderSphereCom, E_FAIL);
	//m_pColliderSphereCom->AddRef();
	//m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderSphere", m_pColliderSphereCom);

	//// Collider - Box
	//m_pColliderBoxCom = static_cast<Engine::CColliderBox*>(m_pComponentMgr->Clone_Component(L"ColliderBox", Engine::COMPONENTID::ID_DYNAMIC));
	//Engine::NULL_CHECK_RETURN(m_pColliderBoxCom, E_FAIL);
	//m_pColliderBoxCom->AddRef();
	//m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderBox", m_pColliderBoxCom);

	// NaviMesh
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(wstrNaviMeshTag, Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	return S_OK;
}

HRESULT CPCGladiator::SetUp_PCWeapon()
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
										 m_pMeshCom->Find_HierarchyDesc("Weapon_Back"),
										 &m_pTransCom->m_matWorld,
										 _rgba(0.64f, 0.96f, 0.97f, 1.0f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"ThisPlayerWeaponTwoHand", m_pWeapon), E_FAIL);


	return S_OK;
}

void CPCGladiator::Set_ConstantTable()
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

void CPCGladiator::Set_ConstantTableShadowDepth()
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

void CPCGladiator::Key_Input(const _float& fTimeDelta)
{
	if (!g_bIsActive) return;

	KeyInput_Move(fTimeDelta);
	KeyInput_Attack(fTimeDelta);

	/* Player Attack to Monster --- TEST */
	if (Engine::KEY_PRESSING(DIK_M))
	{
		m_pPacketMgr->send_attackToMonster(5000);
		m_pPacketMgr->send_attackToMonster(5001);
		m_pPacketMgr->send_attackToMonster(5002);
		m_pPacketMgr->send_attackToMonster(5003);
	}
}

void CPCGladiator::KeyInput_Move(const _float& fTimeDelta)
{
	if (!g_bIsActive || m_bIsAttack || !m_bIsCompleteStanceChange)
		return;

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	if (Engine::KEY_PRESSING(DIK_W))
	{
		// 대각선 - 우 상단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_UP;
			m_eKeyState             = MVKEY::K_RIGHT_UP;			
		}
		// 대각선 - 좌 상단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_UP;
			m_eKeyState             = MVKEY::K_LEFT_UP;
		}
		// 직진.
		else
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + FRONT;
			m_eKeyState             = MVKEY::K_FRONT;
		}

		m_last_move_time = high_resolution_clock::now();
		m_bIsKeyDown     = true;
	}

	else if (Engine::KEY_PRESSING(DIK_S))
	{
		// 대각선 - 우 하단.
		if (Engine::KEY_PRESSING(DIK_D))
		{

			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_DOWN;
			m_eKeyState             = MVKEY::K_RIGHT_DOWN;
		}
		// 대각선 - 좌 하단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{

			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_DOWN;
			m_eKeyState             = MVKEY::K_LEFT_DOWN;
		}
		// 후진.
		else
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + BACK;
			m_eKeyState             = MVKEY::K_BACK;
		}

		m_last_move_time = high_resolution_clock::now();
		m_bIsKeyDown     = true;
	}
	// 좌로 이동.
	else if (Engine::KEY_PRESSING(DIK_A))
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_LEFT;
		m_bIsKeyDown            = true;
	}
	// 우로 이동.	
	else if (Engine::KEY_PRESSING(DIK_D))
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_RIGHT;
		m_bIsKeyDown            = true;
	}
	// 움직임 Stop
	else
	{
		if (m_bIsKeyDown)
		{
			m_bIsKeyDown = false;
			m_bIsSameDir = true;
		}
		else if (!m_bIsKeyDown &&
				 Gladiator::MIN_SPEED == m_pInfoCom->m_fSpeed)
		{
			m_pPacketMgr->send_move_stop(m_pTransCom->m_vPos, m_pTransCom->m_vDir, m_uiAnimIdx);
		}
	}

	SetUp_RunAnimation();
	SetUp_RunToIdleAnimation(fTimeDelta);
}

void CPCGladiator::KeyInput_Attack(const _float& fTimeDelta)
{
	KeyInput_StanceChange(fTimeDelta);

	if (Gladiator::STANCE_ATTACK == m_eStance && m_bIsCompleteStanceChange)
	{
		KeyInput_ComboAttack(fTimeDelta);
	}

}

void CPCGladiator::KeyInput_StanceChange(const _float& fTimeDelta)
{
	if (m_bIsAttack)
		return;

	// ATTACK -> NONE_ATTACK
	if (Engine::KEY_DOWN(DIK_LSHIFT) && Gladiator::STANCE_ATTACK == m_eStance &&
		!m_bIsKeyDown && m_pInfoCom->m_fSpeed == Gladiator::MIN_SPEED &&
		m_bIsCompleteStanceChange &&
		m_uiAnimIdx != Gladiator::NONE_ATTACK_WALK && m_uiAnimIdx != Gladiator::ATTACK_RUN &&
		m_uiAnimIdx != Gladiator::IN_WEAPON && m_uiAnimIdx != Gladiator::OUT_WEAPON)
	{
		m_bIsCompleteStanceChange = false;
		SetUp_PlayerStance_FromAttackToNoneAttack();

		// Send Packet.
		m_pPacketMgr->send_stance_change(m_uiAnimIdx, false);
	}

	// NONE_ATTACK -> ATTACK
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) && Gladiator::STANCE_NONEATTACK == m_eStance &&
		m_bIsCompleteStanceChange &&
		m_uiAnimIdx != Gladiator::IN_WEAPON && m_uiAnimIdx != Gladiator::OUT_WEAPON)
	{
		m_bIsKeyDown = false;
		m_bIsCompleteStanceChange = false;
		SetUp_PlayerStance_FromNoneAttackToAttack();

		// Send Packet.
		m_pPacketMgr->send_stance_change(m_uiAnimIdx, true);
	}

	Change_PlayerStance(fTimeDelta);
}

void CPCGladiator::KeyInput_ComboAttack(const _float& fTimeDelta)
{
	// ComoboAttack
	SetUp_ComboAttackAnimation();
	// Return to AttackWait
	SetUp_FromComboAttackToAttackWait(fTimeDelta);

	if (m_bIsAttack)
	{
		m_bIsSameDir = true;

		SetUp_ComboAttackMove(fTimeDelta, Gladiator::COMBOCNT_1, Gladiator::COMBO1, Gladiator::COMBO1_MOVESTOP_TICK, 1.0f, -3.0f);
		SetUp_ComboAttackMove(fTimeDelta, Gladiator::COMBOCNT_2, Gladiator::COMBO2, Gladiator::COMBO2_MOVESTOP_TICK, 1.0f, -3.5f);
		SetUp_ComboAttackMove(fTimeDelta, Gladiator::COMBOCNT_3, Gladiator::COMBO3, Gladiator::COMBO3_MOVESTOP_TICK, 1.0f, -3.5f);
		SetUp_ComboAttackMove(fTimeDelta, Gladiator::COMBO_END, Gladiator::COMBO4, Gladiator::COMBO4_MOVESTOP_TICK, 0.75f, -1.0f);

		SetUp_ComboAttackTrail(Gladiator::COMBOCNT_1, Gladiator::COMBO1, Gladiator::COMBO1_TRAIL_START_TICK, Gladiator::COMBO1_TRAIL_STOP_TICK);
		SetUp_ComboAttackTrail(Gladiator::COMBOCNT_2, Gladiator::COMBO2, Gladiator::COMBO2_TRAIL_START_TICK, Gladiator::COMBO2_TRAIL_STOP_TICK);
		SetUp_ComboAttackTrail(Gladiator::COMBOCNT_3, Gladiator::COMBO3, Gladiator::COMBO3_TRAIL_START_TICK, Gladiator::COMBO3_TRAIL_STOP_TICK);
		SetUp_ComboAttackTrail(Gladiator::COMBO_END, Gladiator::COMBO4, Gladiator::COMBO4_TRAIL_START_TICK, Gladiator::COMBO4_TRAIL_STOP_TICK);

		AttackMove_OnNaviMesh(fTimeDelta);
	}
}

void CPCGladiator::SetUp_ComboAttackAnimation()
{
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) && 
		m_pMeshCom->Is_BlendingComplete())
	{
		// ATTACK_WAIT ==> COMBO1
		if (Gladiator::COMBOCNT_0 == m_uiComoboCnt)
		{
			m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_pWeapon->Set_IsRenderTrail(false);
			m_bIsAttack   = true;
			m_bIsKeyDown  = false;
			m_uiComoboCnt = Gladiator::COMBOCNT_1;
			m_uiAnimIdx   = Gladiator::COMBO1;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO1 ==> COMBO2
		else if (Gladiator::COMBOCNT_1 == m_uiComoboCnt &&
				 Gladiator::COMBO1 == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame >= m_ui3DMax_NumFrame * 0.75f)
		{
			m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_pWeapon->Set_IsRenderTrail(false);
			m_bIsKeyDown  = false;
			m_bIsAttack   = true;
			m_uiComoboCnt = Gladiator::COMBOCNT_2;
			m_uiAnimIdx   = Gladiator::COMBO2;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO1R ==> COMBO2 
		else if (Gladiator::COMBOCNT_1 == m_uiComoboCnt &&
				 Gladiator::COMBO1R == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame <= 10)
		{
			m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_pWeapon->Set_IsRenderTrail(false);
			m_bIsAttack   = true;
			m_bIsKeyDown  = false;
			m_uiComoboCnt = Gladiator::COMBOCNT_2;
			m_uiAnimIdx   = Gladiator::COMBO2;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO2 ==> COMBO3
		else if (Gladiator::COMBOCNT_2 == m_uiComoboCnt &&
				 Gladiator::COMBO2 == m_uiAnimIdx &&
				 m_ui3DMax_CurFrame >= m_ui3DMax_NumFrame * 0.75f)
		{
			m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_pWeapon->Set_IsRenderTrail(false);
			m_bIsAttack   = true;
			m_bIsKeyDown  = false;
			m_uiComoboCnt = Gladiator::COMBOCNT_3;
			m_uiAnimIdx   = Gladiator::COMBO3;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO2R ==> COMBO3
		else if (Gladiator::COMBOCNT_2 == m_uiComoboCnt &&
				 Gladiator::COMBO2R == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame <= m_ui3DMax_NumFrame * 0.5f)
		{
			m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_pWeapon->Set_IsRenderTrail(false);
			m_bIsAttack   = true;
			m_bIsKeyDown  = false;
			m_uiComoboCnt = Gladiator::COMBOCNT_3;
			m_uiAnimIdx   = Gladiator::COMBO3;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO3 ==> COMBO4
		else if (Gladiator::COMBOCNT_3 == m_uiComoboCnt &&
				 Gladiator::COMBO3 == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame >= m_ui3DMax_NumFrame * 0.85f)
		{
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_bIsAttack   = true;
			m_bIsKeyDown  = false;
			m_uiComoboCnt = Gladiator::COMBO_END;
			m_uiAnimIdx   = Gladiator::COMBO4;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		// COMBO3R ==> COMBO4
		else if (Gladiator::COMBOCNT_3 == m_uiComoboCnt &&
				 Gladiator::COMBO3R == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame <= m_ui3DMax_NumFrame * 0.55f)
		{
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_bIsAttack   = true;
			m_bIsKeyDown  = false;
			m_uiComoboCnt = Gladiator::COMBO_END;
			m_uiAnimIdx   = Gladiator::COMBO4;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
	}
}

void CPCGladiator::SetUp_FromComboAttackToAttackWait(const _float& fTimeDelta)
{
	if (Gladiator::COMBO1 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
	{
		m_bIsKeyDown = false;
		m_uiAnimIdx  = Gladiator::COMBO1R;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pWeapon->Set_IsRenderTrail(false);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
	else if (Gladiator::COMBO2 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
	{
		m_bIsKeyDown = false;
		m_uiAnimIdx  = Gladiator::COMBO2R;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pWeapon->Set_IsRenderTrail(false);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
	else if (Gladiator::COMBO3 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
	{
		m_bIsKeyDown = false;
		m_uiAnimIdx  = Gladiator::COMBO3R;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pWeapon->Set_IsRenderTrail(false);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}

	else if ((Gladiator::COMBO1R == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta)) ||
			 (Gladiator::COMBO2R == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta)) ||
			 (Gladiator::COMBO3R == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta)) ||
			 (Gladiator::COMBO4 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta)))
	{
		m_bIsAttack   = false;
		m_uiComoboCnt = Gladiator::COMBOCNT_0;
		m_uiAnimIdx   = Gladiator::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pWeapon->Set_IsRenderTrail(false);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCGladiator::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsKeyDown || Gladiator::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		if (!m_pServerMath->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
																  &m_pTransCom->m_vDir,
																  m_pInfoCom->m_fSpeed * fTimeDelta);
		}
	}
}

void CPCGladiator::AttackMove_OnNaviMesh(const _float& fTimeDelta)
{
	// Set Speed
	m_tAttackMoveSpeedInterpolationDesc.linear_ratio += m_tAttackMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tAttackMoveSpeedInterpolationDesc.v1,
													   m_tAttackMoveSpeedInterpolationDesc.v2,
													   m_tAttackMoveSpeedInterpolationDesc.linear_ratio);
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

void CPCGladiator::Send_Player_Move()
{
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();
	
	switch (m_eKeyState)
	{
	case K_FRONT:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_BACK:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_RIGHT:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_RIGHT_UP:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_RIGHT_DOWN:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_LEFT:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_LEFT_UP:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	case K_LEFT_DOWN:
		m_pPacketMgr->send_move(m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_uiAnimIdx);
		break;
	}

	m_fPreAngle = m_pTransCom->m_vAngle.y;
}

bool CPCGladiator::Is_Change_CamDirection()
{
	if (m_fPreAngle + 5.f < m_pTransCom->m_vAngle.y)
		return true;

	else if (m_fPreAngle - 5.f > m_pTransCom->m_vAngle.y)
		return true;

	return false;
}

void CPCGladiator::SetUp_RunMoveSpeed(const _float& fTimeDelta)
{
	// Move On
	if (m_bIsKeyDown)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f;

	// Move Off
	else if (!m_bIsKeyDown)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = -Gladiator::MOVE_STOP_SPEED;

	m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, 
													   m_tMoveSpeedInterpolationDesc.v2, 
													   m_tMoveSpeedInterpolationDesc.linear_ratio);
}

void CPCGladiator::SetUp_RunAnimation()
{
	if (m_bIsKeyDown && Gladiator::STANCE_NONEATTACK == m_eStance)
	{
		m_uiAnimIdx = Gladiator::NONE_ATTACK_WALK;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}

	else if (m_bIsKeyDown && Gladiator::STANCE_ATTACK == m_eStance)
	{
		m_uiAnimIdx = Gladiator::ATTACK_RUN;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCGladiator::SetUp_RunToIdleAnimation(const _float& fTimeDelta)
{
	if (!m_bIsKeyDown &&
		Gladiator::NONE_ATTACK_WALK == m_uiAnimIdx &&
		Gladiator::STANCE_NONEATTACK == m_eStance)
	{
		m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
	else if (!m_bIsKeyDown &&
			 Gladiator::ATTACK_RUN == m_uiAnimIdx &&
			 Gladiator::STANCE_ATTACK == m_eStance)
	{
		m_uiAnimIdx = Gladiator::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCGladiator::SetUp_PlayerStance_FromAttackToNoneAttack()
{
	// NONE_ATTACK -> ATTACK
	if (Gladiator::STANCE_ATTACK == m_eStance)
	{
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);

		m_eStance   = Gladiator::STANCE_NONEATTACK;
		m_uiAnimIdx = Gladiator::IN_WEAPON;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCGladiator::SetUp_PlayerStance_FromNoneAttackToAttack()
{
	// NONE_ATTACK -> ATTACK
	if (Gladiator::STANCE_NONEATTACK == m_eStance)
	{
		if (Gladiator::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Gladiator::NONE_ATTACK_WALK == m_uiAnimIdx)
		{
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
			m_pWeapon->Set_HierarchyDesc(m_pMeshCom->Find_HierarchyDesc("L_Sword"));
		}

		m_eStance   = Gladiator::STANCE_ATTACK;
		m_uiAnimIdx = Gladiator::OUT_WEAPON;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCGladiator::Change_PlayerStance(const _float& fTimeDelta)
{
	if (!m_bIsCompleteStanceChange)
	{
		// NONE_ATTACK -> ATACK
		if (Gladiator::STANCE_ATTACK == m_eStance &&
			Gladiator::OUT_WEAPON == m_uiAnimIdx &&
			m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
		{
			m_uiAnimIdx = Gladiator::ATTACK_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;

		}

		// ATTACK -> NONE_ATTACK
		else if (Gladiator::STANCE_NONEATTACK == m_eStance  &&
				 Gladiator::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta))
		{
			m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;
		}

		if (Gladiator::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
			m_pWeapon->Set_HierarchyDesc(m_pMeshCom->Find_HierarchyDesc("Weapon_Back"));

		// Check Is Complete Stance Change
		if (Gladiator::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Gladiator::ATTACK_WAIT == m_uiAnimIdx)
		{
			m_bIsCompleteStanceChange = true;
		}
	}
}

void CPCGladiator::Ready_AngleInterpolationValue(const _float& fEndAngle)
{
	m_tAngleInterpolationDesc.is_start_interpolation = true;
	m_tAngleInterpolationDesc.v1                     = m_pTransCom->m_vAngle.y;
	m_tAngleInterpolationDesc.v2                     = fEndAngle;
	m_tAngleInterpolationDesc.linear_ratio           = Engine::MIN_LINEAR_RATIO;
	m_tAngleInterpolationDesc.interpolation_speed    = 3.0f;
}

void CPCGladiator::SetUp_AngleInterpolation(const _float& fTimeDelta)
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

void CPCGladiator::SetUp_ComboAttackMove(const _float& fTimeDelta, 
										  const _uint& uiComboCnt,
										  const _uint& uiAniIdx, 
										  const _uint& uiStopTick,
										  const _float& fMoveSpeed,
										  const _float& fStopSpeed)
{
	if (uiComboCnt == m_uiComoboCnt &&
		uiAniIdx == m_uiAnimIdx &&
		m_pMeshCom->Is_BlendingComplete())
	{
		// Move On
		if (m_ui3DMax_CurFrame < uiStopTick)
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fMoveSpeed;

		// Move Off
		else
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fStopSpeed;
	}
}

void CPCGladiator::SetUp_ComboAttackTrail(const _uint& uiComboCnt,
										  const _uint& uiAniIdx,
										  const _uint& uiStartTick,
										  const _uint& uiStopTick)
{
	if (uiComboCnt == m_uiComoboCnt && 
		uiAniIdx == m_uiAnimIdx && 
		m_pMeshCom->Is_BlendingComplete())
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

Engine::CGameObject* CPCGladiator::Create(ID3D12Device* pGraphicDevice, 
										  ID3D12GraphicsCommandList* pCommandList,
										  wstring wstrMeshTag, 
										  wstring wstrNaviMeshTag, 
										  const _vec3& vScale,
										  const _vec3& vAngle, 
										  const _vec3& vPos,
										  const char& chWeaponType)
{
	CPCGladiator* pInstance = new CPCGladiator(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos, chWeaponType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPCGladiator::Free()
{
	m_pWeapon->Set_DeadGameObject();

	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pDynamicCamera);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);
	Engine::Safe_Release(m_pNaviMeshCom);
	Engine::Safe_Release(m_pFont);
}
