#include "ColliderSphere.h"
#include "GraphicDevice.h"
#include "Renderer.h"
#include "ComponentMgr.h"

USING(Engine)

CColliderSphere::CColliderSphere(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CVIBuffer(pGraphicDevice, pCommandList)
{
	ZeroMemory(&m_BoundingInfo, sizeof(BoundingSphere));
}

CColliderSphere::CColliderSphere(const CColliderSphere & rhs)
	: CVIBuffer(rhs)
	, m_BoundingInfo(rhs.m_BoundingInfo)
	// , m_pShaderColorInstancing(CShaderColorInstancing::Get_Instance())
{
}

HRESULT CColliderSphere::Ready_Buffer(const _uint& uiSliceCnt, const _uint& uiStackCnt)
{
	FAILED_CHECK_RETURN(CVIBuffer::Ready_Buffer(), E_FAIL);

	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_BoundingInfo.Center = _vec3(0.0f, 0.0f, 0.0f);
	m_BoundingInfo.Radius = 0.5f;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	// - Compute the vertices stating at the top pole and moving down the stacks.
	VTXCOL topVertex	= VTXCOL(_vec3(0.0f, +m_BoundingInfo.Radius, 0.0f), _rgba(RANDOM_COLOR));
	VTXCOL bottomVertex = VTXCOL(_vec3(0.0f, -m_BoundingInfo.Radius, 0.0f), _rgba(RANDOM_COLOR));

	_float phiStep		= XM_PI / uiStackCnt;
	_float thetaStep	= 2.0f * XM_PI / uiSliceCnt;

	// - Compute vertices for each stack ring (do not count the poles as rings).
	vector<VTXCOL> vertices;

	vertices.emplace_back(topVertex);

	for (_uint i = 1; i <= uiStackCnt - 1; ++i)
	{
		_float phi = i * phiStep;

		// vertices of ring.
		for (_uint j = 0; j <= uiSliceCnt; ++j)
		{
			float theta = j * thetaStep;

			VTXCOL vtx;

			// spherical to cartesian
			vtx.vPos.x = m_BoundingInfo.Radius * sinf(phi)*cosf(theta);
			vtx.vPos.y = m_BoundingInfo.Radius * cosf(phi);
			vtx.vPos.z = m_BoundingInfo.Radius * sinf(phi)*sinf(theta);

			vtx.vColor = _rgba(RANDOM_COLOR);

			vertices.emplace_back(vtx);
		}

	}

	vertices.emplace_back(bottomVertex);

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	// - Compute indices for top stack.
	// - The top stack was written first to the vertex buffer and connects the top pole to the first ring.
	vector<_uint> indices;

	for (_uint i = 1; i <= uiSliceCnt; ++i)
	{
		indices.emplace_back(0);
		indices.emplace_back(i + 1);
		indices.emplace_back(i);
	}

	// - Compute indices for inner stacks (not connected to poles).
	// - Offset the indices to the index of the first vertex in the first ring.
	// - This is just skipping the top pole vertex.
	_uint baseIndex			= 1;
	_uint ringVertexCount	= uiSliceCnt + 1;

	for (_uint i = 0; i < uiStackCnt - 2; ++i)
	{
		for (_uint j = 0; j < uiSliceCnt; ++j)
		{
			indices.emplace_back(baseIndex + i * ringVertexCount + j);
			indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
			indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.emplace_back(baseIndex + i * ringVertexCount + j + 1);
			indices.emplace_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	// - Compute indices for bottom stack.
	// - The bottom stack was written last to the vertex buffer
	// - and connects the bottom pole to the bottom ring.

	// - South pole vertex was added last.
	_uint southPoleIndex = (_uint)vertices.size() - 1;

	// - Offset the indices to the index of the first vertex in the last ring.
	baseIndex = southPoleIndex - ringVertexCount;

	for (_uint i = 0; i < uiSliceCnt; ++i)
	{
		indices.emplace_back(southPoleIndex);
		indices.emplace_back(baseIndex + i);
		indices.emplace_back(baseIndex + i + 1);
	}


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

HRESULT CColliderSphere::Ready_Collider()
{
	FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	m_pTransCom->m_vPos		= _vec3(0.0f, 0.0f, 0.0f);
	m_pTransCom->m_vAngle	= _vec3(0.0f, 0.0f, 0.0f);
	m_pTransCom->m_vScale	= _vec3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

void CColliderSphere::Update_Component(const _float & fTimeDelta)
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
	[ Final Collider Matrix ]
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
}

void CColliderSphere::Render_Component(const _float & fTimeDelta)
{
	Set_ConstantTable();

	m_pShaderCom->Begin_Shader();
	Begin_Buffer();
	Render_Buffer();


	///*__________________________________________________________________________________________________________
	//[ Add Instance ]
	//____________________________________________________________________________________________________________*/
	//m_pShaderColorInstancing->Add_Instance(COLOR_BUFFER::BUFFER_SPHERE, m_uiColorPipelineStatePass);
	//_uint iInstanceIdx = m_pShaderColorInstancing->Get_InstanceCount(COLOR_BUFFER::BUFFER_SPHERE, m_uiColorPipelineStatePass) - 1;

	//Set_ConstantTable(COLOR_BUFFER::BUFFER_SPHERE, iInstanceIdx);

}

void CColliderSphere::Begin_Buffer()
{
	CVIBuffer::Begin_Buffer();
}

void CColliderSphere::Render_Buffer()
{
	CVIBuffer::Render_Buffer();
}

HRESULT CColliderSphere::Add_Component()
{
	// Transform
	m_pTransCom = CTransform::Create();
	NULL_CHECK_RETURN(m_pTransCom, E_FAIL);

	// Shader
	m_pShaderCom = static_cast<CShaderColor*>(CComponentMgr::Get_Instance()->Clone_Component(L"ShaderColor", COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->Set_PipelineStatePass(1);

	m_uiColorPipelineStatePass = 1;

	return S_OK;
}

void CColliderSphere::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderColor.vColor		= m_vColor;

	m_pShaderCom->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
}

void CColliderSphere::Set_ConstantTable(const COLOR_BUFFER& eBuffer, const _int& iInstanceIdx)
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= CShader::Compute_MatrixTranspose(m_pTransCom->m_matWorld);
	tCB_ShaderColor.vColor		= m_vColor;

	m_pShaderColorInstancing->Get_UploadBuffer_ShaderColor(eBuffer, m_uiColorPipelineStatePass)->CopyData(iInstanceIdx, tCB_ShaderColor);
}

CComponent * CColliderSphere::Clone()
{
	 CComponent* pComponent = new CColliderSphere(*this);
	 static_cast<CColliderSphere*>(pComponent)->Ready_Collider();

	CShaderColorInstancing::Get_Instance()->Add_TotalInstancCount(COLOR_BUFFER::BUFFER_SPHERE);

	return pComponent;
}

CColliderSphere * CColliderSphere::Create(ID3D12Device * pGraphicDevice, 
										  ID3D12GraphicsCommandList * pCommandList,
										  const _uint& uiSliceCnt,
										  const _uint& uiStackCnt)
{
	CColliderSphere* pInstance = new CColliderSphere(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Buffer(uiSliceCnt, uiStackCnt)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CColliderSphere::Free()
{
	CVIBuffer::Free();
	CShaderColorInstancing::Get_Instance()->Erase_TotalInstanceCount(COLOR_BUFFER::BUFFER_SPHERE);
	Safe_Release(m_pTransCom);
	Safe_Release(m_pShaderCom);
}
