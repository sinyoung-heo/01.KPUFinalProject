#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderBumpTerrain : public CShader
{
private:
	explicit CShaderBumpTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderBumpTerrain(const CShaderBumpTerrain& rhs);
	virtual ~CShaderBumpTerrain() = default;

public:
	// Get
	CUploadBuffer<CB_SHADER_MESH>* Get_UploadBuffer_ShaderMesh()	{ return m_pCB_ShaderMesh; }

	HRESULT	SetUp_ShaderConstantBuffer();
	HRESULT SetUp_ShaderDescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture);

	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();

	// SingleThread Rendering
	virtual void	Begin_Shader();

	// multiThread Rendering
	virtual void	Begin_Shader(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

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
	CUploadBuffer<CB_SHADER_MESH>*	m_pCB_ShaderMesh     = nullptr;
	ID3D12DescriptorHeap*			m_pTexDescriptorHeap = nullptr;

public:
	virtual CComponent* Clone() override;
	static	CShader*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();
};

END