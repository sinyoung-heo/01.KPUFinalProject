#pragma once
#include "Shader.h"

BEGIN(Engine)

class CScreenTex;

class ENGINE_DLL CShaderLightingInstancing final : public CShader
{
	DECLARE_SINGLETON(CShaderLightingInstancing)

private:
	explicit CShaderLightingInstancing() = default;
	virtual ~CShaderLightingInstancing() = default;

public:
	// Get
	_uint								Get_InstanceCount(const _uint& iPipelineStatePass) { return m_vecInstancing[iPipelineStatePass].iInstanceCount; };
	CUploadBuffer<CB_SHADER_LIGHTING>*	Get_UploadBuffer_ShaderLighting(const _uint& uiPipelineStatepass);

	HRESULT Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	void	SetUp_VIBuffer();
	void	SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice);
	HRESULT	SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> pvecTargetTexture);
	void	Resize_ConstantBuffer(ID3D12Device* pGraphicDevice);
	void	Add_Instance(const _uint& iPipelineStateIdx)	{ ++(m_vecInstancing[iPipelineStateIdx].iInstanceCount); };
	void	Add_TotalInstancCount(const _uint& iPipelineStateIdx)	{ ++m_vecTotalInstanceCnt[iPipelineStateIdx]; }
	void	Reset_Instance();
	void	Reset_InstancingConstantBuffer();
	void	Render_Instance();

private:
	void	SetUp_Instancing();
	virtual HRESULT	Create_RootSignature();
	virtual HRESULT	Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC> Create_InputLayout(string VS_EntryPoint = "VS_MAIN", string PS_EntryPoint = "PS_MAIN");

private:
	CScreenTex*				m_pBufferCom             = nullptr;
	ID3D12DescriptorHeap*	m_pTexDescriptorHeap     = nullptr;
	_bool					m_bIsSetUpDescruptorHeap = false;

	/*__________________________________________________________________________________________________________
	LightingType (Direction, Point, Spot)
	vector의 Index는 PipelineStateIndex, Size는 Instance개수.
	____________________________________________________________________________________________________________*/
	vector<INSTANCING_DESC>						m_vecInstancing;
	vector<CUploadBuffer<CB_SHADER_LIGHTING>*>	m_vecCB_ShaderLighting;
	vector<_uint>								m_vecTotalInstanceCnt;
	_uint										m_uiPipelineStateCnt = 0;

private:
	virtual void Free();
};

END