#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderShadow final : public CShader
{
	explicit CShaderShadow(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
	explicit CShaderShadow(const CShaderShadow& rhs);
	virtual ~CShaderShadow() = default;

public:
	// Get
	CUploadBuffer<CB_SHADER_SHADOW>* Get_UploadBuffer_ShaderShadow() { return m_pCB_ShadowShader; }
	CUploadBuffer<CB_SKINNING_MATRIX>* Get_UploadBuffer_SkinningMatrix() { return m_pCB_SkinningMatrix; }

	// CShader��(��) ���� ��ӵ�
	virtual HRESULT	Ready_Shader();
	virtual void	Begin_Shader(ID3D12DescriptorHeap * pTexDescriptorHeap = nullptr,
								 const _uint & iIdx = 0);

	// 2020.06.11 MultiThreadRendering
	virtual void	Begin_Shader(ID3D12GraphicsCommandList * pCommandList,
								 const _int & iContextIdx,
								 ID3D12DescriptorHeap * pTexDescriptorHeap = nullptr,
								 const _uint & iIdx = 0);

	void			SetUp_ShaderConstantBuffer(const _uint & iNumSubsetMesh);

private:
	virtual HRESULT								Create_DescriptorHeaps();
	virtual HRESULT								Create_ConstantBuffer(const _uint & iNumSubsetMesh);
	virtual HRESULT								Create_RootSignature();
	virtual HRESULT								Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint = "VS_MAIN", string PS_EntryPoint = "PS_MAIN");
	virtual D3D12_BLEND_DESC					Create_BlendState(const _bool & bIsBlendEnable = false,
																  const D3D12_BLEND & SrcBlend = D3D12_BLEND_ONE,
																  const D3D12_BLEND & DstBlend = D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP & BlendOp = D3D12_BLEND_OP_ADD,
																  const D3D12_BLEND & SrcBlendAlpha = D3D12_BLEND_ONE,
																  const D3D12_BLEND & DstBlendAlpha = D3D12_BLEND_ZERO,
																  const D3D12_BLEND_OP & BlendOpAlpha = D3D12_BLEND_OP_ADD);

private:
	CUploadBuffer<CB_SHADER_SHADOW>* m_pCB_ShadowShader = nullptr;
	CUploadBuffer<CB_SKINNING_MATRIX>* m_pCB_SkinningMatrix = nullptr;

public:
	virtual CComponent* Clone() override;
	static	CShader* Create(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList);
private:
	virtual void Free();
};

END