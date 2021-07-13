#pragma once
#include "Component.h"

BEGIN(Engine)

#define _3DMAX_FPS			30.0f
#define ANIMA_INTERPOLATION 1.0f

typedef map<string, _uint>		MAP_BONENAME;
typedef vector<BONE_DESC>		VECTOR_BONE_DESC;
typedef vector<_matrix>			VECTOR_MATRIX;
typedef vector<SKINNING_MATRIX>	VECTOR_SKINNING_MATRIX;

typedef struct tagHierarchyDesc 
{
	tagHierarchyDesc() {}
	tagHierarchyDesc(const aiNode* pNode) : pAiNode(pNode) {}
	~tagHierarchyDesc() = default;

	const aiNode*	pAiNode				= nullptr;	// AiNode 정보.
	_matrix			matScale			= INIT_MATRIX;
	_matrix			matRotate			= INIT_MATRIX;
	_matrix			matTrans			= INIT_MATRIX;
	_matrix			matBoneTransform	= INIT_MATRIX;
	_matrix			matParentTransform	= INIT_MATRIX;
	_matrix			matGlobalTransform	= INIT_MATRIX;
	vector<_uint>					vecBoneMapIdx;		// 영향을 받는 BoneMap Index 정보.
	map<_uint, const aiNodeAnim*>	mapNodeAnim;		// <AnimationIdx, aiNodeAnim*>

} HIERARCHY_DESC;


class ENGINE_DLL CAniCtrl final : public CComponent
{
private:
	explicit CAniCtrl(const aiScene* pScene);
	explicit CAniCtrl(const CAniCtrl& rhs);
	virtual ~CAniCtrl() = default;

public:
	// Get
	vector<VECTOR_MATRIX>*			Get_VecBoneTransform()	{ return m_vecBoneTransform; }
	vector<VECTOR_SKINNING_MATRIX>* Get_VecSkinningMatrix() { return m_vecSkinningMatrix; }
	_uint*							Get_NumAnimation()		{ return &m_uiNumAnimation; }
	_uint*							Get_3DMaxNumFrame()		{ return &m_ui3DMax_NumFrame; }
	_uint*							Get_3DMaxCurFrame()		{ return &m_ui3DMax_CurFrame; }
	const _uint&					Get_CurAnimationIdx()	{ return m_uiCurAniIndex; }
	const _uint&					Get_NewAnimationIdx()	{ return m_uiNewAniIdx; }
	const _float&					Get_AnimationTime()		{ return m_fAnimationTime; }
	// Set
	void Set_AnimationKey(const _uint& uiAniKey);
	void Set_BlendingSpeed(const _float& fBlendingSpeed) { m_fBlendingSpeed = fBlendingSpeed; }
	void Set_VecSkinningMatrix(const vector<VECTOR_SKINNING_MATRIX>& vecSkinningMatrix);
	void Set_AnimationTime(const _float& fTime)				{ m_fAnimationTime = fTime; }
	void Set_IsBlendingComplete(const _bool& bIsComplete)	{ m_bIsBlendingComplete = bIsComplete; }

	// Method
	void				Play_Animation(_float fTimeDelta, const _bool& bIsRepeat = true);
	SKINNING_MATRIX*	Find_SkinningMatrix(string strBoneName);
	HIERARCHY_DESC&		Find_HierarchyDesc(string strBoneName);
	_bool				Is_AnimationSetEnd(const _float& fTimeDelta, const _float& fAnimationSpeed = 4'800.0f);
	_bool				Is_BlendingComplete() { return m_bIsBlendingComplete; };

private:
	HRESULT	Ready_AniCtrl();
	void	Ready_NodeHierarchy(const aiNode* pNode);
	void	Update_NodeHierarchy(_float fAnimationTime, 
								 const aiNode* pNode, 
								 const _matrix& matParentTransform);
	aiNodeAnim*		Find_NodeAnimation(const aiAnimation* pAnimation, const string strNodeName);
	aiVector3D		Calc_InterPolatedValue_From_Key(const _float& fAnimationTime, 
													const _uint& uiNumKeys, 
													const aiVectorKey* pVectorKey,
													const _uint& uiNewNumKeys,
													const aiVectorKey* pNewVectorKey);
	aiQuaternion	Calc_InterPolatedValue_From_Key(const _float& fAnimationTime,
													const _uint& uiNumKeys,
													const aiQuatKey* pQuatKey,
													const _uint& uiNewNumKeys,
													const aiQuatKey* pNewQuatKey);
	_uint	Find_KeyIndex(const _float& fAnimationTime, const _uint& uiNumKey, const aiVectorKey* pVectorKey);
	_uint	Find_KeyIndex(const _float& fAnimationTime, const _uint& uiNumKey, const aiQuatKey* pQuatKey);
	_matrix	Convert_AiToMat4(const aiMatrix4x4& m);
	_matrix	Convert_AiToMat3(const aiMatrix3x3& m);

private:
	/*__________________________________________________________________________________________________________
	[ Mesh Container ]
	____________________________________________________________________________________________________________*/
	const aiScene* m_pScene	= nullptr;	// 모든 Mesh의 정보를 갖고있는 구조체.

	/*__________________________________________________________________________________________________________
	[ Mesh Skinning Matrix ]
	____________________________________________________________________________________________________________*/
	vector<MAP_BONENAME>			m_vecBoneNameMap;				// Bone의 이름이 속해있는 SubsetMesh와 Index 정보.
	vector<VECTOR_BONE_DESC>*		m_vecBoneDesc		= nullptr;	// Bone의 행렬 정보를 갖고있는 컨테이너.
	vector<VECTOR_MATRIX>*			m_vecBoneTransform	= nullptr;	// Bone의 애니메이션 최종 변환 행렬.
	vector<VECTOR_SKINNING_MATRIX>*	m_vecSkinningMatrix = nullptr;	// Bone Skinning에 필요한 행렬 정보들.

	/*__________________________________________________________________________________________________________
	[ Mesh Hierarchy Info ]
	____________________________________________________________________________________________________________*/
	map<string, HIERARCHY_DESC>*	m_mapNodeHierarchy;		// Node Hierarchy 정보.

	/*__________________________________________________________________________________________________________
	[ Animation ]
	____________________________________________________________________________________________________________*/
	_uint	m_uiNumAnimation		= 0;
	_uint	m_uiCurAniIndex			= 0;
	_uint	m_uiNewAniIdx			= 0;

	_float	m_fAnimationTime		= 0.0f;
	_float	m_fNewAnimationTime     = 0.0f;

	_float	m_fBlendingTime			= 1.0f;
	_float	m_fBlendAnimationTime	= 0.0f;
	_bool	m_bIsBlendingComplete	= true;
	_float	m_fBlendingSpeed		= 0.001f;

	_uint	m_ui3DMax_NumFrame		= 0;	// 3DMax에서 애니메이션의 총 Frame 개수.
	_uint	m_ui3DMax_CurFrame		= 0;	// 3DMAx에서 현재 애니메이션의 Frame 위치.

public:
	virtual CComponent* Clone();
	static CAniCtrl*	Create(const aiScene* pScece);
private:
	virtual void		Free();
};

END