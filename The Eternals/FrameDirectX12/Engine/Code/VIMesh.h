#pragma once
#include "Component.h"
#include "Shader.h"

BEGIN(Engine)

class CAniCtrl;

typedef vector<_matrix>		VECTOR_MATRIX;

class ENGINE_DLL CVIMesh final : public CComponent
{
private:
	explicit CVIMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CVIMesh(const CVIMesh& rhs);
	virtual ~CVIMesh() = default;

public:
	// Get
	D3D12_VERTEX_BUFFER_VIEW		Get_VertexBufferView(const _uint& iIndex)	const;
	D3D12_INDEX_BUFFER_VIEW			Get_IndexBufferView(const _uint& iIndex)	const;
	vector<ComPtr<ID3D12Resource>>	Get_DiffTexture()		{ return m_vecDiffResource; };
	vector<ComPtr<ID3D12Resource>>	Get_NormTexture()		{ return m_vecNormResource; };
	vector<ComPtr<ID3D12Resource>>	Get_SpecTexture()		{ return m_vecSpecResource; };
	ID3D12DescriptorHeap*			Get_TexDescriptorHeap() { return m_pTexDescriptorHeap; }

	// Set
	void							Set_AniCtrl(CAniCtrl* pAniCtrl) { m_pAniCtrl = pAniCtrl; }

	// Method
	HRESULT							Ready_Component(const aiScene * pScene, wstring wstrPath);
	HRESULT							Ready_Mesh(const aiMesh* pAiMesh,
											   vector<VTXMESH>& vecVertex, 
											   vector<_uint>& vecIndex);
	HRESULT							Ready_Texture();
	HRESULT							Create_TextureDescriptorHeap();
	virtual void					Release_UploadBuffer();

	ID3D12Resource*					Create_DefaultBuffer(const void* InitData,
														 const UINT64& uiByteSize,
														 ID3D12Resource*& pUploadBuffer);

	void							Render_DynamicMesh(CShader* pShader);
	void							Render_StaticMesh(CShader* pShader);
	void							Render_DynamicMeshShadowDepth(CShader* pShader);
	void							Render_StaticMeshShadowDepth(CShader* pShader);

	// 2020.06.11 MultiThreadRendering
	void							Render_DynamicMesh(ID3D12GraphicsCommandList* pCommandList,
													   const _int& iContextIdx,
													   CShader* pShader);
	void							Render_StaticMesh(ID3D12GraphicsCommandList* pCommandList,
													  const _int& iContextIdx, 
													  CShader* pShader);
	void							Render_DynamicMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, 
																  const _int& iContextIdx, 
																  CShader* pShader);
	void							Render_StaticMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList,
																 const _int& iContextIdx,
																 CShader* pShader);

private:
	/*__________________________________________________________________________________________________________
	[ Vertex Buffer / Index Buffer ]
	____________________________________________________________________________________________________________*/
	vector<ID3DBlob*>		m_vecVB_CPU;
	vector<ID3DBlob*>		m_vecIB_CPU;

	vector<ID3D12Resource*> m_vecVB_GPU;
	vector<ID3D12Resource*> m_vecIB_GPU;

	vector<ID3D12Resource*> m_vecVB_Uploader;
	vector<ID3D12Resource*> m_vecIB_Uploader;

	_uint			        m_uiVertexByteStride	= 0;
	vector<_uint>			m_vecVB_ByteSize;
	vector<_uint>			m_vecIB_ByteSize;
	DXGI_FORMAT		        m_IndexFormat			= DXGI_FORMAT_R32_UINT;

	vector<SUBMESH_GEOMETRY> m_vecSubMeshGeometry;
	D3D12_PRIMITIVE_TOPOLOGY m_PrimitiveTopology	= D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/*__________________________________________________________________________________________________________
	[ Texture ]
	____________________________________________________________________________________________________________*/
	wstring							m_wstrFilePath;

	vector<ComPtr<ID3D12Resource>>	m_vecDiffResource;
	vector<ComPtr<ID3D12Resource>>	m_vecNormResource;
	vector<ComPtr<ID3D12Resource>>	m_vecSpecResource;

	vector<ComPtr<ID3D12Resource>>	m_vecDiffUpload;
	vector<ComPtr<ID3D12Resource>>	m_vecNormUpload;
	vector<ComPtr<ID3D12Resource>>	m_vecSpecUpload;

	ID3D12DescriptorHeap*			m_pTexDescriptorHeap = nullptr;
	_uint							m_uiSubsetMeshSize	 = 0;

	/*__________________________________________________________________________________________________________
	[ Assimp ]
	____________________________________________________________________________________________________________*/
	const aiScene*		m_pScene;		// Mesh의 모든 정보를 담고 있는 객체.
	vector<MESH_ENTRY>	m_vecMeshEntry;	// Subset Mesh들의 정보들.

	/*__________________________________________________________________________________________________________
	[ Animation Ctrl ]
	____________________________________________________________________________________________________________*/
	CAniCtrl* m_pAniCtrl = nullptr;

public:
	virtual CComponent* Clone();
	static CVIMesh*		Create(ID3D12Device* pGraphicDevice,
							   ID3D12GraphicsCommandList* pCommandList,
							   const aiScene* pScene,
							   wstring wstrPath);
private:
	virtual void		Free();
};

END