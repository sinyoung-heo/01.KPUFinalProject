#include "stdafx.h"
#include "DistTrail.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CDistTrail::CDistTrail(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CDistTrail::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	
	return S_OK;
}

HRESULT CDistTrail::LateInit_GameObject()
{

	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	m_pDescriptorHeaps = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"EffectPublic");

	m_uiDiffuse =8;
	m_fNormalMapDeltatime = 8;//NormIdx
	m_fPatternMapDeltatime = 8;//SpecIdx
	return S_OK;	
}

_int CDistTrail::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_DistTrail_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	
	m_fAlpha -= fTimeDelta;
	m_fOffsetTime += fTimeDelta * 0.15f;

	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime > 0.15f)
	{
		m_pTransCom->m_vScale += (_vec3(0.3f-m_fOffSet*0.001f)- _vec3(m_fOffsetTime)) * fTimeDelta;
		if (m_fLifeTime > 2.f)
			m_bIsReturn = true;
	}
	
	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	if(m_bisCrossFilter)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_CROSSFILTER, this), -1);


	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);
	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CDistTrail::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}


void CDistTrail::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, m_uiDiffuse, m_fNormalMapDeltatime, m_fPatternMapDeltatime
		,0,4);
}
void CDistTrail::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}
HRESULT CDistTrail::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(6), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	m_pCrossFilterShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pCrossFilterShaderCom, E_FAIL);
	m_pCrossFilterShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pCrossFilterShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pCrossFilterShaderCom);

	return S_OK;
}

void CDistTrail::Set_ConstantTable()
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
	tCB_ShaderMesh.fOffset1 = m_fAlpha;
	
	tCB_ShaderMesh.vLightPos = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;

	
	if(m_pShaderCom->Get_UploadBuffer_ShaderMesh()!=nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);


	if (m_bisCrossFilter)
	{
		m_fDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 1.5f * m_fDeltatimeVelocity;
		m_fDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 1.5f * m_fDeltatimeVelocity2;
		tCB_ShaderMesh.fOffset1 = sin(m_fDeltatime);//¹øÁüÈ¿°ú
		tCB_ShaderMesh.fOffset2 = m_fDeltatime2;
		tCB_ShaderMesh.fOffset3 = m_fDeltatime3;

		if (m_fLifeTime > 1.f)
		{
			m_fDeltatime2 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 1.5f;
			tCB_ShaderMesh.fDissolve = m_fDeltatime2;
			if (!m_bisLifeInit)
			{
				m_bisLifeInit = true;
				m_fDeltatimeVelocity2 = 9;
			}
		}
		tCB_ShaderMesh.vEmissiveColor.x = 135.f / 255.f;
		tCB_ShaderMesh.vEmissiveColor.y = 150.f / 255.f;
		tCB_ShaderMesh.vEmissiveColor.z = (rand() % 100 + 155.f) / 255.f;
		tCB_ShaderMesh.vEmissiveColor.w = 1.f;

		m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
		if (m_fDeltatime > 1.f)
			m_fDeltatimeVelocity = -1.f;
		else if (m_fDeltatime < 0.f)
			m_fDeltatimeVelocity = 1.f;

		if (m_fDeltatime3 > 1.f)
			m_fDeltatimeVelocity2 = -1.f;
		else if (m_fDeltatime3 < 0.f)
		{
			m_fDeltatime3 = 0.0f;
			m_fDeltatimeVelocity2 = 0.f;
			m_bisCrossFilter = false;
		}
	}
	
}
void CDistTrail::Set_ConstantTableShadowDepth()
{

}
void CDistTrail::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_pTransCom->m_vPos.y += 0.1f;
	
	m_fDeltatime = 0.f;
	m_fDeltatime2 = 0.f;
	m_fDeltatime3 = 0.f;
	m_fPatternMapDeltatime = 0.f;
	m_fAlpha = 1.f;
	m_fOffSet = 0.f;
	m_fDeltatimeVelocity = 0.f;
	m_fDeltatimeVelocity2 = 1.f;
	m_fLifeTime = 0.f;
	m_fOffsetTime = 0.f;
	m_bisLifeInit = false;
	m_bisCrossFilter = false;
}

Engine::CGameObject* CDistTrail::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos)
{
	CDistTrail* pInstance = new CDistTrail(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CDistTrail** CDistTrail::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CDistTrail** ppInstance = new (CDistTrail * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CDistTrail(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"DistTrail", _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CDistTrail::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
//	Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
}
