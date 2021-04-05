#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CTexture;
	class CShaderTexture;
}

const _uint TRAIL_SIZE = 22;

typedef	struct	tagIndex32
{
	_uint _1, _2, _3;

}INDEX32;

class CEffectTrail : public Engine::CGameObject
{
private:
	explicit CEffectTrail(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CEffectTrail() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrTextureTag,
									 const _uint& uiTexIdx,
									 const _vec3& vScale,
									 const _vec3& vAngle);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);

	void SetUp_TrailByCatmullRom(_vec3* vMin, _vec3* vMax);

private:
	virtual HRESULT Add_Component(wstring wstrTextureTag);
private:
	void Set_ConstantTable();

	// Create TrailBuffer
	ID3D12Resource*	Create_DynamicVertex(const void* InitData, UINT64 uiByteSize, ID3D12Resource*& pUploadBuffer);
	ID3D12Resource*	Create_DefaultBuffer(const void* InitData, UINT64 uiByteSize, ID3D12Resource*& pUploadBuffer);
	D3D12_VERTEX_BUFFER_VIEW Get_VertexBufferView()	const;
	D3D12_INDEX_BUFFER_VIEW	Get_IndexBufferView()	const;


private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CTexture*		m_pTextureCom	= nullptr;
	Engine::CShaderTexture*	m_pShaderCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Trail Buffer ]
	____________________________________________________________________________________________________________*/
	ID3DBlob*					m_pVB_CPU		     = nullptr;
	ID3DBlob*					m_pIB_CPU		     = nullptr;
	ID3D12Resource*				m_pVB_GPU		     = nullptr;	// 실제 Vertex Buffer 자원.
	ID3D12Resource*				m_pIB_GPU		     = nullptr;	// 실제 Index Buffer 자원.
	ID3D12Resource*				m_pVB_Uploader	     = nullptr;	// GPU로 Vertex Buffer를 올리기 위한 자원.
	ID3D12Resource*				m_pIB_Uploader	     = nullptr;	// GPU로 Index Buffer를 올리기 위한 자원.
	_uint						m_uiVertexByteStride = 0;
	_uint						m_uiVB_ByteSize		 = 0;
	_uint						m_uiIB_ByteSize		 = 0;
	DXGI_FORMAT					m_IndexFormat		 = DXGI_FORMAT_R32_UINT;	// 64비트 환경은 R32_UINT, 32비트면 R16_UINT
	Engine::SUBMESH_GEOMETRY	m_tSubMeshGeometry;

	array<Engine::VTXTEX, TRAIL_SIZE>	m_arrVertices;
	UINT8*								m_pVetexData = nullptr;
	array<_vec3, TRAIL_SIZE - 1>		m_arrMax;


	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrTextureTag = L"";
	_uint	m_uiTexIdx		 = 0;

public:
	static CEffectTrail* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
								wstring wstrTextureTag,
								const _uint& uiTexIdx,
								const _vec3& vScale = _vec3(0.1f),
								const _vec3& vAngle = _vec3(0.0f));
private:
	virtual void Free();
};

