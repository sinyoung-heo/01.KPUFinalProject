#include "stdafx.h"
#include "TrailTexEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CTrailTexEffect::CTrailTexEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CTrailTexEffect::Ready_GameObject(wstring wstrTextureTag,
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
	m_pTextureHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(wstrTextureTag);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	for (int i = 0; i < ParticleCnt; i++)
	{
		m_pShaderCom[i]->SetUp_ShaderConstantBuffer();

		TempPos[i] = vPos;
	}
	m_uiTexIdx = 0;
	m_tFrame = tFrame;
	m_fAlpha = 1.f;
	m_bisAlphaObject = true;
	return S_OK;
}

HRESULT CTrailTexEffect::LateInit_GameObject()
{
	
	return S_OK;
}

_int CTrailTexEffect::Update_GameObject(const _float& fTimeDelta)
{
	
	if (m_bIsReturn)
	{
		m_pTextureHeap = nullptr;
		//Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Particle_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Update Sprite Frame ]
	____________________________________________________________________________________________________________*/
	Update_SpriteFrame(fTimeDelta);

	idx += fTimeDelta*(m_iParticleCnt*2.f);
	idx = (int)idx % (m_iParticleCnt);
	Set_ConstantTable(idx);
	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	
	return NO_EVENT;
}

_int CTrailTexEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CTrailTexEffect::Render_GameObject(const _float& fTimeDelta)
{
	if (m_pTextureHeap == nullptr)
		return;
	for (int i = 0; i < m_iParticleCnt; i++)
	{
		
		m_pShaderCom[i]->Begin_Shader(m_pTextureHeap, 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
		m_pBufferCom[i]->Begin_Buffer();
		m_pBufferCom[i]->Render_Buffer();
	}

}

HRESULT CTrailTexEffect::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	for (int i = 0; i < m_iParticleCnt; i++)
	{
		wstring strShader = L"Com_Shader" + to_wstring(i);
		wstring strBuffer = L"Com_Buffer" + to_wstring(i);
		m_pBufferCom[i] = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
		Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
		m_pBufferCom[i]->AddRef();
		m_mapComponent[Engine::ID_STATIC].emplace(strBuffer, m_pBufferCom[i]);

		m_pShaderCom[i] = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
		Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
		m_pShaderCom[i]->AddRef();
		Engine::FAILED_CHECK_RETURN(m_pShaderCom[i]->Set_PipelineStatePass(2), E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(strShader, m_pShaderCom[i]);
	}


	return S_OK;
}

void CTrailTexEffect::Set_ConstantTable(_int i)
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	
	_vec3 Dir;
	if (i == 0)
	{
		Dir = *m_pParentTransform - TempPos[i];
		Dir.Normalize();
		m_pTransCom->m_vPos += Dir;
		TempPos[i] = m_pTransCom->m_vPos;
	}
	else
	{
		Dir = TempPos[i - 1]-TempPos[i] ;
		Dir.Normalize();
		m_pTransCom->m_vPos += Dir;
		TempPos[i] = m_pTransCom->m_vPos;
	}
	m_pTransCom->Update_Component(0.0f);
	Engine::CGameObject::SetUp_BillboardMatrix();


	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	tCB_ShaderTexture.fFrameCnt = m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.fSceneCnt = m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurScene = (_int)m_tFrame.fCurScene;
	tCB_ShaderTexture.fAlpha = m_fAlpha;

	if(m_pShaderCom[i]->Get_UploadBuffer_ShaderTexture()!=nullptr)
		m_pShaderCom[i]->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

void CTrailTexEffect::Update_SpriteFrame(const _float& fTimeDelta)
{
	m_tFrame.fCurFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	// Sprite XÃà
	if (m_tFrame.fCurFrame > m_tFrame.fFrameCnt)
	{
		m_tFrame.fCurFrame = 0.0f;
		m_tFrame.fCurScene += 1.0f;
	}

	// Sprite YÃà
	if (m_tFrame.fCurScene >= m_tFrame.fSceneCnt)
	{
		m_tFrame.fCurScene = 0.0f;
	}

}

void CTrailTexEffect::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle,
	const _vec3& vPos, wstring TexTag, Engine::CTransform* ParentTransform ,const FRAME& tFrame, const _int& PipeLine, const _int& ParticleCnt)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	
	if (nullptr == m_pTextureHeap)
		m_pTextureHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(TexTag);

	for (int i = 0; i < ParticleCnt; i++)
	{
		TempPos[i] = vPos;
	
	}
	m_Pipeline = PipeLine;
	m_iParticleCnt = ParticleCnt;

	m_uiTexIdx = 0;
	m_tFrame = tFrame;
	m_fAlpha = 1.f;
	m_bisAlphaObject = true;

}

Engine::CGameObject* CTrailTexEffect::Create(ID3D12Device* pGraphicDevice,
	ID3D12GraphicsCommandList* pCommandList,
	wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame,
	const _int& PipeLine, const _int& ParticleCnt)
{
	CTrailTexEffect* pInstance = new CTrailTexEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame, PipeLine, ParticleCnt)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CTrailTexEffect** CTrailTexEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt, wstring TexTag)
{
	CTrailTexEffect** ppInstance = new (CTrailTexEffect * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CTrailTexEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(TexTag, _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CTrailTexEffect::Free()
{
	Engine::CGameObject::Free();
	for (_int i = 0; i < m_iParticleCnt; i++)
	{
		Engine::Safe_Release(m_pBufferCom[i]);
		Engine::Safe_Release(m_pShaderCom[i]);
	}
	//Engine::Safe_Release(m_pTextureCom);
}
