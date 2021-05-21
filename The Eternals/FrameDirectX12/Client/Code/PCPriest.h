#pragma once
#include "Include.h"
#include "GameObject.h"
#include "ArcherAnimation.h"

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
class CInstancePoolMgr;

class CPCPriest : public Engine::CGameObject
{
private:
	explicit CPCPriest(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPCPriest() = default;


public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos,
									   const char& chWeaponType);
private:
	virtual void Free();
};

