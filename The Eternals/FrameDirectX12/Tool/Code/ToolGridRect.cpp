#include "stdafx.h"
#include "ToolGridRect.h"

CToolGridRect::CToolGridRect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CToolGridRect::Ready_GameObject(const _vec3& vScale,
										const _vec3& vPos,
										const _long& iUIDepth)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vPos		= vPos;
	m_UIDepth               = iUIDepth;

	m_pShaderCom->Set_PipelineStatePass(4);

	Update_Rect();

	return S_OK;
}

HRESULT CToolGridRect::LateInit_GameObject()
{
	return S_OK;
}

_int CToolGridRect::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	if (m_bIsSelect)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_UI, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	m_vConvert = m_pTransCom->m_vPos.Convert_2DWindowToDescartes(WINCX, WINCY);

	_matrix matScale = XMMatrixScaling(m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z);
	_matrix matTrans = XMMatrixTranslation(m_vConvert.x, m_vConvert.y, m_vConvert.z);

	m_pTransCom->m_matWorld = matScale *  matTrans;

	return NO_EVENT;
}

_int CToolGridRect::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CToolGridRect::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pShaderCom->Begin_Shader(nullptr, 0, Engine::MATRIXID::ORTHO);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CToolGridRect::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcCol*>(m_pComponentMgr->Clone_Component(L"RcCol", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// ShaderColor
	m_pShaderCom = static_cast<Engine::CShaderColor*>(m_pComponentMgr->Clone_Component(L"ShaderColor", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CToolGridRect::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderColor.vColor		= _rgba(0.0f, 1.0f, 0.0f, 0.25f);

	m_pShaderCom->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
}

void CToolGridRect::Update_Rect()
{
	m_tRect.left   = LONG(m_pTransCom->m_vPos.x - m_pTransCom->m_vScale.x * 0.5f);
	m_tRect.top    = LONG(m_pTransCom->m_vPos.y - m_pTransCom->m_vScale.y * 0.5f);
	m_tRect.right  = LONG(m_pTransCom->m_vPos.x + m_pTransCom->m_vScale.x * 0.5f);
	m_tRect.bottom = LONG(m_pTransCom->m_vPos.y + m_pTransCom->m_vScale.y * 0.5f);
}

Engine::CGameObject* CToolGridRect::Create(ID3D12Device* pGraphicDevice, 
										   ID3D12GraphicsCommandList* pCommandList, 
										   const _vec3& vScale, 
										   const _vec3& vPos,
										   const _long& iUIDepth)
{
	CToolGridRect* pInstance = new CToolGridRect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vPos, iUIDepth)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolGridRect::Free()
{
	CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
}
