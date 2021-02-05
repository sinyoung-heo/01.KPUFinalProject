#pragma once
#include "Engine_Include.h"
#include "Base.h"

#include "Transform.h"
#include "Info.h"
#include "RcCol.h"
#include "CubeCol.h"
#include "RcTex.h"
#include "CubeTex.h"
#include "TerrainTex.h"
#include "ScreenTex.h"
#include "Texture.h"
#include "Mesh.h"
#include "NaviMesh.h"
#include "ColliderBox.h"
#include "ColliderSphere.h"
#include "CoordinateCol.h"

#include "ShaderColor.h"
#include "ShaderTexture.h"
#include "ShaderMesh.h"
#include "ShaderSkyBox.h"
#include "ShaderLighting.h"
#include "ShaderBlend.h"
#include "ShaderShadow.h"
#include "ShaderLuminance.h"
#include "ShaderDownSampling.h"
#include "ShaderBlur.h"
#include "ShaderSSAO.h"
BEGIN(Engine)

class ENGINE_DLL CComponentMgr final : public CBase
{
	DECLARE_SINGLETON(CComponentMgr)

private:
	explicit CComponentMgr();
	virtual ~CComponentMgr() = default;

public:
	CComponent*	Get_Component(wstring wstrPrototypeTag, COMPONENTID eID);
	HRESULT		Add_ComponentPrototype(wstring wstrPrototypeTag, const COMPONENTID& eID, CComponent* pComponent);
	CComponent* Clone_Component(wstring wstrPrototypeTag, const COMPONENTID& eID);
	void		Release_UploadBuffer();

private:
	CComponent*	Find_Component(wstring wstrPrototypeTag, const COMPONENTID& eID);

private:
	map<wstring, CComponent*>	m_mapComponent[COMPONENTID::ID_END];

private:
	virtual void Free();
};

END