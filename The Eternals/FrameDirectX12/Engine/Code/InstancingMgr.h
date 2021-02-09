#pragma once
#include "Base.h"

BEGIN(Engine)

typedef struct tagInstancingDesc
{


} INSTANCING_DESC;

class ENGINE_DLL CInstancingMgr : public CBase
{
	DECLARE_SINGLETON(CInstancingMgr)

private:
	explicit CInstancingMgr();
	virtual ~CInstancingMgr() = default;

private:
	map<wstring, INSTANCING_DESC> m_mapMeshInstancingDesc[CONTEXT::CONTEXT_END];
	map<wstring, INSTANCING_DESC> m_mapBufferInstancingDesc;

private:
	virtual void Free();
};

END


/*__________________________________________________________________________________________________________
EX) ResourceTag		: L"BumpTerrainMesh01"
					Shader - DescriptorHeap
					Buffer - Vertex/Index Buffer View
					Buffer - Primitive

	PipelineStateIndex	0
					Shader - Constant Buffer
					Buffer - IndexCount / Instance Count

	PipelineStateIndex	1
					Shader - Constant Buffer
					Buffer - IndexCount / Instance Count

	'
	'
	'
____________________________________________________________________________________________________________*/