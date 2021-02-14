#pragma once
#include "Base.h"

BEGIN(Engine)

class CShaderMeshInstancing;
class CShaderShadowInstancing;

class ENGINE_DLL CInstancingMgr : public CBase
{
	DECLARE_SINGLETON(CInstancingMgr)

private:
	explicit CInstancingMgr();
	virtual ~CInstancingMgr() = default;

public:
	// Get
	_uint Get_MeshInstanceCount(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& iPipelineStatePass)		{ return m_mapMeshInstancing[iContextIdx][wstrMeshTag][iPipelineStatePass].iInstanceCount; };
	CUploadBuffer<CB_SHADER_MESH>*		Get_UploadBuffer_ShaderMesh(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& uiPipelineStatepass);
	// Set
	void	Set_MeshInstancingPipelineStateCnt(const _uint& iCnt)	{ m_uiMeshPipelineStateCnt = iCnt; }
	void	Set_TexPipelineStateCnt(const _uint& iCnt)				{ m_uiTexPipelineStateCnt = iCnt; }

	// Method
	HRESULT SetUp_ShaderComponent();
	void	SetUp_MeshInstancing(wstring wstrMeshTag);
	void	SetUp_MeshConstantBuffer(ID3D12Device* pGraphicDevice);
	void	Add_MeshInstance(const _uint& iContextIdx, wstring wstrMeshTag, const _uint& iPipelineStateIdx);
	void	Reset_MeshInstancing();
	void	Render_MeshInstance(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx);


private:
	/*__________________________________________________________________________________________________________
	[ Shader Component ]
	____________________________________________________________________________________________________________*/
	CShaderMeshInstancing*		m_pShaderMeshInstancing   = nullptr;

	/*__________________________________________________________________________________________________________
	Key값은 ResourceTag
	vector의 Index는 PipelineStateIndex, Size는 Instance개수.
	____________________________________________________________________________________________________________*/
	map<wstring, vector<INSTANCING_DESC>>						m_mapMeshInstancing[CONTEXT::CONTEXT_END];
	map<wstring, vector<CUploadBuffer<CB_SHADER_MESH>*>>		m_mapCB_ShaderMesh[CONTEXT::CONTEXT_END];
	_uint														m_uiMeshPipelineStateCnt = 0;


	_uint m_uiTexPipelineStateCnt = 0;

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