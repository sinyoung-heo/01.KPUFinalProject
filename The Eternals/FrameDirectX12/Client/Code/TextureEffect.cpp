#include "stdafx.h"
#include "TextureEffect.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"
#include "TextureDistortion.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"

CTextureEffect::CTextureEffect(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CTextureEffect::Ready_GameObject(wstring wstrTextureTag, 
										 const _vec3 & vScale,
										 const _vec3 & vAngle,
										 const _vec3 & vPos,
										 const FRAME& tFrame)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_strTextag = wstrTextureTag;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
		m_pTransCom->m_vScale,
		_vec3(0.0f, 0.0f, 0.0f),
		_vec3(0.5f, 0.5f, 0.0f),
		_vec3(-0.5f, -0.5f, 0.0f));
	
	return S_OK;
}

HRESULT CTextureEffect::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_bisInit = true;
	m_pShaderCom->SetUp_ShaderConstantBuffer();
	m_bisAlphaObject = true;
	
	return S_OK;
}

_int CTextureEffect::Update_GameObject(const _float & fTimeDelta)
{
	if(m_bisInit==false)
		Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsReturn)
	{
		m_pTextureHeap = nullptr;
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_TextureEffect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;

	m_fDeltatime += fTimeDelta;
	m_fDeltaTime += fTimeDelta;
	if (m_bisFollowHand)
	{
		if (m_fDeltaTime > 0.05f)
		{
			CEffectMgr::Get_Instance()->Effect_Particle(m_pTransCom->m_vPos, 1, L"Lighting0", _vec3(0.4f));
			m_fDeltaTime = 0.f;
		}
		Follow_PlayerHand();
		m_pTransCom->m_vScale.x += 1.75f * fTimeDelta;
		m_pTransCom->m_vScale.y += 1.75f * fTimeDelta;
	}
	ScaleAnim(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Update Sprite Frame ]
	____________________________________________________________________________________________________________*/
	Update_SpriteFrame(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	// Frustum Culling
	if (m_pRenderer->Get_Frustum().Contains(m_pBoundingBoxCom->Get_BoundingInfo()) != DirectX::DISJOINT)
		Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Update Billboard Matrix ]
	____________________________________________________________________________________________________________*/
	if (m_bisBillBoard)
	{
		Engine::CGameObject::SetUp_BillboardMatrix();
	}
	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);
	return NO_EVENT;
}

_int CTextureEffect::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);


	return NO_EVENT;
}

void CTextureEffect::Render_GameObject(const _float & fTimeDelta)
{
	if (m_pTextureHeap == nullptr)
		return;

	Set_ConstantTable();
	m_pShaderCom->Begin_Shader(m_pTextureHeap, 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

void CTextureEffect::Set_EffectInfo(int PipelineState, bool PlayerFollow)
{
}

void CTextureEffect::Follow_PlayerHand(Engine::HIERARCHY_DESC* pHierarchyDesc,Engine::CTransform * PlayerTransform)
{
	
	_matrix matBoneFinalTransform = INIT_MATRIX;
	_matrix matWorld = INIT_MATRIX;
	matBoneFinalTransform = (m_pHierarchyDesc->matScale * m_pHierarchyDesc->matRotate * m_pHierarchyDesc->matTrans) * m_pHierarchyDesc->matGlobalTransform;
	matWorld = matBoneFinalTransform * m_pPlayerTransform->m_matWorld;
	
	m_pTransCom->m_vPos = _vec3(matWorld._41, matWorld._42, matWorld._43);
}

void CTextureEffect::ScaleAnim(const _float& fTimeDelta)
{
	if (!m_bisScaleAnimation)
		return;

	if (m_ScaleAnimIdx == 0)
	{
		m_fScaleTimeDelta += (fTimeDelta * 10);
		m_pTransCom->m_vScale = _vec3(sin(m_fScaleTimeDelta)) * m_fMaxScale;
		if (sin(m_fScaleTimeDelta) > 0.9f)
			m_bIsReturn = true;
	}
	else if (m_ScaleAnimIdx == 1)
	{
		m_pTransCom->m_vScale -= _vec3(1.f) * fTimeDelta;
		if (m_pTransCom->m_vScale.x<0.f)
			m_bIsReturn = true;
	}
}

HRESULT CTextureEffect::Add_Component(wstring wstrTextureTag)
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
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(2), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CTextureEffect::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= m_tFrame.fFrameCnt;
	tCB_ShaderTexture.fCurFrame	= (_float)(_int)m_tFrame.fCurFrame;
	tCB_ShaderTexture.fSceneCnt	= m_tFrame.fSceneCnt;
	tCB_ShaderTexture.fCurScene	= (_float)(_int)m_tFrame.fCurScene;
	tCB_ShaderTexture.v_Color   = m_vColorOffset;

	if(m_bisFollowHand)
		tCB_ShaderTexture.fOffset2 = m_fDeltatime*0.5f;
	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

void CTextureEffect::Update_SpriteFrame(const _float & fTimeDelta)
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
		if (!m_bisLoop)
			m_bIsReturn = true;
	}

}

void CTextureEffect::Set_CreateInfo(wstring TexTag, const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos,
	 const FRAME& tFrame, bool isLoop, bool isScaleAnim,float maxScale,_int ScaleAnimIdx , _vec4 colorOffset
,  bool isFollowHand , Engine::HIERARCHY_DESC* hierachy , Engine::CTransform* parentTransform )
{
	if (nullptr == m_pTextureHeap)
		m_pTextureHeap = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(TexTag);

	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vScale.z = 0.f;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_vColorOffset = colorOffset;
	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
		m_pTransCom->m_vScale,
		_vec3(0.0f, 0.0f, 0.0f),
		_vec3(0.5f, 0.5f, 0.0f),
		_vec3(-0.5f, -0.5f, 0.0f));

	m_fDeltatime = 0.f;
	m_fAlpha = 1.f;
	m_bisInit = false;
	m_uiTexIdx = 0;
	m_tFrame = tFrame;

	m_ScaleAnimIdx = ScaleAnimIdx;
	m_fMaxScale = maxScale;
	m_bisBillBoard = true;
	m_bisAlphaObject = true;
	m_bisLoop = isLoop;
	m_bisScaleAnimation = isScaleAnim;
	m_fScaleTimeDelta = 0.f;
	m_bisFollowHand = isFollowHand;
	m_pHierarchyDesc = hierachy;
	m_pPlayerTransform = parentTransform;
}

Engine::CGameObject* CTextureEffect::Create(ID3D12Device * pGraphicDevice,
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

CTextureEffect** CTextureEffect::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{
	CTextureEffect** ppInstance = new (CTextureEffect * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CTextureEffect(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"Temp", _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CTextureEffect::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
}
