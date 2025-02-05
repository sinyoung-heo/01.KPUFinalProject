#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderMeshEffect final : public CShader
{
private:
	explicit CShaderMeshEffect(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
	explicit CShaderMeshEffect(const CShaderMeshEffect& rhs);
	virtual ~CShaderMeshEffect() = default;

public:
	// Get
	CUploadBuffer<CB_SHADER_MESH>* Get_UploadBuffer_ShaderMesh() { return m_pCB_ShaderMesh; }
	CUploadBuffer<CB_SKINNING_MATRIX>* Get_UploadBuffer_SkinningMatrix() { return m_pCB_SkinningMatrix; }


	HRESULT SetUp_ShaderConstantBuffer(const _uint& uiNumSubsetMesh = 1);
	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();

	// SingleThread Rendering
	virtual void	Begin_Shader(ID3D12DescriptorHeap * pTexDescriptorHeap = nullptr,
								 const _uint & iSubMeshIdx = 0);
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap,
		ID3D12DescriptorHeap* pTexNormalDescriptorHeap, _uint uiDiffuseIdx,
		_uint uiTexnormalIdx, _uint uiPatternMapIdx, _uint uiShadowDepthIdx, _uint uiDissolveIdx,
		const _uint& iSubMeshIdx);
	// MultiThread Rendering
	virtual void	Begin_Shader(ID3D12GraphicsCommandList * pCommandList,
								 const _int & iContextIdx,
								 ID3D12DescriptorHeap * pTexDescriptorHeap = nullptr,
								 const _uint & iSubMeshIdx = 0);

	virtual void	Begin_Shader(ID3D12GraphicsCommandList * pCommandList,
		const _int & iContextIdx,
		ID3D12DescriptorHeap * pTexDescriptorHeap,
		ID3D12DescriptorHeap * pTexNormalDescriptorHeap,
		_uint uiTexnormalIdx,_uint uiPatternMapIdx,
		const _uint & iSubMeshIdx);
	virtual void	Begin_Shader(ID3D12GraphicsCommandList * pCommandList,
		const _int & iContextIdx,
		ID3D12DescriptorHeap * pTexDescriptorHeap,
		ID3D12DescriptorHeap * pTexNormalDescriptorHeap,
		_uint AlbedoIdx,
		const _uint & iSubMeshIdx);
	
private:
	virtual HRESULT								Create_RootSignature();
	virtual HRESULT								Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint = "VS_MAIN",
																   string PS_EntryPoint = "PS_MAIN");
	virtual D3D12_BLEND_DESC					Create_BlendState(const _bool & bIsBlendEnable = false,
																  const D3D12_BLEND & SrcBlend = D3D12_BLEND_ONE,
																  const D3D12_BLEND & DstBlend = D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP & BlendOp = D3D12_BLEND_OP_ADD,
																  const D3D12_BLEND & SrcBlendAlpha = D3D12_BLEND_ONE,
																  const D3D12_BLEND & DstBlendAlpha = D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP & BlendOpAlpha = D3D12_BLEND_OP_ADD,
																  const _bool & bIsAlphaTest = false);
private:
	_uint m_uiSubsetMeshSize = 0;
	CUploadBuffer<CB_SHADER_MESH>* m_pCB_ShaderMesh = nullptr;
	CUploadBuffer<CB_SKINNING_MATRIX>* m_pCB_SkinningMatrix = nullptr;

public:
	virtual CComponent* Clone() override;
	static	CShader* Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
private:
	virtual void Free();
};

END