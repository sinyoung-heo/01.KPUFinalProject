#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderSkySphere : public CShader
{
private:
	explicit CShaderSkySphere(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderSkySphere(const CShaderSkySphere& rhs);
	virtual ~CShaderSkySphere() = default;

public:
	// Get
	CUploadBuffer<CB_SHADER_SKYSPHERE>* Get_UploadBuffer_ShaderSkySphere() { return m_pCB_ShaderSkySphere; }

	HRESULT			SetUp_ShaderConstantBuffer(const _uint& uiNumSubsetMesh);
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();
	// SingleThread Rendering
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr, 
								 const _uint& iSubMeshIdx = 0);

private:
	virtual HRESULT								Create_RootSignature();
	virtual HRESULT								Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint	= "VS_MAIN",
																   string PS_EntryPoint	= "PS_MAIN");
	virtual D3D12_BLEND_DESC	Create_BlendState(const _bool& bIsBlendEnable			= false,
												  const D3D12_BLEND& SrcBlend			= D3D12_BLEND_ONE,
												  const D3D12_BLEND& DstBlend			= D3D12_BLEND_ZERO,
												  const D3D12_BLEND_OP& BlendOp			= D3D12_BLEND_OP_ADD,
												  const D3D12_BLEND& SrcBlendAlpha		= D3D12_BLEND_ONE,
												  const D3D12_BLEND& DstBlendAlpha		= D3D12_BLEND_ZERO,
												  const D3D12_BLEND_OP& BlendOpAlpha	= D3D12_BLEND_OP_ADD);
	D3D12_RASTERIZER_DESC		Create_RasterizerState(const D3D12_FILL_MODE& eFillMode = D3D12_FILL_MODE_SOLID,
													   const D3D12_CULL_MODE& eCullMode = D3D12_CULL_MODE_BACK);

private:
	_uint m_uiSubsetMeshSize = 0;

	CUploadBuffer<CB_SHADER_SKYSPHERE>* m_pCB_ShaderSkySphere = nullptr;

public:
	virtual CComponent*	Clone() override;
	static	CShader*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();

};


END