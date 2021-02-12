#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderMeshInstancing final : public CShader
{
private:
	explicit CShaderMeshInstancing(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderMeshInstancing(const CShaderMeshInstancing& rhs);
	virtual ~CShaderMeshInstancing() = default;

public:
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();

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

public:
	virtual CComponent* Clone() override;
	static	CShader*	Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
private:
	virtual void Free();
};

END