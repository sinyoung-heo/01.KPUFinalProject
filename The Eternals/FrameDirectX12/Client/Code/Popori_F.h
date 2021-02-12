#pragma once

#include "Include.h"
#include "GameObject.h"

/*____________________________________________________________________
Player Direction
______________________________________________________________________*/
#define	FRONT		0.f
#define	RIGHT		90.f
#define	RIGHT_UP	45.f
#define	RIGHT_DOWN	135.f
#define	LEFT		270.f
#define	LEFT_UP		315.f
#define	LEFT_DOWN	225.f
#define	BACK		180.f

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CColliderSphere;
	class CColliderBox;
	class CFont;
	class CNaviMesh;
}

class CDynamicCamera;

class CPopori_F : public Engine::CGameObject
{
private:
	explicit CPopori_F(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CPopori_F(const CPopori_F& rhs);
	virtual ~CPopori_F() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 wstring wstrNaviMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

	// SingleThread Rendering.
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float & fTimeDelta);

	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

	// Instancing Rendering
	virtual void	Render_GameObjectInstancing(const _float& fTimeDelta, const _int& iContextIdx);
	virtual void	Render_ShadowDepthInstancing(const _float& fTimeDelta, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

	// Instancing
	void			Set_ConstantTable(const _int& iContextIdx, const _int& iInstancingIdx);
	void			Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstancingIdx);

	void			Key_Input(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*				m_pMeshCom				= nullptr;
	Engine::CShaderMesh*		m_pShaderCom			= nullptr;
	Engine::CShaderShadow*		m_pShadowCom			= nullptr;
	Engine::CColliderSphere*	m_pColliderSphereCom	= nullptr;
	Engine::CColliderBox*		m_pColliderBoxCom		= nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom			= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag	           = L"";
	_uint			m_iMeshPipelineStatePass   = 0;
	_uint			m_iShadowPipelineStatePass = 0;
	CDynamicCamera*	m_pDynamicCamera	       = nullptr;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint			m_uiAnimIdx		   = 0;	// 현재 애니메이션 Index
	_uint			m_ui3DMax_NumFrame = 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint			m_ui3DMax_CurFrame = 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치

	
	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont			  = nullptr;
	wstring			m_wstrText		  = L"";
	_tchar			m_szText[MAX_STR] = L"";

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);
private:
	virtual void Free();
};

