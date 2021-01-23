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
									_vec3& vPointC,
									const _int& iOption)
{
	CheckClockWise(vPointA, vPointB, vPointC);

	m_bIsShare			= false;
	m_dwCurrentIdx		= dwIndex;
	m_pPoint[POINT_A]	= new _vec3(vPointA);
	m_pPoint[POINT_B]	= new _vec3(vPointB);
	m_pPoint[POINT_C]	= new _vec3(vPointC);
	m_iOption			= iOption;
	
	m_vCenter			= (*m_pPoint[POINT_A] + *m_pPoint[POINT_B] + *m_pPoint[POINT_C]) / 3.f;

	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

HRESULT CToolCell::Ready_GameObject(const _ulong& dwIndex,
									_vec3* pSharePointA,
									_vec3& vNewPointB,
									_vec3* pSharePointC,
									const _int& iOption,
									const _bool& bIsFindNear)
{
	// 시계방향이면
	if (CheckClockWise(pSharePointA, &vNewPointB, pSharePointC))
	{
		m_pPoint[POINT_A] = pSharePointA;
		m_pPoint[POINT_B] = new _vec3(vNewPointB);
		m_pPoint[POINT_C] = pSharePointC;
		m_bIsClockwise = true;
	}
	// 반시계방향이면
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
	m_iOption			= iOption;

	m_vCenter			= (*m_pPoint[POINT_A] + *m_pPoint[POINT_B] + *m_pPoint[POINT_C]) / 3.f;

	Engine::FAILED_CHECK_RETURN(Add_Component(), E_FAIL);

	return S_OK;
}

HRESULT CToolCell::Ready_GameObject(const _ulong& dwIndex, 
									_vec3* pSharePointA, 
									_vec3* pSharePointB,
									_vec3* pSharePointC,
									const _int& iOption,
									const _bool& bIsFindNear)
{
	// 시계방향이면
	if (CheckClockWise(pSharePointA, pSharePointB, pSharePointC))
	{
		m_pPoint[POINT_A] = pSharePointA;
		m_pPoint[POINT_B] = pSharePointB;
		m_pPoint[POINT_C] = pSharePointC;
		m_bIsClockwise = true;
	}
	// 반시계방향이면
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
	m_iOption			= iOption;

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

	// Collider 위치 갱신.
	for (_int i = 0; i < POINT_END; ++i)
	{
		m_matWorld[i] = XMMatrixTranslation(m_pPoint[i]->x, m_pPoint[i]->y, m_pPoint[i]->z);
		m_pColliderCom[i]->Set_ParentMatrix(&m_matWorld[i]);
		m_pColliderCom[i]->Update_Component(fTimeDelta);

		// VertexBuffer Update.
		m_arrVertices[i].vPos = *m_pPoint[i];
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

	// Vertex버퍼 정보 CopyData.
	memcpy(m_pVetexData, m_arrVertices.data(), m_uiVB_ByteSize);

	m_pShaderCom->Begin_Shader();
	/*__________________________________________________________________________________________________________
	[ 메쉬의 정점 버퍼 뷰와 인덱스 버퍼 뷰를 설정 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetVertexBuffers(0, 						 // 시작 슬롯. (입력 슬롯은 총 16개)
									   1, 						 // 입력 슬롯들에 묶을 정점 버퍼 개수.
									   &Get_VertexBufferView()); // 정점 버퍼 뷰의 첫 원소를 가리키는 포인터.

	m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView());

	/*__________________________________________________________________________________________________________
	[ 메쉬의 프리미티브 유형을 설정 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

	/*__________________________________________________________________________________________________________
	[ 정점들이 파이프라인의 입력 조립기 단계로 공급 ]
	____________________________________________________________________________________________________________*/
	//m_pCommandList->DrawInstanced(3, 1, 0, 0);
	
	m_pCommandList->DrawIndexedInstanced(m_tSubMeshGeometry.uiIndexCount,	// 그리기에 사용할 인덱스들의 개수. (인스턴스 당)
										 1,									// 그릴 인스턴스 개수.
										 0,									// 인덱스 버퍼의 첫 index
										 0, 								// 그리기 호출에 쓰이는 인덱스들에 더할 정수 값.
										 0);	
}

void CToolCell::Reset_CellAndCollider()
{
	// Color Green & Render WireFrame.
	m_vColor = _rgba(0.0f, 1.0f, 0.0f, 1.0f);
	m_pShaderCom->Set_PipelineStatePass(1);	

	for (_int i = 0; i < POINT_END; ++i)
	{
		m_pColliderCom[i]->Set_Color(_rgba(0.0f, 1.0f, 0.0f, 1.0f));
		m_pColliderCom[i]->Set_PipelineStatePass(1);

		m_pColliderCom[i]->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));	// Collider Scale
		m_pColliderCom[i]->Set_Radius(_vec3(1.f, 1.f, 1.f));	// Collider Radius
	}
}

void CToolCell::Set_SelectedPoint(const _int& iIdx)
{
	// Color Red & Render Soild.
	m_pColliderCom[iIdx]->Set_Color(_rgba(1.0f, 0.0f, 0.0f, 1.0f));
	m_pColliderCom[iIdx]->Set_PipelineStatePass(0);

	m_pColliderCom[iIdx]->Set_Scale(_vec3(0.55f, 0.55f, 0.55f));	// Collider Scale
	m_pColliderCom[iIdx]->Set_Radius(_vec3(1.f, 1.f, 1.f));			// Collider Radius
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
	m_arrVertices =
	{
		Engine::VTXCOL(_vec3(*m_pPoint[POINT_A]), _rgba(RANDOM_COLOR)),	// 0
		Engine::VTXCOL(_vec3(*m_pPoint[POINT_B]), _rgba(RANDOM_COLOR)),	// 1
		Engine::VTXCOL(_vec3(*m_pPoint[POINT_C]), _rgba(RANDOM_COLOR)),	// 2
	};

	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	array<_uint, 3> indices = { 0, 1, 2	}; // 첫 번째 삼각형.

	const _int uiVB_ByteSize = (_uint)m_arrVertices.size() * sizeof(Engine::VTXCOL);
	const _int uiIB_ByteSize = (_uint)indices.size() * sizeof(_uint);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_pVB_CPU), E_FAIL);
	CopyMemory(m_pVB_CPU->GetBufferPointer(), m_arrVertices.data(), uiVB_ByteSize);

	Engine::FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_pIB_CPU), E_FAIL);
	CopyMemory(m_pIB_CPU->GetBufferPointer(), indices.data(), uiIB_ByteSize);

	// VB 생성.
	m_pVB_GPU = Create_DynamicVertex(m_arrVertices.data(), uiVB_ByteSize, m_pVB_Uploader);
	Engine::NULL_CHECK_RETURN(m_pVB_GPU, E_FAIL);
	// IB 생성.
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
	/*__________________________________________________________________________________________________________
	[ Set ConstantBuffer Data ]
	____________________________________________________________________________________________________________*/
	Engine::CB_SHADER_COLOR tCB_ShaderColor;
	ZeroMemory(&tCB_ShaderColor, sizeof(Engine::CB_SHADER_COLOR));
	tCB_ShaderColor.matWorld	= Engine::CShader::Compute_MatrixTranspose(INIT_MATRIX);
	tCB_ShaderColor.vColor		= m_vColor;

	m_pShaderCom->Get_UploadBuffer_ShaderColor()->CopyData(0, tCB_ShaderColor);
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
	[ 실제 기본 버퍼 자원을 생성 ]
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_COMMON,
																		  nullptr,
																		  IID_PPV_ARGS(&pDefaultBuffer)), 
																		  NULL);
 
	/*__________________________________________________________________________________________________________
	- CPU 메모리의 자료를 기본 버퍼에 복사하려면, 임시 업로드 힙을 만들어야 한다.
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&pUploadBuffer)), 
																		  NULL);

	/*__________________________________________________________________________________________________________
	[ 기본 버퍼에 복사할 자료를 서술 ]
	____________________________________________________________________________________________________________*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData		= InitData;
	subResourceData.RowPitch	= uiByteSize;
	subResourceData.SlicePitch	= subResourceData.RowPitch;


	/*__________________________________________________________________________________________________________
	[ 기본 버퍼 자원으로의 자료 복사를 요청 ]
	- 계략적으로 말하자면, 보조 함수 UpdateSubresources는 CPU 메모리를 임시 업로드 힙에 복사하고,
	ID3D12CommandList::CopySubresourceRegion을 이용해서 임시 업로드 힙의 자료를 mBuffer에 복사한다.

	[ 주의 ]
	- 위의 함수 호출 이후에도, UploadBuffer를 계속 유지해야 한다.
	- 실제로 복사를 수행하는 명령 목록이 아직 실행되지 않았기 때문이다.
	- 복사가 완료되었음이 확실해진 후에 호출자가 UploadBuffer를 해제하면 된다.
	____________________________________________________________________________________________________________*/
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


ID3D12Resource* CToolCell::Create_DynamicVertex(const void* InitData, 
												UINT64 uiByteSize, 
												ID3D12Resource*& pUploadBuffer)
{
	Engine::CGraphicDevice::Get_Instance()->Begin_ResetCmdList(Engine::CMDID::CMD_MAIN);

	ID3D12Resource* pDefaultBuffer = nullptr;

	/*__________________________________________________________________________________________________________
	- CPU 메모리의 자료를 기본 버퍼에 복사하려면, 임시 업로드 힙을 만들어야 한다.
	____________________________________________________________________________________________________________*/
	Engine::FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&pDefaultBuffer)),
																		  NULL);


	pDefaultBuffer->Map(0, nullptr, (void**)&m_pVetexData);	// Write Vertex Data to pVertexDataBegin dynamically
	
	/*__________________________________________________________________________________________________________
	[ 주의 ]
	- 위의 함수 호출 이후에도, UploadBuffer를 계속 유지해야 한다.
	- 실제로 복사를 수행하는 명령 목록이 아직 실행되지 않았기 때문이다.
	- 복사가 완료되었음이 확실해진 후에 호출자가 UploadBuffer를 해제하면 된다.
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
							 _vec3& vPointC,
							  const _int& iOption)
{
	CToolCell* pInstance = new CToolCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(dwIndex, vPointA, vPointB, vPointC, iOption)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CToolCell* CToolCell::ShareCreate(ID3D12Device* pGraphicDevice, 
								  ID3D12GraphicsCommandList* pCommandList, 
								  const _ulong& dwIndex, 
								  _vec3* pSharePointA,
								  _vec3& vNewPointB,
								  _vec3* pSharePointC,
								  const _int& iOption,
								  const _bool& bIsFindNear)
{
	CToolCell* pInstance = new CToolCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(dwIndex, pSharePointA, vNewPointB, pSharePointC, iOption, bIsFindNear)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CToolCell* CToolCell::ShareCreate(ID3D12Device* pGraphicDevice,
								  ID3D12GraphicsCommandList* pCommandList,
								  const _ulong& dwIndex, 
								  _vec3* pSharePointA,
								  _vec3* pSharePointB, 
								  _vec3* pSharePointC,
								  const _int& iOption,
								  const _bool& bIsFindNear)
{
	CToolCell* pInstance = new CToolCell(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(dwIndex, pSharePointA, pSharePointB, pSharePointC, iOption, bIsFindNear)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CToolCell::Free()
{
	Engine::CGameObject::Free();
	Engine::Safe_Release(m_pShaderCom);

	for (auto& pCollider : m_pColliderCom)
		Engine::Safe_Release(pCollider);



	// 공유받았다면, 새로 생성한 Point만 할당 해제.
	if (m_bIsShare)
	{
		if (!m_bIsFindNear)
		{
			// 시계방향이었을 때.
			if (m_bIsClockwise)
				Engine::Safe_Delete(m_pPoint[POINT_B]);
			else
				Engine::Safe_Delete(m_pPoint[POINT_C]);
		}
	}
	// 원본이라면 모두 해제.
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
