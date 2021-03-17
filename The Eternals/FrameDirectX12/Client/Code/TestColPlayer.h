#pragma once
#include "GameObject.h"

namespace Engine
{
	class CShaderColorInstancing;
	class CColliderSphere;
	class CColliderBox;
}

//enum DIRECTION	{ DIR_X, DIR_Y, DIR_Z, DIR_STOP };
//enum MOVE		{ MOVE_MINUS, MOVE_PLUS };

const float OFFSET_MAX = 256.0f;
const float	OFFSET_MIN = 0.0f;

class CDynamicCamera;

class CTestColPlayer final : public Engine::CGameObject
{
private:
	explicit CTestColPlayer(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTestColPlayer(const CTestColPlayer& rhs);
	virtual ~CTestColPlayer() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(const _vec3 & vScale,
									 const _vec3 & vAngle,
									 const _vec3 & vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Process_Collision();
	virtual void	Render_GameObject(const _float& fTimeDelta);

private:
	virtual HRESULT Add_Component();

	void			Key_Input(const _float& fTimeDelta);
	void			Send_Player_Move();
	bool			Is_Change_CamDirection();

	void			Attack(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CShaderColorInstancing* m_pShaderColorInstancing = nullptr;
	Engine::CColliderSphere*		m_pColliderSphereCom     = nullptr;
	Engine::CColliderBox*			m_pColliderBoxCom        = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	_float			m_fMove = 1.0f;
	_float			m_fAttackTime = 0.f;

	CDynamicCamera* m_pDynamicCamera = nullptr;
	/* Server */
	bool			m_bIsKeyDown = false;
	bool			m_bIsSameDir = false;
	bool			m_bIsAttack = false;
	float			m_fBazierSpeed = 0.f;
	MVKEY			m_eKeyState = MVKEY::K_END;
	float			m_vecPreAngle = 0.f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vAngle,
									   const _vec3 & vPos);
private:
	virtual void Free();

};

