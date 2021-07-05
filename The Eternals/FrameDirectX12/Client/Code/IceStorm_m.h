#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CIceStorm_m : public Engine::CGameObject
{
private:
	explicit CIceStorm_m(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CIceStorm_m() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos,const float& fMaxScale);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

	virtual void Render_GameObject(const _float& fTimeDelta);

	virtual void Render_CrossFilterGameObject(const _float& fTimeDelta);
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();

	HRESULT SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*			m_pMeshCom                = nullptr;
	Engine::CShaderMesh* m_pCrossFilterShaderCom = nullptr;
	Engine::CShaderMeshEffect* m_pShaderCom = nullptr;
	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";
	_uint m_uiDiffuse = 0;
	_uint m_uiNormal = 0;
	_uint m_uiSpec = 0;

	bool  m_bisScaleAnim = false;
	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDeltatime3 = 0.f;
	float m_fDegree = 0.f;
	float m_fAlpha = 1.f;
	float m_fShaderDegree = 0.f;
	float m_fLifeTime = 0.f;
	
	float m_fCrossDeltatime = 0.f;
	float m_fCrossDeltatime2 = 0.f;
	float m_fCrossDeltatime3 = 0.f;

	float m_fDeltatimeVelocity = 0.f;
	float m_fDeltatimeVelocity2 = 1.f;

	float m_fMaxScale = 0.f;
public:
	void Set_CreateInfo(const _vec3& vScale,const _vec3& vAngle,const _vec3& vPos, const float& fMaxScale);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos , const float& fMaxScale);

	static CIceStorm_m** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

