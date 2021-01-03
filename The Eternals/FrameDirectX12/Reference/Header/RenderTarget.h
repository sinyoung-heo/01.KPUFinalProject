#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class CScreenTex;
class CShaderTexture;

class ENGINE_DLL CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CRenderTarget() = default;

public:
	// Get
	vector<ComPtr<ID3D12Resource>> Get_TargetTexture() { return m_vecTargetTexture; };

	// Set
	void	Set_TargetClearColor(const _uint& iIdx,  const _rgba& vColor, const DXGI_FORMAT& TargetFormat);
	void	Set_TargetRenderPos(const _vec3& vPos)								{ m_vPos = vPos; };
	void	Set_TargetRenderScale(const _vec3& vScale)							{ m_vScale = vScale; };
	void	Set_TargetTextureSize(const _uint& iIdx, const _float& fWidth, const _float& fHeight);

	// Method
	HRESULT	Ready_RenderTarget(const _uint& uiTargetCnt);
	HRESULT Create_TextureDescriptorHeap();
	void	SetUp_ShaderConstantBuffer();

	HRESULT	SetUp_DefaultSetting();				// Depth & Stencil	O
	HRESULT	SetUp_LightSetting();				// Depth & Stencil	X
	HRESULT	SetUp_ShadowDepthSetting();			// DSVHeap			O

	HRESULT	SetUp_OnGraphicDevice();			// Depth & Stencil	O
	HRESULT	SetUp_LightOnGraphicDevice();		// Depth & Stencil	X
	HRESULT	SetUp_ShadowDepthOnGraphicDevice();

	HRESULT	Release_OnGraphicDevice();
	HRESULT	Release_ShadowDepthOnGraphicDevice();

	// 2020.06.12 MultiThreadRendering
	HRESULT	Begin_RenderTargetOnContext(ID3D12GraphicsCommandList* pCommandList, const THREADID& eID);
	HRESULT Bind_RenderTargetOnContext(ID3D12GraphicsCommandList* pCommandList, const THREADID& eID);
	HRESULT	End_RenderTargetOnContext(ID3D12GraphicsCommandList* pCommandList, const THREADID& eID);

	void	Render_RenderTarget();

private:
	void	Set_ConstantTable(const _uint& iIdx);

private:
	/*__________________________________________________________________________________________________________
	[ Graphic Device ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice	{ nullptr };
	ID3D12GraphicsCommandList*	m_pCommandList		{ nullptr };

	ID3D12GraphicsCommandList4* pTemp;

	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CScreenTex*		m_pBufferCom					{ nullptr };
	CShaderTexture*	m_pShaderCom					{ nullptr };

	/*__________________________________________________________________________________________________________
	[ Descriptor ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap*	m_pRTV_Heap				{ nullptr };
	ID3D12DescriptorHeap*	m_pDSV_Heap				{ nullptr };

	_uint m_uiRTV_DescriptorSize					{ 0 };
	_uint m_uiDSV_DescriptorSize					{ 0 };

	/*__________________________________________________________________________________________________________
	[ Target Texture Resource ]
	____________________________________________________________________________________________________________*/
	vector<ComPtr<ID3D12Resource>>	m_vecTargetTexture;			// RenderTarget Txture.
	vector<ComPtr<ID3D12Resource>>	m_vecDepthStencilBuffer;	// DepthStencil Buffer.

	vector<DXGI_FORMAT>				m_vecTargetFormat;			// RenderTarget Format.
	vector<D3D12_CLEAR_VALUE>		m_vecClearColor;			// RenderTarget Clear ����.
	vector<D3D12_VIEWPORT>			m_vecViewPort;				// RenderTarget TextureSizew.

	ID3D12DescriptorHeap*			m_pTexDescriptorHeap = nullptr;
	_uint							m_uiTargetCnt        = 0;	// RenderTarget ����.

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_vec3	m_vPos		{ INIT_VEC3(0.0f) };
	_vec3	m_vScale	{ INIT_VEC3(0.0f) };
	_matrix m_matWorld	{ INIT_MATRIX };

public:
	static CRenderTarget*	Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* pCommandList,
								   const _uint& uiTargetCnt);
private:
	virtual void			Free();
};

END