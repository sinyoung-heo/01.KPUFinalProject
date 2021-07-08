#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CGridShieldEffect : public Engine::CGameObject
{
private:
	explicit CGridShieldEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CGridShieldEffect() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos, const int& iPipeLineIdx=2);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);
	void Set_TexIDX(_uint Diffuse, _uint Normal, _uint Specular) { m_uiDiffuse = Diffuse, m_uiNormal = Normal, m_uiSpec = Specular; }
	virtual void Render_GameObject(const _float& fTimeDelta);
	void Set_isScaleAnim(bool isScaling) { m_bisScaleAnim = isScaling; }
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderMeshEffect* m_pShaderCom = nullptr;
	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";

	_uint m_uiDiffuse = 0;
	_uint m_uiNormal = 0;
	_uint m_uiSpec = 0;
	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDeltatime3 = 0.f;
	float m_fDegree = 0.f;
	float m_fLifeTime = 0.f;
	int   m_iPipeLineIdx = 0;
	bool  m_bisScaleAnim = false;
	bool  m_bisFollowPlayer = false;

	const Engine::CTransform* m_pParentTransform = nullptr;
public:
	void Set_CreateInfo(const _vec3& vScale,const _vec3& vAngle,const _vec3& vPos,_int Diff,_int Norm,_int Spec,
		const int& iPipeLineIdx = 2, bool bisScaleAnim=true,
		bool bisFollow=true,const Engine::CTransform* ParentTransform=nullptr);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos, const int& iPipeLineIdx=2);


	static CGridShieldEffect** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

