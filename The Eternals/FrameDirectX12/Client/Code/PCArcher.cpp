#include "stdafx.h"
#include "PCArcher.h"
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
#include "PCWeaponBow.h"
#include "CollisionArrow.h"
#include "LightingParticle.h"
#include "MagicCircle.h"
#include "SnowParticle.h"
#include "DistTrail.h"
#include "TextureEffect.h"
#include "PartySuggestCanvas.h"
#include "StoreMgr.h"
#include "MainMenuInventory.h"

CPCArcher::CPCArcher(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
	, m_pMouserMgr(CMouseCursorMgr::Get_Instance())
	, m_pPartySystemMgr(CPartySystemMgr::Get_Instance())
	, m_pQuickSlotMgr(CQuickSlotMgr::Get_Instance())
{
}

HRESULT CPCArcher::Ready_GameObject(wstring wstrMeshTag, 
									wstring wstrNaviMeshTag,
									const _vec3& vScale, 
									const _vec3& vAngle,
									const _vec3& vPos,
									const char& chWeaponType)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag, wstrNaviMeshTag), E_FAIL);
	m_wstrMeshTag         = wstrMeshTag;
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

	m_pInfoCom->m_fSpeed     = Archer::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_eKeyState			= MVKEY::K_END;
	m_bIsKeyDown		= false;
	m_bIsSameDir		= false;
	m_bIsSendMoveStop	= true;
	m_fPreAngle			= m_pTransCom->m_vAngle.y;

	/*__________________________________________________________________________________________________________
	[ 선형보간 설정 ]
	____________________________________________________________________________________________________________*/
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Archer::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Archer::MAX_SPEED;

	m_tAttackMoveSpeedInterpolationDesc.linear_ratio        = 0.0f;
	m_tAttackMoveSpeedInterpolationDesc.v1                  = Archer::MIN_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.v2                  = Archer::MAX_ATTACK_SPEED;
	m_tAttackMoveSpeedInterpolationDesc.interpolation_speed = 0.0f;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_eStance   = Archer::STANCE_NONEATTACK;
	m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;

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

HRESULT CPCArcher::LateInit_GameObject()
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

	// UI Equipment
	Engine::FAILED_CHECK_RETURN(SetUp_Equipment(), E_FAIL);

	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(0), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Skill KeyInput ]
	____________________________________________________________________________________________________________*/
	vector<CQuickSlot*> vecQuickSlot = m_pQuickSlotMgr->Get_QuickSlotList();

	m_mapSkillKeyInput[L"RAPID_SHOT"]    = -1;
	m_mapSkillKeyInput[L"ARROW_SHOWER"]  = -1;
	m_mapSkillKeyInput[L"ESCAPING_BOMB"] = -1;
	m_mapSkillKeyInput[L"ARROW_FALL"]    = -1;
	m_mapSkillKeyInput[L"CHARGE_ARROW"]  = -1;
	m_mapSkillKeyInput[L"HP_POTION"]     = -1;
	m_mapSkillKeyInput[L"MP_POTION"]     = -1;

	vecQuickSlot[0]->Set_CurQuickSlotName(QUCKSLOT_SKILL_RAPID_SHOT);
	vecQuickSlot[1]->Set_CurQuickSlotName(QUCKSLOT_SKILL_ARROW_SHOWER);
	vecQuickSlot[2]->Set_CurQuickSlotName(QUCKSLOT_SKILL_ESCAPING_BOMB);
	vecQuickSlot[3]->Set_CurQuickSlotName(QUCKSLOT_SKILL_ARROW_FALL);
	vecQuickSlot[4]->Set_CurQuickSlotName(QUCKSLOT_SKILL_CHARGE_ARROW);
	vecQuickSlot[8]->Set_CurQuickSlotName(QUCKSLOT_POTION_HP);
	vecQuickSlot[9]->Set_CurQuickSlotName(QUCKSLOT_POTION_MP);

	return S_OK;
}

_int CPCArcher::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	Set_HpMPGauge();
	Is_ChangeWeapon();
	SetUp_StageID();

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

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	Key_Input(fTimeDelta);

	if (g_bIsStageChange)
		m_bIsKeyDown = false;

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
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MINIMAP, this), -1);

	/*__________________________________________________________________________________________________________
	[ Collision - Add Collision List ]
	____________________________________________________________________________________________________________*/
	m_pCollisonMgr->Add_CollisionCheckList(this);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	//// AfterImage
	//Make_AfterImage(fTimeDelta);

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->Update_GameObject(fTimeDelta);

	// Create CollisionTick
	if (m_pMeshCom->Is_BlendingComplete())
		SetUp_CollisionArrow(fTimeDelta);

	// SetUp_CameraEffect(fTimeDelta);

	return NO_EVENT;
}

_int CPCArcher::LateUpdate_GameObject(const _float& fTimeDelta)
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

void CPCArcher::Process_Collision()
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

	Suggest_PartyToOthers();
	Leave_PartyThisPlayer();
}

void CPCArcher::Send_PacketToServer()
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

void CPCArcher::Render_GameObject(const _float& fTimeDelta)
{
	// Render AfterImage
	if (m_uiAfterImgSize)
	{
		m_pShaderCom->Set_PipelineStatePass(5);
		Render_AfterImage(fTimeDelta);
	}
}

void CPCArcher::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTableMiniMap();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiMiniMapTexIdx, 
								   Engine::MATRIXID::TOP_VIEW);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CPCArcher::Render_AfterImage(const _float& fTimeDelta)
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
		tCB_ShaderMesh.vEmissiveColor = _rgba(1.0f, 1.0f, 1.0f, 1.0f);
		if (Alpha_begin != Alpha_end)
			Alpha_begin++;

		m_pShaderCom->Get_UploadBuffer_AFShaderMesh()->CopyData(i, tCB_ShaderMesh);

		// Render Buffer
		m_pMeshCom->Render_DynamicMeshAfterImage(m_pShaderCom, i);
	}
}

void CPCArcher::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	m_pShaderCom->Set_PipelineStatePass(0);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCArcher::Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPCArcher::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

	m_pWinterNaviMeshCom = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"StageWinter_NaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pWinterNaviMeshCom, E_FAIL);

	return S_OK;
}

HRESULT CPCArcher::SetUp_PCWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		m_pWeapon = static_cast<CPCWeaponBow*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponBow(m_chCurWeaponType)));
		m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
		m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
		m_pWeapon->Update_GameObject(0.016f);

		m_chPreWeaponType = m_chCurWeaponType;
	}

	return S_OK;
}

HRESULT CPCArcher::SetUp_ClassFrame()
{
	/*__________________________________________________________________________________________________________
	[ CharacterClassFrame ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUICharacterClassFrameArcher.2DUI" };
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
					m_pHpGauge = static_cast<CCharacterHpGauge*>(pChildUI);
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
					m_pMpGauge = static_cast<CCharacterMpGauge*>(pChildUI);
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

HRESULT CPCArcher::SetUp_Equipment()
{
	CEquipmentButtonClose* pButtonXMouseNormal  = nullptr;
	CEquipmentButtonClose* pButtonXMouseOn      = nullptr;
	CEquipmentButtonClose* pButtonXMouseClicked = nullptr;
	{
		wifstream fin{ L"../../Bin/ToolData/2DUIEquipmentArcher.2DUI" };
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
			pRootUI = CEquipmentCanvas::Create(m_pGraphicDevice, m_pCommandList,
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
			CInventoryEquipmentMgr::Get_Instance()->Set_EquipmentCanvasClass(static_cast<CEquipmentCanvas*>(pRootUI));

			// UIChild 생성.
			for (_int i = 0; i < iChildUISize; ++i)
			{
				Engine::CGameObject* pChildUI = nullptr;

				if (L"EquipmentHelmet" == vecObjectTag[i] ||
					L"EquipmentArmor" == vecObjectTag[i] || 
					L"EquipmentShoes" == vecObjectTag[i] || 
					L"EquipmentWeapon" == vecObjectTag[i])
				{
					pChildUI = CEquipmentItemSlot::Create(m_pGraphicDevice, m_pCommandList,
														  wstrRootObjectTag,				// RootObjectTag
														  vecObjectTag[i],					// ObjectTag
														  vecDataFilePath[i],				// DataFilePath
														  vecPos[i],						// Pos
														  vecScale[i],						// Scane
														  (_bool)vecIsSpriteAnimation[i],	// Is Animation
														  vecFrameSpeed[i],					// FrameSpeed
														  vecRectPosOffset[i],				// RectPosOffset
														  vecRectScale[i],					// RectScaleOffset
														  vecUIDepth[i]);					// UI Depth

					CInventoryEquipmentMgr::Get_Instance()->Add_EquipmentSlot(vecObjectTag[i], static_cast<CEquipmentItemSlot*>(pChildUI));
				}
				else if (L"EquipmentButtonCloseNormal" == vecObjectTag[i] ||
						 L"EquipmentButtonCloseMouseOn" == vecObjectTag[i] ||
						 L"EquipmentButtonCloseMouseClicked" == vecObjectTag[i])
				{
					pChildUI = CEquipmentButtonClose::Create(m_pGraphicDevice, m_pCommandList,
															 wstrRootObjectTag,					// RootObjectTag
															 vecObjectTag[i],					// ObjectTag
															 vecDataFilePath[i],				// DataFilePath
															 vecPos[i],							// Pos
															 vecScale[i],						// Scane
															 (_bool)vecIsSpriteAnimation[i],	// Is Animation
															 vecFrameSpeed[i],					// FrameSpeed
															 vecRectPosOffset[i],				// RectPosOffset
															 vecRectScale[i],					// RectScaleOffset
															 vecUIDepth[i]);					// UI Depth

					if (L"EquipmentButtonCloseMouseOn" == vecObjectTag[i])
						pButtonXMouseOn = static_cast<CEquipmentButtonClose*>(pChildUI);
					else if (L"EquipmentButtonCloseMouseClicked" == vecObjectTag[i])
						pButtonXMouseClicked = static_cast<CEquipmentButtonClose*>(pChildUI);
					else
						pButtonXMouseNormal = static_cast<CEquipmentButtonClose*>(pChildUI);

				}

				if (nullptr != pChildUI &&
					(L"EquipmentButtonCloseMouseOn" != vecObjectTag[i] &&
					 L"EquipmentButtonCloseMouseClicked" != vecObjectTag[i]))
				{
					m_pObjectMgr->Add_GameObject(L"Layer_UI", vecObjectTag[i], pChildUI);
					static_cast<CGameUIRoot*>(pRootUI)->Add_ChildUI(pChildUI);
				}
			}
		}

		UI_CHILD_STATE tState;

		tState.tFrame         = pButtonXMouseOn->Get_Frame();
		tState.vPos           = pButtonXMouseOn->Get_Transform()->m_vPos;
		tState.vScale         = pButtonXMouseOn->Get_Transform()->m_vScale;
		tState.vRectPosOffset = pButtonXMouseOn->Get_RectOffset();
		tState.vRectScale     = pButtonXMouseOn->Get_TransformColor()->m_vScale;
		pButtonXMouseNormal->SetUp_MainMenuState(L"MouseOn", tState);

		tState.tFrame         = pButtonXMouseClicked->Get_Frame();
		tState.vPos           = pButtonXMouseClicked->Get_Transform()->m_vPos;
		tState.vScale         = pButtonXMouseClicked->Get_Transform()->m_vScale;
		tState.vRectPosOffset = pButtonXMouseClicked->Get_RectOffset();
		tState.vRectScale     = pButtonXMouseClicked->Get_TransformColor()->m_vScale;
		pButtonXMouseNormal->SetUp_MainMenuState(L"MouseClicked", tState);

		pButtonXMouseNormal = nullptr;
		Engine::Safe_Release(pButtonXMouseOn);
		Engine::Safe_Release(pButtonXMouseClicked);
	}

	CInventoryEquipmentMgr::Get_Instance()->Set_ThisPlayerJob(m_chO_Type);

	return S_OK;
}

void CPCArcher::SetUp_StageID()
{
	if (m_chCurStageID != m_chPreStageID)
	{
		if (STAGE_VELIKA == m_chCurStageID)
		{
			Engine::CShaderMgr::Get_Instance()->Set_TreeAlphaTest(1.0f);

			m_pNaviMeshCom = m_pVelikaNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			// m_pTransCom->m_vPos = _vec3(STAGE_VELIKA_X, 0.0f, STAGE_VELIKA_Z);
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}
		else if (STAGE_BEACH == m_chCurStageID)
		{
			Engine::CShaderMgr::Get_Instance()->Set_TreeAlphaTest(1.0f);

			m_pNaviMeshCom = m_pBeachNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			// m_pTransCom->m_vPos = _vec3(STAGE_BEACH_X, 0.0f, STAGE_BEACH_Z);
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}
		else if (STAGE_WINTER == m_chCurStageID)
		{
			m_pNaviMeshCom = m_pWinterNaviMeshCom;
			m_mapComponent[Engine::ID_DYNAMIC][L"Com_NaviMesh"] = m_pNaviMeshCom;
			// m_pTransCom->m_vPos = _vec3(STAGE_WINTER_X, 0.0f, STAGE_WINTER_X);
			m_pNaviMeshCom->Set_CurrentCellIndex(m_pNaviMeshCom->Get_CurrentPositionCellIndex(m_pTransCom->m_vPos));
		}

		m_chPreStageID = m_chCurStageID;
	}
}

void CPCArcher::Set_ConstantTable()
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
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.25f, 1.25f, 1.25f, 0.0f);

	m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CPCArcher::Set_ConstantTableShadowDepth()
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

void CPCArcher::Set_ConstantTableMiniMap()
{
	m_pTransMiniMap->m_vPos.x = m_pTransCom->m_vPos.x;
	m_pTransMiniMap->m_vPos.y = 1.1f;
	m_pTransMiniMap->m_vPos.z = m_pTransCom->m_vPos.z;
	m_pTransMiniMap->m_vPos.y = 0.5f;
	m_pTransMiniMap->m_vAngle = _vec3(90.0f, m_pDynamicCamera->Get_Transform()->m_vAngle.y, 0.0f);
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

void CPCArcher::Set_IsRepeatAnimation()
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

void CPCArcher::Set_AnimationSpeed()
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
		m_fAnimationSpeed = TPS * 2.25f;
	}
	else
		m_fAnimationSpeed = TPS;
}

void CPCArcher::Set_BlendingSpeed()
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

void CPCArcher::Set_HpMPGauge()
{
	if (nullptr != m_pHpGauge && nullptr != m_pMpGauge)
	{
		m_pHpGauge->Set_Percent((_float)m_pInfoCom->m_iHp / (_float)m_pInfoCom->m_iMaxHp, m_pInfoCom->m_iHp, m_pInfoCom->m_iMaxHp);
		m_pMpGauge->Set_Percent((_float)m_pInfoCom->m_iMp / (_float)m_pInfoCom->m_iMaxMp, m_pInfoCom->m_iMp, m_pInfoCom->m_iMaxMp);
	}
}

void CPCArcher::Key_Input(const _float& fTimeDelta)
{
	if (!g_bIsActive) return;

	KeyInput_Move(fTimeDelta);
	KeyInput_Attack(fTimeDelta);

	// StageChange Stage WINTER
	if (Engine::KEY_DOWN(DIK_O) && NO_EVENT_STATE)
	{
		g_bIsStageChange = true;
		m_bIsKeyDown     = false;

		if (Archer::STANCE_ATTACK == m_eStance)
			m_uiAnimIdx = Archer::ATTACK_WAIT;
		else
			m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;

		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
		if (nullptr != pGameObject)
		{
			static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
			static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_WINTER);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
		}
	}

	// StageChange Stage VELIKA
	if (Engine::KEY_DOWN(DIK_P) && NO_EVENT_STATE)
	{
		g_bIsStageChange = true;
		m_bIsKeyDown     = false;

		if (Archer::STANCE_ATTACK == m_eStance)
			m_uiAnimIdx = Archer::ATTACK_WAIT;
		else
			m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;

		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

		Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
		if (nullptr != pGameObject)
		{
			static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
			static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_VELIKA);
			m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
		}
	}
}

void CPCArcher::KeyInput_Move(const _float& fTimeDelta)
{
	if (!g_bIsActive || m_bIsAttack || !m_bIsCompleteStanceChange || m_bIsSkillLoop)
		return;

	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	if (Engine::KEY_PRESSING(DIK_W) && NO_EVENT_STATE)
	{
		// 대각선 - 우 상단.
		if (Engine::KEY_PRESSING(DIK_D) && NO_EVENT_STATE)
		{
			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_UP;
			m_eKeyState             = MVKEY::K_RIGHT_UP;			
		}
		// 대각선 - 좌 상단.
		else if (Engine::KEY_PRESSING(DIK_A) && NO_EVENT_STATE)
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
	}

	else if (Engine::KEY_PRESSING(DIK_S) && NO_EVENT_STATE)
	{
		// 대각선 - 우 하단.
		if (Engine::KEY_PRESSING(DIK_D) && NO_EVENT_STATE)
		{

			m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT_DOWN;
			m_eKeyState             = MVKEY::K_RIGHT_DOWN;
		}
		// 대각선 - 좌 하단.
		else if (Engine::KEY_PRESSING(DIK_A) && NO_EVENT_STATE)
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
	}
	// 좌로 이동.
	else if (Engine::KEY_PRESSING(DIK_A) && NO_EVENT_STATE)
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_LEFT;
		m_bIsKeyDown            = true;
		m_bIsSendMoveStop		= false;
	}
	// 우로 이동.	
	else if (Engine::KEY_PRESSING(DIK_D) && NO_EVENT_STATE)
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + RIGHT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_RIGHT;
		m_bIsKeyDown            = true;
		m_bIsSendMoveStop		= false;
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
				 Archer::MIN_SPEED == m_pInfoCom->m_fSpeed)
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

void CPCArcher::KeyInput_Attack(const _float& fTimeDelta)
{
	if (nullptr == m_pWeapon)
		return;

	KeyInput_StanceChange(fTimeDelta);

	if (Archer::STANCE_ATTACK == m_eStance && m_bIsCompleteStanceChange)
	{
		if (m_pMeshCom->Is_BlendingComplete())
		{
			KeyInput_AttackArrow(fTimeDelta);
			KeyInput_SkillAttack(fTimeDelta);
			KeyInput_BackDash(fTimeDelta);
		}

		if (m_bIsAttack)
		{
			// SkillAttack Move
			SetUp_AttackMove(Archer::BACK_DASH, Archer::BACK_DASH_MOVE_START, Archer::BACK_DASH_MOVE_STOP, 8.0f, -5.0f);
			SetUp_AttackMove(Archer::ESCAPING_BOMB, Archer::ESCAPING_BOMB_MOVE_START, Archer::ESCAPING_BOMB_MOVE_STOP, 10.0f, -5.0f);
			SetUp_AttackMove(Archer::CHARGE_ARROW_SHOT, Archer::CHARGE_ARROW_MOVE_START, Archer::CHARGE_ARROW_MOVE_STOP, 4.0f, -5.0f);

			AttackMove_OnNaviMesh(fTimeDelta);
		}
	}
}

void CPCArcher::KeyInput_StanceChange(const _float& fTimeDelta)
{
	if (m_bIsAttack)
		return;

	// ATTACK -> NONE_ATTACK
	if (Engine::KEY_DOWN(DIK_LSHIFT) && Archer::STANCE_ATTACK == m_eStance &&
		!m_bIsKeyDown && m_pInfoCom->m_fSpeed == Archer::MIN_SPEED &&
		m_bIsCompleteStanceChange &&
		m_uiAnimIdx != Archer::NONE_ATTACK_WALK && m_uiAnimIdx != Archer::ATTACK_RUN &&
		m_uiAnimIdx != Archer::IN_WEAPON && m_uiAnimIdx != Archer::OUT_WEAPON && 
		NO_EVENT_STATE)
	{
		m_bIsCompleteStanceChange = false;
		SetUp_PlayerStance_FromAttackToNoneAttack();

		// Send Packet.
		m_pPacketMgr->send_stance_change(m_uiAnimIdx, false);
	}

	// NONE_ATTACK -> ATTACK
	if ((Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"RAPID_SHOT"]) ||
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"ESCAPING_BOMB"]) ||
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"ARROW_SHOWER"]) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"ARROW_FALL"]) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"CHARGE_ARROW"])) &&
		Archer::STANCE_NONEATTACK == m_eStance &&
		m_bIsCompleteStanceChange &&
		m_uiAnimIdx != Archer::IN_WEAPON && m_uiAnimIdx != Archer::OUT_WEAPON && 
		NO_EVENT_STATE)
	{
		m_bIsKeyDown = false;
		m_bIsCompleteStanceChange = false;
		SetUp_PlayerStance_FromNoneAttackToAttack();

		// Send Packet.
		m_pPacketMgr->send_stance_change(m_uiAnimIdx, true);
	}

	Change_PlayerStance(fTimeDelta);
}

void CPCArcher::KeyInput_AttackArrow(const _float& fTimeDelta)
{
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) && !m_bIsSkill && !m_bIsSkillLoop &&
		m_uiAnimIdx != Archer::ATTACK_ARROW && 
		NO_EVENT_STATE)
	{
		SetUp_WeaponLHand();
		SetUp_AttackSetting();
		m_uiAnimIdx = Archer::ATTACK_ARROW;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
	}

	if (Archer::ATTACK_ARROW == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
	{
		SetUp_WeaponLHand();
		m_bIsAttack   = false;
		m_uiAnimIdx   = Archer::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCArcher::KeyInput_SkillAttack(const _float& fTimeDelta)
{
	if (!m_bIsSkillLoop)
	{
		if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"RAPID_SHOT"]) && 
			m_uiAnimIdx != Archer::RAPID_SHOT1 && 
			NO_EVENT_STATE)
		{
			SetUp_AttackSetting();
			m_bIsSkill  = true;
			//m_bIsSkillLoop = true;
			m_uiAnimIdx = Archer::RAPID_SHOT1;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"ESCAPING_BOMB"]) &&
				 m_uiAnimIdx != Archer::ESCAPING_BOMB && 
				 NO_EVENT_STATE)
		{
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Archer::ESCAPING_BOMB;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"ARROW_SHOWER"]) &&
				 m_uiAnimIdx != Archer::ARROW_SHOWER_START && 
				 NO_EVENT_STATE)
		{
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Archer::ARROW_SHOWER_START;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"ARROW_FALL"]) &&
				 m_uiAnimIdx != Archer::ARROW_FALL_START && 
				 NO_EVENT_STATE)
		{
			m_pDynamicCamera->Set_CameraState(CAMERA_STATE::ARCHER_ARROW_FALL);
			m_pDynamicCamera->SetUp_ThirdPersonViewOriginData();
			m_pDynamicCamera->Set_FovY(50.0f);
			m_pDynamicCamera->Set_CameraAtParentMatrix(m_pMeshCom->Find_SkinningMatrix("Bip01-Neck"));

			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Archer::ARROW_FALL_START;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
			

		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"CHARGE_ARROW"]) &&
				 m_uiAnimIdx != Archer::CHARGE_ARROW_START && 
				 NO_EVENT_STATE)
		{
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Archer::CHARGE_ARROW_START;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
	}
	else
	{
		// ARROW_SHOWER
		if (m_uiAnimIdx == Archer::ARROW_SHOWER_START && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::ARROW_SHOWER_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Archer::ARROW_SHOWER_LOOP && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::ARROW_SHOWER_SHOT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		// ARROW_FALL
		else if (m_uiAnimIdx == Archer::ARROW_FALL_START && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::ARROW_FALL_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Archer::ARROW_FALL_LOOP && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::ARROW_FALL_SHOT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		} 
		// CHARGE_ARROW
		else if (m_uiAnimIdx == Archer::CHARGE_ARROW_START && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::CHARGE_ARROW_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Archer::CHARGE_ARROW_LOOP && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::CHARGE_ARROW_SHOT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		} 
	}

	// RAPID_SHOT
	if (m_uiAnimIdx == Archer::RAPID_SHOT1 && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
	{
		Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		m_uiAnimIdx = Archer::RAPID_SHOT2;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
	}

	// Skill Attack ==> ATTACK_WAIT
	if ((Archer::RAPID_SHOT2 == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Archer::ESCAPING_BOMB == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Archer::ARROW_SHOWER_SHOT == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Archer::ARROW_FALL_SHOT == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Archer::CHARGE_ARROW_SHOT == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)))
	{
		if (Archer::ARROW_FALL_SHOT == m_uiAnimIdx)
		{
			m_pDynamicCamera->Set_CameraAtParentMatrix(nullptr);
			m_pDynamicCamera->Set_ResetFovY();
			m_pDynamicCamera->Set_CameraState(CAMERA_STATE::THIRD_PERSON_VIEW);
		}

		m_bIsAttack    = false;
		m_bIsSkill     = false;
		m_bIsSkillLoop = false;
		m_uiAnimIdx    = Archer::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCArcher::KeyInput_BackDash(const _float& fTimeDelta)
{
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_RB) && !m_bIsSkillLoop && NO_EVENT_STATE)
	{
		SetUp_WeaponLHand();
		SetUp_AttackSetting();
		m_bIsSkillLoop = true;
		m_uiAnimIdx    = Archer::BACK_DASH;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
	}

	if ((Archer::BACK_DASH == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)))
	{
		m_bIsAttack    = false;
		m_bIsSkill     = false;
		m_bIsSkillLoop = false;
		m_uiAnimIdx    = Archer::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCArcher::KeyInput_OpenShop(const char& npcNumber)
{
	if (g_bIsOpenShop)
	{
		if (npcNumber == NPC_POPORI_MERCHANT || npcNumber == NPC_BARAKA_MERCHANT || npcNumber == NPC_BARAKA_MYSTELLIUM)
		{
			static_cast<CInGameStoreCanvas*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", L"UIInGameStoreCanvas"))->Set_IsActive(true);
			static_cast<CInGameStoreCanvas*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", L"UIInGameStoreCanvas"))->Set_IsChildActive(true);
			static_cast<CMainMenuInventory*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", L"OptionInventoryNormal"))->Set_IsActiveCanvas(true);
			static_cast<CInventoryCanvas*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", L"UIInventoryCanvas"))->Get_Transform()->m_vPos = _vec3(1100.0f, 450.0f, 1.0f);
			CMouseCursorMgr::Get_Instance()->Set_IsActiveMouse(true);

			switch (npcNumber)
			{
			case NPC_POPORI_MERCHANT:		// 무기상인
			{
				CStoreMgr::Get_Instance()->Set_StoreState(STORE_STATE::STORE_WEAPON);
				CStoreMgr::Get_Instance()->Set_StoreItemType(ItemType_WeaponTwoHand);
			}
			break;
			case NPC_BARAKA_MERCHANT:		// 물약상인
			{
				CStoreMgr::Get_Instance()->Set_StoreState(STORE_STATE::STORE_POTION);
				CStoreMgr::Get_Instance()->Set_StoreItemType(ItemType_Potion);
			}
			break;
			case NPC_BARAKA_MYSTELLIUM:		// 방어구상인
			{
				CStoreMgr::Get_Instance()->Set_StoreState(STORE_STATE::STORE_ARMOR);
				CStoreMgr::Get_Instance()->Set_StoreItemType(ItemType_Helmet);
			}
			break;
			default:
				break;
			}

			// NPC에 맞는 상점 리소스 생성
			cout << "상점 오픈" << endl;
		}		
	}
	else
	{
		// BuySlotList Clear
		CStoreMgr::Get_Instance()->Reset_StoreItemBuySlotList();
		CStoreMgr::Get_Instance()->Reset_StoreItemSellSlotList();

		static_cast<CInGameStoreCanvas*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", L"UIInGameStoreCanvas"))->Set_IsActive(false);
		static_cast<CInGameStoreCanvas*>(m_pObjectMgr->Get_GameObject(L"Layer_UI", L"UIInGameStoreCanvas"))->Set_IsChildActive(false);
		cout << "상점 종료" << endl;
	}
}

void CPCArcher::KeyInput_OpenQuest(const char& npcNumber)
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

void CPCArcher::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsKeyDown || Archer::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
															  &m_pTransCom->m_vDir,
															  m_pInfoCom->m_fSpeed * fTimeDelta);
	}
}

void CPCArcher::AttackMove_OnNaviMesh(const _float& fTimeDelta)
{
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

void CPCArcher::SetUp_RunMoveSpeed(const _float& fTimeDelta)
{
	if (Archer::STANCE_ATTACK == m_eStance)
	{
		m_tMoveSpeedInterpolationDesc.v2 = Archer::MAX_ATTACK_SPEED;
	}
	else
	{
		m_tMoveSpeedInterpolationDesc.v2 = Archer::MAX_SPEED;
	}

	// Move On
	if (m_bIsKeyDown)
	{
		if (Archer::STANCE_ATTACK == m_eStance)
		{
			m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.5f;
		}
		else
		{
			m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f;
		}
	}

	// Move Off
	else if (!m_bIsKeyDown)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = -Archer::MOVE_STOP_SPEED;

	m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, 
													   m_tMoveSpeedInterpolationDesc.v2, 
													   m_tMoveSpeedInterpolationDesc.linear_ratio);
}

void CPCArcher::SetUp_RunAnimation()
{
	if (m_bIsKeyDown && Archer::STANCE_NONEATTACK == m_eStance)
	{
		m_uiAnimIdx = Archer::NONE_ATTACK_WALK;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}

	else if (m_bIsKeyDown && Archer::STANCE_ATTACK == m_eStance)
	{
		m_uiAnimIdx = Archer::ATTACK_RUN;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCArcher::SetUp_RunToIdleAnimation(const _float& fTimeDelta)
{
	if (!m_bIsKeyDown &&
		Archer::NONE_ATTACK_WALK == m_uiAnimIdx &&
		Archer::STANCE_NONEATTACK == m_eStance)
	{
		m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
	else if (!m_bIsKeyDown &&
			 Archer::ATTACK_RUN == m_uiAnimIdx &&
			 Archer::STANCE_ATTACK == m_eStance)
	{
		m_uiAnimIdx = Archer::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCArcher::SetUp_PlayerStance_FromAttackToNoneAttack()
{
	if (Archer::STANCE_ATTACK == m_eStance)
	{
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);

		m_eStance   = Archer::STANCE_NONEATTACK;
		m_uiAnimIdx = Archer::IN_WEAPON;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCArcher::SetUp_PlayerStance_FromNoneAttackToAttack()
{
	if (Archer::STANCE_NONEATTACK == m_eStance)
	{
		if (Archer::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Archer::NONE_ATTACK_WALK == m_uiAnimIdx)
		{
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
			SetUp_WeaponLHand();
		}

		m_eStance   = Archer::STANCE_ATTACK;
		m_uiAnimIdx = Archer::OUT_WEAPON;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCArcher::Change_PlayerStance(const _float& fTimeDelta)
{
	if (!m_bIsCompleteStanceChange)
	{
		// NONE_ATTACK -> ATACK
		if (Archer::STANCE_ATTACK == m_eStance &&
			Archer::OUT_WEAPON == m_uiAnimIdx &&
			m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::ATTACK_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;

		}

		// ATTACK -> NONE_ATTACK
		else if (Archer::STANCE_NONEATTACK == m_eStance  &&
				 Archer::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;
		}

		if (Archer::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
			SetUp_WeaponBack();

		// Check Is Complete Stance Change
		if (Archer::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Archer::ATTACK_WAIT == m_uiAnimIdx)
		{
			m_bIsCompleteStanceChange = true;
		}
	}
}

void CPCArcher::SetUp_AttackMove(const _uint& uiAniIdx, 
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

void CPCArcher::Send_Player_Move()
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

bool CPCArcher::Is_Change_CamDirection()
{
	if (m_fPreAngle + 5.f < m_pTransCom->m_vAngle.y)
		return true;

	else if (m_fPreAngle - 5.f > m_pTransCom->m_vAngle.y)
		return true;

	return false;
}

void CPCArcher::Is_ChangeWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(0.016f);
		}

		m_pWeapon = static_cast<CPCWeaponBow*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponBow(m_chCurWeaponType)));
		m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
		m_pWeapon->Update_GameObject(0.016f);

		if (Archer::STANCE_ATTACK == m_eStance)
		{
			SetUp_WeaponLHand();
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
		}
		else if (Archer::STANCE_NONEATTACK == m_eStance)
		{
			SetUp_WeaponBack();
			m_pWeapon->Set_DissolveInterpolation(1.0f);
			m_pWeapon->Set_IsRenderShadow(false);
		}

		m_chPreWeaponType = m_chCurWeaponType;
	}
}

void CPCArcher::SetUp_AttackSetting()
{
	m_bIsSetCollisionTick                        = false;
	m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
	m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
	m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

	m_bIsAttack  = true;
	m_bIsKeyDown = false;
	m_tAttackMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
}

void CPCArcher::SetUp_WeaponLHand()
{
	m_pWeapon->Get_Transform()->m_vAngle.x = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.y = -160.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 210.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));
}

void CPCArcher::SetUp_WeaponBack()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

void CPCArcher::Ready_AngleInterpolationValue(const _float& fEndAngle)
{
	m_tAngleInterpolationDesc.is_start_interpolation = true;
	m_tAngleInterpolationDesc.v1                     = m_pTransCom->m_vAngle.y;
	m_tAngleInterpolationDesc.v2                     = fEndAngle;
	m_tAngleInterpolationDesc.linear_ratio           = Engine::MIN_LINEAR_RATIO;
	m_tAngleInterpolationDesc.interpolation_speed    = 5.0f;
}

void CPCArcher::SetUp_AngleInterpolation(const _float& fTimeDelta)
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

void CPCArcher::SetUp_CollisionArrow(const _float& fTimeDelta)
{
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
			m_vArrowFallPos.y = 0.2f;
			CEffectMgr::Get_Instance()->Effect_MagicCircle_Effect(_vec3(0.0f), _vec3(0.0f), 
				m_vArrowFallPos, 0, 0, 2, true, true, nullptr, false);

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
			m_tCollisionTickDesc.iMaxCollisionTick       = 48;

			m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
			m_pTransCom->m_vDir.Normalize();
			m_vArrowFallPos   = m_pTransCom->m_vPos + m_pTransCom->m_vDir * Archer::ARROW_FALL_DIST;
			m_vArrowFallPos.y = 20.0f;

			m_tCollisionTickDesc.iMaxCollisionTick       = 1;

			m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("L_Sword")));

		
		}
	}

	// Create CollisionArrow
	if (m_bIsSetCollisionTick && m_tCollisionTickDesc.bIsCreateCollisionTick && m_tCollisionTickDesc.iCurCollisionTick < m_tCollisionTickDesc.iMaxCollisionTick)
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

			// Camera Effect
			SetUp_CameraEffect(fTimeDelta);

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
						pCollisionArrow->Set_CollisionTag(L"CollisionTick_ThisPlayer");
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
					pCollisionArrow->Set_CollisionTag(L"CollisionTick");
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
				CCollisionTick* pCollisionTick = static_cast<CCollisionTick*>(Pop_Instance(m_pInstancePoolMgr->Get_CollisionTickPool()));
				if (nullptr != pCollisionTick)
				{
					pCollisionTick->Get_BoundingSphere()->Get_BoundingInfo().Radius = 0.5f;
					pCollisionTick->Set_CollisionTag(L"CollisionTick_ThisPlayer");
					pCollisionTick->Set_Damage(m_pInfoCom->Get_RandomDamage());
					pCollisionTick->Set_LifeTime(0.25f);
					pCollisionTick->Get_Transform()->m_vScale = _vec3(5.0f);
					pCollisionTick->Get_Transform()->m_vPos   = m_pTransCom->m_vPos;
					pCollisionTick->Get_BoundingSphere()->Set_Radius(pCollisionTick->Get_Transform()->m_vScale);
					m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"CollisionTick_ThisPlayer", pCollisionTick);
				}
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
					pCollisionArrow->Set_CollisionTag(L"CollisionTick_ThisPlayer");
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
		m_fArrowFallTime += fTimeDelta;

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
				pCollisionArrow->Set_CollisionTag(L"");
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
	if (m_uiAnimIdx == Archer::CHARGE_ARROW_START && m_bisHandEffect==false)
	{
		m_bisHandEffect = true;
		Engine::HIERARCHY_DESC* pHierarchyDesc = &(m_pMeshCom->Find_HierarchyDesc("L_Sword"));
		_vec3 Pos = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer")->Get_Transform()->Get_PositionVector();
		Pos.y += 2.f;
		CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Lighting2", _vec3(1.f), _vec3(0.0f), Pos, FRAME(5, 16, 25.0f)
			, false, false
			, _vec4(0.0f), true, pHierarchyDesc, m_pTransCom);
		
	}
	if (m_uiAnimIdx == Archer::CHARGE_ARROW_SHOT)
	{
		m_bisHandEffect = false;
	}
	if ( m_uiAnimIdx == Archer::ARROW_FALL_LOOP )
	{
		static float Time = 0.f;
		Time += fTimeDelta;
		static int Temp = 10;
		CGameObject* pGameObj = nullptr;
		_vec3 Pos = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer")->Get_Transform()->Get_PositionVector();
		if (Temp > 0 && Time > 0.03f)
		{
			Time = 0.f;
			Temp--;
			if (Temp == 0)
				Temp = 10;
			Pos.y += 2.f;


			//CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Lighting0", _vec3(3.5f, 25.5f, 3.5f),					// Scale
			//	_vec3((rand() % 120 - 60), 0.0f, 0.f),		// Angle
			//	Pos,			// Pos
			//	FRAME(1, 1, 1.0f),false,false);

			//pGameObj = CLightingParticle::Create(m_pGraphicDevice, m_pCommandList,
			//	L"Lighting0",						// TextureTag
			//	_vec3(3.5f, 25.5f, 3.5f),					// Scale
			//	_vec3((rand() % 120 - 60), 0.0f, 0.f),		// Angle
			//	Pos,			// Pos
			//	FRAME(1, 1, 1.0f));			// Sprite Image Frame
			//Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"Lighting", pGameObj), E_FAIL);

			
		}
	}
}

void CPCArcher::SetUp_CameraEffect(const _float& fTimeDelta)
{
	if (nullptr == m_pDynamicCamera)
		return;

	switch (m_uiAnimIdx)
	{
	case Archer::ATTACK_ARROW:
	{
		CAMERA_ZOOM_DESC tCameraZoomDesc;
		tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
		tCameraZoomDesc.fPower     = 0.03f;
		tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 7.0f;
		m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
	}
		break;

	case Archer::RAPID_SHOT1:
	case Archer::RAPID_SHOT2:
	{
		CAMERA_ZOOM_DESC tCameraZoomDesc;
		tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
		tCameraZoomDesc.fPower     = 0.025f;
		tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 12.5f;
		m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
	}
		break;

	case Archer::ARROW_SHOWER_START:
	case Archer::ARROW_SHOWER_LOOP:
	case Archer::ARROW_SHOWER_SHOT:
	{
		CAMERA_ZOOM_DESC tCameraZoomDesc;
		tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_IN;
		tCameraZoomDesc.fPower     = 0.05f;
		tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 10.0f;
		m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
	}
		break;

	case Archer::ESCAPING_BOMB:
	{
		CAMERA_SHAKING_DESC tCameraShakingDesc;
		tCameraShakingDesc.fUpdateShakingTime = 0.5f;
		tCameraShakingDesc.vMin = _vec2(-14.0f, 0.0f);
		tCameraShakingDesc.vMax = _vec2(14.0f, 0.0f);
		tCameraShakingDesc.tOffsetInterpolationDesc.interpolation_speed = 12.0f;
		m_pDynamicCamera->Set_CameraShakingDesc(tCameraShakingDesc);

		CAMERA_ZOOM_DESC tCameraZoomDesc;
		tCameraZoomDesc.eZoomState = CAMERA_ZOOM::ZOOM_OUT;
		tCameraZoomDesc.fPower     = 0.02f;
		tCameraZoomDesc.tFovYInterpolationDesc.interpolation_speed = 15.0f;
		m_pDynamicCamera->Set_CameraZoomDesc(tCameraZoomDesc);
	}
		break;

	default:
		break;
	}
}

void CPCArcher::Collision_MonsterMultiCollider(list<Engine::CColliderSphere*>& lstMonsterCollider)
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

void CPCArcher::Collision_PortalVelikaToBeach(list<Engine::CColliderSphere*>& lstPortalCollider)
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

				if (Archer::STANCE_ATTACK == m_eStance)
					m_uiAnimIdx = Archer::ATTACK_WAIT;
				else
					m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;

				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// FadeInOut
				Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
				if (nullptr != pGameObject)
				{
					static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
					static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_BEACH);
					m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
				}
				//Engine::CGameObject* pGameObject = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
				//static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_BEACH);
				//m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
			}
		}
	}
}

void CPCArcher::Collision_PortalBeachToVelika(list<Engine::CColliderSphere*>& lstPortalCollider)
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

				if (Archer::STANCE_ATTACK == m_eStance)
					m_uiAnimIdx = Archer::ATTACK_WAIT;
				else
					m_uiAnimIdx = Archer::NONE_ATTACK_IDLE;

				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// FadeInOut
				Engine::CGameObject* pGameObject = Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool());
				if (nullptr != pGameObject)
				{
					static_cast<CFadeInOut*>(pGameObject)->Set_FadeInOutEventType(EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
					static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_VELIKA);
					m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
				}
				//Engine::CGameObject* pGameObject = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
				//static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_VELIKA);
				//m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
			}
		}
	}
}

void CPCArcher::Collision_Merchant(list<Engine::CColliderSphere*>& lstMerchantCollider, int npcServerNumber)
{
	if (!g_bIsActive) return;

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
					g_bIsOpenShop = !g_bIsOpenShop;
					KeyInput_OpenShop(pObj->Get_NPCNumber());
				}
			}
		}
	}
}

void CPCArcher::Collision_Quest(list<Engine::CColliderSphere*>& lstMerchantCollider, int npcServerNumber)
{
	if (!g_bIsActive) return;

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
				if (Engine::KEY_DOWN(DIK_F) && NO_EVENT_STATE)
				{
					KeyInput_OpenQuest(pObj->Get_NPCNumber());
				}
			}
		}
	}
}

void CPCArcher::Suggest_PartyToOthers()
{
	// Picking Others
	if (m_pMouserMgr->Get_IsActiveMouse() && Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON(Engine::DIM_RB)) && !g_bIsOpenShop)
	{
		Engine::OBJLIST*		pOthersList    = m_pObjectMgr->Get_OBJLIST(L"Layer_GameObject", L"Others");
		Engine::CGameObject*	pPickingOthers = nullptr;
		if (nullptr == pOthersList || pOthersList->empty())
			return;

		m_pPartySystemMgr->Set_SelectPlayer(nullptr);
		m_pPartySystemMgr->Get_PartySuggestCanvas()->Set_IsActive(false);
		m_pPartySystemMgr->Get_PartySuggestCanvas()->Set_IsChildActive(false);

		if (m_pMouserMgr->Check_PickingBoundingBox(&pPickingOthers, pOthersList))
		{
			POINT pt = m_pMouserMgr->Get_CursorPoint();

			m_pPartySystemMgr->Set_SelectPlayer(pPickingOthers);
			m_pPartySystemMgr->Get_PartySuggestCanvas()->Set_IsActive(true);
			m_pPartySystemMgr->Get_PartySuggestCanvas()->Set_IsChildActive(true);
			m_pPartySystemMgr->Get_PartySuggestCanvas()->Get_Transform()->m_vPos = _vec3((_float)pt.x + 32.0f, (_float)pt.y, 1.0f);
		}
	}
}

void CPCArcher::Leave_PartyThisPlayer()
{
	if (!g_bIsActive) return;

	if (Engine::KEY_DOWN(DIK_P) && !g_bIsOnDebugCaemra && !g_bIsStageChange && !g_bIsOpenShop && !g_bIsChattingInput)
	{
		m_pPartySystemMgr->Get_PartyLeaveCanvas()->Reverse_IsActive();
		m_pPartySystemMgr->Get_PartyLeaveCanvas()->Reverse_IsActiveChild();
	}
}

void CPCArcher::Effect_Loop(const _float& fTimeDelta)
{
}

Engine::CGameObject* CPCArcher::Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag, 
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos, 
									   const char& chWeaponType)
{

	CPCArcher* pInstance = new CPCArcher(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos, chWeaponType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPCArcher::Free()
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
	Engine::Safe_Release(m_pWinterNaviMeshCom);
	Engine::Safe_Release(m_pFont);
}
