#pragma once
#include "Include.h"
#include "Base.h"

class CLoading final : public Engine::CBase
{
public:
	enum LOADINGID 
	{ 
		LOADING_STAGE, 
		LOADING_END 
	};

private:
	explicit CLoading(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CLoading() = default;

public:
	CRITICAL_SECTION*	Get_Crt()					{ return &m_Crt; }
	const LOADINGID&	Get_LoadingID()		const	{ return m_eLoadingID; }
	const _bool&		Get_Finish()		const	{ return m_bIsFinish; }
	const _tchar*		Get_LoadingString()	const 	{ return m_szLoadingStr; }
	const _int&			Get_CurLoadingCnt() const	{ return m_iCurLoadingCnt; }

	HRESULT				Ready_Loading(LOADINGID eLoadingID);
	_uint				Loading_For_Stage();

private:
	HRESULT				Loading_MeshFromFilePath();
	HRESULT				Loading_TextureFromFilePath();

	HRESULT             Loading_StaticMesh();
	HRESULT				Loading_DynamicMesh();
	HRESULT             Loading_Texture();

	static unsigned int __stdcall Thread_Main(void* pArg);

private:
	/*__________________________________________________________________________________________________________
	[ GraphicDevice ]
	____________________________________________________________________________________________________________*/
	ID3D12Device*	            m_pGraphicDevice		= nullptr;
	ID3D12CommandQueue*			m_pCommandQueue			= nullptr;
	ID3D12CommandAllocator*		m_pCommandAllocator		= nullptr;
	ID3D12GraphicsCommandList*	m_pCommandList			= nullptr;

	/*__________________________________________________________________________________________________________
	[ Thread ]
	____________________________________________________________________________________________________________*/
	HANDLE			         	m_hThread				= NULL;
	CRITICAL_SECTION	        m_Crt					{ NULL };

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	LOADINGID			        m_eLoadingID			= LOADING_END;
	_bool				        m_bIsFinish				= false;
	_tchar						m_szLoadingStr[MAX_STR]	= L"";
	_int						m_iCurLoadingCnt		= 0;

public:
	static CLoading*	Create(ID3D12Device* pGraphicDevice,
							   ID3D12GraphicsCommandList* pCommandList, 
							   LOADINGID eLoadingID);
private:
	virtual void		Free(void);
};

