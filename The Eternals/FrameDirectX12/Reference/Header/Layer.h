#pragma once
#include "GameObject.h"


BEGIN(Engine)

/*__________________________________________________________________________________________________________
[ GameObject List를 관리할 Layer 클래스 ]
____________________________________________________________________________________________________________*/
typedef list<CGameObject*>	OBJLIST;
typedef OBJLIST::iterator	OBJITER;

class ENGINE_DLL CLayer final : public CBase
{
public:
	explicit CLayer();
	virtual ~CLayer() = default;

public:
	// Get
	OBJLIST*		Get_OBJLIST(wstring wstrObjTag);
	CGameObject*	Get_GameObject(wstring wstrObjTag, _int iIdx = 0);
	CGameObject*	Get_ServerObject(wstring wstrObjTag, int num);

	// Add & Delete
	HRESULT			Add_GameObject(wstring wstrObjTag, CGameObject* pGameObject);
	HRESULT			Delete_GameObject(wstring wstrObjTag, _int iIdx = 0);
	HRESULT			Delete_ServerObject(wstring wstrObjTag, int num);
	HRESULT			Clear_OBJLIST(wstring wstrObjTag);

	// Method
	HRESULT			Ready_Layer();
	_int			Update_Layer(const _float& fTimeDelta);
	_int			LateUpdate_Layer(const _float& fTimeDelta);
	void			Send_PacketToServer();
	void			Render_Layer(const _float& fTimeDelta);

private:
	map<wstring, OBJLIST> m_mapObjLst;

public:
	static CLayer*	Create();
private:
	virtual void	Free();
};

END