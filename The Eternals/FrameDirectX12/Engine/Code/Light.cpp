#include "Light.h"

#include "GraphicDevice.h"
#include "ComponentMgr.h"
#include "Renderer.h"

USING(Engine)

CLight::CLight(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
{
}

void CLight::Set_ColliderColorSelected()
{
	m_pColliderCom->Set_Color(m_tLightInfo.Diffuse);
	m_pColliderCom->Set_PipelineStatePass(1);
}

void CLight::Set_ColliderColorDiffuse()
{
	m_pColliderCom->Set_Color(m_tLightInfo.Diffuse);
	m_pColliderCom->Set_PipelineStatePass(0);
}

void CLight::Set_ColliderPosition()
{
	m_pColliderCom->Set_Pos(_vec3(m_tLightInfo.Position.x, m_tLightInfo.Position.y, m_tLightInfo.Position.z));
}

HRESULT CLight::Ready_Light(const D3DLIGHT & tLightInfo)
{
	m_tLightInfo = tLightInfo;

	// Buffer
	m_pBufferCom = static_cast<Engine::CScreenTex*>(CComponentMgr::Get_Instance()->Clone_Component(L"ScreenTex", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pBufferCom, E_FAIL);

	if (m_tLightInfo.Type == LIGHTTYPE::D3DLIGHT_DIRECTIONAL)
	{
		// Shader
		m_pShaderCom = static_cast<Engine::CShaderLighting*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderLighting", COMPONENTID::ID_STATIC));
		NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(0), E_FAIL);
	}
	if (m_tLightInfo.Type == LIGHTTYPE::D3DLIGHT_POINT)
	{
		// Shader
		m_pShaderCom = static_cast<Engine::CShaderLighting*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderLighting", COMPONENTID::ID_STATIC));
		NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
		FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(1), E_FAIL);

		// Collider
		m_pColliderCom = static_cast<Engine::CColliderBox*>(CComponentMgr::Get_Instance()->Clone_Component(L"ColliderBox", COMPONENTID::ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pColliderCom, E_FAIL);
		m_pColliderCom->Set_PipelineStatePass(0);
		m_pColliderCom->Set_Pos(_vec3(m_tLightInfo.Position.x, m_tLightInfo.Position.y, m_tLightInfo.Position.z));
		m_pColliderCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));
		m_pColliderCom->Set_Extents(_vec3(1.0f));
		m_pColliderCom->Set_Color(m_tLightInfo.Diffuse);
		m_pColliderCom->Get_Transform()->Update_Component(0.0f);
	}



	return S_OK;
}

_int CLight::Update_Light()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	m_pColliderCom->Update_Component(0.0f);

	return NO_EVENT;
}

void CLight::Render_Light(vector<ComPtr<ID3D12Resource>> pvecTargetTexture)
{
	if (!m_bIsSetTexture)
	{
		m_bIsSetTexture = true;

		// TargetTexture를 Set해준다.
		m_pShaderCom->SetUp_ShaderTexture(pvecTargetTexture);
	}

	if (CRenderer::Get_Instance()->Get_RenderOnOff(L"Collider") &&
		D3DLIGHT_POINT == m_tLightInfo.Type)
	{
		CRenderer::Get_Instance()->Add_Renderer(m_pColliderCom);
	}

	Set_ConstantTable();

	m_pShaderCom->Begin_Shader();
	m_pBufferCom->Begin_Buffer();

	m_pBufferCom->Render_Buffer();
}

void CLight::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Light Info ]
	____________________________________________________________________________________________________________*/
	CB_LIGHT_DESC	tCB_LightDesc;
	ZeroMemory(&tCB_LightDesc, sizeof(CB_LIGHT_DESC));

	tCB_LightDesc.vLightDiffuse		= m_tLightInfo.Diffuse;
	tCB_LightDesc.vLightSpecular	= m_tLightInfo.Specular;
	tCB_LightDesc.vLightAmibient	= m_tLightInfo.Ambient;
	tCB_LightDesc.vLightDirection	= m_tLightInfo.Direction;

	tCB_LightDesc.vLightPosition	= m_tLightInfo.Position;
	tCB_LightDesc.fLightRange		= m_tLightInfo.Range;

	m_pShaderCom->Get_UploadBuffer_LightDesc()->CopyData(0, tCB_LightDesc);

	/*__________________________________________________________________________________________________________
	[ Camera Info ]
	____________________________________________________________________________________________________________*/
	CB_CAMERA_DESC tCB_CameraDesc;
	ZeroMemory(&tCB_CameraDesc, sizeof(CB_CAMERA_DESC));

	_matrix* pmatView = CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::VIEW);
	_matrix* pmatProj = CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::PROJECTION);
	if (nullptr == pmatView ||
		nullptr == pmatProj)
	{
		m_pShaderCom->Get_UploadBuffer_CameraDesc()->CopyData(0, tCB_CameraDesc);
		return;
	}

	_matrix matViewInv		= MATRIX_INVERSE(*pmatView);
	_matrix matProjInv		= MATRIX_INVERSE(*pmatProj);
	_vec4	vCameraPosition = INIT_VEC4(0.0f);
	memcpy(&vCameraPosition, &matViewInv._41, sizeof(_vec4));

	XMStoreFloat4x4(&tCB_CameraDesc.matViewInv, XMMatrixTranspose(matViewInv));
	XMStoreFloat4x4(&tCB_CameraDesc.matProjInv, XMMatrixTranspose(matProjInv));
	tCB_CameraDesc.vCameraPosition	= vCameraPosition;
	tCB_CameraDesc.fProjNear		= 1.0f;
	tCB_CameraDesc.fProjFar			= 1000.0f;

	m_pShaderCom->Get_UploadBuffer_CameraDesc()->CopyData(0, tCB_CameraDesc);
}

CLight * CLight::Create(ID3D12Device * pGraphicDevice,
						ID3D12GraphicsCommandList * pCommandList,
						const D3DLIGHT & tLightInfo)
{
	CLight* pInstance = new CLight(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Light(tLightInfo)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);

	if (m_tLightInfo.Type == D3DLIGHT_POINT)
		Safe_Release(m_pColliderCom);

}
