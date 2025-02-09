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

class CNPC_Stander : public Engine::CGameObject
{
private:
	explicit CNPC_Stander(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CNPC_Stander() = default;

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

	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag, wstring wstrNaviMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	void			SetUp_AngleInterpolation(const _float& fTimeDelta);

	void			Change_Animation(const _float& fTimeDelta);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh* m_pMeshCom = nullptr;
	Engine::CShaderMesh* m_pShaderCom = nullptr;
	Engine::CShaderShadow* m_pShadowCom = nullptr;
	Engine::CColliderSphere* m_pColliderSphereCom = nullptr;
	Engine::CColliderBox* m_pColliderBoxCom = nullptr;
	Engine::CNaviMesh* m_pNaviMeshCom = nullptr;

	/*__________________________________________________________________________________________________________
	[ Manager ]
	____________________________________________________________________________________________________________*/
	CPacketMgr* m_pPacketMgr = nullptr;
	CServerMath* m_pServerMath = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrMeshTag = L"";

	/*__________________________________________________________________________________________________________
	[ Animation Frame ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiAnimIdx = 0;	// 현재 애니메이션 Index
	_uint	m_ui3DMax_NumFrame = 0;	// 3DMax에서 애니메이션의 총 Frame 개수
	_uint	m_ui3DMax_CurFrame = 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치

public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   wstring wstrNaviMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);

	static CNPC_Stander** Create_InstancePool(ID3D12Device* pGraphicDevice,
											  ID3D12GraphicsCommandList* pCommandList, 
											  wstring wstrMeshTag,
											  const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

