#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderMeshTerrain final : public CShader
{
private:
	explicit CShaderMeshTerrain(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderMeshTerrain(const CShaderMeshTerrain& rhs);
	virtual ~CShaderMeshTerrain() = default;

public:
	// Get
	CUploadBuffer<CB_SHADER_MESH>*		Get_UploadBuffer_ShaderMesh()		{ return m_pCB_ShaderMesh; }
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();

	// SingleThread Rendering
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr, 
								 const _uint& iSubMeshIdx = 0);

	// MultiThread Rendering
	virtual void	Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
								 const _int& iContextIdx,
								 ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr,
								 const _uint& iSubMeshIdx = 0);

	virtual void	Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
		const _int& iContextIdx,
		ID3D12DescriptorHeap* pTexDescriptorHeap, 
		ID3D12DescriptorHeap* pTexNormalDescriptorHeap,
		_uint uiTexnormalIdx,_uint uiPatternMapIdx,
		const _uint& iSubMeshIdx);

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
																  const D3D12_BLEND_OP& BlendOpAlpha	= D3D12_BLEND_OP_ADD,
																  const _bool & bIsAlphaTest = false);
private:
	_uint m_uiSubsetMeshSize = 0;

	CUploadBuffer<CB_SHADER_MESH>*		m_pCB_ShaderMesh		= nullptr;


public:
	virtual CComponent* Clone() override;
	static	CShader*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void Free();
};

END