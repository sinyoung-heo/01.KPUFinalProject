#pragma once
#include "Include.h"
#include "GameObject.h"
#include "GladiatorAnimation.h"

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

class CPCGladiator : public Engine::CGameObject
{
private:
	explicit CPCGladiator(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCGladiator() = default;

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

	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

	void			Key_Input(const _float& fTimeDelta);
	void			Send_Player_Move();
	bool			Is_Change_CamDirection();

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*				m_pMeshCom           = nullptr;
	Engine::CShaderMesh*		m_pShaderCom         = nullptr;
	Engine::CShaderShadow*		m_pShadowCom         = nullptr;
	Engine::CColliderSphere*	m_pColliderSphereCom = nullptr;
	Engine::CColliderBox*		m_pColliderBoxCom    = nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom       = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CDynamicCamera* m_pDynamicCamera = nullptr;
	wstring			m_wstrMeshTag    = L"";

	// Server
	_bool			m_bIsKeyDown   = false;
	_bool			m_bIsSameDir   = false;
	_float			m_fBazierSpeed = 0.f;
	MVKEY			m_eKeyState    = MVKEY::K_END;
	_float			m_vecPreAngle  = 0.f;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint			m_uiAnimIdx        = 0;	// 현재 애니메이션 Index
	_uint			m_ui3DMax_NumFrame = 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint			m_ui3DMax_CurFrame = 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont           = nullptr;
	wstring			m_wstrText        = L"";
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

