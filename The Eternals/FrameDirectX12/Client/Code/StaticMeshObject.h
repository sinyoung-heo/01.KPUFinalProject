#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
}

class CDynamicCamera;

class CStaticMeshObject : public Engine::CGameObject
{
private:
	explicit CStaticMeshObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CStaticMeshObject(const CStaticMeshObject& rhs);
	virtual ~CStaticMeshObject() = default; 

public:
	// CGameObject을(를) 통해 상속됨
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

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*			m_pMeshCom		= nullptr;
	Engine::CShaderMesh*	m_pShaderCom	= nullptr;
	Engine::CShaderShadow*	m_pShadowCom	= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag			= L"";
	CDynamicCamera*	m_pDynamicCamera		= nullptr;

public:
	static CStaticMeshObject* Create(ID3D12Device* pGraphicDevice, 
									 ID3D12GraphicsCommandList* pCommandList,
									 wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos);
private:
	virtual void Free();
};

