#pragma once
#include "Engine_Include.h"
#include "Base.h"
#include <process.h>

BEGIN(Engine)

class CComponentMgr;
class CGameObject;
class CComponent;
class CRenderTarget;
class CScreenTex;
class CShaderBlend;
class CShaderLuminance;
class CShaderDownSampling;
class CShaderBlur;
class CShaderSSAO;
class CShaderNPathDir;
class CShaderMesh;
class CShaderEdge;
class CShaderAddEffect;
#define WIDTH_FIRST		112
#define WIDTH_SECOND	224+112
#define WIDTH_THIRD		448+112
#define WIDTH_FOURTH	672+112
#define WIDTH_FIFTH		672+224+112
#define WIDTH_SIXTH		672+224+224+112
#define WIDTH_SEVENTH	672+224+224+224+112
#define HEIGHT_FIRST	63
#define HEIGHT_SECOND	126 + 63
#define HEIGHT_THIRD	126+126 + 63
#define HEIGHT_FOURTH	126+126+126+63
#define HEIGHT_FIFTH	126+126+126+126+63
#define HEIGHT_SIXTH	126+126+126+126+126+63

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
		RENDER_DISTORTION,
		RENDER_EDGE,
		RENDER_CROSSFILTER,
		RENDER_COLLIDER,
		RENDER_MAGICCIRCLE,
		RENDER_ALPHA,
		RENDER_UI,
		RENDER_FONT,
		RENDER_END
	};
	enum BUFFERID { LUMINANCE,BUFFER_END};
private:
	explicit CRenderer();
	virtual ~CRenderer() = default;

public:
	// Get
	_uint				Get_RenderLstSize(const RENDERGROUP& eRenderGroup);
	_uint				Get_ColliderLstSize();
	const _uint&		Get_CntShaderFile()					{ return m_uiCnt_ShaderFile; }
	const _uint&		Get_CntPipelineState()				{ return m_uiCnt_PipelineState; }
	const _uint&		Get_CntSetPipelineState()			{ return m_uiCnt_SetPipelineState; }
	_bool				Get_RenderOnOff(wstring wstrTag);
	CRenderTarget*		Get_TargetShadowDepth()				{ return m_pTargetShadowDepth; }
	BoundingFrustum&	Get_Frustum()						{ return m_tFrustum; } 

	// Set
	HRESULT	Set_CurPipelineState(ID3D12PipelineState* pPipelineState);
	HRESULT	Set_CurPipelineState(ID3D12GraphicsCommandList* pCommandList, 
								 ID3D12PipelineState* pPipelineState,
								 const _int& iContextIdx);
	HRESULT	Set_RenderOnOff(wstring wstrTag);
	void	Add_PipelineStateCnt()							{ ++m_uiCnt_PipelineState; };
	void	Reset_SetPipelineStateCnt()						{ m_uiCnt_SetPipelineState = 0; }
	void	Set_bIsLoadingFinish()							{ m_bIsLoadingFinish = true; }
	void	Set_Frustum(const BoundingFrustum& tFrustum)	{ m_tFrustum = tFrustum; }

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
	void	Render_Luminance();
	void	Render_NPathDir();
	void	Render_DownSampling();
	void	Render_Blur();
	void	Render_SSAO();
	void	Render_Blend();
	void	Render_EdgeObject(const _float& fTimeDelta);
	void	Render_Edge();
	void	Render_Distortion(const _float& fTimeDelta);
	void	Render_CrossFilter(const _float& fTimeDelta);
	void	Render_Alpha(const _float& fTimeDelta);
	void	Render_AddEffect();
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
	BoundingFrustum			m_tFrustum;

	/*__________________________________________________________________________________________________________
	[ Multi RenderTarget ]
	____________________________________________________________________________________________________________*/
	// Render Target - 타겟추가후 꼭 릴리즈
	CRenderTarget*	m_pTargetDeferred	  = nullptr; // 디퍼드		-> Target5
	CRenderTarget*	m_pTargetLight		  = nullptr; // 라이트		-> Target2
	CRenderTarget*	m_pTargetShadowDepth  = nullptr; // 셰도우		-> Target1
	CRenderTarget*	m_pTargetDownSampling = nullptr; // 다운샘플		-> Target2
	CRenderTarget*	m_pTargetBlur         = nullptr; // 블러			-> Target3
	CRenderTarget*	m_pTargetSSAO         = nullptr; // SSAO		-> Target1
	CRenderTarget* m_pTargetDistortion    = nullptr; // Distortion
	CRenderTarget* m_pTargetBlend = nullptr; // Distortion
	CRenderTarget* m_pTargetCrossFilter = nullptr; // CrossFilter

	CRenderTarget* m_pTargetEdgeObject = nullptr; // EdgeObject

	// Edge
	CRenderTarget* m_pTargetEdge = nullptr;
	CShaderEdge* m_pEdgeShader = nullptr;
	CScreenTex* m_pEdgeBuffer = nullptr;
	_bool m_bIsSetEdgeTexture{ false };
	// N_PathBlurTarget
	CRenderTarget * m_pTargetNPathDir = nullptr; // n방향 타겟
	CShaderNPathDir* m_pNPathDirShader = nullptr;
	CScreenTex* m_pNPathDirBuffer = nullptr;
	//SUNSHINE
	CScreenTex* m_pSunShineBuffer = nullptr;
	CShaderNPathDir* m_pSunShineShader = nullptr;
	CRenderTarget* m_pTargetSunShine = nullptr; // n방향 타겟

	_bool m_bisSetNPathDirTexture		{false};
	// Blend
	CScreenTex*		m_pBlendBuffer					= nullptr;
	CShaderBlend*	m_pBlendShader					= nullptr;
	_bool			m_bIsSetBlendTexture			{ false };

	CShaderBlend* m_pHDRShader = nullptr;
	//Luminance

	CRenderTarget* m_pTargetLuminance[6]; // 루미넌스		-> Target6
	CScreenTex*			m_pLuminanceBuffer			= nullptr;
	CShaderLuminance*   m_pLuminanceShader[6]			;
	_bool				m_bIsSetLuminanceTexture	{ false };

	//DownSampleTarget
	CScreenTex*			 m_pDownSamplingBuffer		= nullptr;
	CShaderDownSampling* m_pDownSamplingShader		= nullptr;
	_bool				 m_bIsSetDownSamplingTexture{ false };

	//Luminance
	CScreenTex* m_pBlurBuffer = nullptr;
	CShaderBlur* m_pBlurShader = nullptr;
	_bool				m_bIsSetBlurTexture{ false };

	//SSAO
	CScreenTex* m_pSSAOBuffer = nullptr;
	CShaderSSAO* m_pSSAOShader = nullptr;
	_bool				m_bIsSetSSAOTexture{ false };

	//Effect (Alpha)
	CRenderTarget* m_pTargetpEffect = nullptr;
	CScreenTex* m_pEffectBuffer = nullptr;
	//Effect (Add)
	_bool m_bisSetAddEffectTexture{ false };
	CRenderTarget* m_pTargetAddEffect = nullptr;
	CScreenTex* m_pAddEffectBuffer = nullptr;
	CShaderAddEffect* m_pAddEffectShader = nullptr;
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
	HANDLE m_hWorkerBeginRender[CONTEXT::CONTEXT_END];				// 각 쓰레드에게 Render시작을 알림.
	HANDLE m_hWorkerFinishShadow[CONTEXT::CONTEXT_END];				// Render ShadowDepth.
	HANDLE m_hWorkerFinishedRenderPriority[CONTEXT::CONTEXT_END];	// Render Priority.
	HANDLE m_hWorkerFinishedRenderNoneAlpha[CONTEXT::CONTEXT_END];	// Render NoneAlpha.
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