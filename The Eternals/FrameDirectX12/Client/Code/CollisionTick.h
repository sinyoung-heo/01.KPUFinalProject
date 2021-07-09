#pragma once
#include "Include.h"
#include "GameObject.h"

class CInstancePoolMgr;
class CDynamicCamera;

class CCollisionTick : public Engine::CGameObject
{
protected:
	explicit CCollisionTick(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCollisionTick() = default;

public:
	void Set_Damage(const _uint& uiDamage)		{ m_uiDamage = uiDamage; }
	void Set_LifeTime(const _float& fLifeTime)	{ m_fLifeTime = fLifeTime; }
	void Set_SkillAffect(const char& chAffect)	{ m_chAffect = chAffect; }

	// CGameObject��(��) ���� ��ӵ�
	virtual HRESULT	Ready_GameObject(wstring wstrCollisionTag,
									 const _vec3& vScale,
									 const _vec3& vPos,
									 const _uint& uiDamage,
									 const _float& fLifeTime);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
private:
	void SetUp_GladiatorCameraEvent();
	void SetUp_PriestCameraEvent();
protected:
	/*__________________________________________________________________________________________________________
	[ Manager ]
	____________________________________________________________________________________________________________*/
	CPacketMgr*			m_pPacketMgr       = nullptr;
	CInstancePoolMgr*	m_pInstancePoolMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	Engine::CGameObject*	m_pThisPlayer    = nullptr;
	CDynamicCamera*			m_pDynamicCamera = nullptr;

	char	m_chAffect		  = AFFECT_FINCH;
	_uint	m_uiDamage		  = 0;
	_float	m_fTimeDelta	  = 0.0f;
	_float	m_fLifeTime		  = 1.0f;
	_bool	m_bIsCameraEffect = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, 
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrCollisionTag,
									   const _vec3& vScale,
									   const _vec3& vPos,
									   const _uint& uiDamage,
									   const _float& fLifeTime);
	static CCollisionTick** Create_InstancePool(ID3D12Device* pGraphicDevice,
												ID3D12GraphicsCommandList* pCommandList,
												const _uint& uiInstanceCnt);
protected:
	virtual void Free();
};

