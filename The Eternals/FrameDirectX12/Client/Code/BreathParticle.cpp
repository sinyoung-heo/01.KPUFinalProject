#include "stdafx.h"
#include "BreathParticle.h"
#include "GraphicDevice.h"
#include "DescriptorHeapMgr.h"
#include <random>

CBreathParticle::CBreathParticle(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

void CBreathParticle::Set_Instnace(wstring wstrTextueTag, const _uint& uiTexSize, const _vec3& vPos, const _vec3& vDir)
{
	m_wstrTextureTag = wstrTextueTag;
	m_uiTexSize      = uiTexSize;
	m_fAlpha = 0.7f;

	m_pTransCom->m_vScale = _vec3(4.0f);
	m_pTransCom->m_vPos   = vPos;
	m_pTransCom->m_vDir   = vDir;
	m_pTransCom->m_vDir.z = -1.0f;
	m_pTransCom->m_vDir.Normalize();

	random_device					rd_texIdx;
	default_random_engine			dre_texIdx{ rd_texIdx() };
	uniform_int_distribution<_int>	uid_texIdx{ 0, (_int)(m_uiTexSize) - 1 };

	m_fTexIdx = (_float)(uid_texIdx(dre_texIdx));

	random_device					rd_frameSpeed;
	default_random_engine			dre_frameSpeed{ rd_frameSpeed() };
	uniform_int_distribution<_int>	uid_frameSpeed{ (_int)(m_uiTexSize), (_int)((_float)(m_uiTexSize) * 1.5f)};

	m_fFrameSpeed = (_float)(uid_frameSpeed(dre_frameSpeed));

	random_device					rd_Speed;
	default_random_engine			dre_Speed{ rd_Speed() };
	uniform_int_distribution<_int>	uid_Speed{ 35, 45 };

	m_pInfoCom->m_fSpeed = (_float)(uid_Speed(dre_Speed));
}

HRESULT CBreathParticle::Ready_GameObject(const _vec3& vScale, 
										  const _vec3& vPos,
										  const _vec3& vDir)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vPos   = vPos;
	m_pTransCom->m_vDir   = vDir;

	// BoundingBox.
	Engine::CGameObject::SetUp_BoundingBox(&(m_pTransCom->m_matWorld),
										   m_pTransCom->m_vScale,
										   _vec3(0.0f, 0.0f, 0.0f),
										   _vec3(0.5f, 0.5f, 0.0f),
										   _vec3(-0.5f, -0.5f, 0.0f));
	
	return S_OK;
}

HRESULT CBreathParticle::LateInit_GameObject()
{
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CBreathParticle::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsReturn)
	{
		m_pTransCom->m_vScale = _vec3(6.0f);

		m_bIsCalcAlpha      = false;
		m_fAlpha            = 0.8f;
		m_fLifeTime         = 0.0f;
		m_fTexIdx           = 0.0f;
		m_uiTexSize         = 0;
		m_fFrameSpeed       = 0;
		m_pTransCom->m_vDir = _vec3(0.0f);

		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_BreathParticlePool(), m_uiInstanceIdx);

		return RETURN_OBJ;
	}

	if (m_bIsDead)
		return DEAD_OBJ;

	// Update Texture Frame
	m_fTexIdx += fTimeDelta * m_fFrameSpeed;
	if (m_fTexIdx >= (_float)m_uiTexSize)
		m_fTexIdx = 0.0f;

	// Move
	m_pTransCom->m_vPos += m_pTransCom->m_vDir * m_pInfoCom->m_fSpeed * fTimeDelta;

	if (m_pTransCom->m_vPos.y <= 0.0f)
	{
		m_pTransCom->m_vPos.y = 0.0f;
		m_bIsCalcAlpha = true;
	}

	// Alpha
	if (m_bIsCalcAlpha)
	{
		m_fAlpha -= fTimeDelta * 0.5f;
		if (m_fAlpha <= 0.0f)
		{
			m_fAlpha = 0.0f;
			m_bIsReturn = true;
		}
	}

	// Scale
	m_pTransCom->m_vScale += _vec3(10.0) * fTimeDelta;
	if (m_pTransCom->m_vScale.x >= 12.0f)
		m_pTransCom->m_vScale = _vec3(12.0f);


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
	Make_BillboardMatrix(m_pTransCom->m_matWorld, m_pTransCom->m_vScale);

	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);

	return NO_EVENT;
}

_int CBreathParticle::LateUpdate_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();

	return NO_EVENT;
}

void CBreathParticle::Render_GameObject(const _float& fTimeDelta)
{
	if (nullptr == m_pShaderCom->Get_UploadBuffer_ShaderTexture())
		return;

	// Render Buffer
	m_pShaderCom->Begin_Shader(Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(m_wstrTextureTag),
							   0,
							   (_uint)m_fTexIdx,
							   Engine::MATRIXID::PROJECTION);

	m_pBufferCom->Begin_Buffer();
	m_pBufferCom->Render_Buffer();
}

HRESULT CBreathParticle::Add_Component()
{
	// Buffer
	m_pBufferCom = static_cast<Engine::CRcTex*>(m_pComponentMgr->Clone_Component(L"RcTex", Engine::COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);
	m_pBufferCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Buffer", m_pBufferCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(12);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CBreathParticle::Set_ConstantTable()
{
	// Set ConstantTable
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderTexture.fFrameCnt	= 1.0f;
	tCB_ShaderTexture.fCurFrame	= 0.0f;
	tCB_ShaderTexture.fSceneCnt	= 1.0f;
	tCB_ShaderTexture.fCurScene	= 0.0f;
	tCB_ShaderTexture.fAlpha    = m_fAlpha;

	if (nullptr != m_pShaderCom->Get_UploadBuffer_ShaderTexture())
		m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
 }

void CBreathParticle::Make_BillboardMatrix(_matrix& matrix, const _vec3& vScale)
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW);
	if (nullptr == pmatView)
		return;

	_matrix matBillboard = (*pmatView);
	memset(&matBillboard._41, 0, sizeof(_vec3));

	matBillboard = MATRIX_INVERSE(matBillboard);

	/*__________________________________________________________________________________________________________
	[ GameObject의 Scale값 반영. ]
	____________________________________________________________________________________________________________*/
	_float fScale[3] = { vScale.x, vScale.y, vScale.z };

	for (_int i = 0; i < 3; ++i)
	{
		for (_int j = 0; j < 4; ++j)
		{
			matBillboard(i, j) *= fScale[i];
		}
	}

	/*__________________________________________________________________________________________________________
	[ Scale + Rotate 입력 ]
	____________________________________________________________________________________________________________*/
	memcpy(&matrix._11, &matBillboard._11, sizeof(_vec3));
	memcpy(&matrix._21, &matBillboard._21, sizeof(_vec3));
	memcpy(&matrix._31, &matBillboard._31, sizeof(_vec3));
}

Engine::CGameObject* CBreathParticle::Create(ID3D12Device* pGraphicDevice, 
											 ID3D12GraphicsCommandList* pCommandList, 
											 const _vec3& vScale, 
											 const _vec3& vPos, 
											 const _vec3& vDir)
{
	return nullptr;
}

CBreathParticle** CBreathParticle::Create_InstancePool(ID3D12Device* pGraphicDevice,
													   ID3D12GraphicsCommandList* pCommandList, 
													   const _uint& uiInstanceCnt)
{
	CBreathParticle** ppInstance = new (CBreathParticle * [uiInstanceCnt]);

	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CBreathParticle(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(_vec3(6.0f),	// Scale
										_vec3(0.0f),	// Pos
										_vec3(0.0f));	// Dir
	}

	return ppInstance;
}

void CBreathParticle::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
}
