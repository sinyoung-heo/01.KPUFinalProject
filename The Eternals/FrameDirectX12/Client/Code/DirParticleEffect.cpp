#include "stdafx.h"
#include "DirParticleEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CDirParticleEffect::CDirParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CDirParticleEffect::Ready_GameObject(wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame,const _int& PipeLine, const _int& ParticleCnt)
{
	m_Pipeline = PipeLine;
	m_iParticleCnt = ParticleCnt;
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_strTextag = wstrTextureTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	

	m_pShaderCom->SetUp_ShaderConstantBuffer(DIRPARTICLECNT);

	m_uiTexIdx = 0;
	m_tFrame = tFrame;
	m_fAlpha = 1.f;
	m_bisAlphaObject = true;
	return S_OK;
}

HRESULT CDirParticleEffect::LateInit_GameObject()
{
	
	return S_OK;
}

_int CDirParticleEffect::Update_GameObject(const _float& fTimeDelta)
{

	m_fAlpha -= fTimeDelta;
	if (m_fAlpha < 0.f)
		m_bIsReturn = true;
	if (m_bIsReturn)
	{
		m_pTextureHeap = nullptr;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_DirParticle_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	
	
	return NO_EVENT;
}

_int CDirParticleEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CDirParticleEffect::Render_GameObject(const _float& fTimeDelta)
{
	if (m_pTextureHeap == nullptr)
		return;

	Set_ConstantTable(fTimeDelta);

}

HRESULT CDirParticleEffect::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	wstring strShader = L"Com_Shader";
	wstring strBuffer = L"Com_Buffer";
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(strBuffer, m_pBufferCom);

	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(2), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(strShader, m_pShaderCom);

	return S_OK;
}

void CDirParticleEffect::Set_ConstantTable(float fTimeDelta)
{
	_uint uiIdx = 0;

	for (uiIdx = 0; uiIdx < m_iParticleCnt; ++uiIdx)
	{
		Update_SpriteFrame(fTimeDelta, &m_ParticleFrame[uiIdx]);

		ParticlePos[uiIdx] += m_vecRandomvector[uiIdx] * fTimeDelta * m_fDist[uiIdx];
		
		m_fCurDist[uiIdx] = ParticlePos[uiIdx].Get_Distance(m_DstPos);
		_matrix matScale = INIT_MATRIX;
		_matrix  matRotateX = INIT_MATRIX;
		_matrix  matRotateY = INIT_MATRIX;
		_matrix  matRotateZ = INIT_MATRIX;
		_matrix matTrans = INIT_MATRIX;

		ParticleAlpha[uiIdx] = 1-(m_fCurDist[uiIdx] / m_fDist[uiIdx]);
		float Ratio = 1.f - (m_fCurDist[uiIdx] / m_fDist[uiIdx]);
		ParticleScale[uiIdx] = _vec3(Ratio, Ratio,0.f);
		ParticleScale[uiIdx] *= 2.f;
		matScale = XMMatrixScaling(ParticleScale[uiIdx].x, ParticleScale[uiIdx].y, ParticleScale[uiIdx].z);
		matTrans = XMMatrixTranslation(ParticlePos[uiIdx].x, ParticlePos[uiIdx].y, ParticlePos[uiIdx].z);
		matRotateX = XMMatrixRotationX(XMConvertToRadians(ParticleAngle[uiIdx].x));
		matRotateY = XMMatrixRotationY(XMConvertToRadians(ParticleAngle[uiIdx].y));
		matRotateZ = XMMatrixRotationZ(XMConvertToRadians(ParticleAngle[uiIdx].z));
		m_matWorld[uiIdx] = matScale  * matTrans;

		SetUp_BillboardMatrix(&m_matWorld[uiIdx],ParticleScale[uiIdx]);

		Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();
		Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
		ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
		tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_matWorld[uiIdx]);
		tCB_ShaderTexture.fFrameCnt = m_ParticleFrame[uiIdx].fFrameCnt;
		tCB_ShaderTexture.fCurFrame = (_float)(_int)m_ParticleFrame[uiIdx].fCurFrame;
		tCB_ShaderTexture.fSceneCnt = m_ParticleFrame[uiIdx].fSceneCnt;
		tCB_ShaderTexture.fCurScene = (_float)(_int)m_ParticleFrame[uiIdx].fCurScene;
		tCB_ShaderTexture.fAlpha = ParticleAlpha[uiIdx];
		tCB_ShaderTexture.v_Color = _rgba(-0.2f,-0.2f,-0.2f,0.f);


		if (m_pShaderCom->Get_UploadBuffer_ShaderTexture() != nullptr)
			m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(uiIdx, tCB_ShaderTexture);
		 
		m_pShaderCom->Begin_Shader(m_pTextureHeap, uiIdx, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
		m_pBufferCom->Begin_Buffer();
		m_pBufferCom->Render_Buffer();
	}
}

void CDirParticleEffect::Update_SpriteFrame(const _float& fTimeDelta,FRAME * pFrame)
{
	pFrame->fCurFrame += fTimeDelta * pFrame->fFrameSpeed;

	// Sprite XÃà
	
	if (pFrame->fCurFrame > pFrame->fFrameCnt)
	{
		pFrame->fCurFrame = 0.0f;
		pFrame->fCurScene += 1.0f;
	}

	// Sprite YÃà
	if (pFrame->fCurScene >= pFrame->fSceneCnt)
	{
		pFrame->fCurScene = 0.0f;/*
		m_bIsReturn = true;*/
	}

}

void CDirParticleEffect::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle,
	const _vec3& vPos, wstring TexTag,
	const _vec3& StartPos,
	const _vec3& DstPos ,const FRAME& tFrame, const _int& PipeLine, const _int& ParticleCnt)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	
	
	if (nullptr == m_pTextureHeap)
		m_pTextureHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(TexTag);


	m_DstPos = DstPos;
	for (int i = 0; i < ParticleCnt; i++)
	{
		m_fRandomVelocity[i] = (rand() % 300 + 300)*0.003f;
		ParticlePos[i] = StartPos;
		ParticleScale[i] = _vec3(0.f);
		ParticleAngle[i] = vAngle;
		m_ParticleFrame[i] = tFrame;

		m_vecRandomvector[i].x = DstPos.x + ((rand() % 100) - 50.f)*0.1f ;
		m_vecRandomvector[i].y = 0;
		m_vecRandomvector[i].z = DstPos.z + ((rand() % 100) - 50.f)*0.1f;
		
		m_vecRandomvector[i] = m_vecRandomvector[i] - StartPos;
		m_vecRandomvector[i].Normalize();
		ParticleAlpha[i] = 1.f;
		_vec3 Temp = StartPos;
		m_fDist[i] = Temp.Get_Distance(DstPos);
	}
	m_Pipeline = PipeLine;
	m_iParticleCnt = ParticleCnt;

	m_uiTexIdx = 0;
	m_tFrame = tFrame;
	m_fAlpha = 1.f;
	m_bisAlphaObject = true;

}

Engine::CGameObject* CDirParticleEffect::Create(ID3D12Device* pGraphicDevice,
	ID3D12GraphicsCommandList* pCommandList,
	wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame, const _int& PipeLine, const _int& ParticleCnt)
{
	CDirParticleEffect* pInstance = new CDirParticleEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame, PipeLine, ParticleCnt)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CDirParticleEffect** CDirParticleEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt, wstring TexTag)
{
	CDirParticleEffect** ppInstance = new (CDirParticleEffect * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CDirParticleEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(TexTag, _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CDirParticleEffect::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);

}
