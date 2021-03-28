#include "stdafx.h"
#include "PCWeaponTwoHand.h"

CPCWeaponTwoHand::CPCWeaponTwoHand(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CPCWeapon(pGraphicDevice, pCommandList)
{
}

HRESULT CPCWeaponTwoHand::Ready_GameObject(wstring wstrMeshTag, 
										   const _vec3& vScale, 
										   const _vec3& vAngle, 
										   const _vec3& vPos, 
										   Engine::HIERARCHY_DESC* pHierarchyDesc,
										   _matrix* pParentMatrix)
{
	Engine::FAILED_CHECK_RETURN(CPCWeapon::Ready_GameObject(wstrMeshTag,
															vScale, 
															vAngle, 
															vPos, 
															pHierarchyDesc, 
															pParentMatrix), E_FAIL);

	return S_OK;
}

HRESULT CPCWeaponTwoHand::LateInit_GameObject()
{
	Engine::FAILED_CHECK_RETURN(CPCWeapon::LateInit_GameObject(), E_FAIL);

	return S_OK;
}

_int CPCWeaponTwoHand::Update_GameObject(const _float& fTimeDelta)
{
	Engine::FAILED_CHECK_RETURN(Engine::CGameObject::LateInit_GameObject(), E_FAIL);

	return CPCWeapon::Update_GameObject(fTimeDelta);
}

_int CPCWeaponTwoHand::LateUpdate_GameObject(const _float& fTimeDelta)
{
	return CPCWeapon::LateUpdate_GameObject(fTimeDelta);;
}

void CPCWeaponTwoHand::Render_GameObject(const _float& fTimeDelta, 
										 ID3D12GraphicsCommandList* pCommandList, 
										 const _int& iContextIdx)
{
	CPCWeapon::Render_GameObject(fTimeDelta, pCommandList, iContextIdx);
}

void CPCWeaponTwoHand::Render_ShadowDepth(const _float& fTimeDelta, 
										  ID3D12GraphicsCommandList* pCommandList, 
										  const _int& iContextIdx)
{
	CPCWeapon::Render_ShadowDepth(fTimeDelta, pCommandList, iContextIdx);

}

CPCWeaponTwoHand* CPCWeaponTwoHand::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
										   wstring wstrMeshTag, 
										   const _vec3& vScale, 
										   const _vec3& vAngle, 
										   const _vec3& vPos, 
										   Engine::HIERARCHY_DESC* pHierarchyDesc,
										   _matrix* pParentMatrix)
{
	CPCWeaponTwoHand* pInstance = new CPCWeaponTwoHand(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObject(wstrMeshTag,
										   vScale,
										   vAngle,
										   vPos,
										   pHierarchyDesc,
										   pParentMatrix)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CPCWeaponTwoHand::Free()
{
	CPCWeapon::Free();
}
