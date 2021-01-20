#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderLighting final : public CShader
{
private:
	explicit CShaderLighting(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderLighting(const CShaderLighting& rhs);
	virtual ~CShaderLighting() = default;

public:
	// Get
	CUploadBuffer<CB_LIGHT_DESC>*		Get_UploadBuffer_LightDesc()	{ return m_pCB_LightDesc; }
	CUploadBuffer<CB_CAMERAINV_DESC>*	Get_UploadBuffer_CameraDesc()	{ return m_pCB_CameraDesc; }

	// Set
	void							SetUp_ShaderTexture(vector< ComPtr<ID3D12Resource>> pvecTargetTexture);

	// CShader��(��) ���� ��ӵ�
	virtual HRESULT					Ready_Shader();
	virtual void					Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr,
												 const _uint& iIdx = 0);

private:
	virtual HRESULT								Create_DescriptorHeaps(vector<ComPtr<ID3D12Resource>> pVecTexture);
	virtual HRESULT								Create_ConstantBuffer();
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
	ID3D12DescriptorHeap*			m_pTexDescriptorHeap = nullptr;
	CUploadBuffer<CB_LIGHT_DESC>*	m_pCB_LightDesc		 = nullptr;
	CUploadBuffer<CB_CAMERAINV_DESC>*	m_pCB_CameraDesc	 = nullptr;


public:
	virtual CComponent *		Clone() override;
	static	CShaderLighting*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void				Free();
};

END