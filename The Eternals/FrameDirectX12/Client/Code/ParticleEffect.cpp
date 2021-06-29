#include "stdafx.h"
#include "ParticleEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
CParticleEffect::CParticleEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList), m_bisPivot(true)
{
}

HRESULT CParticleEffect::Ready_GameObject(wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame,const int PipeLine)
{
	m_Pipeline = PipeLine;
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_strTextag = wstrTextureTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	for (int i = 0; i < PARTICLECNT; i++)
		TempPos[i] = vPos;
	
	m_uiTexIdx = 0;
	m_tFrame = tFrame;
	return S_OK;
}

HRESULT CParticleEffect::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	for (int i = 0; i < PARTICLECNT; i++)
	{
		m_pShaderCom[i]->SetUp_ShaderConstantBuffer();

		m_vecRandomvector[i].x = (rand() % 200 - 100);
		m_vecRandomvector[i].y = (rand() % 100);
		m_vecRandomvector[i].z = (rand() % 200 - 100);
		m_vecRandomvector[i].Normalize();
		
	}
	
	return S_OK;
}

_int CParticleEffect::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_fAlpha<0.f)
		return DEAD_OBJ;

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

	for (int i = 0; i < PARTICLECNT; i++)
	{
		Set_ConstantTable(i);
		m_pShaderCom[i]->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
		m_pBufferCom[i]->Begin_Buffer();
		m_pBufferCom[i]->Render_Buffer();
	}

}

HRESULT CParticleEffect::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	for (int i = 0; i < PARTICLECNT; i++)
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
	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);
	// Shader
	

	return S_OK;
}

void CParticleEffect::Set_ConstantTable(int i)
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	
	
	TempPos[i] += m_vecRandomvector[i] * (Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta")) * 0.8f;
	
	m_pTransCom->m_vPos = TempPos[i];
	m_pTransCom->Update_Component(0.0f);
	Engine::CGameObject::SetUp_BillboardMatrix();


	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	tCB_ShaderTexture.fFrameCnt = m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.fSceneCnt = m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurScene = (_int)m_tFrame.fCurScene;
	m_fAlpha = 0.5f;
	tCB_ShaderTexture.fAlpha = m_fAlpha;
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

Engine::CGameObject* CParticleEffect::Create(ID3D12Device* pGraphicDevice,
	ID3D12GraphicsCommandList* pCommandList,
	wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame, const int PipeLine)
{
	CParticleEffect* pInstance = new CParticleEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame, PipeLine)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CParticleEffect::Free()
{
	Engine::CGameObject::Free();
	for (int i = 0; i < PARTICLECNT; i++)
	{
		Engine::Safe_Release(m_pBufferCom[i]);
		Engine::Safe_Release(m_pShaderCom[i]);
	}
	Engine::Safe_Release(m_pTextureCom);
}
