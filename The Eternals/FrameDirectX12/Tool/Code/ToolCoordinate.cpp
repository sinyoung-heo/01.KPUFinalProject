#include "stdafx.h"
#include "ToolCoordinate.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"

CToolCoordinate::CToolCoordinate(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CToolCoordinate::CToolCoordinate(const CToolCoordinate& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CToolCoordinate::Ready_GameObject(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	return S_OK;
}

HRESULT CToolCoordinate::LateInit_GameObject()
{
	return S_OK;
}

_int CToolCoordinate::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_PRIORITY, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CToolCoordinate::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTable();

	return NO_EVENT;
}

void CToolCoordinate::Render_GameObject(const _float& fTimeDelta)
{
	m_pShaderCom->Begin_Shader();
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CToolCoordinate::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CCoordinateCol*>(m_pComponentMgr->Clone_Component(L"CoordinateCol", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderColor*>(m_pComponentMgr->Clone_Component(L"ShaderColor", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(2), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);


	return S_OK;
}

void CToolCoordinate::Set_ConstantTable()
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW);
	_matrix* pmatProj = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION);

	if (nullptr == pmatView || nullptr == pmatProj)
		return;

	/*__________________________________________________________________________________________________________
	[ Set CB Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(Engine::CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * (*pmatView) * (*pmatProj)));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_MatrixDesc.matView, XMMatrixTranspose(*pmatView));
	XMStoreFloat4x4(&tCB_MatrixDesc.matProj, XMMatrixTranspose(*pmatProj));
	
	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(0, tCB_MatrixDesc);
}

CToolCoordinate* CToolCoordinate::Create(ID3D12Device* pGraphicDevice, 
										 ID3D12GraphicsCommandList* pCommandList, 
										 const _vec3& vScale, 
										 const _vec3& vAngle,
										 const _vec3& vPos)
{
	CToolCoordinate* pInstance = new CToolCoordinate(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolCoordinate::Free()
{
	CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
}
