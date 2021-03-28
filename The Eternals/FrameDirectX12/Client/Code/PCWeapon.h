#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadowDepth;
}

class CPCWeapon : public Engine::CGameObject
{
protected:
	explicit CPCWeapon(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCWeapon() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 Engine::HIERARCHY_DESC* pHierarchyDesc,
									 _matrix* pParentMatrix);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	void			Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx);
	void			Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx);

protected:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderMesh*				m_pShaderCom              = nullptr;
	Engine::CShaderShadow*				m_pShadowCom              = nullptr;
	Engine::CShaderShadowInstancing*	m_pShaderShadowInstancing = nullptr;
	Engine::CShaderMeshInstancing*		m_pShaderMeshInstancing   = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_uint	m_iAttackPower             = 0;
	wstring	m_wstrMeshTag		       = L"";
	_uint	m_iMeshPipelineStatePass   = 0;
	_uint	m_iShadowPipelineStatePass = 0;

	Engine::HIERARCHY_DESC*	m_pHierarchyDesc = nullptr;
	_matrix*				m_pParentMatrix  = nullptr;


protected:
	virtual void Free();
};

