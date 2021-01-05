#include "stdafx.h"
#include "Popori_F.h"

#include "GraphicDevice.h"
#include "DirectInput.h"
#include "ObjectMgr.h"
#include "LightMgr.h"
#include "DynamicCamera.h"
#include "Font.h"
#include "RenderTarget.h"


CPopori_F::CPopori_F(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CPopori_F::CPopori_F(const CPopori_F & rhs)
	: Engine::CGameObject(rhs)
	, m_wstrMeshTag(rhs.m_wstrMeshTag)
{
}

HRESULT CPopori_F::Ready_GameObject(wstring wstrMeshTag, 
									const _vec3 & vScale, 
									const _vec3 & vAngle, 
									const _vec3 & vPos)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(true, true), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrMeshTag), E_FAIL);

	m_pTransCom->m_vScale	= vScale;
	m_pTransCom->m_vAngle	= vAngle;
	m_pTransCom->m_vPos		= vPos;

	/*__________________________________________________________________________________________________________
	[ 애니메이션 설정 ]
	____________________________________________________________________________________________________________*/
	m_uiAnimIdx = 1;
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);

	/*__________________________________________________________________________________________________________
	[ Collider Bone Setting ]
	____________________________________________________________________________________________________________*/
	Engine::SKINNING_MATRIX*	pmatSkinning	= nullptr;
	_matrix*					pmatParent		= nullptr;

	// ColliderSphere
	pmatSkinning	= m_pMeshCom->Find_SkinningMatrix("Bip01-L-Hand");
	pmatParent		= &(m_pTransCom->m_matWorld);
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);

	m_pColliderSphereCom->Set_SkinningMatrix(pmatSkinning);		// Bone Matrix
	m_pColliderSphereCom->Set_ParentMatrix(pmatParent);			// Parent Matrix
	m_pColliderSphereCom->Set_Scale(_vec3(2.f, 2.f, 2.f));		// Collider Scale
	m_pColliderSphereCom->Set_Radius(m_pTransCom->m_vScale);	// Collider Radius

	// ColliderBox
	pmatSkinning	= m_pMeshCom->Find_SkinningMatrix("Bip01-R-Hand");
	pmatParent		= &(m_pTransCom->m_matWorld);
	Engine::NULL_CHECK_RETURN(pmatSkinning, E_FAIL);

	m_pColliderBoxCom->Set_SkinningMatrix(pmatSkinning);	// Bone Matrix
	m_pColliderBoxCom->Set_ParentMatrix(pmatParent);		// Parent Matrix
	m_pColliderBoxCom->Set_Scale(_vec3(4.f, 4.f, 4.f));		// Collider Scale
	m_pColliderBoxCom->Set_Extents(m_pTransCom->m_vScale);	// Box Offset From Center

	/*__________________________________________________________________________________________________________
	[ Font 생성 ]
	____________________________________________________________________________________________________________*/
	m_pFont = static_cast<Engine::CFont*>(Engine::CObjectMgr::Get_Instance()->Clone_GameObjectPrototype(L"Prototype_Font_NetmarbleLight"));
	Engine::NULL_CHECK_RETURN(m_pFont, E_FAIL);
	Engine::FAILED_CHECK_RETURN(m_pFont->Ready_GameObject(L"", _vec2(900.f, 0.f), D2D1::ColorF::Yellow), E_FAIL);

	return S_OK;
}

HRESULT CPopori_F::LateInit_GameObject()
{
	/*__________________________________________________________________________________________________________
	[ Get GameObject - DynamicCamera ]
	____________________________________________________________________________________________________________*/
	m_pDynamicCamera = static_cast<CDynamicCamera*>(m_pObjectMgr->Get_GameObject(L"Layer_Camera", L"DynamicCamera"));
	Engine::NULL_CHECK_RETURN(m_pDynamicCamera, E_FAIL);
	m_pDynamicCamera->AddRef();

	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));
	m_pShadowCom->SetUp_ShaderConstantBuffer((_uint)(m_pMeshCom->Get_DiffTexture().size()));

	return S_OK;
}

_int CPopori_F::Update_GameObject(const _float & fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	if (m_bIsDead)
		return DEAD_OBJ;

	/*__________________________________________________________________________________________________________
	[ Key Input ]
	____________________________________________________________________________________________________________*/
	Key_Input(fTimeDelta);

	m_ui3DMax_NumFrame = *(m_pMeshCom->Get_3DMaxNumFrame());
	m_ui3DMax_CurFrame = *(m_pMeshCom->Get_3DMaxCurFrame());

	/*__________________________________________________________________________________________________________
	[ Font Update ]
	____________________________________________________________________________________________________________*/
	if (Engine::CRenderer::Get_Instance()->Get_RenderOnOff(L"Font"))
	{
		m_wstrText = wstring(L"[ Character Info ] \n") +
					 wstring(L"Num Animation \t %d \n") +
					 wstring(L"Current Ani Index \t %d \n") +
					 wstring(L"Max Frame \t %d \n") +
					 wstring(L"Current Frame \t %d");

		wsprintf(m_szText, m_wstrText.c_str(),
				 *(m_pMeshCom->Get_NumAnimation()),
				 m_uiAnimIdx,
				 m_ui3DMax_NumFrame,
				 m_ui3DMax_CurFrame);

		m_pFont->Update_GameObject(fTimeDelta);
		m_pFont->Set_Text(wstring(m_szText));
	}

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_NONALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);


	return NO_EVENT;
}

_int CPopori_F::LateUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::NULL_CHECK_RETURN(m_pRenderer, -1);
	
	Set_ConstantTable();
	Set_ConstantTableShadowDepth();

	return NO_EVENT;
}

void CPopori_F::Render_GameObject(const _float & fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);

	m_pMeshCom->Render_DynamicMesh(m_pShaderCom);
}

void CPopori_F::Render_ShadowDepth(const _float & fTimeDelta)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(m_pShadowCom);
}

void CPopori_F::Render_GameObject(const _float& fTimeDelta, 
								  ID3D12GraphicsCommandList * pCommandList,
								  const _int& iContextIdx)
{
	/*__________________________________________________________________________________________________________
	[ Play Animation ]
	____________________________________________________________________________________________________________*/
	m_pMeshCom->Set_AnimationKey(m_uiAnimIdx);
	m_pMeshCom->Play_Animation(fTimeDelta * TPS);

	m_pMeshCom->Render_DynamicMesh(pCommandList, iContextIdx, m_pShaderCom);
}

void CPopori_F::Render_ShadowDepth(const _float& fTimeDelta, 
								   ID3D12GraphicsCommandList * pCommandList,
								   const _int& iContextIdx)
{
	m_pMeshCom->Render_DynamicMeshShadowDepth(pCommandList, iContextIdx, m_pShadowCom);
}

HRESULT CPopori_F::Add_Component(wstring wstrMeshTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Mesh
	m_pMeshCom = static_cast<Engine::CMesh*>(m_pComponentMgr->Clone_Component(wstrMeshTag.c_str(), Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pMeshCom, E_FAIL);
	m_pMeshCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Mesh", m_pMeshCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderMesh*>(m_pComponentMgr->Clone_Component(L"Prototype_ShaderMesh", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Shadow
	m_pShadowCom = static_cast<Engine::CShaderShadow*>(m_pComponentMgr->Clone_Component(L"Prototype_ShaderShadow", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShadowCom, E_FAIL);
	m_pShadowCom->AddRef();
	Engine::FAILED_CHECK_RETURN(m_pShadowCom->Set_PipelineStatePass(0), E_FAIL);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shadow", m_pShadowCom);

	// Collider - Sphere
	m_pColliderSphereCom = static_cast<Engine::CColliderSphere*>(m_pComponentMgr->Clone_Component(L"Prototype_ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pColliderSphereCom, E_FAIL);
	m_pColliderSphereCom->AddRef();
	m_pColliderSphereCom->Ready_Collider();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderSphere", m_pColliderSphereCom);

	// Collider - Box
	m_pColliderBoxCom = static_cast<Engine::CColliderBox*>(m_pComponentMgr->Clone_Component(L"Prototype_ColliderBox", Engine::COMPONENTID::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pColliderBoxCom, E_FAIL);
	m_pColliderBoxCom->AddRef();
	m_pColliderBoxCom->Ready_Collider();
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_ColliderBox", m_pColliderBoxCom);

	// NaviMesh
	m_pNaviMesh = static_cast<Engine::CNaviMesh*>(m_pComponentMgr->Clone_Component(L"TestNaviMesh", Engine::ID_DYNAMIC));
	Engine::NULL_CHECK_RETURN(m_pNaviMesh, E_FAIL);
	m_mapComponent[Engine::ID_DYNAMIC].emplace(L"Com_NaviMesh", m_pNaviMesh);

	return S_OK;
}

void CPopori_F::Set_ConstantTable()
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::VIEW);
	_matrix* pmatProj = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::PROJECTION);

	if (nullptr == pmatView || nullptr == pmatProj)
		return;

	/*__________________________________________________________________________________________________________
	[ Matrix Info ]
	____________________________________________________________________________________________________________*/
	Engine::CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(Engine::CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * (*pmatView) * (*pmatProj)));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_MatrixDesc.matView, XMMatrixTranspose(*pmatView));
	XMStoreFloat4x4(&tCB_MatrixDesc.matProj, XMMatrixTranspose(*pmatProj));

	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(0, tCB_MatrixDesc);
}

void CPopori_F::Set_ConstantTableShadowDepth()
{
	if (nullptr == m_pDynamicCamera)
		return;

	_vec3 vLightDir			= _vec3(Engine::CLightMgr::Get_Instance()->Get_LightInfo(0).Direction);
	_vec3 vDynamicCamEye	= m_pDynamicCamera->Get_CameraInfo().vEye;

	Engine::CGameObject::SetUp_ShadowDepth(vDynamicCamEye, vDynamicCamEye, vLightDir);

	/*__________________________________________________________________________________________________________
	[ ShadowDepth ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADOWDEPTH_DESC	tCB_ShadowDepthDesc;
	ZeroMemory(&tCB_ShadowDepthDesc, sizeof(Engine::CB_SHADOWDEPTH_DESC));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matWVP, XMMatrixTranspose(m_pTransCom->m_matWorld * m_tShadowInfo.matLightView * m_tShadowInfo.matLightProj));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matWorld, XMMatrixTranspose(m_pTransCom->m_matWorld));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matLightView, XMMatrixTranspose(m_tShadowInfo.matLightView));
	XMStoreFloat4x4(&tCB_ShadowDepthDesc.matLightProj, XMMatrixTranspose(m_tShadowInfo.matLightProj));
	tCB_ShadowDepthDesc.vLightPosition = m_tShadowInfo.vLightPosition;
	tCB_ShadowDepthDesc.fLightPorjFar  = m_tShadowInfo.fLightPorjFar;

	m_pShadowCom->Get_UploadBuffer_ShadowDepthDesc()->CopyData(0, tCB_ShadowDepthDesc);

	/*__________________________________________________________________________________________________________
	[ ShadowInfo ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADOW_DESC	tCB_ShadowDesc;
	ZeroMemory(&tCB_ShadowDesc, sizeof(Engine::CB_SHADOW_DESC));
	XMStoreFloat4x4(&tCB_ShadowDesc.matLightView, XMMatrixTranspose(m_tShadowInfo.matLightView));
	XMStoreFloat4x4(&tCB_ShadowDesc.matLightProj, XMMatrixTranspose(m_tShadowInfo.matLightProj));
	tCB_ShadowDesc.vLightPosition	= m_tShadowInfo.vLightPosition;
	tCB_ShadowDesc.fLightPorjFar	= m_tShadowInfo.fLightPorjFar;

	m_pShaderCom->Get_UploadBuffer_ShadowDesc()->CopyData(0, tCB_ShadowDesc);

}

void CPopori_F::Key_Input(const _float & fTimeDelta)
{
	if (Engine::KEY_DOWN(DIK_1))
	{
		m_uiAnimIdx = 0;
	}

	else if (Engine::KEY_DOWN(DIK_2))
	{
		m_uiAnimIdx = 1;
	}
}

CPopori_F * CPopori_F::Create(ID3D12Device * pGraphicDevice, 
							  ID3D12GraphicsCommandList * pCommandList,
							  wstring wstrMeshTag, 
							  const _vec3 & vScale, 
							  const _vec3 & vAngle, 
							  const _vec3 & vPos)
{
	CPopori_F* pInstance = new CPopori_F(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag, vScale, vAngle, vPos)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPopori_F::Free()
{
	Engine::CGameObject::Free();

	Engine::Safe_Release(m_pDynamicCamera);

	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pShadowCom);
	Engine::Safe_Release(m_pColliderSphereCom);
	Engine::Safe_Release(m_pColliderBoxCom);

	Engine::Safe_Release(m_pFont);
}
