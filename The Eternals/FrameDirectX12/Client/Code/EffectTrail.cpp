#include "stdafx.h"
#include "EffectTrail.h"
#include "Renderer.h"
#include "ObjectMgr.h"
#include "GraphicDevice.h"

CEffectTrail::CEffectTrail(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

HRESULT CEffectTrail::Ready_GameObject(wstring wstrTextureTag, 
									   const _uint& uiTexIdx, 
									   const _vec3& vScale, 
									   const _vec3& vAngle)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::Ready_GameObject(), E_FAIL);
	Engine::FAILED_CHECK_RETURN(Add_Component(wstrTextureTag), E_FAIL);

	m_wstrTextureTag      = wstrTextureTag;
	m_uiTexIdx            = uiTexIdx;
	m_pTransCom->m_vScale = vScale;
	m_pTransCom->m_vAngle = vAngle;
	m_pTransCom->m_vPos	  = _vec3(0.0f);

	Engine::FAILED_CHECK_RETURN(m_pShaderCom->Set_PipelineStatePass(7), E_FAIL);

	return S_OK;
}

HRESULT CEffectTrail::LateInit_GameObject()
{
	// SetUp Shader ConstantBuffer
	m_pShaderCom->SetUp_ShaderConstantBuffer();

	return S_OK;
}

_int CEffectTrail::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

	/*__________________________________________________________________________________________________________
	[ TransCom - Update WorldMatrix ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CEffectTrail::LateUpdate_GameObject(const _float& fTimeDelta)
{
	return NO_EVENT;
}

void CEffectTrail::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	// Vertex滚欺 沥焊 CopyData.
	memcpy(m_pVetexData, m_arrVertices.data(), m_uiVB_ByteSize);

	m_pShaderCom->Begin_Shader(m_pTextureCom->Get_TexDescriptorHeap(), 0, m_uiTexIdx, Engine::MATRIXID::PROJECTION);

	m_pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView());
	m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView());
	m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pCommandList->DrawIndexedInstanced(m_tSubMeshGeometry.uiIndexCount, 1, 0, 0, 0);	
}

void CEffectTrail::SetUp_TrailByCatmullRom(_vec3* vMin, _vec3* vMax)
{
	for (_int i = TRAIL_SIZE - 2; i > 0; --i)
		m_arrMax[i] = m_arrMax[i - 1];

	m_arrMax[0] = *vMax;

	// CatmullRom
	for (_int i = 2; i < TRAIL_SIZE - 2; ++i)
	{
		_vec3 catmullrom;
		catmullrom.Vector3CatmullRom(m_arrMax[i - 2], m_arrMax[i - 1], m_arrMax[i], m_arrMax[i + 1], 0.5f);

		m_arrMax[i + 1] = (m_arrMax[i] + m_arrMax[i + 1]) * 0.5f;
		m_arrMax[i]     = (catmullrom + m_arrMax[i]) * 0.5f;
		m_arrMax[i - 1] = (catmullrom + m_arrMax[i - 1]) * 0.5f;
	}

	// SetUp Vertex
	m_arrVertices[0].vPos   = _vec3(vMin->x, vMin->y, vMin->z);
	m_arrVertices[0].vTexUV = _vec2(0.0f, 1.0f);

	for (int i = 0; i < TRAIL_SIZE - 1; ++i)
	{
		float fTex = _float(i) / (TRAIL_SIZE - 2);

		if (i > 2 && i < TRAIL_SIZE - 2)
		{
			m_arrVertices[i + 1].vPos   = _vec3(m_arrMax[i].x, m_arrMax[i].y, m_arrMax[i].z);
			m_arrVertices[i + 1].vTexUV = _vec2(fTex, 0.f);
		}
		else
		{
			m_arrVertices[i + 1].vPos   = _vec3(m_arrMax[i].x, m_arrMax[i].y, m_arrMax[i].z);
			m_arrVertices[i + 1].vTexUV = _vec2(fTex, 0.f);
		}
	}
}

HRESULT CEffectTrail::Add_Component(wstring wstrTextureTag)
{
	Engine::NULL_CHECK_RETURN(m_pComponentMgr, E_FAIL);

	// Texture
	m_pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Clone_Component(wstrTextureTag, Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pTextureCom, E_FAIL);
	m_pTextureCom->AddRef();
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Texture", m_pTextureCom);

	// Shader
	m_pShaderCom = static_cast<Engine::CShaderTexture*>(m_pComponentMgr->Clone_Component(L"ShaderTexture", Engine::COMPONENTID::ID_STATIC));
	Engine::NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();;
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Trail Buffer
	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_arrVertices.size(); ++i)
	{
		m_arrVertices[i].vPos   = _vec3(0.0f);
		m_arrVertices[i].vTexUV = _vec2(0.0f);
	}

	for (_uint i = 0; i < m_arrMax.size(); ++i)
		m_arrMax[i] = _vec3(0.0f);


	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, (TRAIL_SIZE - 2) * 3> indices;
	// _uint j = 0;
	for (_uint i = 0, j = 0; i < (TRAIL_SIZE - 3) * 3; i += 3, ++j)
	{
		indices[i]		= 0;
		indices[i + 1]	= j + 1;
		indices[i + 2]	= j + 2;
	}


	const _int uiVB_ByteSize = (_uint)m_arrVertices.size() * sizeof(Engine::VTXTEX);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), m_arrVertices.data(), uiVB_ByteSize);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	// VB 积己.
	m_pVB_GPU = Create_DynamicVertex(m_arrVertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	Engine::NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);
	// IB 积己.
	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	Engine::NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);

	m_uiVertexByteStride = sizeof(Engine::VTXTEX);
	m_uiVB_ByteSize		 = uiVB_ByteSize;
	m_uiIB_ByteSize		 = uiIB_ByteSize;
	m_IndexFormat		 = DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CEffectTrail::Set_ConstantTable()
{
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_TEXTURE tCB_ShaderTexture;
	ZeroMemory(&tCB_ShaderTexture, sizeof(Engine::CB_SHADER_TEXTURE));
	tCB_ShaderTexture.matWorld	= Engine::CShader::Compute_MatrixTranspose(INIT_MATRIX);

	m_pShaderCom->Get_UploadBuffer_ShaderTexture()->CopyData(0, tCB_ShaderTexture);
}

ID3D12Resource* CEffectTrail::Create_DynamicVertex(const void* InitData, 
												   UINT64 uiByteSize, 
												   ID3D12Resource*& pUploadBuffer)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(Engine::CMDID::CMD_MAIN);

	ID3D12Resource* pDefaultBuffer = nullptr;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&pDefaultBuffer)),
																		  NULL);

	// Write Vertex Data to pVertexDataBegin dynamically
	pDefaultBuffer->Map(0, nullptr, (void**)&m_pVetexData);
	
	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(Engine::CMDID::CMD_MAIN);

	return pDefaultBuffer;
}

ID3D12Resource* CEffectTrail::Create_DefaultBuffer(const void* InitData, 
												   UINT64 uiByteSize, 
												   ID3D12Resource*& pUploadBuffer)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(Engine::CMDID::CMD_MAIN);

	ID3D12Resource* pDefaultBuffer = nullptr;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_COMMON,
																		  nullptr,
																		  IID_PPV_ARGS(&pDefaultBuffer)), 
																		  NULL);
 
	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&pUploadBuffer)), 
																		  NULL);

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData		= InitData;
	subResourceData.RowPitch	= uiByteSize;
	subResourceData.SlicePitch	= subResourceData.RowPitch;

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer,
																			 D3D12_RESOURCE_STATE_COMMON, 
																			 D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(m_pCommandList, pDefaultBuffer, pUploadBuffer, 0, 0, 1, &subResourceData);

	m_pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer,
																			 D3D12_RESOURCE_STATE_COPY_DEST, 
																			 D3D12_RESOURCE_STATE_GENERIC_READ));

	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(Engine::CMDID::CMD_MAIN);

	return pDefaultBuffer;
}

D3D12_VERTEX_BUFFER_VIEW CEffectTrail::Get_VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	VertexBufferView.BufferLocation	= m_pVB_GPU->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes	= m_uiVertexByteStride;
	VertexBufferView.SizeInBytes	= m_uiVB_ByteSize;

	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW CEffectTrail::Get_IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	IndexBufferView.BufferLocation	= m_pIB_GPU->GetGPUVirtualAddress();
	IndexBufferView.SizeInBytes		= m_uiIB_ByteSize;
	IndexBufferView.Format			= m_IndexFormat;

	return IndexBufferView;
}

CEffectTrail* CEffectTrail::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
								   wstring wstrTextureTag,
								   const _uint& uiTexIdx, 
								   const _vec3& vScale, 
								   const _vec3& vAngle)
{
	CEffectTrail* pInstance = new CEffectTrail(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrTextureTag, uiTexIdx, vScale, vAngle)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CEffectTrail::Free()
{
	Engine::CGameObject::Free();
	//Engine::Safe_Release(m_pBufferCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pTextureCom);

	Engine::Safe_Release(m_pVB_CPU);
	Engine::Safe_Release(m_pIB_CPU);
	Engine::Safe_Release(m_pVB_GPU);
	Engine::Safe_Release(m_pIB_GPU);
}
