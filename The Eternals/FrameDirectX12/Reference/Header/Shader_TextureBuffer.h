#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShader_TextureBuffer : public CShader
{
private:
	explicit CShader_TextureBuffer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShader_TextureBuffer(const CShader_TextureBuffer& rhs);
	virtual ~CShader_TextureBuffer() = default;

public:
	ID3D12DescriptorHeap*			Get_SRVDescriptorHeap()			{ return m_pSRV_DescriptorHeap; }
	CUploadBuffer<CB_MATRIX_INFO>*	Get_UploadBuffer_MatrixInfo()	{ return m_pCB_MatrixInfo; }
public:
	HRESULT			Ready_Shader();
	virtual void	Begin_Shader();
	virtual void	End_Shader(const _uint& iIdx = 0);
public:
	void			Set_ShaderTexture(vector<ComPtr<ID3D12Resource>> pVecTexture);
private:
	HRESULT			Create_DescriptorHeaps(vector<ComPtr<ID3D12Resource>> pVecTexture);
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Create_DescriptorHeaps() override;
	virtual HRESULT	Create_ConstantBuffer()	override;
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
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint = "VS_NORMAL_MAIN",
																   string PS_EntryPoint = "PS_NORMAL_MAIN") override;

private:
	ID3D12DescriptorHeap*			m_pSRV_DescriptorHeap	= nullptr;
	CUploadBuffer<CB_MATRIX_INFO>*	m_pCB_MatrixInfo		= nullptr;

public:
	virtual CComponent* Clone() override;
public:
	static CShader_TextureBuffer*	Create(ID3D12Device* pGraphicDevice,
										   ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();
};

END