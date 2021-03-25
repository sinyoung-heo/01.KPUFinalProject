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
	vector<SUBMESH_GEOMETRY>&		Get_SubMeshGeometry()	{ return m_vecSubMeshGeometry; }
	const D3D12_PRIMITIVE_TOPOLOGY& Get_PrimitiveTopology() { return m_PrimitiveTopology; }
	const _vec3&					Get_CenterPos()			{ return m_vCenter; }
	const _vec3&					Get_MinVector()			{ return m_vMin; }
	const _vec3&					Get_MaxVector()			{ return m_vMax; }
	wstring							Get_FileName()			{ return m_wstrFileName; }
	// Set
	void							Set_AniCtrl(CAniCtrl* pAniCtrl)			{ m_pAniCtrl = pAniCtrl; }
	void							Set_FileName(wstring& wstrFileName)		{ m_wstrFileName = wstrFileName; }
	void							Set_AfterImgSize(const _uint& uiSize)	{ m_uiAfterImgSize = uiSize; }
	void							Set_AfterImgTime(const _float& fTime) { m_fDeltaTime = fTime; }
	void							Set_AfterImgMakeTime(const _float& fTime) { m_fAfterImgMakeTime = fTime; }
	void							Set_AfterImgSubAlpha(const _float& fSub) { m_fAfterImgSubAlpha = fSub; }

	// Method
	HRESULT			Ready_Component(const aiScene* pScene, wstring wstrFileName, wstring wstrPath);
	HRESULT			Ready_Mesh(const aiMesh* pAiMesh, vector<VTXMESH>& vecVertex, vector<_uint>& vecIndex);
	HRESULT			Ready_Texture();
	HRESULT			Create_TextureDescriptorHeap();
	virtual void	Release_UploadBuffer();
private:
	ID3D12Resource*	Create_DefaultBuffer(const void* InitData, const UINT64& uiByteSize, ID3D12Resource*& pUploadBuffer);

public:
	// SingleThread Rendering
	void Render_DynamicMesh(CShader* pShader);
	void Render_StaticMesh(CShader* pShader);
	void Render_DynamicMeshShadowDepth(CShader* pShader);
	void Render_StaticMeshShadowDepth(CShader* pShader);
	void Render_DynamicMeshAfterImage(CShader* pShader, const _uint& iAfterImgIdx);
	// MultiThread Rendering
	void Render_DynamicMesh(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_DynamicMeshAfterImage(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader, const _uint& iAfterImgIdx);
	void Render_StaticMesh(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_DynamicMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_StaticMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	
	void Render_WaterMesh(ID3D12GraphicsCommandList* pCommandList, 
		const _int& iContextIdx,
		CShader* pShader,
		ID3D12DescriptorHeap *pTexnormalDescriptorHeap,_uint uiNormalTextureIdx, _uint uiPatternMapIdx);
	void Begin_Buffer(ID3D12GraphicsCommandList* pCommandList, const _int& iSubMeshIdx);
	void Render_Buffer(ID3D12GraphicsCommandList* pCommandList, const _int& iSubMeshIdx);

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
	wstring							m_wstrFileName;
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
	const aiScene*		m_pScene;		// Mesh�� ��� ������ ��� �ִ� ��ü.
	vector<MESH_ENTRY>	m_vecMeshEntry;	// Subset Mesh���� ������.

	/*__________________________________________________________________________________________________________
	[ Animation Ctrl ]
	____________________________________________________________________________________________________________*/
	CAniCtrl* m_pAniCtrl = nullptr;

	/*__________________________________________________________________________________________________________
	[ CenterPos and MIN/MAX ]
	____________________________________________________________________________________________________________*/
	_int	m_iSumVertex	= 0;
	_vec3	m_vCenter		= _vec3(0.0f);
	_vec3	m_vMin			= _vec3(0.0f);
	_vec3	m_vMax			= _vec3(0.0f);

	// AfterImage
	list<_rgba>				m_lstAFAlpha;
	list<CB_SKINNING_MATRIX>	m_lstAFSkinningMatrix;
	_uint						m_uiAfterImgSize = 0;
	_float						m_fDeltaTime = 0.f;
	_float						m_fAfterImgMakeTime = 0.f;
	_float						m_fAfterImgSubAlpha = 0.f;
public:
	virtual CComponent* Clone();
	static CVIMesh*		Create(ID3D12Device* pGraphicDevice,
							   ID3D12GraphicsCommandList* pCommandList,
							   const aiScene* pScene,
							   wstring wstrFileName,
							   wstring wstrPath);
private:
	virtual void		Free();
};

END