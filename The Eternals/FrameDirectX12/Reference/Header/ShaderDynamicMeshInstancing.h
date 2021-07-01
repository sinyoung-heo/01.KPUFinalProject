#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderDynamicMeshInstancing : public CShader
{
	DECLARE_SINGLETON(CShaderDynamicMeshInstancing)

private:
	explicit CShaderDynamicMeshInstancing() = default;
	virtual ~CShaderDynamicMeshInstancing() = default;

public:
	HRESULT Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	void SetUp_Instancing(wstring wstrMeshTag, const _uint& uiSubMeshSize);
	void Add_TotalInstanceCount(wstring wstrMeshTag, const _uint& uiSubMeshSize);
private:
	virtual HRESULT	Create_RootSignature();
	virtual HRESULT	Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC> Create_InputLayout(string VS_EntryPoint = "VS_MAIN", string PS_EntryPoint = "PS_MAIN");
	virtual D3D12_BLEND_DESC Create_BlendState(const _bool& bIsBlendEnable			= false,
							 				   const D3D12_BLEND& SrcBlend			= D3D12_BLEND_ONE,
							 				   const D3D12_BLEND& DstBlend			= D3D12_BLEND_ZERO,
							 				   const D3D12_BLEND_OP& BlendOp		= D3D12_BLEND_OP_ADD,
							 				   const D3D12_BLEND& SrcBlendAlpha		= D3D12_BLEND_ONE,
							 				   const D3D12_BLEND& DstBlendAlpha		= D3D12_BLEND_ZERO,
							 				   const D3D12_BLEND_OP& BlendOpAlpha	= D3D12_BLEND_OP_ADD,
											   const _bool& bIsAlphaTest = false);
private:
	/*__________________________________________________________________________________________________________
	Key값은 ResourceTag
	vector의 Index는 PipelineStateIndex, Size는 Instance개수.
	____________________________________________________________________________________________________________*/
	map<wstring, vector<INSTANCING_DESC>>									m_mapInstancing[CONTEXT::CONTEXT_END];
	map<wstring, vector<CUploadBuffer<CB_SHADER_DYAMICMESH_INSTANCEING>*>>	m_mapCB_ShaderDynamicMesh[CONTEXT::CONTEXT_END];
	map<wstring, _uint>														m_mapTotalInstanceCnt;
	_uint																	m_uiPipelineStateCnt = 0;

private:
	virtual void Free();
};

END