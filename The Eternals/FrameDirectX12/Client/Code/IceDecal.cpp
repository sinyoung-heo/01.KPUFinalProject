#include "stdafx.h"
#include "IceDecal.h"
#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "RenderTarget.h"
#include "TimeMgr.h"
#include "DescriptorHeapMgr.h"
#include "TextureEffect.h"
CIceDecal::CIceDecal(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}


HRESULT CIceDecal::Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_wstrMeshTag = wstrMeshTag;
	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	m_fAlpha = 1.f;
	return S_OK;
}

HRESULT CIceDecal::LateInit_GameObject()
{

	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pDescriptorHeaps = Engine::CDescriptorHeapMgr::Get_Instance()->Find_DescriptorHeap(L"PublicMagic");
	return S_OK;	
}

_int CIceDecal::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_fAlpha < 0.f)
		m_bIsReturn = true;

	if (m_bIsReturn)
	{
		Return_Instance(CInstancePoolMgr::Get_Instance()->Get_Effect_IceDecal_Effect(), m_uiInstanceIdx);
		return RETURN_OBJ;
	}
	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/

	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_MAGICCIRCLE, this), -1);
	
	_vec4 vPosInWorld = _vec4(m_pTransCom->m_vPos, 1.0f);
	Engine::CGameObject::Compute_ViewZ(vPosInWorld);
	/*____________________________________________________________________
	TransCom - Update WorldMatrix.
	______________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CIceDecal::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);

	Set_ConstantTable();

	return NO_EVENT;
}


void CIceDecal::Render_GameObject(const _float& fTimeDelta)
{
	m_pMeshCom->Render_MagicCircleMesh(m_pShaderCom, m_pDescriptorHeaps, 33, 5, 2
		,0,4);
}

HRESULT CIceDecal::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);


	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMeshEffect*>(m_pComponentMgr->Clone_Component(L"ShaderMeshEffect", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(7), E_FAIL);
	m_pShaderCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	return S_OK;
}

void CIceDecal::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::SHADOW_DESC tShadowDesc = CShadowLightMgr::Get_Instance()->Get_ShadowDesc();

	Engine::CB_SHADER_MESH tCB_ShaderMesh;
	ZeroMemory(&tCB_ShaderMesh, sizeof(Engine::CB_SHADER_MESH));
	tCB_ShaderMesh.matWorld = Engine::CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderMesh.matLightView = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightView);
	tCB_ShaderMesh.matLightProj = Engine::CShader::Compute_MatrixTranspose(tShadowDesc.matLightProj);
	tCB_ShaderMesh.vLightPos = tShadowDesc.vLightPosition;
	tCB_ShaderMesh.fLightPorjFar = tShadowDesc.fLightPorjFar;
	m_fDeltaTime  += Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 0.5f;
	m_fDeltatime2 += Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 0.3f* m_fDelta2Velocity;
	if (fabsf(m_fDeltatime2) > 0.3f)
	{
		m_fDelta2Velocity *= -1.f;
	}
	tCB_ShaderMesh.fOffset1 = m_fDeltaTime;
	tCB_ShaderMesh.fOffset2 = m_fDeltatime2;
	m_fAlpha -= Engine::CTimerMgr::Get_Instance()->Get_TimeDelta(L"Timer_TimeDelta") * 0.3f;
	tCB_ShaderMesh.fOffset6 = m_fAlpha;

	if(m_pShaderCom->Get_UploadBuffer_ShaderMesh()!=nullptr)
		m_pShaderCom->Get_UploadBuffer_ShaderMesh()->CopyData(0, tCB_ShaderMesh);

	
}

void CIceDecal::Set_ConstantTableShadowDepth()
{

}
void CIceDecal::Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos)
{
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos = vPos;
	m_fDeltatime = 0.f;
	m_fDeltatime2 = 0.f;
	m_fDelta2Velocity = 1.f;
	m_fDeltatime3 = 0.f;
	m_fAlpha = 1.f;
}

Engine::CGameObject* CIceDecal::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList,
												wstring wstrMeshTag, 
												const _vec3 & vScale,
												const _vec3 & vAngle,
												const _vec3 & vPos)
{
	CIceDecal* pInstance = new CIceDecal(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CIceDecal** CIceDecal::Create_InstancePool(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList, const _uint& uiInstanceCnt)
{

	CIceDecal** ppInstance = new (CIceDecal * [uiInstanceCnt]);
	for (_uint i = 0; i < uiInstanceCnt; ++i)
	{
		ppInstance[i] = new CIceDecal(pGraphicDevice, pCommandList);
		ppInstance[i]->m_uiInstanceIdx = i;
		ppInstance[i]->Ready_GameObject(L"PublicPlane00", _vec3(0.f), _vec3(0.f), _vec3(0.f));
	}
	return ppInstance;
}

void CIceDecal::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pMeshCom);
//	Engine::Safe_Release(m_pDescriptorHeaps);
	Engine::Safe_Release(m_pShaderCom);
}
