#pragma once
#include "GameObject.h"

namespace Engine
{
	class CShaderColor;
	class CColliderSphere;
}

class CToolCell : public Engine::CGameObject
{
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };

private:
	explicit CToolCell(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CToolCell(const CToolCell& rhs);
	virtual ~CToolCell() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _ulong& dwIndex,
									 _vec3& vPointA,
									 _vec3& vPointB,
									 _vec3& vPointC,
									 const _int& iOption);
	virtual HRESULT Ready_GameObject(const _ulong& dwIndex,
									 _vec3* pSharePointA,
									 _vec3& vNewPointB,
									 _vec3* pSharePointC,
									 const _int& iOption,
									 const _bool& bIsFindNear = false);
	virtual HRESULT Ready_GameObject(const _ulong& dwIndex,
									 _vec3* pSharePointA,
									 _vec3* pSharePointB,
									 _vec3* pSharePointC,
									 const _int& iOption,
									 const _bool& bIsFindNear = false);

	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

	void			Reset_CellAndCollider();
	void			Set_SelectedPoint(const _int& iIdx);

private:
	HRESULT			Add_Component();
	void			Set_ConstantTable();
	void			CheckClockWise(_vec3& p0, _vec3& p1, _vec3& p2);
	_bool			CheckClockWise(_vec3* p0, _vec3* p1, _vec3* p2);

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CShaderColor*		m_pShaderCom = nullptr;
	Engine::CColliderSphere*	m_pColliderCom[POINT_END];

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_matrix m_matWorld[POINT_END];
	_vec3*	m_pPoint[POINT_END];
	_vec3	m_vCenter		= _vec3(0.0f);
	_ulong	m_dwCurrentIdx	= 0;
	_int	m_iOption		= 0;

	_bool	m_bIsShare		= false;
	_bool	m_bIsFindNear	= false;
	_bool	m_bIsClockwise	= true;
	_rgba	m_vColor		= _rgba(1.0f, 1.0f, 0.0f, 1.0f);
	

private:
#pragma region BUFFER
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

	Engine::SUBMESH_GEOMETRY m_tSubMeshGeometry;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology	= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	array<Engine::VTXCOL, 3> m_arrVertices;
	UINT8* m_pVetexData = nullptr;

private:
	ID3D12Resource*	Create_DefaultBuffer(const void* InitData,
										 UINT64 uiByteSize, 
										 ID3D12Resource*& pUploadBuffer);
	ID3D12Resource*	Create_DynamicVertex(const void* InitData,
										 UINT64 uiByteSize, 
										 ID3D12Resource*& pUploadBuffer);


	D3D12_VERTEX_BUFFER_VIEW	Get_VertexBufferView()	const;
	D3D12_INDEX_BUFFER_VIEW		Get_IndexBufferView()	const;
#pragma endregion
public:
	static CToolCell* Create(ID3D12Device* pGraphicDevice,
							 ID3D12GraphicsCommandList* pCommandList,
							 const _ulong& dwIndex,
							 _vec3& vPointA,
							 _vec3& vPointB,
							 _vec3& vPointC,
							 const _int& iOption);

	static CToolCell* ShareCreate(ID3D12Device* pGraphicDevice,
								  ID3D12GraphicsCommandList* pCommandList,
								  const _ulong& dwIndex,
								  _vec3* pSharePointA,
								  _vec3& vNewPointB,
								  _vec3* pSharePointC,
								  const _int & iOption,
								  const _bool& bIsFindNear = false);

	static CToolCell* ShareCreate(ID3D12Device* pGraphicDevice,
								  ID3D12GraphicsCommandList* pCommandList,
								  const _ulong& dwIndex,
								  _vec3* pSharePointA,
								  _vec3* pSharePointB,
								  _vec3* pSharePointC,
								  const _int& iOption,
								  const _bool& bIsFindNear = false);


private:
	virtual void Free();

};

