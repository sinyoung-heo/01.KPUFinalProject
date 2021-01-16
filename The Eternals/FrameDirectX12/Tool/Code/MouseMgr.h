#pragma once
#include "ObjectMgr.h"

namespace Engine
{
	class CLight;
}

class CToolTerrain;
class CToolStaticMesh;

class CMouseMgr : public Engine::CBase
{
	DECLARE_SINGLETON(CMouseMgr)

private:
	explicit CMouseMgr();
	virtual ~CMouseMgr() = default;

public:
	static _vec3 Picking_OnTerrain(CToolTerrain* pTerrain);
	static _bool IntersectTriangle(_vec3& v0,
								   _vec3& v1,
								   _vec3& v2,
								   _vec3& orig,
								   _vec3& dir,
								   _float* u, _float* v, _float* t);

	_bool Picking_Object(Engine::CGameObject** pPickingObject, Engine::OBJLIST* pOBJLIST);
	_bool Picking_Light(Engine::CLight** pPickingLight, vector<Engine::CLight*>& vecPointLight);

private:
	virtual void Free();
};

