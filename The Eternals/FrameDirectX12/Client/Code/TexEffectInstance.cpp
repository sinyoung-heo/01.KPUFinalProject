#include "stdafx.h"
#include "TexEffectInstance.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"

CTexEffectInstance::CTexEffectInstance(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderTextureInstancing(Engine::CShaderTextureInstancing::Get_Instance())
{
}

CTexEffectInstance::CTexEffectInstance(const CTexEffectInstance& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CTexEffectInstance::Ready_GameObject(wstring wstrTextureTag, 
											 const _vec3& vScale, 
											 const _vec3& vAngle,
											 const _vec3& vPos,
											 const FRAME& tFrame)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_wstrTextureTag		= wstrTextureTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   _vec3(0.0f, 0.0f ,0.0f),
										   _vec3(0.5f, 0.5f ,0.0f),
										   _vec3(-0.5f, -0.5f ,0.0f));

	m_uiTexIdx	            = 0;
	m_iTexPipelineStatePass = 1;
	m_tFrame	            = tFrame;

	return S_OK;
}

HRESULT CTexEffectInstance::LateInit_GameObject()
{
	return S_OK;
}

_int CTexEffectInstance::Update_GameObject(const _float& fTimeDelta)
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
	// Frustum Culling
	if (m_pRenderer->Get_Frustum().Contains(m_pBoundingBoxCom->Get_BoundingInfo()) != DirectX::DISJOINT)
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

_int CTexEffectInstance::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CTexEffectInstance::Render_GameObject(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Add Instance ]
	____________________________________________________________________________________________________________*/
	m_pShaderTextureInstancing->Add_Instance(m_wstrTextureTag, Engine::INSTANCE::INSTANCE_ALPHA, m_iTexPipelineStatePass);
	_uint iInstanceIdx = m_pShaderTextureInstancing->Get_InstanceCount(m_wstrTextureTag, Engine::INSTANCE::INSTANCE_ALPHA, m_iTexPipelineStatePass) - 1;

	Set_ConstantTable(Engine::INSTANCE::INSTANCE_ALPHA, iInstanceIdx);
}

HRESULT CTexEffectInstance::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	return S_OK;
}

void CTexEffectInstance::Set_ConstantTable(const Engine::INSTANCE& eInstanceID, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.iFrameCnt = m_tFrame.iFrameCnt;
	tCB_ShaderTexture.iCurFrame = (_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.iSceneCnt = m_tFrame.iSceneCnt;
	tCB_ShaderTexture.iCurScene = (_int)m_tFrame.fCurScene;

	m_pShaderTextureInstancing->Get_UploadBuffer_ShaderTexture(m_wstrTextureTag, eInstanceID, m_iTexPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderTexture);
}

void CTexEffectInstance::Update_SpriteFrame(const _float& fTimeDelta)
{
	m_tFrame.fCurFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	// Sprite XÃà
	if (m_tFrame.fCurFrame > m_tFrame.iFrameCnt)
	{
		m_tFrame.fCurFrame = 0.0f;
		m_tFrame.fCurScene += 1.0f;
	}

	// Sprite YÃà
	if (m_tFrame.fCurScene >= m_tFrame.iSceneCnt)
	{
		m_tFrame.fCurScene = 0.0f;
	}
}

Engine::CGameObject* CTexEffectInstance::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
												wstring wstrTextureTag, 
												const _vec3& vScale,
												const _vec3& vAngle,
												const _vec3& vPos, 
												const FRAME& tFrame)
{
	CTexEffectInstance* pInstance = new CTexEffectInstance(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTexEffectInstance::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
}
