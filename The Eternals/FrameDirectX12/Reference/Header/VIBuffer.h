#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : public CComponent
{
protected:
	explicit CVIBuffer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	D3D12_VERTEX_BUFFER_VIEW	Get_VertexBufferView()	const;
	D3D12_INDEX_BUFFER_VIEW		Get_IndexBufferView()	const;
	D3D12_PRIMITIVE_TOPOLOGY&	Get_PrimitiveTopology() { return m_PrimitiveTopology; }
	SUBMESH_GEOMETRY&			Get_SubMeshGeometry()	{ return m_tSubMeshGeometry; }

	HRESULT						Ready_Buffer();
	void						Begin_Buffer();
	void						Render_Buffer();
	virtual void				Release_UploadBuffer();

protected:
	ID3D12Resource*				Create_DefaultBuffer(const void* InitData,
													 UINT64 uiByteSize, 
													 ID3D12Resource*& pUploadBuffer);

protected:
	/*__________________________________________________________________________________________________________
	- 시스템 메모리 복사본.
	- 정점/인덱스 형식이 범용적일 수 있으므로, ID3DBlob를 사용.
	- 실제로 사용할 때에는 클라이언트에서 적절히 캐스팅해야 한다.
	____________________________________________________________________________________________________________*/
	ID3DBlob*		m_pVB_CPU				= nullptr;
	ID3DBlob*		m_pIB_CPU				= nullptr;

	ID3D12Resource* m_pVB_GPU				= nullptr;	// 실제 Vertex Buffer 자원.
	ID3D12Resource* m_pIB_GPU				= nullptr;	// 실제 Index Buffer 자원.

	ID3D12Resource* m_pVB_Uploader			= nullptr;	// GPU로 Vertex Buffer를 올리기 위한 자원.
	ID3D12Resource* m_pIB_Uploader			= nullptr;	// GPU로 Index Buffer를 올리기 위한 자원.

	_uint			m_uiVertexByteStride	= 0;
	_uint			m_uiVB_ByteSize			= 0;
	_uint			m_uiIB_ByteSize			= 0;
	DXGI_FORMAT		m_IndexFormat			= DXGI_FORMAT_R32_UINT;	// 64비트 환경은 R32_UINT, 32비트면 R16_UINT

	SUBMESH_GEOMETRY			m_tSubMeshGeometry;
	D3D12_PRIMITIVE_TOPOLOGY	m_PrimitiveTopology	= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

public:
	virtual CComponent* Clone(void);
protected:
	virtual void Free();
};

END