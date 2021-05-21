#include "stdafx.h"
#include "PCPriest.h"

CPCPriest::CPCPriest(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

Engine::CGameObject* CPCPriest::Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList, 
									   wstring wstrMeshTag, 
									   wstring wstrNaviMeshTag, 
									   const _vec3& vScale, 
									   const _vec3& vAngle, 
									   const _vec3& vPos, 
									   const char& chWeaponType)
{

	return nullptr;
}

void CPCPriest::Free()
{
	Engine::CGameObject::Free();
}
