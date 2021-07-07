#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshEffect;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CMagicCircle : public Engine::CGameObject
{
private:
	explicit CMagicCircle(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CMagicCircle() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos, const int& iPipeLineIdx = 0);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);


	virtual void	Render_GameObject(const _float& fTimeDelta);
	void Set_isScaleAnim(bool isScaling) { m_bisScaleAnim = isScaling; }
	void Set_isRotate(bool isRotate) { m_bisRotate = isRotate; }
	void Set_TexIDX(_uint Diffuse, _uint Normal, _uint Specular) { m_uiDiffuse = Diffuse, m_uiNormal = Normal, m_uiSpec = Specular; }
private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();

	HRESULT SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth);

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
	_uint m_uiPipeLineIdx = 0;
	_uint m_uiDiffuse = 0;
	_uint m_uiNormal=0;
	_uint m_uiSpec=0;
	_float  m_fDeltatime = 0.f;
	_float  m_fDeltatime2 = 0.f;
	_float  m_fDeltatime3 = 0.f;
	_float m_fLifeTime = 0.f;
	_bool m_bisRotate = false;
	_bool m_bisScaleAnim = false;
	_bool m_bisFollowPlayer = false;

	_float m_fAlpha = 0.f;
	_float m_fDegree = 0.f;

	const Engine::CTransform* m_pParentTransform = nullptr;
public:
	void Set_CreateInfo(const _vec3& vScale, const _vec3& vAngle, const _vec3& vPos, bool bisRotate = false,bool bisScaleAnim=false
		, const Engine::CTransform* ParentTransform=nullptr,bool bisFollowPlayer=false);
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrMeshTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos,
		const int& iPipeLineIdx = 0);

	static CMagicCircle** Create_InstancePool(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		const _uint& uiInstanceCnt);
private:
	virtual void Free();
};

