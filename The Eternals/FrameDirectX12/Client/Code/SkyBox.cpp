#include "stdafx.h"
#include "SkyBox.h"

#include "ObjectMgr.h"
#include "GraphicDevice.h"


CSkyBox::CSkyBox(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CSkyBox::CSkyBox(const CSkyBox & rhs)
	: Engine::CGameObject(rhs)
{
}


HRESULT CSkyBox::Ready_GameObject(wstring wstrTextureTag, 
								  const _vec3 & vScale,
								  const _vec3 & vAngle,
								  const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	m_uiTexIdx	= 0;

	return S_OK;
}

HRESULT CSkyBox::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CSkyBox::Update_GameObject(const _float & fTimeDelta)
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

_int CSkyBox::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTable();

	return NO_EVENT;
}

void CSkyBox::Render_GameObject(const _float & fTimeDelta)
{
	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), m_uiTexIdx);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CSkyBox::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CCubeTex*>(m_pComponentMgr->Clone_Component(L"CubeTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderSkyBox*>(m_pComponentMgr->Clone_Component(L"ShaderSkyBox", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CSkyBox::Set_ConstantTable()
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::VIEW);
	_matrix* pmatProj = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::PROJECTION);

	if (nullptr == pmatProj)
		return;

	/*__________________________________________________________________________________________________________
	[ CB ���� ���� ]
	____________________________________________________________________________________________________________*/
	Engine::CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(Engine::CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * (*pmatView) * (*pmatProj)));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_MatrixDesc.matView, XMMatrixTranspose(*pmatView));
	XMStoreFloat4x4(&tCB_MatrixDesc.matProj, XMMatrixTranspose(*pmatProj));

	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(0, tCB_MatrixDesc);

}

CSkyBox * CSkyBox::Create(ID3D12Device * pGraphicDevice, 
						  ID3D12GraphicsCommandList * pCommandList,
						  wstring wstrTextureTag,
						  const _vec3 & vScale,
						  const _vec3 & vAngle,
						  const _vec3 & vPos)
{
	CSkyBox* pInstance = new CSkyBox(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CSkyBox::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}