#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderShadowInstancing final :  public CShader
{
	DECLARE_SINGLETON(CShaderShadowInstancing)

private:
	explicit CShaderShadowInstancing() = default;
	virtual ~CShaderShadowInstancing() = default;

public:
	_uint								Get_InstanceCount(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& iPipelineStatePass)	{ return m_mapInstancing[iContextIdx][wstrMeshTag][iPipelineStatePass].iInstanceCount; };
	CUploadBuffer<CB_SHADER_SHADOW>*	Get_UploadBuffer_ShaderShadow(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& uiPipelineStatepass);

	HRESULT Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	void	SetUp_Instancing(wstring wstrMeshTag);
	void	SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice);
	void	Add_TotalInstanceCount(wstring wstrMeshTag);
	void	Add_Instance(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& iPipelineStateIdx);
	void	Reset_Instance();
	void	Reset_InstancingContainer();
	void	Reset_InstancingConstantBuffer();
	void	Render_Instance(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

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
	map<wstring, vector<INSTANCING_DESC>>					m_mapInstancing[CONTEXT::CONTEXT_END];
	map<wstring, vector<CUploadBuffer<CB_SHADER_SHADOW>*>>	m_mapCB_ShaderShadow[CONTEXT::CONTEXT_END];
	map<wstring, _uint>										m_mapTotalInstanceCnt;
	_uint													m_uiPipelineStateCnt = 0;

private:
	virtual void Free();
};

END