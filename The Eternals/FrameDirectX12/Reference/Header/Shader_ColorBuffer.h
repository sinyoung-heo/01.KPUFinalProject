#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShader_ColorBuffer : public CShader
{
private:
	explicit CShader_ColorBuffer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShader_ColorBuffer(const CShader_ColorBuffer& rhs);
	virtual ~CShader_ColorBuffer() = default;

public:
	CUploadBuffer<CB_MATRIX_INFO>*	Get_UploadBuffer_MatrixInfo()	{ return m_pCB_MatrixInfo; }
	CUploadBuffer<CB_COLOR_INFO>*	Get_UploadBuffer_ColorInfo()	{ return m_pCB_ColorInfo; }

public:
	HRESULT			Ready_Shader();
	virtual void	Begin_Shader();
	virtual void	End_Shader(const _uint& iIdx = 0);
private:
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
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint = "VS_MAIN",
																   string PS_EntryPoint = "PS_MAIN") override;

private:
	CUploadBuffer<CB_MATRIX_INFO>*	m_pCB_MatrixInfo		= nullptr;
	CUploadBuffer<CB_COLOR_INFO>*	m_pCB_ColorInfo			= nullptr;

public:
	virtual CComponent *		Clone() override;
	static CShader_ColorBuffer* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void				Free();
};

END