#include "stdafx.h"
#include "LightingParticle.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
CLightingParticle::CLightingParticle(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList), m_bisPivot(true)
{
}

HRESULT CLightingParticle::Ready_GameObject(wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_strTextag = wstrTextureTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;

	
	m_uiTexIdx = 0;
	m_tFrame = tFrame;

	return S_OK;
}

HRESULT CLightingParticle::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CLightingParticle::Update_GameObject(const _float& fTimeDelta)
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

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]	
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	/*__________________________________________________________________________________________________________
	[ Update Billboard Matrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::SetUp_BillboardMatrix();

	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	return NO_EVENT;
}

_int CLightingParticle::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CLightingParticle::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CLightingParticle::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);
	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);
	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(10), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CLightingParticle::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt = m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.fSceneCnt = m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurScene = (_float)(_int)m_tFrame.fCurScene;
	
	m_fAlpha -= Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 0.75f;
	tCB_ShaderTexture.fAlpha = m_fAlpha;
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

void CLightingParticle::Update_SpriteFrame(const _float& fTimeDelta)
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

Engine::CGameObject* CLightingParticle::Create(ID3D12Device* pGraphicDevice,
	ID3D12GraphicsCommandList* pCommandList,
	wstring wstrTextureTag,
	const _vec3& vScale,
	const _vec3& vAngle,
	const _vec3& vPos,
	const FRAME& tFrame)
{
	CLightingParticle* pInstance = new CLightingParticle(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLightingParticle::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
