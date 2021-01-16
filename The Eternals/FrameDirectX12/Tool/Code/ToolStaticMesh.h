#pragma once
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CColliderSphere;
}

class CToolStaticMesh : public Engine::CGameObject
{
private:
	explicit CToolStaticMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CToolStaticMesh(const CToolStaticMesh& rhs);
	virtual ~CToolStaticMesh() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 const _bool& bIsRenderShadow,
									 const _bool& bIsBoundingBox		= true,
									 const _bool& bIsBoundingSphere		= false,
									 const _vec3& vBoundingSphereScale	= _vec3(1.0f),
									 const _vec3& vBoundingSpherePos	= _vec3(0.0f),
									 const _bool& bIsMousePicking = true);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

	// SingleThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta);

	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

public:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*				m_pMeshCom				= nullptr;
	Engine::CShaderMesh*		m_pShaderCom			= nullptr;
	Engine::CShaderShadow*		m_pShadowCom			= nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring m_wstrMeshTag		= L"";
	_bool	m_bIsMousePicking	= true;

public:
	static CToolStaticMesh* Create(ID3D12Device* pGraphicDevice,
									 ID3D12GraphicsCommandList* pCommandList,
									 wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 const _bool& bIsRenderShadow,
									 const _bool& bIsBoundingBox		= true,
									 const _bool& bIsBoundingSphere		= false,
									 const _vec3& vBoundingSphereScale	= _vec3(1.0f),
									 const _vec3& vBoundingSpherePos	= _vec3(0.0f),
									 const _bool& bIsMousePicking = true);
private:
	virtual void Free();
};

