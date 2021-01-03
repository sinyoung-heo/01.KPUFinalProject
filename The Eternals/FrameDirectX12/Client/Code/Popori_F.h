#pragma once

#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CColliderSphere;
	class CColliderBox;
	class CFont;
}

class CDynamicCamera;

class CPopori_F : public Engine::CGameObject
{
private:
	explicit CPopori_F(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CPopori_F(const CPopori_F& rhs);
	virtual ~CPopori_F() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float & fTimeDelta);

	// 2020.06.11 MultiThreadRendering
	virtual void	Render_GameObject(ID3D12GraphicsCommandList* pCommandList, 
									  const _int& iContextIdx);
	virtual void	Render_ShadowDepth(ID3D12GraphicsCommandList* pCommandList, 
									   const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

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

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	CDynamicCamera*	m_pDynamicCamera					= nullptr;
	wstring			m_wstrMeshTag						= L"";

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint			m_uiAnimIdx							= 0;	// ���� �ִϸ��̼� Index
	_uint			m_ui3DMax_NumFrame					= 0;	// 3DMax���� �ִϸ��̼��� �� Frame ����
	_uint			m_ui3DMax_CurFrame					= 0;	// 3DMAx���� ���� �ִϸ��̼��� Frame ��ġ

	/*__________________________________________________________________________________________________________
	[ Font ]
	____________________________________________________________________________________________________________*/
	Engine::CFont*	m_pFont								= nullptr;
	wstring			m_wstrText							= L"";
	_tchar			m_szText[MAX_STR]					= L"";

public:
	static CPopori_F* Create(ID3D12Device* pGraphicDevice,
							 ID3D12GraphicsCommandList* pCommandList,
							 wstring wstrMeshTag,
							 const _vec3& vScale,
							 const _vec3& vAngle,
							 const _vec3& vPos);
private:
	virtual void Free();
};

