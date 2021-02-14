#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderTextureInstancing : public CShader
{
	DECLARE_SINGLETON(CShaderTextureInstancing)

private:
	explicit CShaderTextureInstancing() = default;
	virtual ~CShaderTextureInstancing() = default;

public:
	// Get
	_uint								Get_InstanceCount(wstring wstrTextureTag, const _uint& iPipelineStatePass)		{ return m_mapInstancing[wstrTextureTag][iPipelineStatePass].iInstanceCount; };
	CUploadBuffer<CB_SHADER_TEXTURE>*	Get_UploadBuffer_ShaderMesh(wstring wstrTextureTag, const _uint& uiPipelineStatepass);

	HRESULT Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	void	SetUp_Instancing(wstring wstrTextureTag);
	void	SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice);
	void	Add_Instance(wstring wstrTextureTag, const _uint& iPipelineStateIdx);
	void	Reset_Instance();
	void	Reset_InstancingContainer();
	void	Reset_InstancingConstantBuffer();
	void	Render_Instance();

private:
	virtual HRESULT	Create_RootSignature();
	virtual HRESULT	Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC> Create_InputLayout(string VS_EntryPoint = "VS_MAIN", 
																string PS_EntryPoint = "PS_MAIN");
	virtual D3D12_BLEND_DESC Create_BlendState(const _bool& bIsBlendEnable			= false,
							 				   const D3D12_BLEND& SrcBlend			= D3D12_BLEND_ONE,
							 				   const D3D12_BLEND& DstBlend			= D3D12_BLEND_ZERO,
							 				   const D3D12_BLEND_OP& BlendOp		= D3D12_BLEND_OP_ADD,
							 				   const D3D12_BLEND& SrcBlendAlpha		= D3D12_BLEND_ONE,
							 				   const D3D12_BLEND& DstBlendAlpha		= D3D12_BLEND_ZERO,
							 				   const D3D12_BLEND_OP& BlendOpAlpha	= D3D12_BLEND_OP_ADD);
private:
	/*__________________________________________________________________________________________________________
	Key값은 ResourceTag
	vector의 Index는 PipelineStateIndex, Size는 Instance개수.
	____________________________________________________________________________________________________________*/
	map<wstring, vector<INSTANCING_DESC>>					m_mapInstancing;
	map<wstring, vector<CUploadBuffer<CB_SHADER_TEXTURE>*>>	m_mapCB_ShaderTexture;
	_uint													m_uiPipelineStateCnt = 0;

private:
	virtual void Free();
};

END