#include "stdafx.h"
#include "PCArcher.h"

CPCArcher::CPCArcher(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: Engine::CGameObject(pGraphicDevice, pCommandList)
{
}

Engine::CGameObject* CPCArcher::Create(ID3D12Device* pGraphicDevice, 
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

void CPCArcher::Free()
{
	Engine::CGameObject::Free();
}
