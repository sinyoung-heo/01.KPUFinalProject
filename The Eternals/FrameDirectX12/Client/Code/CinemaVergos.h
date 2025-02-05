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

class CCinemaVergos : public Engine::CGameObject
{
private:
	explicit CCinemaVergos(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CCinemaVergos() = default;

public:
	Engine::CMesh* Get_MeshComponent() { return m_pMeshCom; }
	const _uint& Get_MaxAnimationFrame() { return m_ui3DMax_NumFrame; }
	const _uint& Get_CurAnimationFrame() { return m_ui3DMax_CurFrame; }
	void Set_IsUpdate(const _bool& bIsUpdate) { m_bIsUpdate = bIsUpdate; }
	void Set_MonsterState(const _uint& iState) { m_iMonsterStatus = iState; }
	void Set_CurAnimationFrame(const _uint& uiFrame)	{ m_ui3DMax_CurFrame = uiFrame; }
	void Set_IsCameraShaking(const _bool& bIsShaking)	{ m_bIsCameraShaking = bIsShaking; }
	void Reset_SoundValue();

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
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	void Set_ConstantTable();
	void Set_ConstantTableShadowDepth();
	void Set_ConstantTableMiniMap();
	void SetUp_AngleInterpolation(const _float& fTimeDelta);
	void Active_Monster(const _float& fTimeDelta);
	void Change_Animation(const _float& fTimeDelta);
	void SetUp_PlaySound();
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*				m_pMeshCom				= nullptr;
	Engine::CShaderMesh*		m_pShaderCom			= nullptr;
	Engine::CShaderShadow*		m_pShadowCom			= nullptr;
	Engine::CColliderSphere*	m_pColliderSphereCom	= nullptr;
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
	_bool	m_bIsUpdate              = false;
	_bool	m_bIsCameraShaking       = false;

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx			= 0;	// 현재 애니메이션 Index
	_uint	m_ui3DMax_NumFrame	= 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint	m_ui3DMax_CurFrame	= 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치
	
	// Sound
	_bool m_bIsPlaySoundSpawn          = false;
	_bool m_bIsPlaySoundSpawnFly01     = false;
	_bool m_bIsPlaySoundSpawnFly02     = false;
	_bool m_bIsPlaySoundSpawnFly03     = false;
	_bool m_bIsPlaySoundSpawnFlyEnd    = false;
	_bool m_bIsPlaySoundSpawnCrush01   = false;
	_bool m_bIsPlaySoundSpawnCrush02   = false;
	_bool m_bIsPlaySoundSpawnCrush03   = false;
	_bool m_bIsPlaySoundSpawnRoar      = false;
	_bool m_bIsPlaySoundSpawnAwayFly01 = false;
	_bool m_bIsPlaySoundSpawnAwayFly02 = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);

	static CCinemaVergos** Create_InstancePool(ID3D12Device* pGraphicDevice,
										  ID3D12GraphicsCommandList* pCommandList, 
										  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

