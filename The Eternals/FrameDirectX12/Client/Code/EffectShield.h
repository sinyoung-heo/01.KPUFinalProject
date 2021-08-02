#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CEffectShield : public Engine::CGameObject
{
private:
	explicit CEffectShield(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CEffectShield() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos, const float& vAngleOffset);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_CrossFilterGameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(const _float& fTimeDelta);
	void Set_isScaleAnim(bool isScaling) { m_bisScaleAnim = isScaling; }
	void Set_TexIDX(_uint Diffuse, _uint Normal, _uint Specular) { m_uiDiffuse = Diffuse, m_uiNormal = Normal, m_uiSpec = Specular; }
	void Set_Target(Engine::CGameObject* Target) { m_pTarget = Target; }
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderMeshEffect* m_pShaderCom = nullptr;
	Engine::CShaderMesh* m_pCrossFilterShaderCom = nullptr;
	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";
	
	_uint m_uiDiffuse  = 0;
	_uint m_uiNormal   = 0;
	_uint m_uiSpec     = 0;
	float   m_fDeltatime = -1.f;
	float   m_fDeltatime2 = 0.f;
	float   m_fDeltatime3 = 0.f;
	float   m_fDegree = 0.f;
	float   m_fShaderDegree = 0.f;
	float   m_fLifeTime = 0.f;
	float   m_fLimitLifeTime = 0.f;
	float   m_fLimitScale = 0.f;
	float   m_fRadius = 0.f;
	bool    m_bisScaleAnim = false;
	float   m_fCrossDeltatime = 0.f;
	float   m_fCrossDeltatime2 = 0.f;
	float   m_fCrossDeltatime3 = 0.f;
	float   m_fAngleOffset = 0.f;
	bool    m_bisLifeInit = false;
	float  m_fDeltatimeVelocity = 0.f;
	float  m_fDeltatimeVelocity2 = 1.f;


	Engine::CGameObject* m_pTarget = nullptr;
	bool m_bisMini = false;
	_int m_iParticleCnt = 0;
	const Engine::CTransform* m_pParentTransform =nullptr;
public:
	void Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos,
		float fLimitLifeTime,float fLimitScale,float fRadius, const float& vAngleOffset
	,const Engine::CTransform * ParentTransform, bool isMini=false);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos, const float& vAngleOffset);

	static CEffectShield** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

