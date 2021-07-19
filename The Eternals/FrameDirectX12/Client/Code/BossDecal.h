#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CBossDecal : public Engine::CGameObject
{
private:
	explicit CBossDecal(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CBossDecal() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);


	virtual void	Render_GameObject(const _float& fTimeDelta);
	void Set_TexIdx(_uint Diff, _uint Norm, _uint Spec)
	{
		m_uiDiffuse = Diff, m_uiNorm = Norm, m_uiSpec = Spec;
	}
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

	
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;

	Engine::CShaderMeshEffect* m_pShaderCom = nullptr;
	Engine::CShaderMesh* m_pCrossFilterShaderCom = nullptr;
	Engine::CShaderMesh* m_pEdgeObjectShaderCom = nullptr;

	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";
	_uint			m_iMeshPipelineStatePass   = 0;
	_uint			m_iShadowPipelineStatePass = 0;

	_uint m_uiDiffuse = 0;
	_uint m_uiNorm= 0;
	_uint m_uiSpec=0;
	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDelta2Velocity = 1.f;
	float m_fDeltatime3 = 0.f;
	float m_fAlpha = 1.f;

public:
	void Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos);


	static CBossDecal** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

