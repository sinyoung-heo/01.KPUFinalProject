#pragma once
#include "Component.h"
#include "AniCtrl.h"
#include "VIMesh.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CComponent
{
private:
	explicit CMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	// Get
	vector<ComPtr<ID3D12Resource>>	Get_DiffTexture()			{ return m_pVIMesh->Get_DiffTexture(); };
	vector<ComPtr<ID3D12Resource>>	Get_NormTexture()			{ return m_pVIMesh->Get_NormTexture(); };
	vector<ComPtr<ID3D12Resource>>	Get_SpecTexture()			{ return m_pVIMesh->Get_SpecTexture(); };
	ID3D12DescriptorHeap*			Get_TexDescriptorHeap()		{ return m_pVIMesh->Get_TexDescriptorHeap(); }
	CVIMesh*						Get_VIMesh()				{ return m_pVIMesh; }
	CAniCtrl*						Get_AniCtrl()				{ return m_pAniCtrl; };
	_uint*							Get_NumAnimation()			{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_NumAnimation(); else return nullptr; }
	_uint*							Get_3DMaxNumFrame()			{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_3DMaxNumFrame(); else return nullptr;}
	_uint*							Get_3DMaxCurFrame()			{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_3DMaxCurFrame(); else return nullptr;}
	const _float&					Get_AnimationTime()			{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_AnimationTime(); else return 0.0f; }
	const _float&					Get_BlendTime()				{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_BlendTime(); else return 0.0f; }
	const _float&					Get_BlendAnimationTime()	{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_BlendAnimationTime(); else return 0.0f; }
	const _vec3&					Get_CenterPos()				{ return m_pVIMesh->Get_CenterPos(); }
	const _vec3&					Get_MinVector()				{ return m_pVIMesh->Get_MinVector(); }
	const _vec3&					Get_MaxVector()				{ return m_pVIMesh->Get_MaxVector(); }
	wstring							Get_FileName()				{ return m_pVIMesh->Get_FileName(); }
	_bool							Is_AnimationSetEnd(const _float& fTimeDelta) { return m_pAniCtrl->Is_AnimationSetEnd(fTimeDelta); };

	void							Set_AfterImgSize(const _uint& uiSize) { m_pVIMesh->Set_AfterImgSize(uiSize); }
	void							Set_AfterImgTime(const _float& fTime) { m_pVIMesh->Set_AfterImgTime(fTime); }
	void							Set_AfterImgMakeTime(const _float& fTime) { m_pVIMesh->Set_AfterImgMakeTime(fTime); }
	void							Set_AfterImgSubAlpha(const _float& fSub) { m_pVIMesh->Set_AfterImgSubAlpha(fSub); }

	// Method
	HRESULT Ready_Mesh(wstring wstrFilePath, wstring wstrFileName);

	// SingleThread Rendering
	void Render_DynamicMesh(CShader* pShader);
	void Render_DynamicMeshAfterImage(CShader* pShader, const _uint& iAfterImgIdx);

	void Render_StaticMesh(CShader* pShader);
	void Render_DynamicMeshShadowDepth(CShader* pShader);
	
	void Render_StaticMeshShadowDepth(CShader* pShader);

	// MultiThread Rendering
	void Render_DynamicMesh(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_DynamicMeshAfterImage(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader, const _uint& iAfterImgIdx);

	void Render_StaticMesh(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_DynamicMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_StaticMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_WaterMesh(ID3D12GraphicsCommandList* pCommandList,
		const _int& iContextIdx,
		CShader* pShader,
		ID3D12DescriptorHeap* pTexnormalDescriptorHeap, _uint uiNormalTextureIdx, _uint uiPatternMapIdx);
	void Set_AnimationKey(const _uint& uiAniKey);
	void Play_Animation(_float fAnimationTime);
	SKINNING_MATRIX* Find_SkinningMatrix(string strBoneName);

private:
	/*__________________________________________________________________________________________________________
	[ Assimp ]
	- Assimp를 이용해서 Data를 읽으면 모든 정보는 Scene에 담기게 된다.
	____________________________________________________________________________________________________________*/
	const aiScene*		m_pScene	= nullptr;
	Assimp::Importer	m_Importer;

	/*__________________________________________________________________________________________________________
	[ VIMesh / Animation Control ]
	____________________________________________________________________________________________________________*/
	CVIMesh*	m_pVIMesh			= nullptr;
	CAniCtrl*	m_pAniCtrl			= nullptr;

	/*__________________________________________________________________________________________________________
	[ Component Tag ]
	____________________________________________________________________________________________________________*/
	wstring m_wstrVIMeshTag			= L"";
	wstring m_wstrAniCtrlTag		= L"";

public:
	virtual CComponent* Clone();
public:
	static CMesh*		Create(ID3D12Device* pGraphicDevice, 
							   ID3D12GraphicsCommandList* pCommandList,
							   wstring wstrFilePath, 
							   wstring wstrFileName);
private:
	virtual void		Free();
};

END