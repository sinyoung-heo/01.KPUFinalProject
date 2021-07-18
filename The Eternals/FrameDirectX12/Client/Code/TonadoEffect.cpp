#include "stdafx.h"
#include "TonadoEffect.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CTonadoEffect::CTonadoEffect(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CTonadoEffect::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;
	return S_OK;
}

HRESULT CTonadoEffect::LateInit_GameObject()
{
	m_fDeltaTime = 1.f;
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	m_pDescriptorHeaps = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"PublicMagic");
	m_uiDiffuse =8;
	m_uiNormal = 11;//NormIdx
	m_uiSpec = 11;//SpecIdx
	return S_OK;	
}

_int CTonadoEffect::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	/*
	if (m_fDeltaDegree > 360.f)
		m_bIsReturn = true;*/
	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Tonado_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}

	m_pTransCom->m_vAngle.y += 540.f*fTimeDelta;
	m_pTransCom->m_vPos.y += 2.f * fTimeDelta;
	m_pTransCom->m_vScale.x += 0.3f * fTimeDelta;
	m_pTransCom->m_vScale.y += 0.02f * fTimeDelta;
	m_pTransCom->m_vScale.z += 0.3f * fTimeDelta;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	

	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);
	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CTonadoEffect::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}


void CTonadoEffect::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	/*m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, m_uiDiffuse, m_uiNormal,m_uiSpec
		,0,4);*/
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, 31, 31, 2
		, 0, 25);
}
void CTonadoEffect::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}
HRESULT CTonadoEffect::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);


	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMeshEffect*>(m_pComponentMgr->Clone_Component(L"ShaderMeshEffect", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(5), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	m_pCrossFilterShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pCrossFilterShaderCom, E_FAIL);
	m_pCrossFilterShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pCrossFilterShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pCrossFilterShaderCom);

	return S_OK;
}

void CTonadoEffect::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	
	tCB_ShaderMesh.vLightPos = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;

	//g_fOffSet1 = AnimationUV X  // g_fOffSet3 = Degree Delta°ª
	//g_fOffSet5 = Detail°ª
	m_fDeltaTime -= Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 0.5f;
	m_fDeltaDegree += Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 180.f;
	tCB_ShaderMesh.fOffset6 = m_fDeltaDegree / 180.f;

	tCB_ShaderMesh.fOffset1 = m_fDeltaTime;
	tCB_ShaderMesh.fOffset3 = m_fDeltaDegree;
	
	tCB_ShaderMesh.fOffset5 = 1.f;

	if(m_pShaderCom->Get_UploadBuffer_ShaderMesh()!=nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);


	
}

void CTonadoEffect::Set_ConstantTableShadowDepth()
{

}

void CTonadoEffect::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_fDeltaDegree = 0.f;
	m_fDeltaTime = 1.f;
}

Engine::CGameObject* CTonadoEffect::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos)
{
	CTonadoEffect* pInstance = new CTonadoEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CTonadoEffect** CTonadoEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CTonadoEffect** ppInstance = new (CTonadoEffect * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CTonadoEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"publicTonado", _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CTonadoEffect::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
}
