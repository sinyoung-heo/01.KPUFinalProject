#include "stdafx.h"
#include "ParticleEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
CParticleEffect::CParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList), m_bisPivot(true)
{
}

HRESULT CParticleEffect::Ready_GameObject(wstring wstrTextureTag,
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

HRESULT CParticleEffect::LateInit_GameObject()
{
	
	return S_OK;
}

_int CParticleEffect::Update_GameObject(const _float& fTimeDelta)
{
	m_fDegree += fTimeDelta * 120.f;
	
	m_fAlpha = sin(XMConvertToRadians(m_fDegree * 0.5f));
	

	if (m_fAlpha < 0.f)
		m_bIsReturn = true;
	if (m_bIsReturn)
	{
		m_pTextureHeap = nullptr;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_Particle_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;

	m_fDegree = (_int)m_fDegree % 360;
	/*__________________________________________________________________________________________________________
	[ Update Sprite Frame ]
	____________________________________________________________________________________________________________*/
	Update_SpriteFrame(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	
	//_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	//m_pTransCom->m_vPos +=m_pTransCom->m_vDir* (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta"))* m_fSpeed;
	//Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	return NO_EVENT;
}

_int CParticleEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CParticleEffect::Render_GameObject(const _float& fTimeDelta)
{
	if (m_pTextureHeap == nullptr)
		return;
	for (int i = 0; i < m_iParticleCnt; i++)
	{
		Set_ConstantTable(i);
		m_pShaderCom[i]->Begin_Shader(m_pTextureHeap, 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
		m_pBufferCom[i]->Begin_Buffer();
		m_pBufferCom[i]->Render_Buffer();
	}

}

HRESULT CParticleEffect::Add_Component(wstring wstrTextureTag)
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
		Engine::FAILED_CHECK_RETURN(m_pShaderCom[i]->Set_PipelineStatePass(m_Pipeline), E_FAIL);
		m_mapComponent[Engine::ID_STATIC].emplace(strShader, m_pShaderCom[i]);
	}
	//// Texture
	//m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	//Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	//m_pTextureCom->AddRef();
	//m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);
	//// Shader
	

	return S_OK;
}

void CParticleEffect::Set_ConstantTable(_int i)
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	
	
	TempPos[i] += m_vecRandomvector[i] * (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 1.6f;
	
	m_pTransCom->m_vPos = TempPos[i];
	m_pTransCom->Update_Component(0.0f);
	Engine::CGameObject::SetUp_BillboardMatrix();


	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	tCB_ShaderTexture.fFrameCnt = m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.fSceneCnt = m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurScene = (_int)m_tFrame.fCurScene;
	tCB_ShaderTexture.fAlpha = m_fAlpha;
	tCB_ShaderTexture.fOffset3 = m_fDegree;
	tCB_ShaderTexture.v_Color = m_vecColorOffset;

	if(m_pShaderCom[i]->Get_UploadBuffer_ShaderTexture()!=nullptr)
		m_pShaderCom[i]->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

void CParticleEffect::Update_SpriteFrame(const _float& fTimeDelta)
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

void CParticleEffect::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle,
	const _vec3& vPos, wstring TexTag,const FRAME& tFrame, const _int& PipeLine, const _int& ParticleCnt)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	
	if (nullptr == m_pTextureHeap)
		m_pTextureHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(TexTag);

	for (int i = 0; i < ParticleCnt; i++)
	{
		
		TempPos[i] = vPos;
		m_vecRandomvector[i].x = (rand() % 200 - 100);
		m_vecRandomvector[i].y = (rand() % 100);
		m_vecRandomvector[i].z = (rand() % 200 - 100);
		m_vecRandomvector[i].Normalize();
		TempPos[i] += m_vecRandomvector[i] * (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 50.f;
	}
	m_Pipeline = PipeLine;
	m_iParticleCnt = ParticleCnt;

	m_uiTexIdx = 0;
	m_tFrame = tFrame;
	m_fAlpha = 1.f;
	m_bisAlphaObject = true;
	m_fDegree = 0.f;

}

Engine::CGameObject* CParticleEffect::Create(ID3D12Device* pGraphicDevice,
	ID3D12GraphicsCommandList* pCommandList,
	wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame, const _int& PipeLine, const _int& ParticleCnt)
{
	CParticleEffect* pInstance = new CParticleEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame, PipeLine, ParticleCnt)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CParticleEffect** CParticleEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt, wstring TexTag)
{
	CParticleEffect** ppInstance = new (CParticleEffect * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CParticleEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(TexTag, _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CParticleEffect::Free()
{
	Engine::CGameObject::Free();
	for (_int i = 0; i < m_iParticleCnt; i++)
	{
		Engine::Safe_Release(m_pBufferCom[i]);
		Engine::Safe_Release(m_pShaderCom[i]);
	}
	//Engine::Safe_Release(m_pTextureCom);
}
