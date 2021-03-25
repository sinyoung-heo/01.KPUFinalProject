#include "Scene.h"

USING(Engine)

CScene::CScene(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: m_pGraphicDevice(pGraphicDevice)
	, m_pCommandList(pCommandList)
	, m_pObjectMgr(CObjectMgr::Get_Instance())
	, m_pRenderer(CRenderer::Get_Instance())
{
}


HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

void CScene::Process_PacketFromServer()
{
}

_int CScene::Update_Scene(const _float & fTimeDelta)
{
	NULL_CHECK_RETURN(m_pObjectMgr, -1);

	return m_pObjectMgr->Update_ObjectMgr(fTimeDelta);
}

_int CScene::LateUpdate_Scene(const _float & fTimeDelta)
{
	NULL_CHECK_RETURN(m_pObjectMgr, -1);

	return m_pObjectMgr->LateUpdate_ObjectMgr(fTimeDelta);
}

void CScene::Send_PacketToServer()
{
	m_pObjectMgr->Send_PacketToServer();
}

HRESULT CScene::Render_Scene(const _float & fTimeDelta, const RENDERID& eID)
{
	/*__________________________________________________________________________________________________________
	[ Renderer를 통해서 렌더링 ]
	____________________________________________________________________________________________________________*/
	if (nullptr == m_pRenderer)
		return E_FAIL;

	FAILED_CHECK_RETURN(m_pRenderer->Render_Renderer(fTimeDelta, eID), E_FAIL);

	return S_OK;
}

void CScene::Free()
{
	m_pRenderer->Clear_RenderGroup();
}
