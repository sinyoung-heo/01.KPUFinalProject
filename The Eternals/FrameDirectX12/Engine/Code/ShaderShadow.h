#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderShadow final : public CShader
{
	explicit CShaderShadow(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderShadow(const CShaderShadow& rhs);
	virtual ~CShaderShadow() = default;

public:
	// Get
	CUploadBuffer<CB_SHADOWDEPTH_DESC>*	Get_UploadBuffer_ShadowDepthDesc()	{ return m_pCB_ShadowDepthDesc; }
	CUploadBuffer<CB_SKINNING_MATRIX>*	Get_UploadBuffer_SkinningDesc()		{ return m_pCB_SkinningDesc; }

	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr, 
								 const _uint& iIdx = 0);

	// 2020.06.11 MultiThreadRendering
	virtual void	Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
								 const _int& iContextIdx,
								 ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr,
								 const _uint& iIdx = 0);

	void			SetUp_ShaderConstantBuffer(const _uint& iNumSubsetMesh);

private:
	virtual HRESULT								Create_DescriptorHeaps() ;
	virtual HRESULT								Create_ConstantBuffer(const _uint& iNumSubsetMesh);
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
	CUploadBuffer<CB_SHADOWDEPTH_DESC>*	m_pCB_ShadowDepthDesc	= nullptr;
	CUploadBuffer<CB_SKINNING_MATRIX>*	m_pCB_SkinningDesc		= nullptr;

public:
	virtual CComponent *	Clone() override;
	static	CShaderShadow*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void			Free();
};

END