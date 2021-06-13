#include "stdafx.h"
#include "NormalMonsterHpGauge.h"
#include "DescriptorHeapMgr.h"
#include "GraphicDevice.h"

CNormalMonsterHpGauge::CNormalMonsterHpGauge(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CNormalMonsterHpGauge::Ready_GameObject(const _vec3& vPos, const _vec3& vScale)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vPos		= vPos;
	m_pTransCom->m_vScale	= vScale;
	m_UIDepth				= 1'000;

	m_wstrTextureTag     = L"S1UI_GageBoss";
	m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag);;
	m_uiTexIdx           = 2;

	m_tFrame.fFrameCnt = 1.0f;
	m_tFrame.fSceneCnt = 1.0f;
	m_tFrame.fCurFrame = 0.0f;
	m_tFrame.fCurScene = 0.0f;

	return S_OK;
}

HRESULT CNormalMonsterHpGauge::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	m_pBackShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CNormalMonsterHpGauge::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (!m_bIsActive)
		return NO_EVENT;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	Engine::CGameObject::SetUp_BillboardMatrix();
	Engine::CGameObject::Compute_ViewZ(_vec4(m_pTransCom->m_vPos, 1.0f));

	return NO_EVENT;
}

_int CNormalMonsterHpGauge::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	if (!m_bIsActive)
		return NO_EVENT;

	return NO_EVENT;
}

void CNormalMonsterHpGauge::Render_GameObject(const _float& fTimeDelta)
{
	if (nullptr != m_pTexDescriptorHeap)
	{
		Set_ConstantTable();

		// Back
		m_pBackShaderCom->Begin_Shader(m_pTexDescriptorHeap, 0, 3, Engine::MATRIXID::PROJECTION);
		m_pBackBufferCom->Begin_Buffer();
		m_pBackBufferCom->Render_Buffer();

		// Gauge
		m_pShaderCom->Begin_Shader(m_pTexDescriptorHeap, 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
		m_pBufferCom->Begin_Buffer();
		m_pBufferCom->Render_Buffer();
	}
}

HRESULT CNormalMonsterHpGauge::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(6);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Buffer
	m_pBackBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBackBufferCom, E_FAIL);
	m_pBackBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_BackBuffer", m_pBackBufferCom);

	// Shader
	m_pBackShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBackShaderCom, E_FAIL);
	m_pBackShaderCom->AddRef();
	m_pBackShaderCom->Set_PipelineStatePass(6);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_BackShader", m_pBackShaderCom);

	return S_OK;
}

void CNormalMonsterHpGauge::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurFrame = m_tFrame.fCurFrame;
	tCB_ShaderTexture.fCurScene = m_tFrame.fCurScene;
	tCB_ShaderTexture.fGauge    = m_fPercent;

	if (nullptr != m_pShaderCom->Get_UploadBuffer_ShaderTexture())
		m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);


	m_pTransCom->m_matWorld._22 *= 1.75f;
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fGauge   = 1.0f;

	if (nullptr != m_pBackShaderCom->Get_UploadBuffer_ShaderTexture())
		m_pBackShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

Engine::CGameObject* CNormalMonsterHpGauge::Create(ID3D12Device* pGraphicDevice, 
												   ID3D12GraphicsCommandList* pCommandList,
												   const _vec3& vPos, 
												   const _vec3& vScale)
{
	CNormalMonsterHpGauge* pInstance = new CNormalMonsterHpGauge(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vPos, vScale)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CNormalMonsterHpGauge::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pBackBufferCom);
	Engine::Safe_Release(m_pBackShaderCom);
}
