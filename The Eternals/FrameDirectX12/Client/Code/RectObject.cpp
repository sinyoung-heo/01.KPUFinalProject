#include "stdafx.h"
#include "RectObject.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"


CRectObject::CRectObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderColorInstancing(Engine::CShaderColorInstancing::Get_Instance())
{
}

CRectObject::CRectObject(const CRectObject & rhs)
	: Engine::CGameObject(rhs)
{
}


HRESULT CRectObject::Ready_GameObject(const _vec3 & vScale,
									  const _vec3 & vAngle,
									  const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	return S_OK;
}

HRESULT CRectObject::LateInit_GameObject()
{
	return S_OK;
}

_int CRectObject::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	return NO_EVENT;
}

_int CRectObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CRectObject::Render_GameObject(const _float & fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Add Instance ]
	____________________________________________________________________________________________________________*/
	m_pShaderColorInstancing->Add_Instance(Engine::BUFFER_RECT, m_uiColorPipelineStatePass);
	_uint iInstanceIdx = m_pShaderColorInstancing->Get_InstanceCount(Engine::BUFFER_RECT, m_uiColorPipelineStatePass) - 1;

	Set_ConstantTable(Engine::BUFFER_RECT, iInstanceIdx);
}

HRESULT CRectObject::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcCol*>(m_pComponentMgr->Clone_Component(L"RcCol", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Shader
	m_uiColorPipelineStatePass = 0;

	return S_OK;
}

void CRectObject::Set_ConstantTable(const Engine::COLOR_BUFFER& eBuffer, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderColor.vColor		= _rgba(1.0f, 0.0f, 0.0f, 1.0f);

	m_pShaderColorInstancing->Get_UploadBuffer_ShaderColor(eBuffer, m_uiColorPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderColor);

}

Engine::CGameObject* CRectObject::Create(ID3D12Device * pGraphicDevice,
										 ID3D12GraphicsCommandList * pCommandList,
										 const _vec3 & vScale, 
										 const _vec3 & vAngle,
										 const _vec3 & vPos)
{
	CRectObject* pInstance = new CRectObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CRectObject::Free()
{
	CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
}
