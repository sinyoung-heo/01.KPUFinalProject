#include "Include.h"
#include "GameObject.h"

namespace Engine
{
	class CShaderMeshTerrain;
	class CMesh;
}

class CDynamicCamera;

class CTerrainMeshObject : public Engine::CGameObject
{
private:
	explicit CTerrainMeshObject(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CTerrainMeshObject() = default;

public:
	// CGameObject을(를) 통해 상속됨
	virtual HRESULT	Ready_GameObject(wstring wstrMeshTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos);
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


private:
	/*__________________________________________________________________________________________________________
	[ Component ]
	____________________________________________________________________________________________________________*/
	Engine::CMesh* m_pMeshCom = nullptr;
	Engine::CShaderShadowInstancing* m_pShaderShadowInstancing = nullptr;
	Engine::CShaderMeshInstancing* m_pShaderMeshInstancing = nullptr;

	Engine::CShaderMeshTerrain* m_pShaderCom = nullptr;
	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	wstring			m_wstrMeshTag = L"";

	float m_fDeltatime = 0.f;
	float m_fDeltatime2 = 0.f;
	float m_fDeltatime3 = 0.f;
public:
	static Engine::CGameObject* Create(ID3D12Device* pGraphicDevice,
		ID3D12GraphicsCommandList* pCommandList,
		wstring wstrMeshTag,
		const _vec3& vScale,
		const _vec3& vAngle,
		const _vec3& vPos);
private:
	virtual void Free();
};

