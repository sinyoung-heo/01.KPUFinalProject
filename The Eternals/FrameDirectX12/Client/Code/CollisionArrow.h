#pragma once
#include "CollisionTick.h"

namespace Engine
{
	class CMesh;
}

class CCollisionArrow : public CCollisionTick
{
private:
	explicit CCollisionArrow(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCollisionArrow() = default;

public:
	wstring Get_MeshTag() { return m_wstrMeshTag; }
	void Set_ArrowType(const ARROW_TYPE& eType) { m_eType = eType; }

	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 const ARROW_TYPE& eType,
									 const _uint& uiDamage,
									 const _float& fLifeTime);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderMeshInstancing*		m_pShaderMeshInstancing   = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring		m_wstrMeshTag		       = L"";
	_uint		m_iMeshPipelineStatePass   = 0;
	_uint		m_iShadowPipelineStatePass = 0;

	ARROW_TYPE	m_eType = ARROW_TYPE::ARROW_TYPE_END;



public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos,
									   const ARROW_TYPE& eType,
									   const _uint& uiDamage,
									   const _float& fLifeTime);

	static CCollisionArrow**	Create_InstancePool(ID3D12Device* pGraphicDevice,
													ID3D12GraphicsCommandList* pCommandList,
													wstring wstrMeshTag,
													const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

