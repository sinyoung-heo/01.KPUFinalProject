#include "stdafx.h"
#include "ToolUICanvas.h"

CToolUICanvas::CToolUICanvas(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CToolUICanvas::Ready_GameObject(const _vec3& vPos, 
										const _vec3& vScale, 
										const _long& iUIDepth)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vPos		= vPos;
	m_pTransCom->m_vScale	= vScale;
	m_uiTexIdx	            = 0;
	m_UIDepth	            = iUIDepth;
	

	return S_OK;
}

HRESULT CToolUICanvas::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CToolUICanvas::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_UI, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	m_vConvert              = m_pTransCom->m_vPos.Convert_2DWindowToDescartes(WINCX, WINCY);
	_matrix matScale        = XMMatrixScaling(m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z);
	_matrix matTrans        = XMMatrixTranslation(m_vConvert.x, m_vConvert.y, m_vConvert.z);
	m_pTransCom->m_matWorld = matScale *  matTrans;

	return NO_EVENT;
}

_int CToolUICanvas::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CToolUICanvas::Render_GameObject(const _float & fTimeDelta)
{
	if (nullptr != m_pTexDescriptorHeap)
	{
		Set_ConstantTable();
		m_pShaderCom->Begin_Shader(m_pTexDescriptorHeap,
								   0,
								   m_uiTexIdx,
								   Engine::MATRIXID::ORTHO);
		m_pBufferCom->Begin_Buffer();

		m_pBufferCom->Render_Buffer();
	}
}

HRESULT CToolUICanvas::Add_Component()
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
	m_pShaderCom->Set_PipelineStatePass(3);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CToolUICanvas::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);

	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

Engine::CGameObject* CToolUICanvas::Create(ID3D12Device * pGraphicDevice,
										   ID3D12GraphicsCommandList * pCommandList,
										   const _vec3& vPos,
										   const _vec3& vScale,
										   const _long& iUIDepth)
{
	CToolUICanvas* pInstance = new CToolUICanvas(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vPos, vPos, iUIDepth)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolUICanvas::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
}
