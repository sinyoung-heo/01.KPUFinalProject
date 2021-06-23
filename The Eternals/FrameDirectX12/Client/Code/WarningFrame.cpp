#include "stdafx.h"
#include "WarningFrame.h"

CWarningFrame::CWarningFrame(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CWarningFrame::Ready_GameObject()
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vPos		= _vec3((_float)WINCX / 2.0f, (_float)WINCY / 2.0f, 0.1f);
	m_pTransCom->m_vScale	= _vec3((_float)WINCX, (_float)WINCY, 1.0f);

	m_uiTexIdx	= 0;
	m_UIDepth	= 0;

	return S_OK;
}

HRESULT CWarningFrame::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CWarningFrame::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsRender)
	{
		m_fAlpha += m_fSpeed * fTimeDelta;

		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
			m_fSpeed *= -1.0f;
		}

		if (m_fAlpha < 0.0f)
		{
			m_fAlpha = 0.0f;
			m_fSpeed *= -1.0f;

			m_bIsRender = false;
		}

	}

	if (m_bIsRender)
	{
		/*__________________________________________________________________________________________________________
		[ Renderer - Add Render Group ]
		____________________________________________________________________________________________________________*/
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_UI, this), -1);

		/*__________________________________________________________________________________________________________
		[ TransCom - Update WorldMatrix ]
		____________________________________________________________________________________________________________*/
		m_vConvert = m_pTransCom->m_vPos.Convert_2DWindowToDescartes(WINCX, WINCY);

		_matrix matScale = XMMatrixScaling(m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z);
		_matrix matTrans = XMMatrixTranslation(m_vConvert.x, m_vConvert.y, m_vConvert.z);

		m_pTransCom->m_matWorld = matScale * matTrans;
	}

	return NO_EVENT;
}

_int CWarningFrame::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CWarningFrame::Render_GameObject(const _float & fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::ORTHO);
	m_pBufferCom->Begin_Buffer();
	m_pBufferCom->Render_Buffer();
}

HRESULT CWarningFrame::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"WarningFrame", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(7);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CWarningFrame::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fAlpha   = m_fAlpha;

	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

Engine::CGameObject* CWarningFrame::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CWarningFrame* pInstance = new CWarningFrame(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CWarningFrame::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
