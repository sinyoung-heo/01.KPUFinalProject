#pragma once
#include "Shader.h"
#include "Texture.h"

BEGIN(Engine)

class ENGINE_DLL CShader_Mesh : public CShader
{
private:
	explicit CShader_Mesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShader_Mesh(const CShader_Mesh& rhs);
	virtual ~CShader_Mesh() = default;

public:
	ID3D12DescriptorHeap*				Get_SRVDescriptorHeap()			{ return m_pSRV_DescriptorHeap; }
	CUploadBuffer<CB_MATRIX_INFO>*		Get_UploadBuffer_MatrixInfo()	{ return m_pCB_MatrixInfo; }
	CUploadBuffer<CB_SKINNING_INFO>*	Get_UploadBuffer_SkinningInfo() { return m_pCB_SkinningInfo; }
	// CUploadBuffer<CB_BONE_INFO>*		Get_UploadBuffer_BoneInfo()		{ return m_pCB_BoneInfo; }
public:
	HRESULT			Ready_Shader();
	virtual void	Begin_Shader();
	virtual void	End_Shader(const _uint& iIdx = 0);
public:
	void			Set_ShaderTexture(vector< ComPtr<ID3D12Resource>> pVecTexture);
private:
	HRESULT			Create_DescriptorHeaps(vector<ComPtr<ID3D12Resource>> pVecTexture);
	HRESULT			Create_ConstantBuffer(const _uint& iNumSubsetMesh);
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Create_DescriptorHeaps() override;
	virtual HRESULT	Create_ConstantBuffer() override;
	virtual HRESULT	Create_RootSignature() override;
	virtual HRESULT	Create_PipelineState() override;
private:
	virtual D3D12_RASTERIZER_DESC				Create_RasterizerState(const D3D12_FILL_MODE& eFillMode = D3D12_FILL_MODE_SOLID,
																	   const D3D12_CULL_MODE& eCullMode = D3D12_CULL_MODE_BACK) override;
	virtual D3D12_BLEND_DESC					Create_BlendState(const _bool& bIsBlendEnable			= false,
																  const D3D12_BLEND& SrcBlend			= D3D12_BLEND_ONE,
																  const D3D12_BLEND& DstBlend			= D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP& BlendOp			= D3D12_BLEND_OP_ADD,
																  const D3D12_BLEND& SrcBlendAlpha		= D3D12_BLEND_ONE,
																  const D3D12_BLEND& DstBlendAlpha		= D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP& BlendOpAlpha	= D3D12_BLEND_OP_ADD) override;
	virtual D3D12_DEPTH_STENCIL_DESC			Create_DepthStencilState(const _bool& bIsZWrite = true) override;
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint = "VS_MAIN",
																   string PS_EntryPoint = "PS_MAIN") override;
private:
	ID3D12DescriptorHeap*				m_pSRV_DescriptorHeap	= nullptr;

	CUploadBuffer<CB_MATRIX_INFO>*		m_pCB_MatrixInfo		= nullptr;
	CUploadBuffer<CB_SKINNING_INFO>*	m_pCB_SkinningInfo		= nullptr;
	// CUploadBuffer<CB_BONE_INFO>*		m_pCB_BoneInfo			= nullptr;

public:
	virtual CComponent *	Clone() override;
	static CShader_Mesh*	Create(ID3D12Device* pGraphicDevice,
								   ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void			Free();
};

END