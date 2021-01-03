#pragma once
#include "Engine_Include.h"
#include "Base.h"
#include "Layer.h"

BEGIN(Engine)

class ENGINE_DLL CObjectMgr final : public CBase
{
	DECLARE_SINGLETON(CObjectMgr)

private:
	explicit CObjectMgr();
	virtual ~CObjectMgr() = default;

public:
	// Get
	CLayer*			Get_Layer(wstring wstrLayerTag);
	OBJLIST*		Get_OBJLIST(wstring wstrLayerTag, wstring wstrObjTag);
	CGameObject*	Get_GameObject(wstring wstrLayerTag, wstring wstrObjTag, _int iIdx = 0);

	// Prototype
	HRESULT			Add_GameObjectPrototype(wstring wstrPrototypeTag, CGameObject* pGameObject);
	CGameObject*	Clone_GameObjectPrototype(wstring wstrPrototypeTag);

	// Add
	void			Add_Layer(wstring wstrLayerTag, CLayer* pLayer) { m_mapLayer.emplace(wstrLayerTag, pLayer); }
	HRESULT			Add_GameObject(wstring wstrLayerTag, wstring wstrObjTag, CGameObject* pGameObject);
	
	//  Method
	_int			Update_ObjectMgr(const _float& fTimeDelta);
	_int			LateUpdate_ObjectMgr(const _float& fTimeDelta);
	void			Render_ObjectMgr(const _float& fTimeDelta);

	// Delete
	HRESULT			Delete_GameObject(wstring wstrLayerTag, wstring wstrObjTag, _int iIdx = 0);
	HRESULT			Clear_OBJLIST(wstring wstrLayerTag, wstring wstrObjTag);
	void			Clear_Layer();
	void			Clear_Prototype();

private:
	map<wstring, CLayer*>		m_mapLayer;
	map<wstring, CGameObject*>	m_mapObjectPrototype;

private:
	virtual void Free();
};

END