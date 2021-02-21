#pragma once
#include "GameObject.h"

namespace Engine
{
	class CShaderColorInstancing;
	class CColliderSphere;
	class CColliderBox;
}

enum DIRECTION	{ DIR_X, DIR_Y, DIR_Z, DIR_STOP };
enum MOVE		{ MOVE_MINUS, MOVE_PLUS };

const float OFFSET_MAX = 256.0f;
const float	OFFSET_MIN = 0.0f;


class CTestCollisonObject final : public Engine::CGameObject
{
private:
	explicit CTestCollisonObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTestCollisonObject(const CTestCollisonObject& rhs);
	virtual ~CTestCollisonObject() = default;

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
	void			Move_Direction(const _float& fTimeDelta);

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
	DIRECTION	m_eDir  = DIR_STOP;
	_float		m_fMove = 1.0f;

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   const _vec3 & vScale,
									   const _vec3 & vAngle,
									   const _vec3 & vPos);
private:
	virtual void Free();

};

