#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class CComponentMgr;
class CGameObject;
class CComponent;
class CRenderTarget;
class CScreenTex;
class CShaderBlend;

struct ThreadParameter
{
	_int threadIdx;
};


class ENGINE_DLL CRenderer final : public CBase
{
	DECLARE_SINGLETON(CRenderer)

public:
	enum RENDERGROUP 
	{
		RENDER_PRIORITY,
		RENDER_NONALPHA,
		RENDER_ALPHA,
		RENDER_UI,
		RENDER_FONT,
		RENDER_END
	};

private:
	explicit CRenderer();
	virtual ~CRenderer() = default;

public:
	// Get
	_uint			Get_RenderLstSize(const RENDERGROUP& eRenderGroup);
	_uint			Get_ColliderLstSize();
	const _uint&	Get_CntShaderFile()									{ return m_uiCnt_ShaderFile; }
	const _uint&	Get_CntPipelineState()								{ return m_uiCnt_PipelineState; }
	const _uint&	Get_CntSetPipelineState()							{ return m_uiCnt_SetPipelineState; }
	_bool			Get_RenderOnOff(wstring wstrTag);
	CRenderTarget*	Get_ShadowDepthTarget()								{ return m_pShadowDepthTarget; }

	// Set
	HRESULT	Set_CurPipelineState(ID3D12PipelineState* pPipelineState);
	HRESULT	Set_CurPipelineState(ID3D12GraphicsCommandList* pCommandList, 
								 ID3D12PipelineState* pPipelineState,
								 const _int& iContextIdx);


	HRESULT	Set_RenderOnOff(wstring wstrTag);
	void	Add_PipelineStateCnt()		{ ++m_uiCnt_PipelineState; };
	void	Reset_SetPipelineStateCnt() { m_uiCnt_SetPipelineState = 0; }
	void	Set_bIsLoadingFinish()		{ m_bIsLoadingFinish = true; }

	// Method
	HRESULT	Ready_Renderer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	HRESULT	Add_Renderer(const RENDERGROUP& eRenderID, CGameObject* pGameObject);
	HRESULT Add_Renderer(CComponent* pComponent);
	HRESULT	Render_Renderer(const _float& fTimeDelta, const RENDERID& eID = RENDERID::MULTI_THREAD);
	void	Clear_RenderGroup();

private:
	void	Render_Priority(const _float& fTimeDelta);
	void	Render_ShadowDepth(const _float& fTimeDelta);
	void	Render_NonAlpha(const _float& fTimeDelta);
	void	Render_Light();
	void	Render_Blend();
	void	Render_Alpha(const _float& fTimeDelta);
	void	Render_UI(const _float& fTimeDelta);
	void	Render_Collider(const _float& fTimeDelta);
	void	Render_RenderTarget();
	void	Render_Font(const _float& fTimeDelta);

	// Ready Resource
	HRESULT	Ready_ShaderPrototype();
	HRESULT	Ready_RenderTarget();

private:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice / Mgr ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*				m_pGraphicDevice	= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList		= nullptr;
	CComponentMgr*				m_pComponentMgr		= nullptr;

	/*__________________________________________________________________________________________________________
	[ Render Group ]
	____________________________________________________________________________________________________________*/
	vector<CGameObject*>	m_RenderList[RENDER_END];		// GameObject RenderGroup
	list<CComponent*>		m_ColliderList;					// Collier RenderList
	_uint					m_uiRenderListSize		= 0;	// Render GameObject Cnt
	_uint					m_uiColliderListSize	= 0;	// Render Collider Cnt

	/*__________________________________________________________________________________________________________
	[ Multi RenderTarget ]
	____________________________________________________________________________________________________________*/
	// Render Target
	CRenderTarget*	m_pDeferredTarget		= nullptr;
	CRenderTarget*	m_pLightTarget			= nullptr;
	CRenderTarget*	m_pShadowDepthTarget	= nullptr;

	// Blend
	CScreenTex*		m_pBlendBuffer			= nullptr;
	CShaderBlend*	m_pBlendShader			= nullptr;
	_bool			m_bIsSetBlendTexture	{ false };

	/*__________________________________________________________________________________________________________
	[ Pipeline StateGroup ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState* m_pPrePipelineState = nullptr;	// 이전의 PipelineState.
	ID3D12PipelineState* m_pCurPipelineState = nullptr;	// 현재의 PipelineState.
	_uint	m_uiCnt_ShaderFile			     = 0;
	_uint	m_uiCnt_PipelineState		     = 0;
	_uint	m_uiCnt_SetPipelineState	     = 0;

	/*__________________________________________________________________________________________________________
	[ Rener On/Off ]
	____________________________________________________________________________________________________________*/
	map<wstring, _bool> m_mapRenderOnOff;

	/*__________________________________________________________________________________________________________
	[ MultiThread Rendering ]
	____________________________________________________________________________________________________________*/
	HANDLE m_hWorkerBeginRender[CONTEXT::CONTEXT_END];		// 각 쓰레드에게 Render시작을 알림.
	HANDLE m_hWorkerFinishShadow[CONTEXT::CONTEXT_END];		// Render ShadowDepth.
	HANDLE m_hWorkerFinishedRender[CONTEXT::CONTEXT_END];	// Render NoneAlpha.
	HANDLE m_hThreadHandle[CONTEXT::CONTEXT_END];

	array<ID3D12CommandAllocator*, CONTEXT::CONTEXT_END>	m_arrShadowCommandAllocator		{ nullptr };
	array<ID3D12GraphicsCommandList*, CONTEXT::CONTEXT_END>	m_arrShadowCommandList			{ nullptr };
	ID3D12CommandAllocator*									m_pPreShadowCommandAllocator	= nullptr;
	ID3D12GraphicsCommandList*								m_pPreShadowCommandList			= nullptr;
	ID3D12CommandAllocator*									m_pEndShadowCommandAllocator	= nullptr;
	ID3D12GraphicsCommandList*								m_pEndShadowCommandList			= nullptr;

	array<ID3D12CommandAllocator*, CONTEXT::CONTEXT_END>	m_arrSceneCommandAllocator		{ nullptr };
	array<ID3D12GraphicsCommandList*, CONTEXT::CONTEXT_END>	m_arrSceneCommandList			{ nullptr };
	ID3D12CommandAllocator*									m_pPreSceneCommandAllocator		= nullptr;
	ID3D12GraphicsCommandList*								m_pPreSceneCommandList			= nullptr;
	ID3D12CommandAllocator*									m_pEndSceneCommandAllocator		= nullptr;
	ID3D12GraphicsCommandList*								m_pEndSceneCommandList			= nullptr;

	array<ID3D12PipelineState*, CONTEXT::CONTEXT_END>		m_arrContextPrePipelineState	{ nullptr };
	array<ID3D12PipelineState*, CONTEXT::CONTEXT_END>		m_arrContextCurPipelineState	{ nullptr };

	ThreadParameter											m_tThreadParameter[CONTEXT::CONTEXT_END];

	_bool m_bIsLoadingFinish	= false;
	_bool m_bIsCreateThread		= false;

public:
	void	Create_ThreadContext();
	HRESULT	Create_ThreadCommandList();
	HRESULT	Reset_ThreadCommandList();
	HRESULT Render_MultiThread(const _float& fTimeDelta);
	void	Worker_Thread(_int threadIndex);


private:
	virtual void		Free();
};

END