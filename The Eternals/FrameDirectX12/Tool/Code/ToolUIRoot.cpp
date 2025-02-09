#include "stdafx.h"
#include <fstream>
#include "ToolUIRoot.h"
#include "ToolUIChild.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "DescriptorHeapMgr.h"

CToolUIRoot::CToolUIRoot(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CToolUIRoot::Ready_GameObject(wstring wstrObjectTag,
									  wstring wstrDataFilePath,
									  const _vec3& vPos, 
									  const _vec3& vScale,
									  const _bool& bIsSpriteAnimation, 
									  const _float& fFrameSpeed,
									  const _vec3& vRectOffset,
									  const _vec3& vRectScale,
									  const _long& iUIDepth)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, false), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Read_DataFromFilePath(wstrDataFilePath), E_FAIL);

	m_wstrObjectTag         = wstrObjectTag;
	m_wstrDataFilePath      = wstrDataFilePath;
	m_pTransCom->m_vPos		= vPos;
	m_pTransCom->m_vScale	= vScale;
	m_bIsSpriteAnimation	= bIsSpriteAnimation;
	m_tFrame.fFrameSpeed	= fFrameSpeed;
	m_UIDepth				= iUIDepth;

	m_pTransColor->m_vPos	= vPos;
	m_pTransColor->m_vScale = vRectScale;
	m_vRectOffset		    = vRectOffset;

	return S_OK;
}

HRESULT CToolUIRoot::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CToolUIRoot::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	if (m_bIsSpriteAnimation)
		Update_SpriteFrame(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_UI, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	m_vConvert              = m_pTransCom->m_vPos.Convert_2DWindowToDescartes(WINCX, WINCY);
	_matrix matScale        = XMMatrixScaling(m_pTransCom->m_vScale.x, m_pTransCom->m_vScale.y, m_pTransCom->m_vScale.z);
	_matrix matTrans        = XMMatrixTranslation(m_vConvert.x, m_vConvert.y, m_vConvert.z);
	m_pTransCom->m_matWorld = matScale *  matTrans;

	m_pTransColor->m_vPos     = m_pTransCom->m_vPos;
	m_vConvertRect            = (m_pTransColor->m_vPos + m_vRectOffset).Convert_2DWindowToDescartes(WINCX, WINCY);
	matScale                  = XMMatrixScaling(m_pTransColor->m_vScale.x, m_pTransColor->m_vScale.y, m_pTransColor->m_vScale.z);
	matTrans                  = XMMatrixTranslation(m_vConvertRect.x, m_vConvertRect.y, m_vConvertRect.z);
	m_pTransColor->m_matWorld = matScale * matTrans;
	Update_Rect();

	return NO_EVENT;
}

_int CToolUIRoot::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	return NO_EVENT;
}

void CToolUIRoot::Render_GameObject(const _float& fTimeDelta)
{
	if (nullptr != m_pTexDescriptorHeap)
	{
		Set_ConstantTable();

		// TextureBuffer
		m_pShaderCom->Begin_Shader(m_pTexDescriptorHeap,
								   0,
								   m_uiTexIdx,
								   Engine::MATRIXID::ORTHO);
		m_pBufferCom->Begin_Buffer();
		m_pBufferCom->Render_Buffer();

		// ColorBuffer
		if (m_bIsRenderRect)
		{
			m_pShaderColor->Begin_Shader(nullptr, 0, Engine::MATRIXID::ORTHO);
			m_pBufferColor->Begin_Buffer();
			m_pBufferColor->Render_Buffer();
		}
	}
}

HRESULT CToolUIRoot::Add_Component()
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(5);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);
	

	// Transform
	m_pTransColor = Engine::CTransform::Create();
	Engine::NULL_CHECK_RETURN(m_pTransColor, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_TransColor", m_pTransColor);

	// Buffer
	m_pBufferColor = static_cast<Engine::CRcCol*>(m_pComponentMgr->Clone_Component(L"RcCol", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pBufferColor, E_FAIL);
	m_pBufferColor->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_BufferColor", m_pBufferColor);

	// ShaderColor
	m_pShaderColor = static_cast<Engine::CShaderColor*>(m_pComponentMgr->Clone_Component(L"ShaderColor", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderColor, E_FAIL);
	m_pShaderColor->AddRef();
	m_pShaderColor->Set_PipelineStatePass(4);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_ShaderColor", m_pShaderColor);

	return S_OK;
}

HRESULT CToolUIRoot::Read_DataFromFilePath(wstring wstrDataFilePath)
{
	wifstream fin { wstrDataFilePath };
	if (fin.fail())
		E_FAIL;

	wstring	wstrTextureTag = L"";
	_int	iTextureIndex  = 0;
	_float	fFrameCnt	   = 0.0f;
	_float	fCurFrame	   = 0.0f;
	_float	fSceneCnt	   = 0.0f;
	_float	fCurScene	   = 0.0f;
	_float	fCanvasWidth   = 0.0f;
	_float	fCanvasHeight  = 0.0f;
	_float	fGridWidth     = 0.0f;
	_float	fGridHeight    = 0.0f;

	while (true)
	{
		fin >> wstrTextureTag 	// TextureTag
			>> iTextureIndex	// TextureIndex
			>> fFrameCnt		// FrameCnt
			>> fCurFrame		// CurFrame
			>> fSceneCnt		// SceneCnt
			>> fCurScene		// CurScene
			>> fCanvasWidth		// CanvasWidth
			>> fCanvasHeight	// CanvasHeight
			>> fGridWidth		// GridWidth
			>> fGridHeight;		// GridHeight

		if (fin.eof())
			break;

		m_wstrTextureTag     = wstrTextureTag.c_str();
		m_pTexDescriptorHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag);
		m_uiTexIdx		     = iTextureIndex;
		m_tFrame.fFrameCnt   = fFrameCnt;
		m_tFrame.fCurFrame   = fCurFrame;
		m_tFrame.fSceneCnt   = fSceneCnt;
		m_tFrame.fCurScene   = fCurScene;
	}

	return S_OK;
}

void CToolUIRoot::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	if (m_bIsSpriteAnimation)
	{
		tCB_ShaderTexture.fCurFrame = (_float)(_int)m_tFrame.fCurFrame;
		tCB_ShaderTexture.fCurScene	= (_float)(_int)m_tFrame.fCurScene;
	}
	else
	{
		tCB_ShaderTexture.fCurFrame = m_tFrame.fCurFrame;
		tCB_ShaderTexture.fCurScene = m_tFrame.fCurScene;
	}
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);



	Engine::CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransColor->m_matWorld);
	tCB_ShaderColor.vColor		= _rgba(0.0f, 1.0f, 0.0f, 0.25f);

	m_pShaderColor->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
}

void CToolUIRoot::Update_SpriteFrame(const _float& fTimeDelta)
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
		m_tFrame.fCurScene = 0.0f;
}

void CToolUIRoot::Update_Rect()
{
	m_tRect.left   = LONG((m_pTransColor->m_vPos.x + m_vRectOffset.x) - m_pTransCom->m_vScale.x * 0.5f);
	m_tRect.top    = LONG((m_pTransColor->m_vPos.y + m_vRectOffset.y) - m_pTransCom->m_vScale.y * 0.5f);
	m_tRect.right  = LONG((m_pTransColor->m_vPos.x + m_vRectOffset.x) + m_pTransCom->m_vScale.x * 0.5f);
	m_tRect.bottom = LONG((m_pTransColor->m_vPos.y + m_vRectOffset.y) + m_pTransCom->m_vScale.y * 0.5f);
}


Engine::CGameObject* CToolUIRoot::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
										 wstring wstrObjectTag,
										 wstring wstrDataFilePath, 
										 const _vec3& vPos, 
										 const _vec3& vScale, 
										 const _bool& bIsSpriteAnimation, 
										 const _float& fFrameSpeed,
										 const _vec3& vRectOffset,
										 const _vec3& vRectScale,
										 const _long& iUIDepth)
{
	CToolUIRoot* pInstance = new CToolUIRoot(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrObjectTag,
										   wstrDataFilePath, 
										   vPos, 
										   vScale, 
										   bIsSpriteAnimation, 
										   fFrameSpeed,
										   vRectOffset,
										   vRectScale, 
										   iUIDepth)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolUIRoot::Free()
{
	Engine::CGameObject::Free();

	m_vecUIChild.clear();

	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pBufferColor);
	Engine::Safe_Release(m_pShaderColor);
}
