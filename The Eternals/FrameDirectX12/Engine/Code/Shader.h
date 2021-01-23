#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader : public CComponent
{
protected:
	explicit CShader(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	// Get
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6>	Get_StaticSamplers();
	ID3D12RootSignature*						Get_RootSignature()						{ return m_pRootSignature; }
	ID3D12PipelineState*						Get_PipelineState(const _uint& iIdx = 0);
	ID3D12PipelineState*						Get_PipelineStatePass()					{ return m_pPipelineState; }
	CUploadBuffer<CB_CAMERA_MATRIX>*			Get_UploadBuffer_CameraProjMatrix()		{ return m_pCB_CameraProjMatrix; }
	CUploadBuffer<CB_CAMERA_MATRIX>*			Get_UploadBuffer_CameraOrthoMatrix()	{ return m_pCB_CameraOrthoMatrix; }

	// Set
	HRESULT					Set_PipelineStatePass(const _uint& iIdx = 0);

	// CShader��(��) ���� ��ӵ�
	virtual HRESULT			Ready_Shader();
	virtual void			Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr, 
										 const _uint& iIdx = 0);

	/*__________________________________________________________________________________________________________
	20.06.11 MultiThreadRendering
	____________________________________________________________________________________________________________*/
	virtual void			Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
										 const _int& iContextIdx,
										 ID3D12DescriptorHeap* pTexDescriptorHeap = nullptr,
										 const _uint& iIdx = 0);

public:
	static XMFLOAT4X4	Compute_MatrixTranspose(_matrix& mat);

protected:
	virtual HRESULT								Create_DescriptorHeaps();
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
	D3D12_RASTERIZER_DESC						Create_RasterizerState(const D3D12_FILL_MODE& eFillMode = D3D12_FILL_MODE_SOLID,
																	   const D3D12_CULL_MODE& eCullMode = D3D12_CULL_MODE_BACK);
	D3D12_DEPTH_STENCIL_DESC					Create_DepthStencilState(const _bool& bIsZWrite			= true);
	ID3DBlob* Compile_Shader(const wstring& wstrFilename,
							 const D3D_SHADER_MACRO* tDefines,
							 const string& strEntrypoint,
							 const string& strTarget);
protected:
	/*__________________________________________________________________________________________________________
	[ ��Ʈ ����(Root Signature) ]
	- �׸��� ȣ�� ���� ���� ���α׷��� �ݵ�� ������ ���������ο� ����� �ϴ� �ڿ����� �����̰�, 
	�� �ڿ����� ���̴� �Է� �������͵鿡 ��� �����Ǵ����� �����Ѵ�.
	- ��Ʈ ������ �ݵ�� �׸��� ȣ�⿡ ���̴� ���̴���� ȣȯ�ž� �Ѵ�.
	- ��Ʈ ������ ��ȿ���� ���������� ���� ��ü�� ������ �� �����ȴ�.
	- �׸��� ȣ�⸶�� ���� �ٸ� ���̴� ���α׷����� ����� �� ������, �׷� ��� ��Ʈ ���� �޶�� �Ѵ�.

	[ ���������� ���� ��ü(Pipeline State Object, PSO) ]
	- DirectX 9 Shader���� technique - pass�� ����.
	- PSO ������ �������� ���� �ð��� �ɸ� �� �����Ƿ�, �ʱ�ȭ �������� �����ؾ� �Ѵ�.
	- ������ ���ؼ��� PSO ���� ������ �ּ�ȭ�ؾ� �Ѵ�.
	- ���� PSO�� ����� �� �ִ� ��ü���� ��� �Բ� �׷��� �����ϴ�.
	____________________________________________________________________________________________________________*/
	ID3D12RootSignature*			m_pRootSignature			= nullptr;
	ID3D12PipelineState*			m_pPipelineState			= nullptr;	// Current PipelineState
	vector<ID3D12PipelineState*>	m_vecPipelineState;						// All Shader PipelineState

	_uint		m_uiCBV_SRV_UAV_DescriptorSize					= 0;
	_uint		m_uiTexSize										= 0;

	ID3DBlob*	m_pVS_ByteCode									= nullptr;
	ID3DBlob*	m_pPS_ByteCode									= nullptr;

	// Camera ConstantBuffer
	CUploadBuffer<CB_CAMERA_MATRIX>* m_pCB_CameraProjMatrix		= nullptr;
	CUploadBuffer<CB_CAMERA_MATRIX>* m_pCB_CameraOrthoMatrix	= nullptr;


public:
	virtual CComponent* Clone() { return nullptr; };
protected:
	virtual void		Free();
};

END