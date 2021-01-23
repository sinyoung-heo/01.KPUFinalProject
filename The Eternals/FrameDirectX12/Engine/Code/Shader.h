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

	// CShader을(를) 통해 상속됨
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
	[ 루트 서명(Root Signature) ]
	- 그리기 호출 전에 응용 프로그램이 반드시 렌더링 파이프라인에 묶어야 하는 자원들이 무엇이고, 
	그 자원들이 셰이더 입력 레지스터들에 어떻게 대응되는지를 정의한다.
	- 루트 서명은 반드시 그리기 호출에 쓰이는 셰이더들과 호환돼야 한다.
	- 루트 서명의 유효성은 파이프라인 상태 객체를 생성할 때 검증된다.
	- 그리기 호출마다 서로 다른 셰이더 프로그램들을 사용할 수 있으며, 그런 경우 루트 서명도 달라야 한다.

	[ 파이프라인 상태 객체(Pipeline State Object, PSO) ]
	- DirectX 9 Shader에서 technique - pass의 개념.
	- PSO 검증과 생성에는 많은 시간이 걸릴 수 있으므로, 초기화 시점에서 생성해야 한다.
	- 성능을 위해서는 PSO 상태 변경을 최소화해야 한다.
	- 같은 PSO를 사용할 수 있는 물체들은 모두 함께 그려야 마땅하다.
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