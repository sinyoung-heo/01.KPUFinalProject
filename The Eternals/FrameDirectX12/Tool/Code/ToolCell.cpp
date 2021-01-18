#include "stdafx.h"
#include "ToolCell.h"
#include "GraphicDevice.h"
#include "ComponentMgr.h"

CToolCell::CToolCell(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

CToolCell::CToolCell(const CToolCell& rhs)
	: Engine::CGameObject(rhs)
{
}

HRESULT CToolCell::Ready_GameObject(const _ulong& dwIndex, 
									_vec3& vPointA, 
									_vec3& vPointB, 
									_vec3& vPointC)
{
	CheckClockWise(vPointA, vPointB, vPointC);

	m_bIsShare			= false;
	m_dwCurrentIdx		= dwIndex;
	m_pPoint[POINT_A]	= new _vec3(vPointA);
	m_pPoint[POINT_B]	= new _vec3(vPointB);
	m_pPoint[POINT_C]	= new _vec3(vPointC);
	m_vCenter			= (*m_pPoint[POINT_A] + *m_pPoint[POINT_B] + *m_pPoint[POINT_C]) / 3.f;

	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

HRESULT CToolCell::Ready_GameObject(const _ulong& dwIndex,
									_vec3* pSharePointA,
									_vec3& vNewPointB,
									_vec3* pSharePointC,
									const _bool& bIsFindNear)
{
	// �ð�����̸�
	if (CheckClockWise(pSharePointA, &vNewPointB, pSharePointC))
	{
		m_pPoint[POINT_A] = pSharePointA;
		m_pPoint[POINT_B] = new _vec3(vNewPointB);
		m_pPoint[POINT_C] = pSharePointC;
		m_bIsClockwise = true;
	}
	// �ݽð�����̸�
	else
	{
		m_pPoint[POINT_A] = pSharePointA;
		m_pPoint[POINT_B] = pSharePointC;
		m_pPoint[POINT_C] = new _vec3(vNewPointB);
		m_bIsClockwise = false;
	}

	m_bIsShare			= true;
	m_bIsFindNear		= bIsFindNear;
	m_dwCurrentIdx		= dwIndex;
	m_vCenter			= (*m_pPoint[POINT_A] + *m_pPoint[POINT_B] + *m_pPoint[POINT_C]) / 3.f;

	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

HRESULT CToolCell::Ready_GameObject(const _ulong& dwIndex, 
									_vec3* pSharePointA, 
									_vec3* pSharePointB,
									_vec3* pSharePointC,
									const _bool& bIsFindNear)
{
	// �ð�����̸�
	if (CheckClockWise(pSharePointA, pSharePointB, pSharePointC))
	{
		m_pPoint[POINT_A] = pSharePointA;
		m_pPoint[POINT_B] = pSharePointB;
		m_pPoint[POINT_C] = pSharePointC;
		m_bIsClockwise = true;
	}
	// �ݽð�����̸�
	else
	{
		m_pPoint[POINT_A] = pSharePointA;
		m_pPoint[POINT_B] = pSharePointC;
		m_pPoint[POINT_C] = pSharePointB;
		m_bIsClockwise = false;
	}

	m_bIsShare			= true;
	m_bIsFindNear		= bIsFindNear;
	m_dwCurrentIdx		= dwIndex;
	m_vCenter			= (*m_pPoint[POINT_A] + *m_pPoint[POINT_B] + *m_pPoint[POINT_C]) / 3.f;

	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);


	return S_OK;
}

HRESULT CToolCell::LateInit_GameObject()
{
	return S_OK;
}

_int CToolCell::Update_GameObject(const _float& fTimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	Engine::CGameObject::LateInit_GameObject();

	// Collider ��ġ ����.
	for (_int i = 0; i < POINT_END; ++i)
	{
		m_matWorld[i] = XMMatrixTranslation(m_pPoint[i]->x, m_pPoint[i]->y, m_pPoint[i]->z);
		m_pColliderCom[i]->Set_ParentMatrix(&m_matWorld[i]);
		m_pColliderCom[i]->Update_Component(fTimeDelta);
	}

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(Engine::CRenderer::RENDER_ALPHA, this), -1);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return NO_EVENT;
}

_int CToolCell::LateUpdate_GameObject(const _float& fTimeDelta)
{
	return NO_EVENT;
}

void CToolCell::Render_GameObject(const _float& fTimeDelta)
{
	Set_ConstantTable();
	m_pShaderCom->Begin_Shader();
	/*__________________________________________________________________________________________________________
	[ �޽��� ���� ���� ��� �ε��� ���� �並 ���� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetVertexBuffers(0, 						 // ���� ����. (�Է� ������ �� 16��)
									   1, 						 // �Է� ���Ե鿡 ���� ���� ���� ����.
									   &Get_VertexBufferView()); // ���� ���� ���� ù ���Ҹ� ����Ű�� ������.

	m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView());

	/*__________________________________________________________________________________________________________
	[ �޽��� ������Ƽ�� ������ ���� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	/*__________________________________________________________________________________________________________
	[ �������� ������������ �Է� ������ �ܰ�� ���� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->DrawIndexedInstanced(m_tSubMeshGeometry.uiIndexCount,	// �׸��⿡ ����� �ε������� ����. (�ν��Ͻ� ��)
										 1,									// �׸� �ν��Ͻ� ����.
										 0,									// �ε��� ������ ù index
										 0, 								// �׸��� ȣ�⿡ ���̴� �ε����鿡 ���� ���� ��.
										 0);	
}

HRESULT CToolCell::Add_Component()
{
	// Shader
	m_pShaderCom = static_cast<Engine::CShaderColor*>(Engine::CComponentMgr::Get_Instance()->Clone_Component(L"ShaderColor", Engine::COMPONENTID::ID_STATIC));
	NULL_CHECK_RETURN(m_pShaderCom, E_FAIL);
	m_pShaderCom->AddRef();
	m_pShaderCom->Set_PipelineStatePass(1);
	m_mapComponent[Engine::ID_STATIC].emplace(L"Com_Shader", m_pShaderCom);

	// Collider
	for (_int i = 0; i < POINT_END; ++i)
	{
		m_pColliderCom[i] = static_cast<Engine::CColliderSphere*>(Engine::CComponentMgr::Get_Instance()->Clone_Component(L"ColliderSphere", Engine::COMPONENTID::ID_DYNAMIC));
		NULL_CHECK_RETURN(m_pColliderCom[i], E_FAIL);
		// m_pColliderCom[i]->AddRef();

		m_matWorld[i] = XMMatrixTranslation(m_pPoint[i]->x, m_pPoint[i]->y, m_pPoint[i]->z);
		m_pColliderCom[i]->Set_ParentMatrix(&m_matWorld[i]);	// Parent Matrix
		m_pColliderCom[i]->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));	// Collider Scale
		m_pColliderCom[i]->Set_Radius(_vec3(1.f, 1.f, 1.f));	// Collider Radius
	}

	// Buffer
	m_PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	array<Engine::VTXCOL, 3> vertices =
	{
		Engine::VTXCOL(*m_pPoint[POINT_A], _rgba(RANDOM_COLOR)),	// 0
		Engine::VTXCOL(*m_pPoint[POINT_B], _rgba(RANDOM_COLOR)),	// 1
		Engine::VTXCOL(*m_pPoint[POINT_C], _rgba(RANDOM_COLOR)),	// 2
	};

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, 3> indices = { 0, 1, 2	}; // ù ��° �ﰢ��.

	const _int uiVB_ByteSize = (_uint)vertices.size() * sizeof(Engine::VTXCOL);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), vertices.data(), uiVB_ByteSize);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	// VB ����.
	m_pVB_GPU = Create_DefaultBuffer(vertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	Engine::NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);

	// IB ����.
	m_pIB_GPU = Create_DefaultBuffer(indices.data(), uiIB_ByteSize, m_pIB_Uploader);
	Engine::NULL_CHECK_RETURN(m_pIB_GPU, E_FAIL);

	m_uiVertexByteStride	= sizeof(Engine::VTXCOL);
	m_uiVB_ByteSize			= uiVB_ByteSize;
	m_uiIB_ByteSize			= uiIB_ByteSize;
	m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	m_tSubMeshGeometry.uiIndexCount			= (_uint)indices.size();
	m_tSubMeshGeometry.uiStartIndexLocation = 0;
	m_tSubMeshGeometry.iBaseVertexLocation	= 0;

	return S_OK;
}

void CToolCell::Set_ConstantTable()
{
	_matrix* pmatView = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::VIEW);
	_matrix* pmatProj = Engine::CGraphicDevice::Get_Instance()->Get_Transform(Engine::MATRIXID::PROJECTION);

	if (nullptr == pmatView || nullptr == pmatProj)
		return;

	/*__________________________________________________________________________________________________________
	[ Set CB Data ]
	____________________________________________________________________________________________________________*/
	// Matrix Info
	Engine::CB_MATRIX_DESC	tCB_MatrixDesc;
	ZeroMemory(&tCB_MatrixDesc, sizeof(Engine::CB_MATRIX_DESC));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWVP, XMMatrixTranspose((*pmatView) * (*pmatProj)));
	XMStoreFloat4x4(&tCB_MatrixDesc.matWorld, XMMatrixTranspose(INIT_MATRIX));
	XMStoreFloat4x4(&tCB_MatrixDesc.matView, XMMatrixTranspose(*pmatView));
	XMStoreFloat4x4(&tCB_MatrixDesc.matProj, XMMatrixTranspose(*pmatProj));
	m_pShaderCom->Get_UploadBuffer_MatrixDesc()->CopyData(0, tCB_MatrixDesc);

	// Color Info
	Engine::CB_COLOR_DESC	tCB_ColorDesc;
	ZeroMemory(&tCB_ColorDesc, sizeof(Engine::CB_COLOR_DESC));
	tCB_ColorDesc.vColor = m_vColor;
	m_pShaderCom->Get_UploadBuffer_ColorDesc()->CopyData(0, tCB_ColorDesc);
}

void CToolCell::CheckClockWise(_vec3& p0, _vec3& p1, _vec3& p2)
{
	_vec3 u = p1 - p0;
	_vec3 v = p2 - p0;

	_vec3 vResult = u.Cross_InputV2(v);
	vResult.Normalize();

	if (vResult.y < 0.0f)
		swap(p1, p2);

}

_bool CToolCell::CheckClockWise(_vec3* p0, _vec3* p1, _vec3* p2)
{
	_vec3 u = *p1 - *p0;
	_vec3 v = *p2 - *p0;

	_vec3 vResult = u.Cross_InputV2(v);
	vResult.Normalize();

	if (vResult.y < 0.0f)
	{
		// swap(p1, p2);
		return false;
	}

	return true;
}

ID3D12Resource* CToolCell::Create_DefaultBuffer(const void* InitData, 
												UINT64 uiByteSize, 
												ID3D12Resource*& pUploadBuffer)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(Engine::CMDID::CMD_MAIN);

	ID3D12Resource* pDefaultBuffer = nullptr;

	/*__________________________________________________________________________________________________________
	[ ���� �⺻ ���� �ڿ��� ���� ]
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Default_HeapProperties;
	Default_HeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	Default_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Default_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Default_HeapProperties.CreationNodeMask     = 1;
	Default_HeapProperties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC Default_ResourceDesc;
	Default_ResourceDesc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	Default_ResourceDesc.Alignment				= 0;
	Default_ResourceDesc.Width					= uiByteSize;
	Default_ResourceDesc.Height					= 1;
	Default_ResourceDesc.DepthOrArraySize		= 1;
	Default_ResourceDesc.MipLevels				= 1;
	Default_ResourceDesc.Format					= DXGI_FORMAT_UNKNOWN;
	Default_ResourceDesc.SampleDesc.Count		= 1;
	Default_ResourceDesc.SampleDesc.Quality		= 0;
	Default_ResourceDesc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Default_ResourceDesc.Flags					= D3D12_RESOURCE_FLAG_NONE;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Default_HeapProperties,
																		  D3D12_HEAP_FLAG_NONE,
																		  &Default_ResourceDesc,
																		  D3D12_RESOURCE_STATE_COMMON,
																		  nullptr,
																		  IID_PPV_ARGS(&pDefaultBuffer)), 
																		  NULL);
 
	/*__________________________________________________________________________________________________________
	- CPU �޸��� �ڷḦ �⺻ ���ۿ� �����Ϸ���, �ӽ� ���ε� ���� ������ �Ѵ�.
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Upload_HeapProperties;
	Upload_HeapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
	Upload_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Upload_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Upload_HeapProperties.CreationNodeMask     = 1;
	Upload_HeapProperties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC Upload_ResourceDest;
	Upload_ResourceDest.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	Upload_ResourceDest.Alignment				= 0;
	Upload_ResourceDest.Width					= uiByteSize;
	Upload_ResourceDest.Height					= 1;
	Upload_ResourceDest.DepthOrArraySize		= 1;
	Upload_ResourceDest.MipLevels				= 1;
	Upload_ResourceDest.Format					= DXGI_FORMAT_UNKNOWN;
	Upload_ResourceDest.SampleDesc.Count		= 1;
	Upload_ResourceDest.SampleDesc.Quality		= 0;
	Upload_ResourceDest.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Upload_ResourceDest.Flags					= D3D12_RESOURCE_FLAG_NONE;

	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Upload_HeapProperties,
																  D3D12_HEAP_FLAG_NONE,
																  &Upload_ResourceDest,
																  D3D12_RESOURCE_STATE_GENERIC_READ,
																  nullptr,
																  IID_PPV_ARGS(&pUploadBuffer)), 
																  NULL);

	
	/*__________________________________________________________________________________________________________
	[ �⺻ ���ۿ� ������ �ڷḦ ���� ]
	____________________________________________________________________________________________________________*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData		= InitData;
	subResourceData.RowPitch	= uiByteSize;
	subResourceData.SlicePitch	= subResourceData.RowPitch;

	/*__________________________________________________________________________________________________________
	[ �⺻ ���� �ڿ������� �ڷ� ���縦 ��û ]
	- �跫������ �����ڸ�, ���� �Լ� UpdateSubresources�� CPU �޸𸮸� �ӽ� ���ε� ���� �����ϰ�,
	ID3D12CommandList::CopySubresourceRegion�� �̿��ؼ� �ӽ� ���ε� ���� �ڷḦ mBuffer�� �����Ѵ�.
	____________________________________________________________________________________________________________*/
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags                   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	ResourceBarrier.Transition.pResource    = pDefaultBuffer;
	ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COMMON;
	ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_COPY_DEST;

	m_pCommandList->ResourceBarrier(1, &ResourceBarrier);

	UpdateSubresources<1>(m_pCommandList, 
						  pDefaultBuffer, 
						  pUploadBuffer, 
						  0, 
						  0, 
						  1,
						  &subResourceData);

	ResourceBarrier.Transition.pResource    = pDefaultBuffer;
	ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_GENERIC_READ;

	m_pCommandList->ResourceBarrier(1, &ResourceBarrier);

	/*__________________________________________________________________________________________________________
	[ ���� ]
	- ���� �Լ� ȣ�� ���Ŀ���, UploadBuffer�� ��� �����ؾ� �Ѵ�.
	- ������ ���縦 �����ϴ� ���� ����� ���� ������� �ʾұ� �����̴�.
	- ���簡 �Ϸ�Ǿ����� Ȯ������ �Ŀ� ȣ���ڰ� UploadBuffer�� �����ϸ� �ȴ�.
	____________________________________________________________________________________________________________*/

	Engine::CGraphicDevice::Get_Instance()->End_ResetCmdList(Engine::CMDID::CMD_MAIN);

	return pDefaultBuffer;
}

D3D12_VERTEX_BUFFER_VIEW CToolCell::Get_VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	VertexBufferView.BufferLocation	= m_pVB_GPU->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes	= m_uiVertexByteStride;
	VertexBufferView.SizeInBytes	= m_uiVB_ByteSize;

	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW CToolCell::Get_IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	IndexBufferView.BufferLocation	= m_pIB_GPU->GetGPUVirtualAddress();
	IndexBufferView.SizeInBytes		= m_uiIB_ByteSize;
	IndexBufferView.Format			= m_IndexFormat;

	return IndexBufferView;
}

CToolCell* CToolCell::Create(ID3D12Device* pGraphicDevice,
							 ID3D12GraphicsCommandList* pCommandList, 
							 const _ulong& dwIndex, 
							 _vec3& vPointA, 
							 _vec3& vPointB, 
							 _vec3& vPointC)
{
	CToolCell* pInstance = new CToolCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(dwIndex, vPointA, vPointB, vPointC)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CToolCell* CToolCell::ShareCreate(ID3D12Device* pGraphicDevice, 
								  ID3D12GraphicsCommandList* pCommandList, 
								  const _ulong& dwIndex, 
								  _vec3* pSharePointA,
								  _vec3& vNewPointB,
								  _vec3* pSharePointC,
								  const _bool& bIsFindNear)
{
	CToolCell* pInstance = new CToolCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(dwIndex, pSharePointA, vNewPointB, pSharePointC, bIsFindNear)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CToolCell* CToolCell::ShareCreate(ID3D12Device* pGraphicDevice,
								  ID3D12GraphicsCommandList* pCommandList,
								  const _ulong& dwIndex, 
								  _vec3* pSharePointA,
								  _vec3* pSharePointB, 
								  _vec3* pSharePointC,
								  const _bool& bIsFindNear)
{
	CToolCell* pInstance = new CToolCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(dwIndex, pSharePointA, pSharePointB, pSharePointC, bIsFindNear)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolCell::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pShaderCom);

	for (auto& pCollider : m_pColliderCom)
		Engine::Safe_Release(pCollider);



	// �����޾Ҵٸ�, ���� ������ Point�� �Ҵ� ����.
	if (m_bIsShare)
	{
		if (!m_bIsFindNear)
		{
			// �ð�����̾��� ��.
			if (m_bIsClockwise)
				Engine::Safe_Delete(m_pPoint[POINT_B]);
			else
				Engine::Safe_Delete(m_pPoint[POINT_C]);
		}
	}
	// �����̶�� ��� ����.
	else
	{
		Engine::Safe_Delete(m_pPoint[POINT_A]);
		Engine::Safe_Delete(m_pPoint[POINT_B]);
		Engine::Safe_Delete(m_pPoint[POINT_C]);
	}				




	Engine::Safe_Release(m_pVB_CPU);
	Engine::Safe_Release(m_pIB_CPU);
	Engine::Safe_Release(m_pVB_GPU);
	Engine::Safe_Release(m_pIB_GPU);
}