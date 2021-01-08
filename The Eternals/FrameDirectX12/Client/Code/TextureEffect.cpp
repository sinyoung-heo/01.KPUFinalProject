#include "stdafx.h"
#include "TextureEffect.h"

#include "ObjectMgr.h"
#include "GraphicDevice.h"


CTextureEffect::CTextureEffect(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CTextureEffect::CTextureEffect(const CTextureEffect & rhs)
	: Engine::CGameObject(rhs)
{
}


HRESULT CTextureEffect::Ready_GameObject(wstring wstrTextureTag, 
										 const _vec3 & vScale,
										 const _vec3 & vAngle,
										 const _vec3 & vPos,
										 const FRAME& tFrame)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	m_uiTexIdx	= 0;
	m_tFrame	= tFrame;

	return S_OK;
}

HRESULT CTextureEffect::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CTextureEffect::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Update Sprite Frame ]
	____________________________________________________________________________________________________________*/
	Update_SpriteFrame(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

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

_int CTextureEffect::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	
	Set_ConstantTable();

	return NO_EVENT;
}

void CTextureEffect::Render_GameObject(const _float & fTimeDelta)
{
	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), m_uiTexIdx);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

HRESULT CTextureEffect::Add_Component(wstring wstrTextureTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(2), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CTextureEffect::Set_ConstantTable()
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::VIEW);
	_matrix* pmatProj = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::PROJECTION);

	if (nullptr == pmatView || nullptr == pmatProj)
		return;

	/*__________________________________________________________________________________________________________
	[ CB 정보 전달 ]
	____________________________________________________________________________________________________________*/
	Engine::CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(Engine::CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * (*pmatView) * (*pmatProj)));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_MatrixDesc.matView, XMMatrixTranspose(*pmatView));
	XMStoreFloat4x4(&tCB_MatrixDesc.matProj, XMMatrixTranspose(*pmatProj));

	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(0, tCB_MatrixDesc);

	Engine::CB_TEXSPRITE_DESC tCB_SpriteDesc;
	ZeroMemory(&tCB_SpriteDesc, sizeof(Engine::CB_TEXSPRITE_DESC));
	tCB_SpriteDesc.iFrameCnt	= m_tFrame.iFrameCnt;
	tCB_SpriteDesc.iFrameOffset = (_int)m_tFrame.fFrameOffset;
	tCB_SpriteDesc.iSceneCnt	= m_tFrame.iSceneCnt;
	tCB_SpriteDesc.iSceneOffset = (_int)m_tFrame.fSceneOffset;

	m_pShaderCom->Get_UploadBuffer_TexSpriteDesc()->CopyData(0, tCB_SpriteDesc);
}

void CTextureEffect::Update_SpriteFrame(const _float & fTimeDelta)
{
	m_tFrame.fFrameOffset += fTimeDelta * m_tFrame.fFrameSpeed;

	// Sprite X축
	if (m_tFrame.fFrameOffset > m_tFrame.iFrameCnt)
	{
		m_tFrame.fFrameOffset = 0.0f;
		m_tFrame.fSceneOffset += 1.0f;
	}

	// Sprite Y축
	if (m_tFrame.fSceneOffset >= m_tFrame.iSceneCnt)
	{
		m_tFrame.fSceneOffset = 0.0f;
	}

}

CTextureEffect * CTextureEffect::Create(ID3D12Device * pGraphicDevice,
										ID3D12GraphicsCommandList * pCommandList,
										wstring wstrTextureTag, 
										const _vec3 & vScale, 
										const _vec3 & vAngle, 
										const _vec3 & vPos, 
										const FRAME & tFrame)
{
	CTextureEffect* pInstance = new CTextureEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTextureEffect::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
