#include "stdafx.h"
#include "PCGladiator.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "Font.h"
#include "DynamicCamera.h"
#include "CollisionMgr.h"
#include "CollisionTick.h"
#include "InstancePoolMgr.h"
#include "FadeInOut.h"
#include "NPC_Merchant.h"
#include "NPC_Quest.h"
#include "GameUIRoot.h"
#include "GameUIChild.h"
#include "CharacterHpGauge.h"
#include "CharacterMpGauge.h"
#include "ShaderMgr.h"
#include "IceStorm.h"

CPCGladiator::CPCGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

HRESULT CPCGladiator::Ready_GameObject(wstring wstrMeshTag, 
									   wstring wstrNaviMeshTag, 
									   const _vec3& vScale,
									   const _vec3& vAngle, 
									   const _vec3& vPos,
									   const char& chWeaponType)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag, wstrNaviMeshTag), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos   = vPos;
	m_chCurWeaponType     = chWeaponType;
	m_chCurStageID        = STAGE_VELIKA;
	m_chPreStageID        = m_chCurStageID;
	m_wstrCollisionTag    = L"ThisPlayer";

	m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(vPos));

	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector(),
										   1.0f,
										   _vec3(0.0f, 5.0f, 0.0f));

	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  _vec3(30.0f),
											  _vec3(0.0f, 20.0f, 0.0f));
	m_lstCollider.push_back(m_pBoundingSphereCom);

	m_pInfoCom->m_fSpeed     = Gladiator::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_eKeyState			= MVKEY::K_END;
	m_bIsKeyDown		= false;
	m_bIsSameDir		= false;
	m_bIsSendMoveStop	= true;
	m_fPreAngle			= m_pTransCom->m_vAngle.y;

	/*__________________________________________________________________________________________________________
	[ Skill KeyInput ]
	____________________________________________________________________________________________________________*/
	m_mapSkillKeyInput[L"STINGER_BLADE"] = DIK_1;
	m_mapSkillKeyInput[L"CUTTING_SLASH"] = DIK_2;
	m_mapSkillKeyInput[L"JAW_BREAKER"]   = DIK_3;
	// m_mapSkillKeyInput[L"WIND_CUTTER"]   = DIK_4;
	m_mapSkillKeyInput[L"GAIA_CRUSH"]    = DIK_4;
	m_mapSkillKeyInput[L"DRAW_SWORD"]    = DIK_5;
	//m_mapSkillKeyInput[L"CUT_HEAD"]      = DIK_4;

	/*__________________________________________________________________________________________________________
	[ 선형보간 설정 ]
	____________________________________________________________________________________________________________*/
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Gladiator::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Gladiator::MAX_SPEED;

	m_tAttackMoveSpeedInterpolationDesc.linear_ratio        = 0.0f;
	m_tAttackMoveSpeedInterpolationDesc.v1                  = Gladiator::MIN_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.v2                  = Gladiator::MAX_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = 0.0f;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_eStance   = Gladiator::STANCE_NONEATTACK;
	m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;

	// AfterImage
	m_uiAfterImgSize    = 15;
	m_fAfterImgMakeTime = 0.2f;
	m_fAfterSubAlpha    = 0.75f;
	m_pMeshCom->Set_AfterImgMakeTime(m_fAfterImgMakeTime);
	m_pMeshCom->Set_AfterImgSize(m_uiAfterImgSize);
	m_pMeshCom->Set_AfterImgSubAlpha(m_fAfterSubAlpha);

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
	// DynamicCamera
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// ShadowLight
	CShadowLightMgr::Get_Instance()->Set_ShadowType(SHADOW_TYPE::SHADOW_TYPE_PLAYER);

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()), m_uiAfterImgSize);
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	// Create Weapon
	Engine::FAILED_CHECK_RETURN(SetUp_PCWeapon(), E_FAIL);

	// UI ClassFrame
	Engine::FAILED_CHECK_RETURN(SetUp_ClassFrame(), E_FAIL);

	return S_OK;
}

_int CPCGladiator::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	Is_ChangeWeapon();
	SetUp_StageID();

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

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	if (!g_bIsOnDebugCaemra && 
		!g_bIsStageChange)
	{
		Key_Input(fTimeDelta);
	}

	if (g_bIsStageChange)
		m_bIsKeyDown = false;

	// Create CollisionTick
	if (m_pMeshCom->Is_BlendingComplete())
		SetUp_CollisionTick(fTimeDelta);

	// NaviMesh 이동.
	SetUp_RunMoveSpeed(fTimeDelta);
	Move_OnNaviMesh(fTimeDelta);

	// Angle Linear Interpolation
	SetUp_AngleInterpolation(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// AfterImage
	Make_AfterImage(fTimeDelta);

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCGladiator::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	Process_Collision();

	/*__________________________________________________________________________________________________________
	[ Font Update ]
	____________________________________________________________________________________________________________*/
	if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"DebugFont"))
	{
		m_wstrText = wstring(L"[ Character Info ] \n") +
					 wstring(L"Pos::(%d, %d, %d)\n") +
					 wstring(L"AngleY::%d\n") +
					 wstring(L"AniIndex::%d\n") +
					 wstring(L"MaxFrame::%d\n") +
					 wstring(L"CurrentFrame::%d");

		wsprintf(m_szText, m_wstrText.c_str(),
				 (_int)m_pTransCom->m_vPos.x, (_int)m_pTransCom->m_vPos.y, (_int)m_pTransCom->m_vPos.z,
				 (_int)m_pTransCom->m_vAngle.y,
				 m_uiAnimIdx,
				 m_ui3DMax_NumFrame,
				 m_ui3DMax_CurFrame);

		m_pFont->Update_GameObject(fTimeDelta);
		m_pFont->Set_Text(wstring(m_szText));
	}

	Set_ConstantTableShadowDepth();
	Set_ConstantTable();

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->LateUpdate_GameObject(fTimeDelta);

	return NO_EVENT;
}

void CPCGladiator::Process_Collision()
{
	if (g_bIsStageChange)
		return;

	for (auto& pDst : m_lstCollisionDst)
	{
		// Collision Monster
		if (L"Monster_SingleCollider" == pDst->Get_CollisionTag())
		{
			m_pBoundingSphereCom->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
			pDst->Get_BoundingSphere()->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

		}
		if (L"Monster_MultiCollider" == pDst->Get_CollisionTag())
			Collision_MonsterMultiCollider(pDst->Get_ColliderList());

		// Collision Stage Change
		if (L"Portal_VelikaToBeach" == pDst->Get_CollisionTag())
			Collision_PortalVelikaToBeach(pDst->Get_ColliderList());

		if (L"Portal_BeachToVelika" == pDst->Get_CollisionTag())
			Collision_PortalBeachToVelika(pDst->Get_ColliderList());

		if (L"NPC_Merchant" == pDst->Get_CollisionTag())
			Collision_Merchant(pDst->Get_ColliderList(), pDst->Get_ServerNumber());

		if (L"NPC_QUest" == pDst->Get_CollisionTag())
			Collision_Quest(pDst->Get_ColliderList(), pDst->Get_ServerNumber());
	}
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

void CPCGladiator::Render_GameObject(const _float& fTimeDelta)
{
	// Render AfterImage
	if (m_uiAfterImgSize)
	{
		m_pShaderCom->Set_PipelineStatePass(5);
		Render_AfterImage(fTimeDelta);
	}
}

void CPCGladiator::Render_AfterImage(const _float& fTimeDelta)
{
	auto iter_begin = m_lstAFWorldMatrix.begin();
	auto iter_end   = m_lstAFWorldMatrix.end();

	auto Alpha_begin = m_lstAFAlpha.begin();
	auto Alpha_end   = m_lstAFAlpha.end();

	for (_uint i = 0; iter_begin != iter_end; ++i, ++iter_begin)
	{
		/*__________________________________________________________________________________________________________
		[ Set ConstantBuffer Data ]
		____________________________________________________________________________________________________________*/
		Engine::CB_SHADER_MESH tCB_ShaderMesh;
		ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
		tCB_ShaderMesh.matWorld       = Engine::CShader::Compute_MatrixTranspose(*iter_begin);
		tCB_ShaderMesh.vAfterImgColor = *Alpha_begin;
		if (Alpha_begin != Alpha_end)
			Alpha_begin++;

		m_pShaderCom->Get_UploadBuffer_AFShaderMesh()->CopyData(i, tCB_ShaderMesh);

		// Render Buffer
		m_pMeshCom->Render_DynamicMeshAfterImage(m_pShaderCom, i);

	}
}

void CPCGladiator::Render_GameObject(const _float& fTimeDelta, 
									 ID3D12GraphicsCommandList* pCommandList, 
									 const _int& iContextIdx)
{
	m_pShaderCom->Set_PipelineStatePass(0);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCGladiator::Render_ShadowDepth(const _float& fTimeDelta, 
									  ID3D12GraphicsCommandList* pCommandList, 
									  const _int& iContextIdx)
{
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

	// NaviMesh
	m_pNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(wstrNaviMeshTag, Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMeshCom, E_FAIL);
	m_pNaviMeshCom->AddRef();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pNaviMeshCom);

	m_pVelikaNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageVelika_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pVelikaNaviMeshCom, E_FAIL);

	m_pBeachNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageBeach_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pBeachNaviMeshCom, E_FAIL);

	return S_OK;
}

HRESULT CPCGladiator::SetUp_PCWeapon()
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

HRESULT CPCGladiator::SetUp_ClassFrame()
{
	/*__________________________________________________________________________________________________________
	[ CharacterClassFrame ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUICharacterClassFrameWarrior.2DUI" };
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

			// UIRoot 생성.
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

			// UIChild 생성.
			Engine::CGameObject* pChildUI = nullptr;
			for (_int i = 0; i < iChildUISize; ++i)
			{
				if (L"ClassFrameHpFront" == vecObjectTag[i])
				{
					pChildUI = CCharacterHpGauge::Create(m_pGraphicDevice, m_pCommandList,
														 wstrRootObjectTag,				// RootObjectTag
														 vecObjectTag[i],				// ObjectTag
														 vecDataFilePath[i],			// DataFilePath
														 vecPos[i],						// Pos
														 vecScale[i],					// Scane
														 (_bool)vecIsSpriteAnimation[i],// Is Animation
														 vecFrameSpeed[i],				// FrameSpeed
														 vecRectPosOffset[i],			// RectPosOffset
														 vecRectScale[i],				// RectScaleOffset
														 vecUIDepth[i]);				// UI Depth
				}
				else if (L"ClassFrameMpFront" == vecObjectTag[i])
				{
					pChildUI = CCharacterMpGauge::Create(m_pGraphicDevice, m_pCommandList,
														 wstrRootObjectTag,				// RootObjectTag
														 vecObjectTag[i],				// ObjectTag
														 vecDataFilePath[i],			// DataFilePath
														 vecPos[i],						// Pos
														 vecScale[i],					// Scane
														 (_bool)vecIsSpriteAnimation[i],// Is Animation
														 vecFrameSpeed[i],				// FrameSpeed
														 vecRectPosOffset[i],			// RectPosOffset
														 vecRectScale[i],				// RectScaleOffset
														 vecUIDepth[i]);				// UI Depth
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

	return S_OK;
}

void CPCGladiator::SetUp_StageID()
{
	if (m_chCurStageID != m_chPreStageID)
	{
		if (STAGE_VELIKA == m_chCurStageID)
		{
			Engine::CShaderMgr::Get_Instance()->Set_TreeAlphaTest(1.0f);

			m_pNaviMeshCom = m_pVelikaNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			m_pTransCom->m_vPos = _vec3(STAGE_VELIKA_X, 0.0f, STAGE_VELIKA_Z);
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}
		else if (STAGE_BEACH == m_chCurStageID)
		{
			Engine::CShaderMgr::Get_Instance()->Set_TreeAlphaTest(1.0f);

			m_pNaviMeshCom = m_pBeachNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			m_pTransCom->m_vPos = _vec3(STAGE_BEACH_X, 0.0f, STAGE_BEACH_Z);
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}

		m_chPreStageID = m_chCurStageID;
	}
}

void CPCGladiator::Set_ConstantTable()
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

void CPCGladiator::Set_AnimationSpeed()
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

void CPCGladiator::Set_BlendingSpeed()
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

void CPCGladiator::Key_Input(const _float& fTimeDelta)
{
	if (!g_bIsActive) return;

	KeyInput_Move(fTimeDelta);
	KeyInput_Attack(fTimeDelta);

	
	if (Engine::KEY_DOWN(DIK_0))
	{
		Engine::CGameObject* pGameObj = nullptr;
		for (int i = 0; i < 36; i++)
		{
			pGameObj = CIceStorm::Create(m_pGraphicDevice, m_pCommandList,
										 L"IceStorm1",
										 _vec3(0.f),
										 _vec3(0.f, 0.0f, 0.0f),
										 _vec3(0, 0, 0), 5.f, XMConvertToRadians(i * 10.f));
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"IceStorm1", pGameObj), E_FAIL);
		}
	}

}

void CPCGladiator::KeyInput_Move(const _float& fTimeDelta)
{
	if (!g_bIsActive || m_bIsAttack || !m_bIsCompleteStanceChange || m_bIsSkillLoop)
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

		m_last_move_time	= high_resolution_clock::now();
		m_bIsKeyDown		= true;
		m_bIsSendMoveStop	= false;

		if (Gladiator::STANCE_ATTACK == m_eStance)
			SetUp_WeaponRHand();
		else if (Gladiator::STANCE_NONEATTACK == m_eStance)
			SetUp_WeaponBack();
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

		m_last_move_time	= high_resolution_clock::now();
		m_bIsKeyDown		= true;
		m_bIsSendMoveStop	= false;

		if (Gladiator::STANCE_ATTACK == m_eStance)
			SetUp_WeaponRHand();
		else if (Gladiator::STANCE_NONEATTACK == m_eStance)
			SetUp_WeaponBack();
	}
	// 좌로 이동.
	else if (Engine::KEY_PRESSING(DIK_A))
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_LEFT;
		m_bIsKeyDown            = true;
		m_bIsSendMoveStop		= false;

		if (Gladiator::STANCE_ATTACK == m_eStance)
			SetUp_WeaponRHand();
		else if (Gladiator::STANCE_NONEATTACK == m_eStance)
			SetUp_WeaponBack();
	}
	// 우로 이동.	
	else if (Engine::KEY_PRESSING(DIK_D))
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_RIGHT;
		m_bIsKeyDown            = true;
		m_bIsSendMoveStop		= false;

		if (Gladiator::STANCE_ATTACK == m_eStance)
			SetUp_WeaponRHand();
		else if (Gladiator::STANCE_NONEATTACK == m_eStance)
			SetUp_WeaponBack();
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
			if (!m_bIsSendMoveStop)
			{
				m_pPacketMgr->send_move_stop(m_pTransCom->m_vPos, m_pTransCom->m_vDir, m_uiAnimIdx);
				m_bIsSendMoveStop = true;
			}
			
		}
	}

	SetUp_RunAnimation();
	SetUp_RunToIdleAnimation(fTimeDelta);
}

void CPCGladiator::KeyInput_Attack(const _float& fTimeDelta)
{
	if (nullptr == m_pWeapon)
		return;

	KeyInput_StanceChange(fTimeDelta);

	if (Gladiator::STANCE_ATTACK == m_eStance && m_bIsCompleteStanceChange)
	{
		// 기본공격 (스킬공격 중에는 사용 X)
		if (!m_bIsSkill && !m_bIsSkillLoop)
			KeyInput_ComboAttack(fTimeDelta);

		// 스킬공격
		if (m_pMeshCom->Is_BlendingComplete())
			KeyInput_SkillAttack(fTimeDelta);

		if (m_pMeshCom->Is_BlendingComplete())
			KeyInput_Tumbling(fTimeDelta);

		if (m_bIsAttack)
		{
			m_bIsSameDir = true;

			// ComboAttack Move
			SetUp_AttackMove(Gladiator::COMBOCNT_1, Gladiator::COMBO1, 0, Gladiator::COMBO1_MOVE_STOP, 1.0f, -3.0f);
			SetUp_AttackMove(Gladiator::COMBOCNT_2, Gladiator::COMBO2, 0, Gladiator::COMBO2_MOVE_STOP, 1.0f, -3.5f);
			SetUp_AttackMove(Gladiator::COMBOCNT_3, Gladiator::COMBO3, 0, Gladiator::COMBO3_MOVE_STOP, 1.0f, -3.5f);
			SetUp_AttackMove(Gladiator::COMBOCNT_4, Gladiator::COMBO4, 0, Gladiator::COMBO4_MOVE_STOP, 0.75f, -1.0f);

			// ComoboAttack Trail
			SetUp_AttackTrail(Gladiator::COMBOCNT_1, Gladiator::COMBO1, Gladiator::COMBO1_TRAIL_START, Gladiator::COMBO1_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::COMBOCNT_2, Gladiator::COMBO2, Gladiator::COMBO2_TRAIL_START, Gladiator::COMBO2_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::COMBOCNT_3, Gladiator::COMBO3, Gladiator::COMBO3_TRAIL_START, Gladiator::COMBO3_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::COMBOCNT_4, Gladiator::COMBO4, Gladiator::COMBO4_TRAIL_START, Gladiator::COMBO4_TRAIL_STOP);

			// SkillAttack Move
			SetUp_AttackMove(Gladiator::STINGER_BLADE, Gladiator::STINGER_BLADE_MOVE_START, Gladiator::STINGER_BLADE_MOVE_STOP, 3.0f, -3.0f);
			SetUp_AttackMove(Gladiator::CUTTING_SLASH, Gladiator::CUTTING_SLASH_MOVE_START, Gladiator::CUTTING_SLASH_MOVE_STOP, 1.25f, -1.75f);
			SetUp_AttackMove(Gladiator::JAW_BREAKER, Gladiator::JAW_BREAKER_MOVE_START, Gladiator::JAW_BREAKER_MOVE_STOP, 4.0f, -4.0f);
			SetUp_AttackMove(Gladiator::CUT_HEAD, Gladiator::CUT_HEAD_MOVE_START, Gladiator::CUT_HEAD_MOVE_END, 1.5f, -8.0f);
			SetUp_AttackMove(Gladiator::WIND_CUTTER1, Gladiator::WIND_CUTTER1_MOVE_START, Gladiator::WIND_CUTTER1_MOVE_STOP, 3.0f, -4.0f);
			SetUp_AttackMove(Gladiator::WIND_CUTTER2, Gladiator::WIND_CUTTER2_MOVE_START, Gladiator::WIND_CUTTER2_MOVE_STOP, 3.0f, -4.0f);
			SetUp_AttackMove(Gladiator::WIND_CUTTER3, Gladiator::WIND_CUTTER3_MOVE_START, Gladiator::WIND_CUTTER3_MOVE_STOP, 2.0f, -4.0f);
			SetUp_AttackMove(Gladiator::GAIA_CRUSH1, Gladiator::GAIA_CRUSH1_MOVE_START, Gladiator::GAIA_CRUSH1_MOVE_STOP, 0.75f, 0.0f);
			SetUp_AttackMove(Gladiator::GAIA_CRUSH2, Gladiator::GAIA_CRUSH2_MOVE_START, Gladiator::GAIA_CRUSH2_MOVE_STOP, 0.75f, -2.0f);
			SetUp_AttackMove(Gladiator::DRAW_SWORD, Gladiator::DRAW_SWORD_MOVE_START, Gladiator::DRAW_SWORD_MOVE_STOP, 2.5f, -3.0f);
			SetUp_AttackMove(Gladiator::TUMBLING, Gladiator::TUMBLING_MOVE_START, Gladiator::TUMBLING_MOVE_STOP, 5.0f, -5.0f);

			// SkillAttck Trail
			SetUp_AttackTrail(Gladiator::STINGER_BLADE, Gladiator::STINGER_BLADE_TRAIL_START, Gladiator::STINGER_BLADE_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::CUTTING_SLASH, Gladiator::CUTTING_SLASH_TRAIL_START, Gladiator::CUTTING_SLASH_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::JAW_BREAKER, Gladiator::JAW_BREAKER_TRAIL_START, Gladiator::JAW_BREAKER_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::CUT_HEAD, Gladiator::CUT_HEAD_TRAIL_START, Gladiator::CUT_HEAD_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::WIND_CUTTER1, Gladiator::WIND_CUTTER1_TRAIL_START, Gladiator::WIND_CUTTER1_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::WIND_CUTTER2, Gladiator::WIND_CUTTER2_TRAIL_START, Gladiator::WIND_CUTTER2_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::WIND_CUTTER3, Gladiator::WIND_CUTTER3_TRAIL_START, Gladiator::WIND_CUTTER3_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::GAIA_CRUSH1, Gladiator::GAIA_CRUSH1_TRAIL_START, Gladiator::GAIA_CRUSH1_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::GAIA_CRUSH2, Gladiator::GAIA_CRUSH2_TRAIL_START, Gladiator::GAIA_CRUSH2_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::DRAW_SWORD, Gladiator::DRAW_SWORD_TRAIL_START, Gladiator::DRAW_SWORD_TRAIL_STOP);
			SetUp_AttackTrail(Gladiator::DRAW_SWORD_END, Gladiator::DRAW_SWORD_END_TRAIL_START, Gladiator::DRAW_SWORD_END_TRAIL_STOP);

			// SkillAttack AfterImage
			SetUp_AttackAfterImage(Gladiator::WIND_CUTTER1, 0, 18, 0.05f, 1.75f);
			SetUp_AttackAfterImage(Gladiator::WIND_CUTTER2, 0, 10, 0.05f, 1.75f);
			SetUp_AttackAfterImage(Gladiator::WIND_CUTTER3, 0, 20, 0.05f, 1.75f);
			SetUp_AttackAfterImage(Gladiator::GAIA_CRUSH1, 5, 99, 0.15f, 1.25f);
			SetUp_AttackAfterImage(Gladiator::GAIA_CRUSH2, 0, 30, 0.15f, 1.25f);
			SetUp_AttackAfterImage(Gladiator::DRAW_SWORD, 0, 20, 0.05f, 1.05f);

			AttackMove_OnNaviMesh(fTimeDelta);
		}
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
	if ((Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"STINGER_BLADE"]) ||
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"CUTTING_SLASH"]) ||
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"JAW_BREAKER"]) || 
		// Engine::KEY_DOWN(m_mapSkillKeyInput[L"CUT_HEAD"]) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"WIND_CUTTER"]) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"GAIA_CRUSH"]) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"DRAW_SWORD"])) &&
		Gladiator::STANCE_NONEATTACK == m_eStance &&
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
}

void CPCGladiator::KeyInput_SkillAttack(const _float& fTimeDelta)
{
	// SkillAttack
	if (!m_bIsSkillLoop)
	{
		if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"STINGER_BLADE"]))
		{
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
			m_bIsSkill = true;
			m_uiAnimIdx = Gladiator::STINGER_BLADE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"CUTTING_SLASH"]))
		{
			SetUp_WeaponRHand();
			SetUp_AttackSetting();
			m_bIsSkill = true;
			m_uiAnimIdx = Gladiator::CUTTING_SLASH;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"JAW_BREAKER"]))
		{
			SetUp_WeaponRHand();
			SetUp_AttackSetting();
			m_bIsSkill = true;
			m_uiAnimIdx = Gladiator::JAW_BREAKER;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"CUT_HEAD"]))
		{
			SetUp_WeaponRHand();
			SetUp_AttackSetting();
			m_bIsSkill = true;
			m_uiAnimIdx = Gladiator::CUT_HEAD;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"WIND_CUTTER"]))
		{
			SetUp_WeaponRHand();
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Gladiator::WIND_CUTTER1;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"GAIA_CRUSH"]))
		{
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Gladiator::GAIA_CRUSH1;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"DRAW_SWORD"]))
		{
			SetUp_WeaponRHand();
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Gladiator::DRAW_SWORD_CHARGE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
	}
	else
	{
		// WIND_CUTTER
		if (m_uiAnimIdx == Gladiator::WIND_CUTTER1 && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_bIsSetCollisionTick                        = false;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
			m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_uiAnimIdx = Gladiator::WIND_CUTTER2;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (m_uiAnimIdx == Gladiator::WIND_CUTTER2 && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_bIsSetCollisionTick                        = false;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
			m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;
			Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			m_uiAnimIdx = Gladiator::WIND_CUTTER3;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// GAIA_CRUSH
		if (m_uiAnimIdx == Gladiator::GAIA_CRUSH1 && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::GAIA_CRUSH2;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Gladiator::GAIA_CRUSH2 && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::GAIA_CRUSH3;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}

		// DRAW_SWORD
		if (m_uiAnimIdx == Gladiator::DRAW_SWORD_CHARGE && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::DRAW_SWORD_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Gladiator::DRAW_SWORD_LOOP && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::DRAW_SWORD;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Gladiator::DRAW_SWORD && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::DRAW_SWORD_END;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
	}


	// Skill Attack ==> ATTACK_WAIT
	if ((Gladiator::STINGER_BLADE == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Gladiator::CUTTING_SLASH == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Gladiator::JAW_BREAKER == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Gladiator::CUT_HEAD == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Gladiator::WIND_CUTTER3 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Gladiator::GAIA_CRUSH3 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Gladiator::DRAW_SWORD_END == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)))
	{
		m_bIsAttack    = false;
		m_bIsSkill     = false;
		m_bIsSkillLoop = false;
		m_pWeapon->Set_IsRenderTrail(false);
		m_uiComoboCnt = Gladiator::COMBOCNT_0;
		m_uiAnimIdx   = Gladiator::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCGladiator::KeyInput_Tumbling(const _float& fTimeDelta)
{
	_float fAngle = 0.0f;

	if (Engine::KEY_PRESSING(DIK_W) && Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_RB) && !m_bIsSkillLoop)
	{
		// 대각선 - 우 상단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_UP;
		}
		// 대각선 - 좌 상단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_UP;
		}
		// 직진.
		else
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + FRONT;
		}

		SetUp_WeaponLHand();

		m_bIsAttack  = true;
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);

		m_bIsSkill     = true;
		m_bIsSkillLoop = true;
		m_uiAnimIdx    = Gladiator::TUMBLING;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pTransCom->m_vAngle.y);
	}
	else if (Engine::KEY_PRESSING(DIK_S) && Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_RB) && !m_bIsSkillLoop)
	{
		// 대각선 - 우 하단.
		if (Engine::KEY_PRESSING(DIK_D))
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_DOWN;
		}
		// 대각선 - 좌 하단.
		else if (Engine::KEY_PRESSING(DIK_A))
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT_DOWN;
		}
		// 후진.
		else
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + BACK;
		}

		SetUp_WeaponLHand();

		m_bIsAttack  = true;
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);

		m_bIsSkill     = true;
		m_bIsSkillLoop = true;
		m_uiAnimIdx    = Gladiator::TUMBLING;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pTransCom->m_vAngle.y);
	}
	else if (Engine::KEY_PRESSING(DIK_A) && Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_RB) && !m_bIsSkillLoop)
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;

		SetUp_WeaponLHand();

		m_bIsAttack  = true;
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);

		m_bIsSkill     = true;
		m_bIsSkillLoop = true;
		m_uiAnimIdx    = Gladiator::TUMBLING;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pTransCom->m_vAngle.y);
	}
	else if (Engine::KEY_PRESSING(DIK_D) && Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_RB) && !m_bIsSkillLoop)
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;

		SetUp_WeaponLHand();

		m_bIsAttack  = true;
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);

		m_bIsSkill     = true;
		m_bIsSkillLoop = true;
		m_uiAnimIdx    = Gladiator::TUMBLING;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pTransCom->m_vAngle.y);
	}
	else if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_RB) && !m_bIsSkillLoop)
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + BACK;

		SetUp_WeaponLHand();
		m_bIsAttack  = true;
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);

		m_bIsSkill     = true;
		m_bIsSkillLoop = true;
		m_uiAnimIdx    = Gladiator::TUMBLING;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pTransCom->m_vAngle.y);
	}

	if ((Gladiator::TUMBLING == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)))
	{
		m_bIsAttack    = false;
		m_bIsSkill     = false;
		m_bIsSkillLoop = false;
		m_pWeapon->Set_IsRenderTrail(false);
		m_uiComoboCnt = Gladiator::COMBOCNT_0;
		m_uiAnimIdx   = Gladiator::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCGladiator::KeyInput_OpenShop(const char& npcNumber)
{
	g_bIsOpenShop = !g_bIsOpenShop;

	if (g_bIsOpenShop)
	{
		if (npcNumber == NPC_POPORI_MERCHANT || npcNumber == NPC_BARAKA_MERCHANT || npcNumber == NPC_BARAKA_MYSTELLIUM)
		{
			// NPC에 맞는 상점 리소스 생성
			cout << "상점 오픈" << endl;
		}		
	}
	else
	{
		cout << "상점 종료" << endl;
	}
}

void CPCGladiator::KeyInput_OpenQuest(const char& npcNumber)
{
	g_bIsOpenShop = !g_bIsOpenShop;

	if (g_bIsOpenShop)
	{
		if (npcNumber == NPC_CASTANIC_LSMITH)
		{
			// NPC에 맞는 상점 리소스 생성
			cout << "퀘스트창 오픈" << endl;
		}
	}
	else
	{
		cout << "퀘스트창 종료" << endl;
	}
}

void CPCGladiator::SetUp_AttackSetting()
{
	m_bIsSetCollisionTick                        = false;
	m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
	m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
	m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

	m_bIsAttack  = true;
	m_bIsKeyDown = false;
	m_pWeapon->Set_IsRenderTrail(false);
	m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
}

void CPCGladiator::SetUp_ComboAttackAnimation()
{
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB))
	{
		// ATTACK_WAIT ==> COMBO1
		if (Gladiator::COMBOCNT_0 == m_uiComoboCnt)
		{
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
			m_uiComoboCnt = Gladiator::COMBOCNT_1;
			m_uiAnimIdx   = Gladiator::COMBO1;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO1 ==> COMBO2
		else if (Gladiator::COMBOCNT_1 == m_uiComoboCnt &&
				 Gladiator::COMBO1 == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame >= m_ui3DMax_NumFrame * 0.85f)
		{
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
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
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
			m_uiComoboCnt = Gladiator::COMBOCNT_2;
			m_uiAnimIdx   = Gladiator::COMBO2;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}

		// COMBO2 ==> COMBO3
		else if (Gladiator::COMBOCNT_2 == m_uiComoboCnt &&
				 Gladiator::COMBO2 == m_uiAnimIdx &&
				 m_ui3DMax_CurFrame >= m_ui3DMax_NumFrame * 0.85f)
		{
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
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
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
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
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
			m_uiComoboCnt = Gladiator::COMBOCNT_4;
			m_uiAnimIdx   = Gladiator::COMBO4;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		// COMBO3R ==> COMBO4
		else if (Gladiator::COMBOCNT_3 == m_uiComoboCnt &&
				 Gladiator::COMBO3R == m_uiAnimIdx && 
				 m_ui3DMax_CurFrame <= m_ui3DMax_NumFrame * 0.65f)
		{
			SetUp_WeaponLHand();
			SetUp_AttackSetting();
			m_uiComoboCnt = Gladiator::COMBOCNT_4;
			m_uiAnimIdx   = Gladiator::COMBO4;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
	}
}

void CPCGladiator::SetUp_FromComboAttackToAttackWait(const _float& fTimeDelta)
{
	if (Gladiator::COMBO1 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
	{
		SetUp_WeaponLHand();
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);
		m_uiAnimIdx  = Gladiator::COMBO1R;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
	else if (Gladiator::COMBO2 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
	{
		SetUp_WeaponLHand();
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);
		m_uiAnimIdx  = Gladiator::COMBO2R;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
	else if (Gladiator::COMBO3 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
	{
		SetUp_WeaponLHand();
		m_bIsKeyDown = false;
		m_pWeapon->Set_IsRenderTrail(false);
		m_uiAnimIdx  = Gladiator::COMBO3R;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}

	else if ((Gladiator::COMBO1R == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
			 (Gladiator::COMBO2R == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
			 (Gladiator::COMBO3R == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
			 (Gladiator::COMBO4 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)))
	{
		SetUp_WeaponLHand();
		m_bIsAttack   = false;
		m_pWeapon->Set_IsRenderTrail(false);
		m_uiComoboCnt = Gladiator::COMBOCNT_0;
		m_uiAnimIdx   = Gladiator::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCGladiator::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsKeyDown || Gladiator::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
															  &m_pTransCom->m_vDir,
															  m_pInfoCom->m_fSpeed * fTimeDelta);
	}
}

void CPCGladiator::AttackMove_OnNaviMesh(const _float& fTimeDelta)
{
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

void CPCGladiator::Is_ChangeWeapon()
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

		if (Gladiator::STANCE_ATTACK == m_eStance)
		{
			SetUp_WeaponLHand();
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
		}
		else if (Gladiator::STANCE_NONEATTACK == m_eStance)
		{
			SetUp_WeaponBack();
			m_pWeapon->Set_DissolveInterpolation(1.0f);
			m_pWeapon->Set_IsRenderShadow(false);
		}

		m_chPreWeaponType = m_chCurWeaponType;
	}
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
	if (Gladiator::STANCE_NONEATTACK == m_eStance)
	{
		if (Gladiator::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Gladiator::NONE_ATTACK_WALK == m_uiAnimIdx)
		{
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
			SetUp_WeaponLHand();
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
			m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::ATTACK_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;

		}

		// ATTACK -> NONE_ATTACK
		else if (Gladiator::STANCE_NONEATTACK == m_eStance  &&
				 Gladiator::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;
		}

		if (Gladiator::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
			SetUp_WeaponBack();

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
	m_tAngleInterpolationDesc.interpolation_speed    = 5.0f;
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

void CPCGladiator::SetUp_AttackMove(const _uint& uiComboCnt,
									const _uint& uiAniIdx, 
									const _uint& uiStartTick,
									const _uint& uiStopTick,
									const _float& fMoveSpeed,
									const _float& fStopSpeed)
{
	if (uiComboCnt == m_uiComoboCnt && uiAniIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
	{
		// Move On
		if (m_ui3DMax_CurFrame >= uiStartTick && m_ui3DMax_CurFrame < uiStopTick)
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fMoveSpeed;

		// Move Off
		else
			m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = fStopSpeed;
	}
}

void CPCGladiator::SetUp_AttackMove(const _uint& uiAniIdx, 
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

void CPCGladiator::SetUp_AttackTrail(const _uint& uiComboCnt,
									 const _uint& uiAniIdx,
									 const _uint& uiStartTick,
									 const _uint& uiStopTick)
{
	if (uiComboCnt == m_uiComoboCnt && uiAniIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
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

void CPCGladiator::SetUp_AttackTrail(const _uint& uiAniIdx, const _uint& uiStartTick, const _uint& uiStopTick)
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

void CPCGladiator::SetUp_WeaponRHand()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 180.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 0.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("R_Sword")));
}

void CPCGladiator::SetUp_WeaponLHand()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));
}

void CPCGladiator::SetUp_WeaponBack()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

void CPCGladiator::SetUp_AttackAfterImage(const _uint& uiAnimIdx,
										  const _uint& uiStartTick,
										  const _uint& uiStopTick,
										  const _float& fMakeTime,
										  const _float& fAlphaSpeed)
{
	if (uiAnimIdx == m_uiAnimIdx && m_pMeshCom->Is_BlendingComplete())
	{
		if (m_ui3DMax_CurFrame >= uiStartTick && m_ui3DMax_CurFrame < uiStopTick)
		{
			if (!m_bIsMakeAfterImage)
				m_fAfterImgTime = fMakeTime;

			m_bIsMakeAfterImage = true;
			m_fAfterImgMakeTime = fMakeTime;
			m_fAfterSubAlpha    = fAlphaSpeed;
			m_pMeshCom->Set_AfterImgMakeTime(m_fAfterImgMakeTime);
			m_pMeshCom->Set_AfterImgSubAlpha(m_fAfterSubAlpha);
		}
		else if (m_ui3DMax_CurFrame >= uiStopTick)
		{
			m_bIsMakeAfterImage = false;
		}
	}
}

void CPCGladiator::Make_AfterImage(const _float& fTimeDelta)
{
	if (m_bIsMakeAfterImage)
	{
		m_fAfterImgTime += fTimeDelta;
		if (m_fAfterImgTime > m_fAfterImgMakeTime)
		{
			if (m_lstAFWorldMatrix.size() < m_uiAfterImgSize)
			{
				m_lstAFWorldMatrix.emplace_back(m_pTransCom->m_matWorld);
				m_lstAFAlpha.emplace_back(_rgba(0.0f, 0.0f, 0.0f, 1.f));
				m_pMeshCom->Set_AfterImgTime(m_fAfterImgTime);
			}

			m_fAfterImgTime = 0.f;
		}
	}

	if (m_lstAFWorldMatrix.size())
	{
		for (list<_rgba>::iterator& iterFade = m_lstAFAlpha.begin(); iterFade != m_lstAFAlpha.end();)
		{
			(*iterFade).w -= m_fAfterSubAlpha * fTimeDelta;
			//(*iterFade).x -= fTimeDelta;
			//(*iterFade).y -= fTimeDelta;

			if (0 > (*iterFade).w)
			{
				m_lstAFWorldMatrix.pop_front();
				iterFade = m_lstAFAlpha.erase(iterFade);
				continue;
			}
			else
				++iterFade;
		}
	}
}

void CPCGladiator::SetUp_CollisionTick(const _float& fTimeDelta)
{
	// COMBO ATTACK
	if (Gladiator::COMBO1 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::COMBO1_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;
			m_tCollisionTickDesc.fPosOffset              = 1.75f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (Gladiator::COMBO2 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::COMBO2_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;
			m_tCollisionTickDesc.fPosOffset              = 1.75f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (Gladiator::COMBO3 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::COMBO3_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;
			m_tCollisionTickDesc.fPosOffset              = 1.5f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (Gladiator::COMBO4 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::COMBO4_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;
			m_tCollisionTickDesc.fPosOffset              = 2.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 4.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 2;
		}
	}

	// SKILL ATTACK
	else if (Gladiator::STINGER_BLADE == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::STINGER_BLADE_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;
			m_tCollisionTickDesc.fPosOffset              = 3.0f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 8.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 3;
		}
	}
	else if (Gladiator::CUTTING_SLASH == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::CUTTING_SLASH_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick = true;
			m_tCollisionTickDesc.fPosOffset              = 2.5f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (Gladiator::JAW_BREAKER == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::JAW_BREAKER_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick                        = true;
			m_tCollisionTickDesc.fPosOffset              = 2.f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 0.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 1;
		}
	}
	else if (Gladiator::WIND_CUTTER1 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::WIND_CUTTER1_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick                        = true;
			m_tCollisionTickDesc.fPosOffset              = 2.f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 9.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 3;
		}
	}
	else if (Gladiator::WIND_CUTTER2 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::WIND_CUTTER2_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick                        = true;
			m_tCollisionTickDesc.fPosOffset              = 2.f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 9.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 3;
		}
	}
	else if (Gladiator::WIND_CUTTER3 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::WIND_CUTTER3_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick                        = true;
			m_tCollisionTickDesc.fPosOffset              = 2.f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 9.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 3;
		}
	}
	else if (Gladiator::GAIA_CRUSH2 == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::GAIA_CRUSH2_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick                        = true;
			m_tCollisionTickDesc.fPosOffset              = 2.f;
			m_tCollisionTickDesc.fScaleOffset            = 1.5f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 10.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 6;
		}
	}
	else if (Gladiator::DRAW_SWORD == m_uiAnimIdx && m_ui3DMax_CurFrame >= Gladiator::DRAW_SWORD_COLLISIONTICK_START)
	{
		if (!m_bIsSetCollisionTick)
		{
			m_bIsSetCollisionTick                        = true;
			m_tCollisionTickDesc.fPosOffset              = 2.f;
			m_tCollisionTickDesc.fScaleOffset            = 1.5f;
			m_tCollisionTickDesc.bIsCreateCollisionTick  = true;
			m_tCollisionTickDesc.fColisionTickUpdateTime = 1.0f / 10.0f;
			m_tCollisionTickDesc.fCollisionTickTime      = m_tCollisionTickDesc.fColisionTickUpdateTime;
			m_tCollisionTickDesc.iCurCollisionTick       = 0;
			m_tCollisionTickDesc.iMaxCollisionTick       = 9;
		}
	}

	// Create CollisionTick
	if (m_bIsSetCollisionTick && 
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
				pCollisionTick->Set_CollisionTag(L"CollisionTick_ThisPlayer");
				pCollisionTick->Set_Damage(m_pInfoCom->Get_RandomDamage());
				pCollisionTick->Set_LifeTime(0.25f);
				pCollisionTick->Get_Transform()->m_vScale = _vec3(4.0f) * m_tCollisionTickDesc.fScaleOffset;
				pCollisionTick->Get_Transform()->m_vPos   = vPos;
				pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);
				m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_ThisPlayer", pCollisionTick);
			}
		}
	}
}

void CPCGladiator::Collision_MonsterMultiCollider(list<Engine::CColliderSphere*>& lstMonsterCollider)
{
	for (auto& pSrcCollider : m_lstCollider)
	{
		for (auto& pDstCollider : lstMonsterCollider)
		{
			if (Engine::CCollisionMgr::Check_Sphere(pSrcCollider->Get_BoundingInfo(), pDstCollider->Get_BoundingInfo()))
			{
				// Process Collision Event
				pSrcCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDstCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

			}
		}
	}
}

void CPCGladiator::Collision_PortalVelikaToBeach(list<Engine::CColliderSphere*>& lstPortalCollider)
{
	for (auto& pSrcCollider : m_lstCollider)
	{
		for (auto& pDstCollider : lstPortalCollider)
		{
			if (Engine::CCollisionMgr::Check_Sphere(pSrcCollider->Get_BoundingInfo(), pDstCollider->Get_BoundingInfo()))
			{
				// Process Collision Event
				pSrcCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDstCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

				g_bIsStageChange = true;
				m_bIsKeyDown = false;

				if (Gladiator::STANCE_ATTACK == m_eStance)
					m_uiAnimIdx = Gladiator::ATTACK_WAIT;
				else
					m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;

				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// FadeInOut
				Engine::CGameObject* pGameObject = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
				static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_BEACH);
				m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
			}
		}
	}
}

void CPCGladiator::Collision_PortalBeachToVelika(list<Engine::CColliderSphere*>& lstPortalCollider)
{
	for (auto& pSrcCollider : m_lstCollider)
	{
		for (auto& pDstCollider : lstPortalCollider)
		{
			if (Engine::CCollisionMgr::Check_Sphere(pSrcCollider->Get_BoundingInfo(), pDstCollider->Get_BoundingInfo()))
			{
				// Process Collision Event
				pSrcCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDstCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				
				g_bIsStageChange = true;
				m_bIsKeyDown = false;

				if (Gladiator::STANCE_ATTACK == m_eStance)
					m_uiAnimIdx = Gladiator::ATTACK_WAIT;
				else
					m_uiAnimIdx = Gladiator::NONE_ATTACK_IDLE;

				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// FadeInOut
				Engine::CGameObject* pGameObject = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
				static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_VELIKA);
				m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
			}
		}
	}
}

void CPCGladiator::Collision_Merchant(list<Engine::CColliderSphere*>& lstMerchantCollider, int npcServerNumber)
{
	for (auto& pSrcCollider : m_lstCollider)
	{
		for (auto& pDstCollider : lstMerchantCollider)
		{
			if (Engine::CCollisionMgr::Check_Sphere(pSrcCollider->Get_BoundingInfo(), pDstCollider->Get_BoundingInfo()))
			{
				// Process Collision Event
				pSrcCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDstCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
					
				CNPC_Merchant* pObj = static_cast<CNPC_Merchant*>(m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"NPC", npcServerNumber));
				pObj->Set_State(Baraka_M_Merchant::A_TALK);
		
				/* Shop Open */
				if (Engine::KEY_DOWN(DIK_F))
				{				
					KeyInput_OpenShop(pObj->Get_NPCNumber());
				}
			}
		}
	}
}

void CPCGladiator::Collision_Quest(list<Engine::CColliderSphere*>& lstMerchantCollider, int npcServerNumber)
{
	for (auto& pSrcCollider : m_lstCollider)
	{
		for (auto& pDstCollider : lstMerchantCollider)
		{
			if (Engine::CCollisionMgr::Check_Sphere(pSrcCollider->Get_BoundingInfo(), pDstCollider->Get_BoundingInfo()))
			{
				// Process Collision Event
				pSrcCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
				pDstCollider->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));

				CNPC_Quest* pObj = static_cast<CNPC_Quest*>(m_pObjectMgr->Get_ServerObject(L"Layer_GameObject", L"NPC", npcServerNumber));
				if (pObj == nullptr) return;

				pObj->Set_State(Castanic_M_Lsmith::A_TALK);

				/* Shop Open */
				if (Engine::KEY_DOWN(DIK_F))
				{
					KeyInput_OpenQuest(pObj->Get_NPCNumber());
				}
			}
		}
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
	Engine::Safe_Release(m_pWeapon);

	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pDynamicCamera);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pNaviMeshCom);
	Engine::Safe_Release(m_pVelikaNaviMeshCom);
	Engine::Safe_Release(m_pBeachNaviMeshCom);
	Engine::Safe_Release(m_pFont);
}
