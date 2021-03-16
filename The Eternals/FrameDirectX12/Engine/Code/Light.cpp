#include "Light.h"
#include "GraphicDevice.h"
#include "ComponentMgr.h"
#include "Renderer.h"

USING(Engine)

CLight::CLight(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
	, m_pShaderInstancing(CShaderLightingInstancing::Get_Instance())
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
		// SetUp Instancing
		m_iPipelineStatePass = 0;
		m_pShaderInstancing->Add_TotalInstancCount(m_iPipelineStatePass);
	}
	if (m_tLightInfo.Type == LIGHTTYPE::D3DLIGHT_POINT)
	{
		// SetUp Instancing
		m_iPipelineStatePass = 1;
		m_pShaderInstancing->Add_TotalInstancCount(m_iPipelineStatePass);

		// Collider
		m_pColliderCom = static_cast<Engine::CColliderBox*>(CComponentMgr::Get_Instance()->Clone_Component(L"ColliderBox", COMPONENTID::ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pColliderCom, E_FAIL);
		m_pColliderCom->Set_PipelineStatePass(0);
		m_pColliderCom->Set_Pos(_vec3(m_tLightInfo.Position.x, m_tLightInfo.Position.y, m_tLightInfo.Position.z));
		m_pColliderCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));
		m_pColliderCom->Set_Extents(_vec3(1.0f));
		m_pColliderCom->Set_Color(m_tLightInfo.Diffuse);
		m_pColliderCom->Update_Component(0.0f);
	}

	return S_OK;
}

_int CLight::Update_Light()
{
	if (m_bIsDead)
		return DEAD_OBJ;

	if (D3DLIGHT_POINT == m_tLightInfo.Type &&
		CRenderer::Get_Instance()->Get_Frustum().Contains(m_pColliderCom->Get_BoundingInfo()) != DirectX::DISJOINT)
	{
		m_pColliderCom->Update_Component(0.0f);
	}

	return NO_EVENT;
}

void CLight::Render_Light()
{
	// Frustum Culling
	if (D3DLIGHT_POINT == m_tLightInfo.Type &&
		CRenderer::Get_Instance()->Get_Frustum().Contains(m_pColliderCom->Get_BoundingInfo()) != DirectX::DISJOINT)
	{
		/*__________________________________________________________________________________________________________
		[ Add Instance ]
		____________________________________________________________________________________________________________*/
		m_pShaderInstancing->Add_Instance(m_iPipelineStatePass);
		_uint iInstanceIdx = m_pShaderInstancing->Get_InstanceCount(m_iPipelineStatePass) - 1;
		
		Set_ConstantTable(iInstanceIdx);
	}
	else if (D3DLIGHT_DIRECTIONAL == m_tLightInfo.Type)
	{
		/*__________________________________________________________________________________________________________
		[ Add Instance ]
		____________________________________________________________________________________________________________*/
		m_pShaderInstancing->Add_Instance(m_iPipelineStatePass);
		_uint iInstanceIdx = m_pShaderInstancing->Get_InstanceCount(m_iPipelineStatePass) - 1;

		Set_ConstantTable(iInstanceIdx);
	}
}

void CLight::Set_ConstantTable(const _int& iInstanceIdx)
{
	_matrix* pmatView = CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::VIEW);
	_matrix* pmatProj = CGraphicDevice::Get_Instance()->Get_Transform(MATRIXID::PROJECTION);

	_matrix matViewInv	= MATRIX_INVERSE(*pmatView);
	_matrix matProjInv	= MATRIX_INVERSE(*pmatProj);
	_vec4	vCameraPos	= INIT_VEC4(0.0f);
	memcpy(&vCameraPos, &matViewInv._41, sizeof(_vec4));

	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	CB_SHADER_LIGHTING	tCB_ShaderLighting;
	ZeroMemory(&tCB_ShaderLighting, sizeof(CB_SHADER_LIGHTING));
	tCB_ShaderLighting.matViewInv		= CShader::Compute_MatrixTranspose(matViewInv);
	tCB_ShaderLighting.matProjInv		= CShader::Compute_MatrixTranspose(matProjInv);
	tCB_ShaderLighting.vCameraPos		= vCameraPos;
	tCB_ShaderLighting.vProjFar			= _vec4(1000.0f);
	tCB_ShaderLighting.vLightDiffuse	= m_tLightInfo.Diffuse;
	tCB_ShaderLighting.vLightSpecular	= m_tLightInfo.Specular;
	tCB_ShaderLighting.vLightAmbient	= m_tLightInfo.Ambient;
	tCB_ShaderLighting.vLightDir		= m_tLightInfo.Direction;
	tCB_ShaderLighting.vLightPos		= m_tLightInfo.Position;
	tCB_ShaderLighting.vLightRange		= _vec4(m_tLightInfo.Range);

	m_pShaderInstancing->Get_UploadBuffer_ShaderLighting(m_iPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderLighting);
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

	if (m_tLightInfo.Type == D3DLIGHT_POINT)
		Safe_Release(m_pColliderCom);

}
