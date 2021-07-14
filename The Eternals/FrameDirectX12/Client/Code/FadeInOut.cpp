#include "stdafx.h"
#include "FadeInOut.h"
#include "Management.h"
#include "Scene_Logo.h"
#include "InstancePoolMgr.h"
#include "DirectInput.h"
#include "DynamicCamera.h"
#include "CinemaMgr.h"

CFadeInOut::CFadeInOut(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
	, m_pPacketMgr(CPacketMgr::Get_Instance())
{
}

void CFadeInOut::Set_FadeInOutEventType(const EVENT_TYPE& eEventType)
{
	m_eEventType = eEventType;
	m_bIsReturn = false;

	if (EVENT_TYPE::FADE_IN == eEventType ||
		EVENT_TYPE::SCENE_CHANGE_FADEIN_FADEOUT == eEventType)
	{
		m_fAlpha = 1.0f;
	}
	else if (EVENT_TYPE::FADE_OUT == eEventType ||
			 EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN == eEventType ||
			 EVENT_TYPE::SCENE_CAHNGE_LOGO_STAGE == eEventType ||
			 EVENT_TYPE::EVENT_CINEMATIC_ENDING == eEventType)
	{
		m_fAlpha = 0.0f;
		m_bIsCinematicEnding = false;
	}
}

HRESULT CFadeInOut::Ready_GameObject(const EVENT_TYPE& eEventType)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(8), E_FAIL);

	m_matView             = INIT_MATRIX;
	m_matProj             = XMMatrixOrthographicLH(WINCX, WINCY, 0.0f, 1.0f);
	m_pTransCom->m_vPos   = _vec3((_float)WINCX / 2.0f, (_float)WINCY / 2.0f, 0.1f);
	m_pTransCom->m_vScale = _vec3((_float)WINCX, (_float)WINCY, 1.0f);

	m_uiTexIdx = 1;
	m_UIDepth  = 0;

	m_eEventType = eEventType;

	if (EVENT_TYPE::FADE_IN == eEventType ||
		EVENT_TYPE::SCENE_CHANGE_FADEIN_FADEOUT == eEventType)
	{
		m_fAlpha = 1.0f;
	}
	else if (EVENT_TYPE::FADE_OUT == eEventType ||
			 EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN == eEventType ||
			 EVENT_TYPE::SCENE_CAHNGE_LOGO_STAGE == eEventType)
	{
		m_fAlpha = 0.0f;
	}

	return S_OK;
}

HRESULT CFadeInOut::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CFadeInOut::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
	{
		return DEAD_OBJ;
	}
	if (m_bIsReturn)
	{
		m_bIsSendPacket      = false;
		m_bIsReceivePacket   = false;
		m_bIsSetCinematic    = false;
		m_bIsCinematicEnding = false;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_FadeInOutPool(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}

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

	if (fTimeDelta > TIME_OFFSET)
		return NO_EVENT;

	SetUp_FadeInOutEvent(fTimeDelta);

	return NO_EVENT;
}

_int CFadeInOut::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CFadeInOut::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::ORTHO);
	m_pBufferCom->Begin_Buffer();
	m_pBufferCom->Render_Buffer();
}

HRESULT CFadeInOut::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(L"BackBuffer", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CFadeInOut::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_CAMERA_MATRIX tCB_CameraMatrix;
	ZeroMemory(&tCB_CameraMatrix, sizeof(Engine::CB_CAMERA_MATRIX));
	tCB_CameraMatrix.matView	= Engine::CShader::Compute_MatrixTranspose(m_matView);
	tCB_CameraMatrix.matProj	= Engine::CShader::Compute_MatrixTranspose(m_matProj);

	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fAlpha   = m_fAlpha;

	m_pShaderCom->Get_UploadBuffer_CameraOrthoMatrix()->CopyData(0, tCB_CameraMatrix);
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

void CFadeInOut::SetUp_FadeInOutEvent(const _float& fTimeDelta)
{
	if (m_bIsReturn)
		return;

	if (EVENT_TYPE::FADE_IN == m_eEventType)
	{
		m_fAlpha -= fTimeDelta * 0.5f;
		if (m_fAlpha < 0.0f)
		{
			m_fAlpha = 0.0f;
			m_bIsReturn = true;
		}
	}
	else if (EVENT_TYPE::FADE_OUT == m_eEventType)
	{
		m_fAlpha += fTimeDelta * 0.5f;
		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
			m_bIsReturn = true;
		}
	}
	else if (EVENT_TYPE::SCENE_CAHNGE_LOGO_STAGE == m_eEventType)
	{
		m_fAlpha += fTimeDelta * 0.5f;
		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
			static_cast<CScene_Logo*>(Engine::CManagement::Get_Instance()->Get_CurrentScene())->Set_IsSceneChage(true);
			m_bIsReturn = true;
		}
	}
	else if (EVENT_TYPE::SCENE_CHANGE_FADEOUT_FADEIN == m_eEventType)
	{
		// Send StageChange Packet
		if (!m_bIsReceivePacket)
		{
			m_fAlpha += fTimeDelta * 0.5f;
			if (m_fAlpha > 1.0f)
			{
				m_fAlpha = 1.0f;
				
				if (!m_bIsSendPacket)
				{
					m_bIsSendPacket = true;
					m_pPacketMgr->send_stage_change(m_chCurStageID);
				}
			}
		}

		// Receive StageChange Packet
		else if (m_bIsSendPacket && m_bIsReceivePacket)
		{
			// Cinematic Setting
			if (STAGE_WINTER == m_chCurStageID)
			{
				if (!m_bIsSetCinematic)
				{
					m_bIsSetCinematic = true;

					g_bIsCinemaStart = true;
					// Set DynamicCamera State
					CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
					pDynamicCamera->Set_CameraState(CAMERA_STATE::CINEMATIC_LAKAN_ALL);
					pDynamicCamera->SetUp_ThirdPersonViewOriginData();
				}
			}

			m_fAlpha -= fTimeDelta * 0.5f;
			if (m_fAlpha < 0.0f)
			{
				m_fAlpha = 0.0f;
				g_bIsStageChange = false;
				m_bIsReturn = true;
			}
		}
	}
	else if (EVENT_TYPE::EVENT_CINEMATIC_ENDING == m_eEventType)
	{
		if (!m_bIsCinematicEnding)
		{
			m_fAlpha += fTimeDelta * 0.5f;
			if (m_fAlpha > 1.0f)
			{
				m_fAlpha = 1.0f;
				m_bIsCinematicEnding = true;

				// Stage Setting.
				g_bIsCinemaStart = false;
				CDynamicCamera* pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
				pDynamicCamera->Set_CameraState(CAMERA_STATE::THIRD_PERSON_VIEW);
				pDynamicCamera->Set_ResetFovY();
				pDynamicCamera->Set_IsCinematicEnding(false);
				pDynamicCamera->Set_IsSettingCameraCinematicValue(false);
				pDynamicCamera->Set_CameraAtParentMatrix(nullptr);
				CCinemaMgr::Get_Instance()->Reset_PrionBerserkerPosition();
				CCinemaMgr::Get_Instance()->Reset_LakanPosition();
				CCinemaMgr::Get_Instance()->Reset_Vergos();
				CCinemaMgr::Get_Instance()->Set_IsCancleCinematic(false);
				CPacketMgr::Get_Instance()->send_end_cinema();
			}
		}
		else
		{
			m_fAlpha -= fTimeDelta * 0.7f;
			if (m_fAlpha < 0.0f)
			{
				m_fAlpha = 0.0f;
				m_bIsCinematicEnding = false;
				m_bIsReturn = true;
			}
		}
	}
}

Engine::CGameObject* CFadeInOut::Create(ID3D12Device* pGraphicDevice, 
										ID3D12GraphicsCommandList* pCommandList, 
										const EVENT_TYPE& eEventType)
{
	CFadeInOut* pInstance = new CFadeInOut(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(eEventType)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CFadeInOut** CFadeInOut::Create_InstancePool(ID3D12Device* pGraphicDevice, 
											 ID3D12GraphicsCommandList* pCommandList, 
											 const _uint& uiInstanceCnt)
{
	CFadeInOut** ppInstance = new (CFadeInOut*[uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CFadeInOut(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(EVENT_TYPE::FADE_IN);
	}

	return ppInstance;
}

void CFadeInOut::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);
}
