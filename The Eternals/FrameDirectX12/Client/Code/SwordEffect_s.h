#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CSwordEffect_s : public Engine::CGameObject
{
private:
	explicit CSwordEffect_s(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CSwordEffect_s() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
											 const _vec3 & vScale,
											 const _vec3 & vAngle, 
											 const _vec3 & vParentPos, const _vec3& vDir);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_CrossFilterGameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(const _float& fTimeDelta);
	void Set_isScaleAnim(bool isScaling) { m_bisScaleAnim = isScaling; }
	void Set_TexIDX(_uint Diffuse, _uint Normal, _uint Specular) { m_uiDiffuse = Diffuse, m_uiNormal = Normal, m_uiSpec = Specular; }
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
	bool  m_bisScaleAnim = false;
	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDeltatime3 = 0.f;
	float m_fDegree = 0.f;
	float m_fShaderDegree = 0.f;
	float m_fLifeTime = 0.f;
	float m_fCrossDeltatime = 0.f;
	float m_fCrossDeltatime2 = 0.f;
	float m_fCrossDeltatime3 = 0.f;
	float m_fDeltatimeVelocity = 0.f;
	float m_fDeltatimeVelocity2 = 1.f;
	float m_fDistance = 0.f;
	float m_fAlpha = 1.f;
	_vec3 m_vecParentPos = _vec3(0.f);

public:
	void Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, const _vec3& vDir);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
										wstring wstrMeshTag,
										const _vec3& vScale,
										const _vec3& vAngle,
										const _vec3& vParentPos, const _vec3& vDir);


	static CSwordEffect_s** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

