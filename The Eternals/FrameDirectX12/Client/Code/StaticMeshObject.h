#pragma once
#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMesh;
	class CMesh;
}

class CStaticMeshObject : public Engine::CGameObject
{
private:
	explicit CStaticMeshObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CStaticMeshObject() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,
									 const _bool& bIsRenderShadow,
									 const _bool& bIsCollision,
									 const _vec3& vBoundingSphereScale,
									 const _vec3& vBoundingSpherePos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

	// SingleThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta);
	virtual void	Render_EdgeGameObject(const _float& fTimeDelta);
	virtual void	Render_CrossFilterGameObject(const _float& fTimeDelta);
	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

	void			Set_RenderGroup(Engine::CRenderer::RENDERGROUP eGroup) { m_eRenderGroup = eGroup; }
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable(const _int& iContextIdx, const _int& iInstanceIdx);
	void			Set_ConstantTable();

	void			Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx);


private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderShadowInstancing*	m_pShaderShadowInstancing = nullptr;
	Engine::CShaderMeshInstancing*		m_pShaderMeshInstancing   = nullptr;

	Engine::CShaderMesh*				m_pShaderCom            = nullptr;
	Engine::CShaderMesh*				m_pCrossFilterShaderCom = nullptr;
	Engine::CShaderMesh*				m_pEdgeObjectShaderCom  = nullptr;

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";
	_uint			m_iMeshPipelineStatePass   = 0;
	_uint			m_iShadowPipelineStatePass = 0;

	int m_iRandomnumber = 0;
	Engine::CRenderer::RENDERGROUP m_eRenderGroup;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag, 
									   const _vec3 & vScale, 
									   const _vec3 & vAngle, 
									   const _vec3 & vPos,
									   const _bool& bIsRenderShadow,
									   const _bool& bIsCollision,
									   const _vec3& vBoundingSphereScale,
									   const _vec3& vBoundingSpherePos);
private:
	virtual void Free();
};

