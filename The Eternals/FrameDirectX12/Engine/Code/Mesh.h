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
	vector<ComPtr<ID3D12Resource>>	Get_DiffTexture()		{ return m_pVIMesh->Get_DiffTexture(); };
	vector<ComPtr<ID3D12Resource>>	Get_NormTexture()		{ return m_pVIMesh->Get_NormTexture(); };
	vector<ComPtr<ID3D12Resource>>	Get_SpecTexture()		{ return m_pVIMesh->Get_SpecTexture(); };
	ID3D12DescriptorHeap*			Get_TexDescriptorHeap()	{ return m_pVIMesh->Get_TexDescriptorHeap(); }
	CAniCtrl*						Get_AniCtrl()			{ return m_pAniCtrl; };
	_uint*							Get_NumAnimation()		{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_NumAnimation(); else return nullptr; }
	_uint*							Get_3DMaxNumFrame()		{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_3DMaxNumFrame(); else return nullptr;}
	_uint*							Get_3DMaxCurFrame()		{ if (nullptr != m_pAniCtrl) return m_pAniCtrl->Get_3DMaxCurFrame(); else return nullptr;}

	// Method
	HRESULT Ready_Mesh(wstring wstrFilePath, wstring wstrFileName);

	// SingleThread Rendering
	void Render_DynamicMesh(CShader* pShader);
	void Render_StaticMesh(CShader* pShader);
	void Render_DynamicMeshShadowDepth(CShader* pShader);
	void Render_StaticMeshShadowDepth(CShader* pShader);

	// MultiThreadRendering
	void Render_DynamicMesh(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_StaticMesh(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_DynamicMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);
	void Render_StaticMeshShadowDepth(ID3D12GraphicsCommandList* pCommandList, const _int& iContextIdx, CShader* pShader);

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
	[ FilePatn / FileName ]
	____________________________________________________________________________________________________________*/
	wstring	m_wstrFilePath			= L"";
	wstring	m_wstrFullPath			= L"";
	wstring m_wstrFileName			= L"";

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