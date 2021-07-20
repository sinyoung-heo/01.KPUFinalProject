#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CWarningGround : public Engine::CGameObject
{
private:
	explicit CWarningGround(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CWarningGround() = default; 

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
	void Set_isScaleAnim(bool isScaling) { m_bisScaleAnim = isScaling; }
	void Set_TexIDX(_uint Diffuse, _uint Normal, _uint Specular) { m_uiDiffuse = Diffuse, m_uiNormal = Normal, m_uiSpec = Specular; }
	void Set_LimitInfo(_float LimitScale, _float LimitLifeTime) { m_fLimitScale = LimitScale, m_fLimitLifeTime = LimitLifeTime; }
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
	Engine::CShaderMesh* m_pEdgeObjectShaderCom = nullptr;

	wstring			m_wstrMeshTag		       = L"";
	_uint			m_iMeshPipelineStatePass   = 0;
	_uint			m_iShadowPipelineStatePass = 0;

	_uint m_uiDiffuse = 0;
	_uint m_uiNormal=0;
	_uint m_uiSpec=0;
	_float  m_fDeltatime = 0.f;
	_float  m_fDeltatime2 = 0.f;
	_float  m_fDeltatime3 = 0.f;
	_float m_fLifeTime = 0.f;
	
	_bool m_bisScaleAnim = false;

	_float m_fAlpha = 0.f;
	_float m_fDegree = 0.f;

	_float   m_fLimitLifeTime = 0.f;
	_float   m_fLimitScale = 0.f;
	_bool m_bisDragonEffect = false;
	ID3D12DescriptorHeap* m_pTextureHeap = nullptr;
public:
	void Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, bool bisScaleAnim = false, _float LimitScale = 1.f
		, _bool DragonEffect = false);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrMeshTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos);

	static CWarningGround** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

