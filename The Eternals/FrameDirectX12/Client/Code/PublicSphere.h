#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CPublicSphere : public Engine::CGameObject
{
private:
	explicit CPublicSphere(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CPublicSphere() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos, const _vec3& vPosOffset);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

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
	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";
	
	_uint m_uiDiffuse  = 0;
	_uint m_uiNormal   = 0;
	_uint m_uiSpec     = 0;
	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDeltatime3 = 0.f;
	float m_fDegree = 0.f;

	//Temp
	float   fFrameCnt = 8.f;
	float	fCurFrame = 0.f;
	float	fSceneCnt = 2.f;
	float	fCurScene = 0.f;
	float m_fLifeTime = 0.f;
	bool m_bisScaleAnim = false;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
									   ID3D12GraphicsCommandList* pCommandList,
									   wstring wstrMeshTag,
									   const _vec3& vScale,
									   const _vec3& vAngle,
									   const _vec3& vPos, const _vec3& vPosOffset);
private:
	virtual void Free();
};

