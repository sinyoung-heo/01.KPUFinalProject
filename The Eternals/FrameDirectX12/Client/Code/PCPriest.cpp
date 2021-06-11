#include "stdafx.h"
#include "PCPriest.h"
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
#include "PCWeaponRod.h"

CPCPriest::CPCPriest(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
	, m_pServerMath(CServerMath::Get_Instance())
	, m_pInstancePoolMgr(CInstancePoolMgr::Get_Instance())
{
}

HRESULT CPCPriest::Ready_GameObject(wstring wstrMeshTag, 
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
	m_wstrMeshTag         = wstrMeshTag;

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

	m_pInfoCom->m_fSpeed     = Priest::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	m_eKeyState			= MVKEY::K_END;
	m_bIsKeyDown		= false;
	m_bIsSameDir		= false;
	m_bIsSendMoveStop	= true;
	m_fPreAngle			= m_pTransCom->m_vAngle.y;

	/*__________________________________________________________________________________________________________
	[ Skill KeyInput ]
	____________________________________________________________________________________________________________*/
	m_mapSkillKeyInput[L"AURA_ON"]   = DIK_1;
	m_mapSkillKeyInput[L"PURIFY"]    = DIK_2;
	m_mapSkillKeyInput[L"HEAL"]      = DIK_3;
	m_mapSkillKeyInput[L"MP_CHARGE"] = DIK_4;

	/*__________________________________________________________________________________________________________
	[ 선형보간 설정 ]
	____________________________________________________________________________________________________________*/
	m_tMoveSpeedInterpolationDesc.linear_ratio = 0.0f;
	m_tMoveSpeedInterpolationDesc.v1           = Priest::MIN_SPEED;
	m_tMoveSpeedInterpolationDesc.v2           = Priest::MAX_SPEED;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_eStance   = Priest::STANCE_NONEATTACK;
	m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;

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

HRESULT CPCPriest::LateInit_GameObject()
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

_int CPCPriest::Update_GameObject(const _float& fTimeDelta)
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

	//// AfterImage
	//Make_AfterImage(fTimeDelta);

	// Weapon Update
	if (m_pWeapon != nullptr)
		m_pWeapon->Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCPriest::LateUpdate_GameObject(const _float& fTimeDelta)
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

void CPCPriest::Process_Collision()
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

void CPCPriest::Send_PacketToServer()
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

void CPCPriest::Render_GameObject(const _float& fTimeDelta)
{
	// Render AfterImage
	if (m_uiAfterImgSize)
	{
		m_pShaderCom->Set_PipelineStatePass(5);
		Render_AfterImage(fTimeDelta);
	}
}

void CPCPriest::Render_AfterImage(const _float& fTimeDelta)
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

void CPCPriest::Render_GameObject(const _float& fTimeDelta, 
								  ID3D12GraphicsCommandList* pCommandList, 
								  const _int& iContextIdx)
{
	m_pShaderCom->Set_PipelineStatePass(0);
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCPriest::Render_ShadowDepth(const _float& fTimeDelta, 
								   ID3D12GraphicsCommandList* pCommandList,
								   const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPCPriest::Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag)
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

HRESULT CPCPriest::SetUp_PCWeapon()
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

HRESULT CPCPriest::SetUp_ClassFrame()
{
	/*__________________________________________________________________________________________________________
	[ CharacterClassFrame ]
	____________________________________________________________________________________________________________*/
	{
		wifstream fin { L"../../Bin/ToolData/2DUICharacterClassFramePriest.2DUI" };
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

void CPCPriest::SetUp_StageID()
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

void CPCPriest::Set_ConstantTable()
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

void CPCPriest::Set_ConstantTableShadowDepth()
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

void CPCPriest::Set_IsRepeatAnimation()
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

void CPCPriest::Set_AnimationSpeed()
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

void CPCPriest::Set_BlendingSpeed()
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

void CPCPriest::Key_Input(const _float& fTimeDelta)
{
	if (!g_bIsActive) return;

	KeyInput_Move(fTimeDelta);
	KeyInput_Attack(fTimeDelta);
}

void CPCPriest::KeyInput_Move(const _float& fTimeDelta)
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
	}
	// 좌로 이동.
	else if (Engine::KEY_PRESSING(DIK_A))
	{
		m_pTransCom->m_vAngle.y = m_pDynamicCamera->Get_Transform()->m_vAngle.y + LEFT;
		m_last_move_time        = high_resolution_clock::now();
		m_eKeyState             = MVKEY::K_LEFT;
		m_bIsKeyDown            = true;
		m_bIsSendMoveStop		= false;
	}
	// 우로 이동.	
	else if (Engine::KEY_PRESSING(DIK_D))
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
				 Priest::MIN_SPEED == m_pInfoCom->m_fSpeed)
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

void CPCPriest::KeyInput_Attack(const _float& fTimeDelta)
{
	if (nullptr == m_pWeapon)
		return;

	KeyInput_StanceChange(fTimeDelta);

	if (Priest::STANCE_ATTACK == m_eStance && m_bIsCompleteStanceChange)
	{
		if (m_pMeshCom->Is_BlendingComplete())
		{
			KeyInput_AttackRod(fTimeDelta);
			KeyInput_SkillAttack(fTimeDelta);
		}
	}
}

void CPCPriest::KeyInput_StanceChange(const _float& fTimeDelta)
{
	if (m_bIsAttack)
		return;

	// ATTACK -> NONE_ATTACK
	if (Engine::KEY_DOWN(DIK_LSHIFT) && Priest::STANCE_ATTACK == m_eStance &&
		!m_bIsKeyDown && m_pInfoCom->m_fSpeed == Priest::MIN_SPEED &&
		m_bIsCompleteStanceChange &&
		m_uiAnimIdx != Priest::NONE_ATTACK_WALK && m_uiAnimIdx != Priest::ATTACK_RUN &&
		m_uiAnimIdx != Priest::IN_WEAPON && m_uiAnimIdx != Priest::OUT_WEAPON)
	{
		m_bIsCompleteStanceChange = false;
		SetUp_PlayerStance_FromAttackToNoneAttack();

		// Send Packet.
		m_pPacketMgr->send_stance_change(m_uiAnimIdx, false);
	}

	// NONE_ATTACK -> ATTACK
	if ((Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"AURA_ON"]) ||
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"PURIFY"]) ||
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"HEAL"]) || 
		Engine::KEY_DOWN(m_mapSkillKeyInput[L"MP_CHARGE"])) &&
		Priest::STANCE_NONEATTACK == m_eStance &&
		m_bIsCompleteStanceChange &&
		m_uiAnimIdx != Priest::IN_WEAPON && m_uiAnimIdx != Priest::OUT_WEAPON)
	{
		m_bIsKeyDown = false;
		m_bIsCompleteStanceChange = false;
		SetUp_PlayerStance_FromNoneAttackToAttack();

		// Send Packet.
		m_pPacketMgr->send_stance_change(m_uiAnimIdx, true);
	}

	Change_PlayerStance(fTimeDelta);
}

void CPCPriest::KeyInput_AttackRod(const _float& fTimeDelta)
{
	if (Engine::MOUSE_KEYDOWN(Engine::MOUSEBUTTON::DIM_LB) && !m_bIsSkill && !m_bIsSkillLoop)
	{
		SetUp_WeaponRHand();
		SetUp_AttackSetting();
		m_uiAnimIdx = Priest::HASTE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
	}

	if (Priest::HASTE == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
	{
		SetUp_WeaponRHand();
		m_bIsAttack   = false;
		m_uiAnimIdx   = Priest::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCPriest::KeyInput_SkillAttack(const _float& fTimeDelta)
{
	if (!m_bIsSkillLoop)
	{
		if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"AURA_ON"]))
		{
			SetUp_AttackSetting();
			m_bIsSkill  = true;
			m_uiAnimIdx = Priest::AURA_ON;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"PURIFY"]))
		{
			SetUp_AttackSetting();
			m_bIsSkill  = true;
			m_uiAnimIdx = Priest::PURIFY;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"HEAL"]))
		{
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Priest::HEAL_START;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
		else if (Engine::KEY_DOWN(m_mapSkillKeyInput[L"MP_CHARGE"]))
		{
			SetUp_AttackSetting();
			m_bIsSkill     = true;
			m_bIsSkillLoop = true;
			m_uiAnimIdx = Priest::MP_CHARGE_START;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, m_pDynamicCamera->Get_Transform()->m_vAngle.y);
		}
	}
	else
	{
		// HEAL
		if (m_uiAnimIdx == Priest::HEAL_START && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Priest::HEAL_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Priest::HEAL_LOOP && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Priest::HEAL_SHOT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		// MP_CHARGE
		else if (m_uiAnimIdx == Priest::MP_CHARGE_START && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Priest::MP_CHARGE_LOOP;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		}
		else if (m_uiAnimIdx == Priest::MP_CHARGE_LOOP && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Priest::MP_CHARGE_END;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_pPacketMgr->send_attack(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos, -1.0f);
		} 
	}

	// Skill Attack ==> ATTACK_WAIT
	if ((Priest::AURA_ON == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Priest::PURIFY == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Priest::HEAL_SHOT == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)) ||
		(Priest::MP_CHARGE_END == m_uiAnimIdx && m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed)))
	{
		m_bIsAttack    = false;
		m_bIsSkill     = false;
		m_bIsSkillLoop = false;
		m_uiAnimIdx    = Priest::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
		m_pPacketMgr->send_attack_stop(m_uiAnimIdx, m_pTransCom->m_vDir, m_pTransCom->m_vPos);
	}
}

void CPCPriest::KeyInput_OpenShop(const char& npcNumber)
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

void CPCPriest::KeyInput_OpenQuest(const char& npcNumber)
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

void CPCPriest::Move_OnNaviMesh(const _float& fTimeDelta)
{
	if (m_bIsKeyDown || Priest::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		m_pTransCom->m_vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
															  &m_pTransCom->m_vDir,
															  m_pInfoCom->m_fSpeed * fTimeDelta);
	}
}

void CPCPriest::SetUp_RunMoveSpeed(const _float& fTimeDelta)
{
	// Move On
	if (m_bIsKeyDown)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = 1.0f;

	// Move Off
	else if (!m_bIsKeyDown)
		m_tMoveSpeedInterpolationDesc.interpolation_speed = -Priest::MOVE_STOP_SPEED;

	m_tMoveSpeedInterpolationDesc.linear_ratio += m_tMoveSpeedInterpolationDesc.interpolation_speed * fTimeDelta;
	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(m_tMoveSpeedInterpolationDesc.v1, 
													   m_tMoveSpeedInterpolationDesc.v2, 
													   m_tMoveSpeedInterpolationDesc.linear_ratio);
}

void CPCPriest::SetUp_RunAnimation()
{
	if (m_bIsKeyDown && Priest::STANCE_NONEATTACK == m_eStance)
	{
		m_uiAnimIdx = Priest::NONE_ATTACK_WALK;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}

	else if (m_bIsKeyDown && Priest::STANCE_ATTACK == m_eStance)
	{
		m_uiAnimIdx = Priest::ATTACK_RUN;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCPriest::SetUp_RunToIdleAnimation(const _float& fTimeDelta)
{ 
	if (!m_bIsKeyDown &&
		Priest::NONE_ATTACK_WALK == m_uiAnimIdx &&
		Priest::STANCE_NONEATTACK == m_eStance)
	{
		m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
	else if (!m_bIsKeyDown &&
			 Priest::ATTACK_RUN == m_uiAnimIdx &&
			 Priest::STANCE_ATTACK == m_eStance)
	{
		m_uiAnimIdx = Priest::ATTACK_WAIT;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCPriest::SetUp_PlayerStance_FromAttackToNoneAttack()
{
	if (Priest::STANCE_ATTACK == m_eStance)
	{
		m_pWeapon->Set_DissolveInterpolation(1.0f);
		m_pWeapon->Set_IsRenderShadow(false);

		m_eStance   = Priest::STANCE_NONEATTACK;
		m_uiAnimIdx = Priest::IN_WEAPON;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCPriest::SetUp_PlayerStance_FromNoneAttackToAttack()
{
	if (Priest::STANCE_NONEATTACK == m_eStance)
	{
		if (Priest::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Priest::NONE_ATTACK_WALK == m_uiAnimIdx)
		{
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
			SetUp_WeaponRHand();
		}

		m_eStance   = Priest::STANCE_ATTACK;
		m_uiAnimIdx = Priest::OUT_WEAPON;
		m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	}
}

void CPCPriest::Change_PlayerStance(const _float& fTimeDelta)
{
	if (!m_bIsCompleteStanceChange)
	{
		// NONE_ATTACK -> ATACK
		if (Priest::STANCE_ATTACK == m_eStance &&
			Priest::OUT_WEAPON == m_uiAnimIdx &&
			m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Priest::ATTACK_WAIT;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;

		}

		// ATTACK -> NONE_ATTACK
		else if (Priest::STANCE_NONEATTACK == m_eStance  &&
				 Priest::IN_WEAPON == m_uiAnimIdx &&
				 m_pMeshCom->Is_AnimationSetEnd(fTimeDelta, m_fAnimationSpeed))
		{
			m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;
			m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
			m_bIsCompleteStanceChange = true;
		}

		if (Priest::IN_WEAPON == m_uiAnimIdx && m_ui3DMax_CurFrame > 20)
			SetUp_WeaponBack();

		// Check Is Complete Stance Change
		if (Priest::NONE_ATTACK_IDLE == m_uiAnimIdx ||
			Priest::ATTACK_WAIT == m_uiAnimIdx)
		{
			m_bIsCompleteStanceChange = true;
		}
	}
}

void CPCPriest::Send_Player_Move()
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

bool CPCPriest::Is_Change_CamDirection()
{
	if (m_fPreAngle + 5.f < m_pTransCom->m_vAngle.y)
		return true;

	else if (m_fPreAngle - 5.f > m_pTransCom->m_vAngle.y)
		return true;

	return false;
}

void CPCPriest::Is_ChangeWeapon()
{
	if (m_chCurWeaponType != m_chPreWeaponType)
	{
		if (nullptr != m_pWeapon)
		{
			m_pWeapon->Set_IsReturnObject(true);
			m_pWeapon->Update_GameObject(0.016f);
		}

		m_pWeapon = static_cast<CPCWeaponRod*>(Pop_Instance(CInstancePoolMgr::Get_Instance()->Get_PCWeaponRod(m_chCurWeaponType)));
		m_pWeapon->Set_ParentMatrix(&m_pTransCom->m_matWorld);
		m_pWeapon->Update_GameObject(0.016f);

		if (Priest::STANCE_ATTACK == m_eStance)
		{
			SetUp_WeaponRHand();
			m_pWeapon->Set_DissolveInterpolation(-1.0f);
			m_pWeapon->Set_IsRenderShadow(true);
		}
		else if (Priest::STANCE_NONEATTACK == m_eStance)
		{
			SetUp_WeaponBack();
			m_pWeapon->Set_DissolveInterpolation(1.0f);
			m_pWeapon->Set_IsRenderShadow(false);
		}

		m_chPreWeaponType = m_chCurWeaponType;
	}
}

void CPCPriest::SetUp_AttackSetting()
{
	m_bIsSetCollisionTick                        = false;
	m_tCollisionTickDesc.bIsCreateCollisionTick  = false;
	m_tCollisionTickDesc.fColisionTickUpdateTime = -1.0f;
	m_tCollisionTickDesc.fCollisionTickTime      = 0.0f;

	m_bIsAttack = true;
	m_bIsKeyDown = false;
	Ready_AngleInterpolationValue(m_pDynamicCamera->Get_Transform()->m_vAngle.y);
}

void CPCPriest::SetUp_WeaponRHand()
{
	m_pWeapon->Get_Transform()->m_vPos.x = -2.0f;
	m_pWeapon->Get_Transform()->m_vPos.y = 5.0f;
	m_pWeapon->Get_Transform()->m_vPos.z = 1.0f;

	m_pWeapon->Get_Transform()->m_vAngle.x = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 0.0f;

	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("R_Sword")));
}

void CPCPriest::SetUp_WeaponBack()
{
	m_pWeapon->Get_Transform()->m_vAngle.y = 0.0f;
	m_pWeapon->Get_Transform()->m_vAngle.z = 180.0f;
	m_pWeapon->Set_HierarchyDesc(&(m_pMeshCom->Find_HierarchyDesc("Weapon_Back")));
}

void CPCPriest::SetUp_CollisionTick(const _float& fTimeDelta)
{
	if (Priest::HASTE == m_uiAnimIdx && m_ui3DMax_CurFrame >= Priest::HASTE_COLLISIONTICK_START)
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

void CPCPriest::Ready_AngleInterpolationValue(const _float& fEndAngle)
{
	m_tAngleInterpolationDesc.is_start_interpolation = true;
	m_tAngleInterpolationDesc.v1                     = m_pTransCom->m_vAngle.y;
	m_tAngleInterpolationDesc.v2                     = fEndAngle;
	m_tAngleInterpolationDesc.linear_ratio           = Engine::MIN_LINEAR_RATIO;
	m_tAngleInterpolationDesc.interpolation_speed    = 5.0f;
}

void CPCPriest::SetUp_AngleInterpolation(const _float& fTimeDelta)
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

void CPCPriest::Collision_MonsterMultiCollider(list<Engine::CColliderSphere*>& lstMonsterCollider)
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

void CPCPriest::Collision_PortalVelikaToBeach(list<Engine::CColliderSphere*>& lstPortalCollider)
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

				if (Priest::STANCE_ATTACK == m_eStance)
					m_uiAnimIdx = Priest::ATTACK_WAIT;
				else
					m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;

				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// FadeInOut
				Engine::CGameObject* pGameObject = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
				static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_BEACH);
				m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
			}
		}
	}
}

void CPCPriest::Collision_PortalBeachToVelika(list<Engine::CColliderSphere*>& lstPortalCollider)
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

				if (Priest::STANCE_ATTACK == m_eStance)
					m_uiAnimIdx = Priest::ATTACK_WAIT;
				else
					m_uiAnimIdx = Priest::NONE_ATTACK_IDLE;

				m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

				// FadeInOut
				Engine::CGameObject* pGameObject = CFadeInOut::Create(m_pGraphicDevice, m_pCommandList, EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN);
				static_cast<CFadeInOut*>(pGameObject)->Set_CurrentStageID(STAGE_VELIKA);
				m_pObjectMgr->Add_GameObject(L"Layer_UI", L"StageChange_FadeInOut", pGameObject);
			}
		}
	}
}

void CPCPriest::Collision_Merchant(list<Engine::CColliderSphere*>& lstMerchantCollider, int npcServerNumber)
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

void CPCPriest::Collision_Quest(list<Engine::CColliderSphere*>& lstMerchantCollider, int npcServerNumber)
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

Engine::CGameObject* CPCPriest::Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList, 
									   wstring wstrMeshTag, 
									   wstring wstrNaviMeshTag, 
									   const _vec3& vScale, 
									   const _vec3& vAngle, 
									   const _vec3& vPos, 
									   const char& chWeaponType)
{
	CPCPriest* pInstance = new CPCPriest(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, wstrNaviMeshTag, vScale, vAngle, vPos, chWeaponType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPCPriest::Free()
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
