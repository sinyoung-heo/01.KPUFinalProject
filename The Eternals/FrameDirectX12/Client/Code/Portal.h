#pragma once
#include "Include.h"
#include "GameObject.h"

class CPortal : public Engine::CGameObject
{
private:
	explicit CPortal(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPortal() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(wstring wstrCollisionTag, const _vec3& vScale, const _vec3& vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrCollisionTag, 
									   const _vec3& vScale, 
									   const _vec3& vPos);
private:
	virtual void Free();
};

