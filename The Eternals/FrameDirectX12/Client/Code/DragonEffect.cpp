#include "stdafx.h"
#include "DragonEffect.h"
#include "InstancePoolMgr.h"
#include "GraphicDevice.h"
#include "ObjectMgr.h"
#include "TimeMgr.h"
#include "InstancePoolMgr.h"
#include "DescriptorHeapMgr.h"
CDragonEffect::CDragonEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CDragonEffect::Ready_GameObject(wstring wstrMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   m_pMeshCom->Get_CenterPos(),
										   m_pMeshCom->Get_MinVector(),
										   m_pMeshCom->Get_MaxVector());
	Engine::CGameObject::SetUp_BoundingSphere(&(m_pTransCom->m_matWorld),
											  m_pTransCom->m_vScale,
											  _vec3(128.0f),
											  _vec3(2.0f, 37.f, 0.0f));
	
	m_pInfoCom->m_fSpeed = 1.f;
	m_bIsMoveStop = true;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 0;
	
	return S_OK;
}

HRESULT CDragonEffect::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pDescriptorHeaps = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"EffectPublic");

	return S_OK;
}

_int CDragonEffect::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	m_fFireDecalDelta += fTimeDelta;
	if (m_bisFireDecal == false && m_fFireDecalDelta > 1.7f)
	{
	/*	CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone0", _vec3(0.f), _vec3(0.f), m_pTransCom->m_vPos, true, false, 5, 3, 0, 0, 0
			, _vec2(25, 5), 90.f, true);*/
		CEffectMgr::Get_Instance()->Effect_Explosion(m_pTransCom->m_vPos);
		m_bisFireDecal = true;
		CEffectMgr::Get_Instance()->Effect_Dust(m_pTransCom->m_vPos, 5.f, 5.f,24);
		CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.04f),
			_vec3(0.f), m_pTransCom->m_vPos, false, false, 5, 20, 11, 11, 11, _vec2(35, 6), 0, true);
		CEffectMgr::Get_Instance()->Effect_MeshParticle(L"publicStone" + to_wstring(rand() % 4), _vec3(0.02f),
			_vec3(0.f), m_pTransCom->m_vPos, false, false, 5, 20, 11, 11, 11, _vec2(25, 2), 0, true);

		_vec3 newPos = m_pTransCom->m_vPos;
		newPos.y += 4.f;
		CEffectMgr::Get_Instance()->Effect_Dust2(m_pTransCom->m_vPos, 8.f, 8.f, FRAME(10, 9, rand() % 20 + 20), 18, L"Bomb00");
	}

	if (m_bIsDead)
		return DEAD_OBJ;

	m_fAlphaDelta += fTimeDelta;
	if (m_fAlphaDelta > 2.5f)
	{
		m_fDissolve += fTimeDelta*0.6f;
		if (m_fDissolve > 1.f)
			m_bIsReturn = true;
	}

	m_pMeshCom->Set_AnimationKey(0);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);
	if (m_bIsReturn)
	{
		m_iSNum = -1;
		m_fDissolve = -0.05f;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Dragon_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_DYNAMICEFFECT, this), -1);


	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CDragonEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTable();

	return NO_EVENT;
}

void CDragonEffect::Render_GameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_DynamicMeshEffect(m_pShaderCom, m_pDescriptorHeaps, 1, 27, 3
		, 11,18);
}




//void CDragonEffect::Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx)
//{
//
//	m_pMeshCom->Render_DynamicMeshEffect(pCommandList, iContextIdx, m_pShaderCom);
//}


HRESULT CDragonEffect::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderDynamicMeshEffect*>(m_pComponentMgr->Clone_Component(L"ShaderDynamicMeshEffect", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);



	return S_OK;
}

void CDragonEffect::Set_ConstantTable()
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

	m_fDeltaTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 2;
	m_fGridTime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f;
	tCB_ShaderMesh.vLightPos      = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar  = tShadowDesc.fLightPorjFar;
	tCB_ShaderMesh.fDissolve      = abs(sin(m_fDeltaTime))+0.1f;
	tCB_ShaderMesh.fOffset1 = abs(sin(m_fDeltaTime)) + 0.1f;
	tCB_ShaderMesh.fOffset6 = m_fDissolve;
	if(m_pShaderCom->Get_UploadBuffer_ShaderMesh()!=nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
}

void CDragonEffect::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;

	m_fAlphaDelta = 0.f;
	m_fGridTime = 0.f;
	m_fAlpha = 1.f;
	m_bisFireDecal = false;
	m_fDeltaTime = 0.f;
	m_fDissolve = -0.05f;
	m_fFireDecalDelta = 0.f;
	
	m_pMeshCom->Set_AnimationTime(0.f);
}

Engine::CGameObject* CDragonEffect::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, wstring wstrMeshTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	CDragonEffect* pInstance = new CDragonEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CDragonEffect** CDragonEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, 
												 ID3D12GraphicsCommandList* pCommandList,
												 const _uint& uiInstanceCnt)
{
	CDragonEffect** ppInstance = new (CDragonEffect * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CDragonEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"DragonEffect",				// MeshTag
										_vec3(0.05f, 0.05f, 0.05f),	// Scale
										_vec3(0.0f),				// Angle
										_vec3(AWAY_FROM_STAGE));	// Pos
	}

	return ppInstance;
}

void CDragonEffect::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

}
