#include "stdafx.h"
#include "EffectShield.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "SnowParticle.h"
#include "DescriptorHeapMgr.h"
#include "ParticleEffect.h"
CEffectShield::CEffectShield(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CEffectShield::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos,
											 const float& vAngleOffset)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vPos = vPos;

	m_fDeltatime = -1.f;
	m_fAngleOffset = vAngleOffset;

	m_fLimitLifeTime = 5.5f;
	m_fLimitScale = 0.12f;
	m_fRadius = 3.f;
	return S_OK;
}

HRESULT CEffectShield::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	m_pDescriptorHeaps = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"EffectPublic");
	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CEffectShield::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;
	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_ShieldEffect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	m_fDegree += 60.f*fTimeDelta ;
	_vec3 ParentPos = m_pParentTransform->m_vPos;
	m_pTransCom->m_vPos.x = ParentPos.x + m_fRadius * cos(XMConvertToRadians(m_fDegree+m_fAngleOffset));
	m_pTransCom->m_vPos.z = ParentPos.z + m_fRadius * sin(XMConvertToRadians(m_fDegree+m_fAngleOffset));
	m_pTransCom->m_vAngle.y = XMConvertToDegrees( atan2(m_pTransCom->m_vPos.x- ParentPos.x
		, m_pTransCom->m_vPos.z  - ParentPos.z));


	if (m_fLifeTime < m_fLimitLifeTime && m_bisScaleAnim && m_pTransCom->m_vScale.x < m_fLimitScale)
		m_pTransCom->m_vScale += _vec3(fTimeDelta * m_fLimitScale);
	m_fLifeTime += fTimeDelta;
	if (m_fLifeTime > m_fLimitLifeTime )
	{
		m_pTransCom->m_vScale -= _vec3(fTimeDelta * m_fLimitScale);
		if (m_pTransCom->m_vScale.x < 0.00)
			m_bIsReturn = true;
	}
	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_CROSSFILTER, this), -1);

	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/


	
	//m_pTransCom->m_vPos.z += 0.3f;

	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CEffectShield::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}


void CEffectShield::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps,0,27, 16,17,18);
	//D  N S Sha Dis
}
void CEffectShield::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}
HRESULT CEffectShield::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(11), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);


	m_pCrossFilterShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pCrossFilterShaderCom, E_FAIL);
	m_pCrossFilterShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pCrossFilterShaderCom->Set_PipelineStatePass(3), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader2", m_pCrossFilterShaderCom);

	return S_OK;
}

void CEffectShield::Set_ConstantTable()
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

	m_fDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))* 0.5f;
	m_fDeltatime2 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"));
	m_fDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))*1;
	m_fShaderDegree += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 60.f;
	m_fShaderDegree = int(m_fShaderDegree) % 360;
	tCB_ShaderMesh.fOffset1 = m_fDeltatime;
	tCB_ShaderMesh.fOffset2 = m_fDeltatime2;
	tCB_ShaderMesh.fOffset3 = 0.5f;
	tCB_ShaderMesh.fOffset4 = m_fShaderDegree;
	tCB_ShaderMesh.fOffset6 = abs(sin(m_fDeltatime3)) +0.2f;

	tCB_ShaderMesh.vAfterImgColor = _rgba(0.5, 0.3f, 0.f, 1);
	tCB_ShaderMesh.vEmissiveColor = _rgba(0.8f, 0.2f, 0.f, 1);
	tCB_ShaderMesh.vColorOffSet1  =  _rgba(251.f / 255.f, 130.f / 255.f, 0.f, 0.5);
	if (m_pShaderCom->Get_UploadBuffer_ShaderMesh() != nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);

	if (m_fLifeTime > 4.f)
	{
		m_fCrossDeltatime2 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f;
		tCB_ShaderMesh.fDissolve = m_fCrossDeltatime2;
		if (!m_bisLifeInit)
		{
			m_bisLifeInit = true;
			m_fDeltatimeVelocity2 = 3;

			CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, m_iParticleCnt, L"Lighting1", _vec3(0.2f));

		}
	}


	m_fCrossDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f * m_fDeltatimeVelocity;
	m_fCrossDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f * m_fDeltatimeVelocity2;
	tCB_ShaderMesh.fOffset1 = sin(m_fCrossDeltatime);//¹øÁüÈ¿°ú
	tCB_ShaderMesh.fOffset2 = m_fCrossDeltatime2;
	tCB_ShaderMesh.fOffset3 = m_fCrossDeltatime3;
	tCB_ShaderMesh.vEmissiveColor.x = 0.5;
	tCB_ShaderMesh.vEmissiveColor.y = 0.3f;
	tCB_ShaderMesh.vEmissiveColor.z = 0.f;
	tCB_ShaderMesh.vEmissiveColor.w = 1.f;
	if (m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh() != nullptr)
		m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	if (m_fCrossDeltatime > 1.f)
		m_fDeltatimeVelocity = -1.f;
	else if (m_fCrossDeltatime < 0.f)
		m_fDeltatimeVelocity = 1.f;
	if (m_fCrossDeltatime3 > 1.f)
	{
		m_fDeltatimeVelocity2 = -1.f;
		if (m_bisMini)
			CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, 1, L"Lighting1", _vec3(0.1f));
	}
	else if (m_fCrossDeltatime3 < 0.f)
	{
		m_fCrossDeltatime3 = 0.0f;
		m_fDeltatimeVelocity2 = 1.f;
	}
}
void CEffectShield::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, 
	float fLimitLifeTime, float fLimitScale, float fRadius, const float& vAngleOffset
	, const Engine::CTransform* ParentTransform, bool isMini)
{
	m_pParentTransform = ParentTransform;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vPos = vPos;

	m_fAngleOffset = vAngleOffset;

	m_fDeltatime = -1.f;
	m_fDeltatime2 = 0.f;
	m_fDeltatime3 = 0.f;
	m_fDegree = 0.f;
	m_fShaderDegree = 0.f;
	m_fLifeTime = 0.f;
	m_fCrossDeltatime = 0.f;
	m_fCrossDeltatime2 = 0.f;
	m_fCrossDeltatime3 = 0.f;
	m_bisLifeInit = false;
	m_fDeltatimeVelocity = 0.f;
	m_fDeltatimeVelocity2 = 1.f;

	m_fLimitLifeTime = fLimitLifeTime;
	m_fLimitScale = fLimitScale;
	m_fRadius = fRadius;
	m_bisScaleAnim = true;
	m_bisMini = isMini;

	m_bisMini == true ? m_iParticleCnt = 20 : m_iParticleCnt = 4;

	CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, m_iParticleCnt, L"Lighting1", _vec3(0.2f));
}

Engine::CGameObject* CEffectShield::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos, const float& vAngleOffset)
{
	CEffectShield* pInstance = new CEffectShield(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, vAngleOffset)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CEffectShield** CEffectShield::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CEffectShield** ppInstance = new (CEffectShield * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CEffectShield(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"publicShield", _vec3(0.f), _vec3(0.f), _vec3(0.f),0.f);
	}
	return ppInstance;
}

void CEffectShield::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	//Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
}
