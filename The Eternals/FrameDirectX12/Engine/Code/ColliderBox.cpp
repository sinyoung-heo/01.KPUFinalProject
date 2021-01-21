#include "ColliderBox.h"

#include "GraphicDevice.h"
#include "Renderer.h"
#include "ComponentMgr.h"

USING(Engine)

CColliderBox::CColliderBox(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
	ZeroMemory(&m_BoundingInfo, sizeof(BoundingSphere));
}

CColliderBox::CColliderBox(const CColliderBox & rhs)
	: CVIBuffer(rhs)
	, m_BoundingInfo(rhs.m_BoundingInfo)
	, m_arrCorners(rhs.m_arrCorners)
{
}

void CColliderBox::Set_Extents(const _vec3 & vParentScale)
{
	m_BoundingInfo.Extents.x *= m_pTransCom->m_vScale.x * vParentScale.x;
	m_BoundingInfo.Extents.y *= m_pTransCom->m_vScale.y * vParentScale.y;
	m_BoundingInfo.Extents.z *= m_pTransCom->m_vScale.z * vParentScale.z;

	m_vLength.x = m_BoundingInfo.Extents.x * 2.f;
	m_vLength.y = m_BoundingInfo.Extents.y * 2.f;
	m_vLength.z = m_BoundingInfo.Extents.z * 2.f;
}


HRESULT CColliderBox::Ready_Buffer()
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BoundingInfo.Center		= _vec3(0.0f, 0.0f, 0.0f);
	m_BoundingInfo.Extents.x	= 0.5f;	// +,- offset
	m_BoundingInfo.Extents.y	= 0.5f;	// +,- offset
	m_BoundingInfo.Extents.z	= 0.5f;	// +,- offset

	m_BoundingInfo.GetCorners(m_arrCorners.data());

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<VTXCOL, 8> vertices =
	{
		VTXCOL(_vec3(m_arrCorners[0]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[1]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[2]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[3]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[4]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[5]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[6]), _rgba(RANDOM_COLOR)),
		VTXCOL(_vec3(m_arrCorners[7]), _rgba(RANDOM_COLOR))
	};

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, 36> indices =
	{
		1, 5, 6,	
		1, 6, 2,

		4, 0, 3,
		4, 3, 7,

		4, 5, 1,
		4, 1, 0,

		3, 2, 6,
		3, 6, 7,

		7, 6, 5,
		7, 5, 4,

		0, 1, 2,
		0, 2, 3
	};

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(VTXCOL);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);

	FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	// VB 持失.
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	// IB 持失.
	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);


	m_uiVertexByteStride	= sizeof(VTXCOL);
	m_uiVB_ByteSize			= uiVB_ByteSize;
	m_uiIB_ByteSize			= uiIB_ByteSize;
	m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

HRESULT CColliderBox::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vPos		= _vec3(0.0f, 0.0f, 0.0f);
	m_pTransCom->m_vAngle	= _vec3(0.0f, 0.0f, 0.0f);
	m_pTransCom->m_vScale	= _vec3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

void CColliderBox::Update_Component(const _float & fTimeDelta)
{
	/*__________________________________________________________________________________________________________
	[ Renderer - Add RenderGroup ]
	____________________________________________________________________________________________________________*/
	if (CRenderer::Get_Instance()->Get_RenderOnOff(L"Collider"))
	{
		if (E_FAIL == CRenderer::Get_Instance()->Add_Renderer(this))
			return;
	}


	/*__________________________________________________________________________________________________________
	[ Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pTransCom)
		m_pTransCom->Update_Component(fTimeDelta);

	/*__________________________________________________________________________________________________________
	[ Finall Collider Matrix ]
	____________________________________________________________________________________________________________*/
	if (nullptr != m_pSkinningMatrix && nullptr != m_pParentMatrix)
	{
		_matrix matBoneFinalTransform = ((m_pSkinningMatrix->matBoneScale
									  * m_pSkinningMatrix->matBoneRotation
									  * m_pSkinningMatrix->matBoneTrans)
									  * m_pSkinningMatrix->matParentTransform)
									  * m_pSkinningMatrix->matRootTransform;

		m_pTransCom->m_matWorld *= (matBoneFinalTransform) * (*m_pParentMatrix);
	}
	else if (nullptr == m_pSkinningMatrix && nullptr != m_pParentMatrix)
		m_pTransCom->m_matWorld *= (*m_pParentMatrix);

	m_BoundingInfo.Center = m_pTransCom->Get_PositionVector();
	m_BoundingInfo.GetCorners(m_arrCorners.data());

}

void CColliderBox::Render_Component(const _float & fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader();
	Begin_Buffer();

	Render_Buffer();
}

void CColliderBox::Begin_Buffer()
{
	CVIBuffer::Begin_Buffer();
}



void CColliderBox::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

HRESULT CColliderBox::Add_Component()
{
	// Transform
	m_pTransCom = CTransform::Create();
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);

	// Shader
	m_pShaderCom = static_cast<CShaderColor*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderColor", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->Set_PipelineStatePass(1);

	return S_OK;
}

void CColliderBox::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	CB_SHADER_COLOR tCB_ColorShader;
	ZeroMemory(&tCB_ColorShader, sizeof(CB_SHADER_COLOR));
	tCB_ColorShader.matWorld	= CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ColorShader.vColor		= m_vColor;

	m_pShaderCom->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ColorShader);
}

CComponent * CColliderBox::Clone()
{
	 CComponent* pComponent = new CColliderBox(*this);
	 static_cast<CColliderBox*>(pComponent)->Ready_Collider();

	// return new CColliderBox(*this);
	return pComponent;
}

CColliderBox * CColliderBox::Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
{
	CColliderBox* pInstance = new CColliderBox(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CColliderBox::Free()
{
	CVIBuffer::Free();

	Safe_Release(m_pTransCom);
	Safe_Release(m_pShaderCom);
}
