#include "Management.h"
#include "Scene.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
{
}


HRESULT CManagement::SetUp_CurrentScene(CScene * pNewScene)
{
	CRenderer::Get_Instance()->Clear_RenderGroup();

	Engine::Safe_Release(m_pCurrentScene);
	m_pCurrentScene = pNewScene;

	return S_OK;
}

_int CManagement::Update_Management(const _float & fTimeDelta)
{
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->Update_Scene(fTimeDelta);

	return 0;
}

_int CManagement::LateUpdate_Management(const _float & fTimeDelta)
{
	if (m_pCurrentScene != nullptr)
		m_pCurrentScene->LateUpdate_Scene(fTimeDelta);

	return 0;
}

HRESULT CManagement::Render_Management(const _float & fTimeDelta, const RENDERID& eID)
{
	if (nullptr != m_pCurrentScene)
		FAILED_CHECK_RETURN(m_pCurrentScene->Render_Scene(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

void CManagement::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy Management");
#endif

	Engine::Safe_Release(m_pCurrentScene);
}
