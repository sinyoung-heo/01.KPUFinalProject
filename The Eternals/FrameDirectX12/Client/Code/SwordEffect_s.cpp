#include "stdafx.h"
#include "SwordEffect_s.h"
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
#include "TextureEffect.h"
CSwordEffect_s::CSwordEffect_s(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CSwordEffect_s::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos, const _vec3& vDir)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);
	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vPos = vPos;
	return S_OK;
}

HRESULT CSwordEffect_s::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	m_pDescriptorHeaps = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"EffectPublic");
	m_pCrossFilterShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	return S_OK;
}

_int CSwordEffect_s::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);
	if (m_bIsDead)
		return DEAD_OBJ;
	
	m_fAlpha -= fTimeDelta;
	float Curdist = m_pTransCom->m_vPos.Get_Distance(m_vecParentPos);
	if (Curdist < 8.f)
		m_bIsReturn = true;

	if (m_bIsReturn)
	{
		CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, 10, L"Lighting0");
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Sword_s_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;


	m_pTransCom->m_vPos += (m_pTransCom->m_vDir * m_fDistance * fTimeDelta);
	if (m_fLifeTime < 2.5f && m_bisScaleAnim && m_pTransCom->m_vScale.x < 0.24f)
	{
		m_pTransCom->m_vScale += _vec3(fTimeDelta * 0.24);
		m_pTransCom->m_vScale.y += fTimeDelta * 0.24;
	}
	m_fLifeTime += fTimeDelta;
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_CROSSFILTER, this), -1);


	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	Engine::CGameObject::Reset_Collider();
	m_pTransCom->Update_Component(fTimeDelta);

	return NO_EVENT;
}

_int CSwordEffect_s::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	return NO_EVENT;
}


void CSwordEffect_s::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps,0,27, 16,17,18); //D  N S Sha Dis
}
void CSwordEffect_s::Render_CrossFilterGameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pMeshCom->Render_StaticMesh(m_pCrossFilterShaderCom);
}
HRESULT CSwordEffect_s::Add_Component(wstring wstrMeshTag)
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

void CSwordEffect_s::Set_ConstantTable()
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
	tCB_ShaderMesh.fOffset3 = m_fAlpha;
	tCB_ShaderMesh.fOffset4 = m_fShaderDegree;
	tCB_ShaderMesh.fOffset6 = abs(sin(m_fDeltatime3)) +0.2f;
	tCB_ShaderMesh.vAfterImgColor = _rgba(0.5, 0.5f, 0.5f, 1);
	tCB_ShaderMesh.vEmissiveColor = _rgba(1.f, 1.f, 1.f, 1);
	tCB_ShaderMesh.vColorOffSet1 = _rgba(102.f / 255.f, 36.f / 255.f, 239.f/255.f, 0.5);

	if (m_pShaderCom->Get_UploadBuffer_ShaderMesh() != nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);

	m_fCrossDeltatime += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f * m_fDeltatimeVelocity;
	m_fCrossDeltatime3 += (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.5f * m_fDeltatimeVelocity2;
	tCB_ShaderMesh.fOffset1 = sin(m_fCrossDeltatime);//¹øÁüÈ¿°ú
	tCB_ShaderMesh.fOffset2 = m_fCrossDeltatime2;
	tCB_ShaderMesh.fOffset3 = m_fCrossDeltatime3;
	tCB_ShaderMesh.vEmissiveColor= _rgba(102.f / 255.f, 36.f / 255.f, 239.f / 255.f, 1.f);
	if (m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh() != nullptr)
		m_pCrossFilterShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);
	if (m_fCrossDeltatime > 1.f)
		m_fDeltatimeVelocity = -1.f;
	else if (m_fCrossDeltatime < 0.f)
		m_fDeltatimeVelocity = 1.f;
	if (m_fCrossDeltatime3 > 1.f)
		m_fDeltatimeVelocity2 = -1.f;
	else if (m_fCrossDeltatime3 < 0.f)
	{
		m_fCrossDeltatime3 = 0.0f;
		m_fDeltatimeVelocity2 = 0.f;
	}
}
void CSwordEffect_s::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, const _vec3& vDir)
{
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vPos = vPos;

	_vec3 PlayerPos = m_pObjectMgr->Get_GameObject(L"Layer_GameObject", L"ThisPlayer")->Get_Transform()->Get_PositionVector();
	m_vecParentPos = PlayerPos + vDir * 15.f;
	m_pTransCom->m_vDir = _vec3(m_vecParentPos - vPos);
	m_pTransCom->m_vDir.Normalize();
	
	m_fDistance = m_vecParentPos.Get_Distance(vPos); 

	m_fDeltatime = -1.f;
	m_fDeltatime2 = 0.f;
	m_fDeltatime3 = 0.f;
	m_fDegree = 0.f;
	m_fShaderDegree = 0.f;
	m_fLifeTime = 0.f;
	m_fCrossDeltatime = 0.f;
	m_fCrossDeltatime2 = 0.f;
	m_fCrossDeltatime3 = 0.f;
	m_fDeltatimeVelocity = 0.f;
	m_fDeltatimeVelocity2 = 1.f;
	m_fAlpha = 1.f;


	m_pTransCom->m_vAngle.y = XMConvertToDegrees(atan2(m_pTransCom->m_vPos.x - m_vecParentPos.x
		, m_pTransCom->m_vPos.z - m_vecParentPos.z)) + 180.f;
}

Engine::CGameObject* CSwordEffect_s::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos, const _vec3& vDir)
{
	CSwordEffect_s* pInstance = new CSwordEffect_s(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, vDir)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CSwordEffect_s** CSwordEffect_s::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CSwordEffect_s** ppInstance = new (CSwordEffect_s * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CSwordEffect_s(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"publicSword", _vec3(0.f), _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}

	return ppInstance;
}

void CSwordEffect_s::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
	//Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pCrossFilterShaderCom);
}
