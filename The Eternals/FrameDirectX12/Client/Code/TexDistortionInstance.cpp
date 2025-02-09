#include "stdafx.h"
#include "TexDistortionInstance.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "DynamicCamera.h"
#include "DebugCamera.h"
#include "ObjectMgr.h"

CTexDistortionInstance::CTexDistortionInstance(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pShaderTextureInstancing(Engine::CShaderTextureInstancing::Get_Instance())
{
}

HRESULT CTexDistortionInstance::Ready_GameObject(wstring wstrTextureTag, 
												 const _vec3& vScale, 
												 const _vec3& vAngle, 
												 const _vec3& vPos, 
												 const FRAME& tFrame)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vScale.y *= 3.f;
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

HRESULT CTexDistortionInstance::LateInit_GameObject()
{
	//DynamicCamera
	m_pDynamicCamara = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DebugCamera"));

	return S_OK;
}

_int CTexDistortionInstance::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	_vec3 Src = m_pDynamicCamara->Get_CameraInfo().vEye;
	_vec3 Dir = (*m_pParentPosition) - Src;
	Dir.y = 0.f;
	Dir.Normalize();
	m_pTransCom->m_vPos = (*m_pParentPosition) + Dir * 2.f;

	/*__________________________________________________________________________________________________________
	[ Update Sprite Frame ]
	____________________________________________________________________________________________________________*/
	Update_SpriteFrame(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	// Frustum Culling
	if (m_pRenderer->Get_Frustum().Contains(m_pBoundingBoxCom->Get_BoundingInfo()) != DirectX::DISJOINT)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_DISTORTION, this), -1);

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

_int CTexDistortionInstance::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CTexDistortionInstance::Render_GameObject(const _float& fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Add Instance ]
	____________________________________________________________________________________________________________*/
	m_pShaderTextureInstancing->Add_Instance(m_wstrTextureTag, Engine::INSTANCE::INSTANCE_DISTORTION, m_iTexPipelineStatePass);
	_uint iInstanceIdx = m_pShaderTextureInstancing->Get_InstanceCount(m_wstrTextureTag, Engine::INSTANCE::INSTANCE_DISTORTION, m_iTexPipelineStatePass) - 1;

	Set_ConstantTable(Engine::INSTANCE::INSTANCE_DISTORTION, iInstanceIdx);

}

HRESULT CTexDistortionInstance::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	return S_OK;
}

void CTexDistortionInstance::Set_ConstantTable(const Engine::INSTANCE& eInstanceID, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ShaderResource Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= (_float)m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fCurFrame	= (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.fSceneCnt	= (_float)m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurScene	= (_float)(_int)m_tFrame.fCurScene;

	m_pShaderTextureInstancing->Get_UploadBuffer_ShaderTexture(m_wstrTextureTag, eInstanceID, m_iTexPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderTexture);
}

void CTexDistortionInstance::Update_SpriteFrame(const _float& fTimeDelta)
{
	m_tFrame.fCurFrame += fTimeDelta * m_tFrame.fFrameSpeed;

	// Sprite X��
	if (m_tFrame.fCurFrame > m_tFrame.fFrameCnt)
	{
		m_tFrame.fCurFrame = 0.0f;
		m_tFrame.fCurScene += 1.0f;
	}

	// Sprite Y��
	if (m_tFrame.fCurScene >= m_tFrame.fSceneCnt)
	{
		m_tFrame.fCurScene = 0.0f;
	}
}

Engine::CGameObject* CTexDistortionInstance::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, 
													wstring wstrTextureTag,
													const _vec3& vScale,
													const _vec3& vAngle, 
													const _vec3& vPos, 
													const FRAME& tFrame)
{
	CTexDistortionInstance* pInstance = new CTexDistortionInstance(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, vScale, vAngle, vPos, tFrame)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CTexDistortionInstance::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pBufferCom);
}
