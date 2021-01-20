#pragma once
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CShaderColor final : public CShader
{
private:
	explicit CShaderColor(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShaderColor(const CShaderColor& rhs);
	virtual ~CShaderColor() = default;

public:
	// Get
	//CUploadBuffer<CB_MATRIX_DESC>*	Get_UploadBuffer_MatrixDesc()	{ return m_pCB_MatrixDesc; }
	//CUploadBuffer<CB_COLOR_DESC>*	Get_UploadBuffer_ColorDesc()	{ return m_pCB_ColorDesc; }
	CUploadBuffer<CB_SHADER_COLOR>* Get_UploadBuffer_ShaderColor()	{ return m_pCB_ShaderColor; }

	// CShader을(를) 통해 상속됨
	virtual HRESULT	Ready_Shader();
	virtual void	Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr, 
								 const _uint& iIdx = 0);

private:
	virtual HRESULT								Create_DescriptorHeaps();
	virtual HRESULT								Create_ConstantBuffer();
	virtual HRESULT								Create_RootSignature();
	virtual HRESULT								Create_PipelineState();
	virtual vector<D3D12_INPUT_ELEMENT_DESC>	Create_InputLayout(string VS_EntryPoint = "VS_MAIN",
																   string PS_EntryPoint = "PS_MAIN");

private:
	//CUploadBuffer<CB_MATRIX_DESC>*	m_pCB_MatrixDesc	= nullptr;
	//CUploadBuffer<CB_COLOR_DESC>*	m_pCB_ColorDesc		= nullptr;

	CUploadBuffer<CB_SHADER_COLOR>* m_pCB_ShaderColor = nullptr;


public:
	virtual CComponent *	Clone() override;
	static CShaderColor*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
private:
	virtual void			Free();
};

END