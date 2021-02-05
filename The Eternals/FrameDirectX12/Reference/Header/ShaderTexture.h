#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderTexture final : public CShader
{
private:
	explicit CShaderTexture(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderTexture(const CShaderTexture& rhs);
	virtual ~CShaderTexture() = default;

public:
	// Get
	CUploadBuffer<CB_SHADER_TEXTURE>* Get_UploadBuffer_ShaderTexture() { return m_pCB_ShaderTexture; }

	HRESULT			SetUp_ShaderConstantBuffer(const _uint& iNumSubset = 1);
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr,
								 const _uint& iSubsetIdx = 0,
								 const _uint& iTexIdx = 0,
								 const MATRIXID& eID = MATRIXID::PROJECTION);

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
	CUploadBuffer<CB_SHADER_TEXTURE>* m_pCB_ShaderTexture = nullptr;

public:
	virtual CComponent*	Clone() override;
	static	CShader*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();
};

END