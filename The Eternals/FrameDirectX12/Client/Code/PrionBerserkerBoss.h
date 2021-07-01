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

class CPrionBerserkerBoss : public Engine::CGameObject
{
private:
	explicit CPrionBerserkerBoss(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPrionBerserkerBoss() = default;

public:
	// CGameObject��(��) ���� ��ӵ�
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
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	void			Set_ConstantTableMiniMap();
	void			SetUp_AngleInterpolation(const _float& fTimeDelta);
	void			SetUp_Dissolve(const _float& fTimeDelta);
	void			Active_Monster(const _float& fTimeDelta);
	void			Change_Animation(const _float& fTimeDelta);
	void			SetUp_CollisionTick(const _float& fTimeDelta);

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

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx			= 0;	// ���� �ִϸ��̼� Index
	_uint	m_ui3DMax_NumFrame	= 0;	// 3DMax���� �ִϸ��̼��� �� Frame ����
	_uint	m_ui3DMax_CurFrame	= 0;	// 3DMAx���� ���� �ִϸ��̼��� Frame ��ġ
	
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);

	static CPrionBerserkerBoss** Create_InstancePool(ID3D12Device* pGraphicDevice,
										  ID3D12GraphicsCommandList* pCommandList, 
										  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

