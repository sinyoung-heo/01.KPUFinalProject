#pragma once
#include "Shader.h"

BEGIN(Engine)

class CVIBuffer;

class ENGINE_DLL CShaderColorInstancing final : public CShader
{
	DECLARE_SINGLETON(CShaderColorInstancing)

private:
	explicit CShaderColorInstancing() = default;
	virtual ~CShaderColorInstancing() = default;

public:
	// Get
	_uint							Get_InstanceCount(const COLOR_BUFFER & eBuffer, const _uint& iPipelineStatePass) { return m_vecInstancing[eBuffer][iPipelineStatePass].iInstanceCount; };
	CUploadBuffer<CB_SHADER_COLOR>*	Get_UploadBuffer_ShaderColor(const COLOR_BUFFER& eBuffer, const _uint& uiPipelineStatepass);

	HRESULT Ready_Shader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	void	SetUp_ConstantBuffer(ID3D12Device* pGraphicDevice);
	void	Resize_ConstantBuffer(ID3D12Device* pGraphicDevice);
	void	Add_Instance(const COLOR_BUFFER& eBuffer,const _uint& iPipelineStateIdx)	{ ++(m_vecInstancing[eBuffer][iPipelineStateIdx].iInstanceCount); };
	void	Add_TotalInstancCount(const COLOR_BUFFER& eBuffer)							{ ++m_uiTotalInstanceCnt[eBuffer]; }
	void	Reset_Instance();
	void	Reset_InstancingConstantBuffer();
	void	Render_Instance();

private:
	void	SetUp_Instancing();
	void	SetUp_VIBuffer();
	virtual HRESULT	Create_RootSignature();
	virtual HRESULT	Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC> Create_InputLayout(string VS_EntryPoint = "VS_MAIN", string PS_EntryPoint = "PS_MAIN");

private:
	/*__________________________________________________________________________________________________________
	Key값은 BufferTag (Rect, Box, Sphere)
	vector의 Index는 PipelineStateIndex, Size는 Instance개수.
	____________________________________________________________________________________________________________*/
	CVIBuffer*								m_arrVIBuffer[COLOR_BUFFER::COLORBUFFER_END];
	vector<INSTANCING_DESC>					m_vecInstancing[COLOR_BUFFER::COLORBUFFER_END];
	vector<CUploadBuffer<CB_SHADER_COLOR>*>	m_vecCB_ShaderColor[COLOR_BUFFER::COLORBUFFER_END];
	_uint									m_uiTotalInstanceCnt[COLOR_BUFFER::COLORBUFFER_END];
	_uint									m_uiPipelineStateCnt = 0;

private:
		virtual void Free();
};

END