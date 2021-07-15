#pragma once

#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CShaderMesh;
	class CShaderShadow;
	class CColliderSphere;
	class CColliderBox;
	class CNaviMesh;
}

class CGameUIRoot;
class CVergosHpGauge;

class CVergos : public Engine::CGameObject
{
private:
	explicit CVergos(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CVergos() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 wstring wstrNaviMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Send_PacketToServer();
	virtual void	Render_MiniMap(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	HRESULT			Create_Collider();
	HRESULT			Create_HpGauge();
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	void			Set_ConstantTableMiniMap();
	void			SetUp_AngleInterpolation(const _float& fTimeDelta);
	void			SetUp_PositionInterpolation(const _float& fTimeDelta);
	void			SetUp_Dissolve(const _float& fTimeDelta);
	void			Active_Monster(const _float& fTimeDelta);
	void			Change_Animation(const _float& fTimeDelta);
	void			SetUp_CollisionTick(const _float& fTimeDelta);
	void			SetUp_HpGauge(const _float& fTimeDelta);

	void			EffectLoop(const _float& fTimeDelta);
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*				m_pMeshCom				= nullptr;
	Engine::CShaderMesh*		m_pShaderCom			= nullptr;
	Engine::CShaderShadow*		m_pShadowCom			= nullptr;
	Engine::CColliderBox*		m_pColliderBoxCom		= nullptr;
	Engine::CNaviMesh*			m_pNaviMeshCom			= nullptr;

	/*__________________________________________________________________________________________________________
	[ Manager ]
	____________________________________________________________________________________________________________*/
	CPacketMgr*			m_pPacketMgr       = nullptr;
	CServerMath*		m_pServerMath      = nullptr;
	CInstancePoolMgr*	m_pInstancePoolMgr = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrMeshTag            = L"";
	_bool	m_bIsResetNaviMesh       = false;

	_bool	m_bIsCreateCollisionTick = false;
	COLLISION_TICK_DESC m_tCollisionTickDesc;

	_bool	m_bIsStartDissolve       = false;
	_float	m_fDissolve              = -0.05f;
	_rgba	m_vEmissiveColor         = _rgba(1.0f, 0.0f, 0.0f, 1.0f);

	CGameUIRoot*	m_pHpGaugeRoot = nullptr;
	CVergosHpGauge* m_pHpGauge = nullptr;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx			= 0;	// 현재 애니메이션 Index
	_uint	m_ui3DMax_NumFrame	= 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint	m_ui3DMax_CurFrame	= 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치
	
	//Effect
	_bool m_bisDecalEffect = false;

	_bool m_bisWarningEffect = false;
	_bool m_bisBreathEffect = false;
	
	_float m_fSkillOffset = 0.f;
	_float m_bisBreathDelta = 0.f;

	_float m_fParticleTime = 0.f;
	_float m_fBreathTime = 0.f;
	_vec3 BreathPos;
	_vec3 BazierPos[3];

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);

	static CVergos** Create_InstancePool(ID3D12Device* pGraphicDevice,
											  ID3D12GraphicsCommandList* pCommandList, 
											  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

