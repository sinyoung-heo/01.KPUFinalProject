#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMesh;
	class CMesh;
	class CTexture;
}

class CDynamicCamera;

class CWaterFall : public Engine::CGameObject
{
private:
	explicit CWaterFall(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CWaterFall() = default; 

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
									 const _vec3& vScale,
									 const _vec3& vAngle,
									 const _vec3& vPos, const _vec3& vPosOffset);
	virtual HRESULT	LateInit_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LateUpdate_GameObject(const _float& fTimeDelta);

	// MultiThread Rendering
	virtual void	Render_GameObject(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);
	virtual void	Render_ShadowDepth(const _float& fTimeDelta, ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);

private:
	virtual HRESULT Add_Component(wstring wstrMeshTag);
	void			Set_ConstantTable();
	void			Set_ConstantTableShadowDepth();
	// Instancing
	void			Set_ConstantTable(const _int& iContextIdx, const _int& iInstancingIdx);
	void			Set_ConstantTableShadowDepth(const _int& iContextIdx, const _int& iInstanceIdx);

	HRESULT SetUp_DescriptorHeap(vector<ComPtr<ID3D12Resource>> vecTexture, vector<ComPtr<ID3D12Resource>> vecShadowDepth);

private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh*						m_pMeshCom                = nullptr;
	Engine::CShaderShadowInstancing*	m_pShaderShadowInstancing = nullptr;
	Engine::CShaderMeshInstancing*		m_pShaderMeshInstancing   = nullptr;

	Engine::CShaderMesh* m_pShaderCom = nullptr;
	ID3D12DescriptorHeap* m_pDescriptorHeaps = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag		       = L"";
	_uint			m_iMeshPipelineStatePass   = 0;
	_uint			m_iShadowPipelineStatePass = 0;

	float m_fNormalMapDeltatime = 0.f;
	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDeltatime3 = 0.f;
	float m_fPatternMapDeltatime = 0.f;
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

