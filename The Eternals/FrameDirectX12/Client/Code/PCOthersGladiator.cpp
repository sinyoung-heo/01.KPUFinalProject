#include "stdafx.h"
#include "PCOthersGladiator.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "RenderTarget.h"

CPCOthersGladiator::CPCOthersGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
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
		m_pWeapon->Set_HierarchyDesc(m_pMeshCom->Find_HierarchyDesc("L_Sword"));
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

	m_pInfoCom->m_fSpeed     = PCOthersGladiatorConst::MIN_SPEED;
	m_pInfoCom->m_vArrivePos = m_pTransCom->m_vPos;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;

	m_ePreStance = Gladiator::STANCE_NONEATTACK;
	m_eCurStance = Gladiator::STANCE_NONEATTACK;

	// Angle Linear Interpolation Desc
	m_tAngleInterpolationDesc.interpolation_speed = 3.0f;

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

	SetUp_StanceChange(fTimeDelta);

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
	Move_OnNaviMesh(fTimeDelta);
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CPCOthersGladiator::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CPCOthersGladiator::Render_GameObject(const _float& fTimeDelta, 
										   ID3D12GraphicsCommandList* pCommandList, 
										   const _int& iContextIdx)
{
	Set_ConstantTable();
	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPCOthersGladiator::Render_ShadowDepth(const _float& fTimeDelta, 
								   ID3D12GraphicsCommandList* pCommandList, 
								   const _int& iContextIdx)
{
	Set_ConstantTableShadowDepth();
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
										 m_pMeshCom->Find_HierarchyDesc("Weapon_Back"),
										 &m_pTransCom->m_matWorld,
										 _rgba(0.64f, 0.96f, 0.97f, 1.0f));
	Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_GameObject", L"OthersWeaponTwoHand", m_pWeapon), E_FAIL);

	return S_OK;
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
	m_pTransCom->m_vDir = m_pTransCom->Get_LookVector();
	m_pTransCom->m_vDir.Normalize();

	SetUp_MoveSpeed(fTimeDelta);

	if (!m_bIsMoveStop || 
		PCOthersGladiatorConst::MIN_SPEED != m_pInfoCom->m_fSpeed)
	{
		// NaviMesh 이동.		
		if (!CServerMath::Get_Instance()->Is_Arrive_Point(m_pTransCom->m_vPos, m_pInfoCom->m_vArrivePos))
		{
			_vec3 vPos = m_pNaviMeshCom->Move_OnNaviMesh(&m_pTransCom->m_vPos,
														 &m_pTransCom->m_vDir,
														 m_pInfoCom->m_fSpeed * fTimeDelta);
			m_pTransCom->m_vPos = vPos;
			cout << m_bIsMoveStop << "   " << m_pInfoCom->m_fSpeed << endl;
		}
	}
}

void CPCOthersGladiator::SetUp_MoveSpeed(const _float& fTimeDelta)
{
	if (!m_bIsMoveStop)
		m_fLinearRatio += fTimeDelta;
	else
		m_fLinearRatio -= PCOthersGladiatorConst::MOVE_STOP_SPEED * fTimeDelta;

	m_pInfoCom->m_fSpeed = Engine::LinearInterpolation(PCOthersGladiatorConst::MIN_SPEED, 
													   PCOthersGladiatorConst::MAX_SPEED, 
													   m_fLinearRatio);
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
				m_pWeapon->Set_HierarchyDesc(m_pMeshCom->Find_HierarchyDesc("Weapon_Back"));
		}
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

void CPCOthersGladiator::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pNaviMeshCom);
}
