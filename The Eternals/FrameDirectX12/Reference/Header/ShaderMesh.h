#pragma once
#include "Shader.h"
#include "Texture.h"

BEGIN(Engine)

enum TEXTURE_MAPPING 
{ 
	TEX_DIFFUSE,
	TEX_NORMAL, 
	TEX_SPECULAR, 
	TEXTURE_END 
};

class ENGINE_DLL CShaderMesh final : public CShader
{
private:
	explicit CShaderMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderMesh(const CShaderMesh& rhs);
	virtual ~CShaderMesh() = default;

public:
	// Get
	CUploadBuffer<CB_MATRIX_DESC>*		Get_UploadBuffer_MatrixDesc()	{ return m_pCB_MatrixDesc; }
	CUploadBuffer<CB_SKINNING_DESC>*	Get_UploadBuffer_SkinningDesc() { return m_pCB_SkinningDesc; }
	CUploadBuffer<CB_SHADOW_DESC>*		Get_UploadBuffer_ShadowDesc()	{ return m_pCB_ShadowDesc; }


	HRESULT			SetUp_ShaderConstantBuffer(const _uint& uiNumSubsetMesh = 1);
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();

	// SingleThread Rendering
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr, 
								 const _uint& iSubMeshIdx = 0);

	// multiThread Rendering
	virtual void	Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
								 const _int& iContextIdx,
								 ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr,
								 const _uint& iSubMeshIdx = 0);

private:
	virtual HRESULT								Create_RootSignature();
	virtual HRESULT								Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint					= "VS_MAIN",
																   string PS_EntryPoint					= "PS_MAIN");
	virtual D3D12_BLEND_DESC					Create_BlendState(const _bool& bIsBlendEnable			= false,
																  const D3D12_BLEND& SrcBlend			= D3D12_BLEND_ONE,
																  const D3D12_BLEND& DstBlend			= D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP& BlendOp			= D3D12_BLEND_OP_ADD,
																  const D3D12_BLEND& SrcBlendAlpha		= D3D12_BLEND_ONE,
																  const D3D12_BLEND& DstBlendAlpha		= D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP& BlendOpAlpha	= D3D12_BLEND_OP_ADD);
private:
	CUploadBuffer<CB_MATRIX_DESC>*		m_pCB_MatrixDesc		= nullptr;
	CUploadBuffer<CB_SKINNING_DESC>*	m_pCB_SkinningDesc		= nullptr;
	CUploadBuffer<CB_SHADOW_DESC>*		m_pCB_ShadowDesc		= nullptr;

	_uint m_uiSubsetMeshSize = 0;

public:
	virtual CComponent *	Clone() override;
	static	CShaderMesh*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void			Free();
};

END