#include "stdafx.h"
#include "NPCMiniMap.h"

CNPCMiniMap::CNPCMiniMap(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CNPCMiniMap::Ready_GameObject(const _vec3& vPos, const _uint& uiMiniMapTexIdx)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);

	m_pTransCom->m_vPos = vPos;
	m_uiTexidx = uiMiniMapTexIdx;

	return S_OK;
}

HRESULT CNPCMiniMap::LateInit_GameObject()
{
	// MiniMap
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::SetUp_MiniMapComponent(m_uiTexidx), E_FAIL);

	return S_OK;
}

_int CNPCMiniMap::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MINIMAP, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);
	Engine::CGameObject::Compute_ViewZ(_vec4(m_pTransCom->m_vPos, 1.0f));

	return NO_EVENT;
}

_int CNPCMiniMap::LateUpdate_GameObject(const _float& fTimeDelta)
{
	return NO_EVENT;
}

void CNPCMiniMap::Send_PacketToServer()
{
}

void CNPCMiniMap::Render_MiniMap(const _float& fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderMiniMap->Begin_Shader(m_pTextureMiniMap->Get_TexDescriptorHeap(), 
								   0, 
								   m_uiTexidx, 
								   Engine::MATRIXID::PROJECTION);
	m_pBufferMiniMap->Begin_Buffer();
	m_pBufferMiniMap->Render_Buffer();
}

void CNPCMiniMap::Set_ConstantTable()
{
	m_pTransMiniMap->m_vPos   = m_pTransCom->m_vPos;
	m_pTransMiniMap->m_vAngle = _vec3(90.0f, 0.0f, 0.0f);
	m_pTransMiniMap->m_vScale = _vec3(6.0f, 6.0f, 6.0f);
	m_pTransMiniMap->Update_Component(0.16f);

	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_CAMERA_MATRIX tCB_CameraMatrix;
	ZeroMemory(&tCB_CameraMatrix, sizeof(Engine::CB_CAMERA_MATRIX));
	tCB_CameraMatrix.matView = Engine::CShader::Compute_MatrixTranspose(CShadowLightMgr::Get_Instance()->Get_MiniMapView());
	tCB_CameraMatrix.matProj = Engine::CShader::Compute_MatrixTranspose(CShadowLightMgr::Get_Instance()->Get_MiniMapProj());

	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransMiniMap->m_matWorld);
	tCB_ShaderTexture.fAlpha    = 1.0f;

	m_pShaderMiniMap->Get_UploadBuffer_CameraProjMatrix()->CopyData(0, tCB_CameraMatrix);
	m_pShaderMiniMap->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

Engine::CGameObject* CNPCMiniMap::Create(ID3D12Device* pGraphicDevice, 
										 ID3D12GraphicsCommandList* pCommandList, 
										 const _vec3& vPos, 
										 const _uint& uiMiniMapTexIdx)
{
	CNPCMiniMap* pInstance = new CNPCMiniMap(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(vPos, uiMiniMapTexIdx)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CNPCMiniMap::Free()
{
	Engine::CGameObject::Free();
}
