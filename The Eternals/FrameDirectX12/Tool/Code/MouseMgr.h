#pragma once
#include "ObjectMgr.h"

namespace Engine
{
	class CLight;

	class CGameObject;

	class CTransform;
	class CRcTex;
	class CTexture;
	class CShaderTexture;
}

class CToolTerrain;
class CToolStaticMesh;
class CToolCell;

class CMouseMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CMouseMgr)

private:
	explicit CMouseMgr();
	virtual ~CMouseMgr() = default;

public:
	HRESULT	Ready_MouseMgr();
	void	Update_MouseMgr(const _float& fTimeDelta);

public:
	static POINT Get_CursorPoint();
	static _vec3 Picking_OnTerrain(CToolTerrain* pTerrain);
	static _bool IntersectTriangle(_vec3& v0,
								   _vec3& v1,
								   _vec3& v2,
								   _vec3& orig,
								   _vec3& dir,
								   _float* u, _float* v, _float* t);

	_bool Picking_Object(Engine::CGameObject** ppPickingObject, Engine::OBJLIST* pOBJLIST);
	_bool Picking_Light(Engine::CLight** ppPickingLight, vector<Engine::CLight*>& vecPointLight);
	_vec3* Find_NearCellPoint(_vec3& vPickingPos, CToolCell** ppPickingCell, _int* pIndex = nullptr);
	
private:
	Engine::CGameObject* m_pCursorUI = nullptr;

private:
	virtual void Free();
};

