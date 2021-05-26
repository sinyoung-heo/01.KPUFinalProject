#pragma once
#include "PCWeapon.h"

class CPCWeaponRod : public CPCWeapon
{
private:
	explicit CPCWeaponRod(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCWeaponRod() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 Engine::HIERARCHY_DESC* pHierarchyDesc,
									 _matrix* pParentMatrix,
									 const _rgba& vEmissiveColor);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:

public:
	static CPCWeaponRod* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
								wstring wstrMeshTag, 
								const _vec3 & vScale, 
								const _vec3 & vAngle, 
								const _vec3 & vPos,
								Engine::HIERARCHY_DESC* pHierarchyDesc,
								_matrix* pParentMatrix,
								const _rgba& vEmissiveColor);

	static CPCWeaponRod** Create_InstancePool(ID3D12Device* pGraphicDevice,
											  ID3D12GraphicsCommandList* pCommandList, 
											  wstring wstrMeshTag,
											  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

