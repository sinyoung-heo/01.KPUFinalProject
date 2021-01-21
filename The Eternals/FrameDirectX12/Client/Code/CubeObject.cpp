#include "stdafx.h"
#include "CubeObject.h"

#include "ObjectMgr.h"
#include "GraphicDevice.h"


CCubeObject::CCubeObject(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CCubeObject::CCubeObject(const CCubeObject & rhs)
	: Engine::CGameObject(rhs)
{
}


HRESULT CCubeObject::Ready_GameObject(const _vec3 & vScale,
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

HRESULT CCubeObject::LateInit_GameObject()
{
	return S_OK;
}

_int CCubeObject::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pTransCom->m_vAngle.y += 90.f * fTimeDelta;

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	return NO_EVENT;
}

_int CCubeObject::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	return NO_EVENT;
}

void CCubeObject::Render_GameObject(const _float & fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader();
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CCubeObject::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CCubeCol*>(m_pComponentMgr->Clone_Component(L"CubeCol", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderColor*>(m_pComponentMgr->Clone_Component(L"ShaderColor", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CCubeObject::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_COLOR tCB_ColorShader;
	ZeroMemory(&tCB_ColorShader, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ColorShader.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ColorShader.vColor		= _rgba(0.0f, 1.0f, 1.0f, 1.0f);

	m_pShaderCom->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ColorShader);
}

CCubeObject * CCubeObject::Create(ID3D12Device * pGraphicDevice,
								  ID3D12GraphicsCommandList * pCommandList,
								  const _vec3 & vScale,
								  const _vec3 & vAngle,
								  const _vec3 & vPos)
{
	CCubeObject* pInstance = new CCubeObject(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CCubeObject::Free()
{
	CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
}
