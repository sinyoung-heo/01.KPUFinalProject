#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

enum TARGETID { TYPE_DEFAULT, TYPE_LIGHTING, TYPE_SHADOWDEPTH };

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
	void	Set_TargetRenderPos(const _vec3& vPos)		{ m_vPos = vPos; };
	void	Set_TargetRenderScale(const _vec3& vScale)	{ m_vScale = vScale; };
	void	Set_TargetTextureSize(const _uint& iIdx, const _float& fWidth, const _float& fHeight);

	// Method
	HRESULT	Ready_RenderTarget(const _uint& uiTargetCnt);
	HRESULT	SetUp_DefaultSetting(const TARGETID& eID = TYPE_DEFAULT);
	HRESULT SetUp_OnGraphicDevice(const TARGETID& eID = TYPE_DEFAULT);
	HRESULT Release_OnGraphicDevice(const TARGETID& eID = TYPE_DEFAULT);

	// MultiThreadRendering
	HRESULT	Begin_RenderTargetOnContext(ID3D12GraphicsCommandList* pCommandList, const THREADID& eID);
	HRESULT Bind_RenderTargetOnContext(ID3D12GraphicsCommandList* pCommandList, const THREADID& eID);
	HRESULT	End_RenderTargetOnContext(ID3D12GraphicsCommandList* pCommandList, const THREADID& eID);

	void	Render_RenderTarget();

private:
	void	Set_ConstantTable(const _uint& iIdx);
	HRESULT Create_TextureDescriptorHeap();

private:
	/*__________________________________________________________________________________________________________
	[ Graphic Device ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice	= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	CScreenTex*		m_pBufferCom = nullptr;
	CShaderTexture*	m_pShaderCom = nullptr;

	/*__________________________________________________________________________________________________________
	[ Descriptor ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* m_pRTV_Heap = nullptr;
	ID3D12DescriptorHeap* m_pDSV_Heap = nullptr;

	_uint m_uiRTV_DescriptorSize = 0;
	_uint m_uiDSV_DescriptorSize = 0;

	/*__________________________________________________________________________________________________________
	[ Target Texture Resource ]
	____________________________________________________________________________________________________________*/
	vector<ComPtr<ID3D12Resource>>	m_vecTargetTexture;			// RenderTarget Txture.
	vector<ComPtr<ID3D12Resource>>	m_vecDepthStencilBuffer;	// DepthStencil Buffer.

	vector<DXGI_FORMAT>				m_vecTargetFormat;			// RenderTarget Format.
	vector<D3D12_CLEAR_VALUE>		m_vecClearColor;			// RenderTarget Clear 색상.
	vector<D3D12_VIEWPORT>			m_vecViewPort;				// RenderTarget TextureSizew.

	ID3D12DescriptorHeap*			m_pTexDescriptorHeap = nullptr;
	_uint							m_uiTargetCnt        = 0;	// RenderTarget 개수.

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_vec3	m_vPos		= _vec3(0.0f);
	_vec3	m_vScale	= _vec3(0.0f);
	_matrix m_matWorld	= INIT_MATRIX;

public:
	static CRenderTarget*	Create(ID3D12Device* pGraphicDevice, 
								   ID3D12GraphicsCommandList* pCommandList,
								   const _uint& uiTargetCnt);
private:
	virtual void			Free();
};

END