#include "stdafx.h"
#include "MeshParticleEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CMeshParticleEffect::CMeshParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList), m_bisPivot(true)
{
}

HRESULT CMeshParticleEffect::Ready_GameObject(wstring wstrMeshTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos, const _int& PipeLine, const _int& ParticleCnt)
{
	m_Pipeline = PipeLine;
	m_iParticleCnt = ParticleCnt;
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;

	m_pTextureHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"EffectPublic");
	m_pShaderCom->SetUp_ShaderConstantBuffer(20);
	
	m_uiTexIdx = 0;
	m_fAlpha = 1.f;
	return S_OK;
}

HRESULT CMeshParticleEffect::LateInit_GameObject()
{
	
	return S_OK;
}

_int CMeshParticleEffect::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);


	Make_ParticleTrail(fTimeDelta);
	Make_TextureTrail(fTimeDelta);
	m_fDegree += fTimeDelta * 120.f;

	m_fAlpha -= fTimeDelta * 0.1f;
	m_pTransCom->m_vAngle.y += (fTimeDelta * 60.f);
	
	if (m_fAlpha < 0.f)
		m_bIsReturn = true;
	if (m_bIsReturn)
	{
		if(m_wstrMeshTag==L"publicStone0")
			Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock0_Effect(), m_uiInstanceIdx);
		else if (m_wstrMeshTag == L"publicStone1")
			Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock1_Effect(), m_uiInstanceIdx);
		else if (m_wstrMeshTag == L"publicStone2")
			Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock2_Effect(), m_uiInstanceIdx);
		else if (m_wstrMeshTag == L"publicStone3")
			Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock3_Effect(), m_uiInstanceIdx);
		else if (m_wstrMeshTag == L"publicStone4")
			Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_MeshParticleRock4_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	
	//_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	//m_pTransCom->m_vPos +=m_pTransCom->m_vDir* (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))* m_fSpeed;
	//Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	return NO_EVENT;
}

_int CMeshParticleEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CMeshParticleEffect::Render_GameObject(const _float& fTimeDelta)
{
	if (m_pTextureHeap == nullptr)
		return;
	
	Set_ConstantTable(fTimeDelta);
}

void CMeshParticleEffect::Make_ParticleTrail(const _float& fTimeDelta)
{
	if (!m_bisMakeParticleTrail)
		return;


	
}

void CMeshParticleEffect::Make_TextureTrail(const _float& fTimeDelta)
{
	if (!m_bisMakeTexTrail)
		return;

	m_fMakeTexTrail += fTimeDelta;
	if (m_fMakeTexTrail > 0.06f)
	{
		m_fMakeTexTrail = 0.f;
		for (int i = 0; i < m_iParticleCnt; i++)
		{
		/*	CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Bomb01", _vec3(1.f), _vec3(0.f), ParticlePos[i], FRAME(10, 9, 90), false, false);
			CEffectMgr::Get_Instance()->Effect_TextureEffect(L"Dust", _vec3(1.f), _vec3(0.f), ParticlePos[i]+ m_vecRandomvector[i]
				, FRAME(12, 7, 40), false, false);*/
		}
	}
}
HRESULT CMeshParticleEffect::Add_Component(wstring wstrMeshTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(m_Pipeline), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CMeshParticleEffect::Set_ConstantTable(float fTimeDelta)
{
	_uint uiIdx = 0;


	for (uiIdx = 0; uiIdx < m_iParticleCnt; ++uiIdx)
	{

		m_fCurGravity[uiIdx] += (ParticleSpeed_Weight[uiIdx].y * fTimeDelta * 0.1f);

		ParticlePos[uiIdx] += m_vecRandomvector[uiIdx] * fTimeDelta * ParticleSpeed_Weight[uiIdx].x;
		ParticlePos[uiIdx].y -= m_fCurGravity[uiIdx];

		if (ParticlePos[uiIdx].y < 0.f)
		{	
			m_fCurGravity[uiIdx] *= 0.6f;
		}
		_matrix matScale = INIT_MATRIX;
		_matrix  matRotateX = INIT_MATRIX;
		_matrix  matRotateY = INIT_MATRIX;
		_matrix  matRotateZ = INIT_MATRIX;
		_matrix matTrans = INIT_MATRIX;
		matScale = XMMatrixScaling(ParticleScale[uiIdx].x, ParticleScale[uiIdx].y, ParticleScale[uiIdx].z);
		matTrans = XMMatrixTranslation(ParticlePos[uiIdx].x, ParticlePos[uiIdx].y, ParticlePos[uiIdx].z);
		matRotateX = XMMatrixRotationX(XMConvertToRadians(ParticleAngle[uiIdx].x));
		matRotateY = XMMatrixRotationY(XMConvertToRadians(m_pTransCom->m_vAngle.y+ ParticleAngle[uiIdx].y));
		matRotateZ = XMMatrixRotationZ(XMConvertToRadians(ParticleAngle[uiIdx].z));
		m_matWorld[uiIdx] = matScale * matRotateX * matRotateY* matRotateZ * matTrans;
		Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

		Engine::CB_SHADER_MESH tCB_ShaderMesh;
		ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
		tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(m_matWorld[uiIdx]);
		tCB_ShaderMesh.matLightView = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
		tCB_ShaderMesh.matLightProj = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
		tCB_ShaderMesh.vLightPos = tShadowDesc.vLightPosition;
		tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;
		tCB_ShaderMesh.fOffset1 = m_fAlpha;
	

		if (m_pShaderCom->Get_UploadBuffer_ShaderMesh() != nullptr)
			m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(uiIdx, tCB_ShaderMesh);

		m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pTextureHeap, m_uiDiff, m_uiNorm, m_uiSpec
			, 0, 4,uiIdx);
	}

}


void CMeshParticleEffect::Set_CreateInfo(const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos, _vec2 SpeedWeight, _bool isMakeTexTrail , _bool isMakeParticleTrail ,
	const _int& PipeLine , const _int& ParticleCnt)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;

	m_bisMakeTexTrail = isMakeTexTrail;
	m_bisMakeParticleTrail = isMakeParticleTrail;
	//Parent vScale ¿¡¼­ -30~30%  0.8~1.2
	//float random = float(rand() % 9 + 4) * 0.1f;
	for (int i = 0; i < ParticleCnt; i++)
	{
		ParticleInit[i] = false;
		ParticlePos[i] = vPos;


		m_vecRandomvector[i].x = (rand() % 400 - 200);
		m_vecRandomvector[i].y = (rand() % 150+50);
		m_vecRandomvector[i].z = (rand() % 400 - 200);
		m_vecRandomvector[i].Normalize();
		ParticleAngle[i] = _vec3(rand() % 360, rand() % 360, rand() % 360);
		ParticleScale[i] = vScale * _vec3(float(rand() % 9 + 4) * 0.1f, float(rand() % 9 + 4) * 0.1f, float(rand() % 9 + 4) * 0.1f);
		m_fCurGravity[i] = 0.f;
		ParticleSpeed_Weight[i].x = SpeedWeight.x - rand() % int(SpeedWeight.x *0.5f);
		ParticleSpeed_Weight[i].y = SpeedWeight.y + rand() % 3;
		//TempPos[i] += m_vecRandomvector[i] * (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 50.f;
	}
	m_Pipeline = PipeLine;
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(PipeLine), E_FAIL);
	m_iParticleCnt = ParticleCnt;

	m_fAlpha = 1.f;
	m_fDegree = 0.f;

}

Engine::CGameObject* CMeshParticleEffect::Create(ID3D12Device* pGraphicDevice,
	ID3D12GraphicsCommandList* pCommandList,
	wstring wstrMeshTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos, const _int& PipeLine, const _int& ParticleCnt)
{
	CMeshParticleEffect* pInstance = new CMeshParticleEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos, PipeLine, ParticleCnt)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}



CMeshParticleEffect** CMeshParticleEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, wstring MeshTag, const _uint& uiInstanceCnt)
{
	CMeshParticleEffect** ppInstance = new (CMeshParticleEffect * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CMeshParticleEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(MeshTag, _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CMeshParticleEffect::Free()
{
	Engine::CGameObject::Free();
	
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	//Engine::Safe_Release(m_pTextureCom);
}
