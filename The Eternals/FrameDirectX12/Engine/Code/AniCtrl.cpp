#include "AniCtrl.h"
#include "TimeMgr.h"
#include "FrameMgr.h"

USING(Engine)

CAniCtrl::CAniCtrl(const aiScene * pScene)
	: m_pScene(pScene)
{
}

CAniCtrl::CAniCtrl(const CAniCtrl & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)
	, m_uiNumAnimation(rhs.m_uiNumAnimation)
{
	m_vecBoneNameMap.reserve(rhs.m_vecBoneNameMap.size());
	m_vecBoneNameMap = rhs.m_vecBoneNameMap;

	m_mapNodeHierarchy = rhs.m_mapNodeHierarchy;

	// 깊은 복사 수행.
	m_vecBoneDesc = new vector<VECTOR_BONE_DESC>;
	m_vecBoneDesc->reserve(rhs.m_vecBoneDesc->size());
	(*m_vecBoneDesc) = *(rhs.m_vecBoneDesc);

	// 깊은 복사 수행.
	m_vecBoneTransform = new vector<VECTOR_MATRIX>;
	m_vecBoneTransform->reserve(rhs.m_vecBoneTransform->size());
	(*m_vecBoneTransform) = *(rhs.m_vecBoneTransform);

	// 깊은 복사 수행.
	m_vecSkinningMatrix = new vector<VECTOR_SKINNING_MATRIX>;
	m_vecSkinningMatrix->reserve(rhs.m_vecSkinningMatrix->size());
	(*m_vecSkinningMatrix) = *(rhs.m_vecSkinningMatrix);

}


HRESULT CAniCtrl::Ready_AniCtrl()
{

#ifdef ENGINE_LOG
	if (m_pScene->mNumAnimations >= 1)
	{
		COUT_STR("");
		COUT_STR("[ Num Animation   " << m_pScene->mNumAnimations << " ]");

		for (_uint i = 0; i < m_pScene->mNumAnimations; ++i)
		{
			COUT_STR("Animation Name\t" << m_pScene->mAnimations[i]->mName.C_Str());
			COUT_STR("TickPerSecond\t" << m_pScene->mAnimations[i]->mTicksPerSecond);
			COUT_STR("Duration\t" << m_pScene->mAnimations[i]->mDuration);
			COUT_STR("Num MeshChannel\t" << m_pScene->mAnimations[i]->mNumMeshChannels);
			COUT_STR("Num Channel\t" << m_pScene->mAnimations[i]->mNumChannels);

			COUT_STR("");
			if (m_pScene->mAnimations[i]->mNumMeshChannels >= 1)
			{
				COUT_STR("[ MeshChannels ]");
				for (_uint j = 0; j < m_pScene->mAnimations[i]->mNumMeshChannels; ++j)
				{
					COUT_STR("MeshChannel Name\t" << m_pScene->mAnimations[i]->mMeshChannels[j]->mName.C_Str());
					COUT_STR("Num Keys\t" << m_pScene->mAnimations[i]->mMeshChannels[j]->mNumKeys);


					if (m_pScene->mAnimations[i]->mMeshChannels[j]->mNumKeys >= 1)
					{
						COUT_STR("[ MeshChannels Key ]");
						for (_uint k = 0; k < m_pScene->mAnimations[i]->mMeshChannels[j]->mNumKeys; ++k)
						{
							COUT_STR("Keys Value\t" << m_pScene->mAnimations[i]->mMeshChannels[j]->mKeys[k].mValue);
							 

						}	// Num Key
					}
				}	// Num MeshChannel
			}
		} // Num Animation


	}
#endif

	m_uiNumAnimation = m_pScene->mNumAnimations;

	m_vecBoneNameMap.reserve(m_pScene->mNumMeshes);
	m_vecBoneNameMap.resize(m_pScene->mNumMeshes);

	m_vecBoneDesc = new vector<VECTOR_BONE_DESC>;
	m_vecBoneDesc->reserve(m_pScene->mNumMeshes);
	m_vecBoneDesc->resize(m_pScene->mNumMeshes);

	m_vecBoneTransform = new vector<VECTOR_MATRIX>;
	m_vecBoneTransform->reserve(m_pScene->mNumMeshes);
	m_vecBoneTransform->resize(m_pScene->mNumMeshes);

	m_vecSkinningMatrix = new vector<VECTOR_SKINNING_MATRIX>;
	m_vecSkinningMatrix->reserve(m_pScene->mNumMeshes);
	m_vecSkinningMatrix->resize(m_pScene->mNumMeshes);

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
	    _int	iNumBones	= 0;
		aiMesh* pSubsetMesh = m_pScene->mMeshes[i];

		/*__________________________________________________________________________________________________________
		- 뼈 행렬을 담아둘 컨테이너 초기화.
		____________________________________________________________________________________________________________*/
		(*m_vecBoneTransform)[i].reserve(pSubsetMesh->mNumBones);
		(*m_vecBoneTransform)[i].resize(pSubsetMesh->mNumBones);

		(*m_vecSkinningMatrix)[i].reserve(pSubsetMesh->mNumBones);
		(*m_vecSkinningMatrix)[i].resize(pSubsetMesh->mNumBones);

#ifdef ENGINE_LOG
		COUT_STR("[ " << i << "번 째 SubsetMesh BoneName ]");
#endif
		for (_uint j = 0; j < pSubsetMesh->mNumBones; ++j, ++iNumBones)
		{
			_uint	uiboneindex = 0;
			string	strBoneName(pSubsetMesh->mBones[j]->mName.data);

			uiboneindex = iNumBones;

			BONE_DESC tBoneInfo;
			(*m_vecBoneDesc)[i].emplace_back(tBoneInfo);

			m_vecBoneNameMap[i][strBoneName] = uiboneindex;
			(*m_vecBoneDesc)[i][uiboneindex].matBoneOffset = Convert_AiToMat4(pSubsetMesh->mBones[j]->mOffsetMatrix);
#ifdef ENGINE_LOG
			COUT_STR("Index  " << uiboneindex << "\t" << "BoneName : " << strBoneName);
#endif
		}

#ifdef ENGINE_LOG
		COUT_STR("");
#endif
	}

#ifdef ENGINE_LOG
	COUT_STR("--------------------------------------------------------------------------------");
#endif

	/*__________________________________________________________________________________________________________
	[ Hierarchy 정보 ]
	- BoneNameMap안에 각 Node의 Name이 어디에 들어있는지 판단.
	- Animation Index별로 갹 Node에 aiNodeAnim가 들어있는지 탐색.
	____________________________________________________________________________________________________________*/
	Ready_NodeHierarchy(m_pScene->mRootNode);

#ifdef ENGINE_LOG
	COUT_STR("--------------------------------------------------------------------------------");
#endif

	return S_OK;

}

void CAniCtrl::Set_AnimationKey(const _uint & uiAniKey)
{
	if (m_uiNewAniIdx != uiAniKey)
	{
		m_uiNewAniIdx         = uiAniKey;
		m_fBlendingTime	      = 1.0f;
		m_fBlendAnimationTime = m_fAnimationTime;
	}
}

void CAniCtrl::Play_Animation(_float fTimeDelta)
{
	if (m_uiCurAniIndex >= m_uiNumAnimation)
		return;

	/*__________________________________________________________________________________________________________
	[ 애니메이션이 계속 반복되도록 fmod 수행 ]
	____________________________________________________________________________________________________________*/
	m_fAnimationTime += fTimeDelta;
	
	if (m_uiNewAniIdx != m_uiCurAniIndex)
	{
		m_fAnimationTime = m_fBlendAnimationTime;
		m_fBlendingTime	 -= 0.001f * fTimeDelta;
	}

	m_fAnimationTime = (_float)(fmod(m_fAnimationTime, (m_pScene->mAnimations[m_uiCurAniIndex]->mDuration)));

	if (m_fBlendingTime <= 0.0f)
		m_fBlendingTime = 0.0f;

	/*__________________________________________________________________________________________________________
	[ 3DMax 상에서의 Frame 계산 ]
	____________________________________________________________________________________________________________*/
	m_ui3DMax_NumFrame = (_uint)(_3DMAX_FPS * (m_pScene->mAnimations[m_uiCurAniIndex]->mDuration / m_pScene->mAnimations[m_uiCurAniIndex]->mTicksPerSecond));
	m_ui3DMax_CurFrame = (_uint)(_3DMAX_FPS * (m_fAnimationTime / m_pScene->mAnimations[m_uiCurAniIndex]->mTicksPerSecond));

	/*__________________________________________________________________________________________________________
	- Root Node와 단위 행렬을 인자로 넘겨주면 재귀 호출을 통하여 BONE_DESC에 데이터를 저장하는 함수.
	- Read_NodeHierarchy() 함수 호출이 끝나고 나면, 멤버 변수인 m_vecBoneTransform배열에 데이터를 채우고 리턴.
	____________________________________________________________________________________________________________*/
	Update_NodeHierarchy(m_fAnimationTime, m_pScene->mRootNode, INIT_MATRIX);

	if (m_fBlendingTime <= 0.0f)
	{
		m_uiCurAniIndex	      = m_uiNewAniIdx;
		m_fAnimationTime      = 0.0f;
		m_fBlendAnimationTime = 0.0f;
		m_fBlendingTime       = 1.f;
	}

}

SKINNING_MATRIX * CAniCtrl::Find_SkinningMatrix(string strBoneName)
{
	/*__________________________________________________________________________________________________________
	[ Key값 탐색 ]
	- vector<MAP_BONENAME> m_vecBoneNameMap을 순회하면서 strBoneName Key값이 있는지 탐색.
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < m_vecBoneNameMap.size(); ++i)
	{
		// iter_find = i번째 SubMesh의 Index번호.
		auto iter_find = m_vecBoneNameMap[i].find(strBoneName);

		if (iter_find == m_vecBoneNameMap[i].end())
			continue;

		/*__________________________________________________________________________________________________________
		[ 탐색 성공 ]
		- Bone의 m_vecSkinningMatrix에서 SKINNING_MATRIX 반환.
		____________________________________________________________________________________________________________*/
		if (iter_find->first == strBoneName)
		{
			return &((*m_vecSkinningMatrix)[i][iter_find->second]);
		}

		/*__________________________________________________________________________________________________________
		[ 탐색 실패 ]
		- nullptr 반환.
		____________________________________________________________________________________________________________*/
		if (iter_find == m_vecBoneNameMap[m_vecBoneNameMap.size() - 1].end())
			return nullptr;
	}

	return nullptr;
}

HIERARCHY_DESC* CAniCtrl::Find_HierarchyDesc(string strBoneName)
{
	// iter_find = i번째 SubMesh의 Index번호.
	auto iter_find = m_mapNodeHierarchy.find(strBoneName);

	if (iter_find == m_mapNodeHierarchy.end())
		nullptr;

	return iter_find->second;
}

_bool CAniCtrl::Is_AnimationSetEnd(const _float& fTimeDelta)
{
	if (m_fAnimationTime >= m_pScene->mAnimations[m_uiCurAniIndex]->mDuration -
							m_pScene->mAnimations[m_uiCurAniIndex]->mTicksPerSecond * ANIMA_INTERPOLATION * fTimeDelta)
	{
		// m_fAnimationTime = 0.0f;

		//m_fBlendAnimationTime = m_fAnimationTime;
		//m_fBlendAnimationTime = 0.0f;
		return true;
	}

	return false;
}

void CAniCtrl::Ready_NodeHierarchy(const aiNode * pNode)
{
	string strNodeName(pNode->mName.data);

	HIERARCHY_DESC* pNodeHierarchy = new HIERARCHY_DESC(pNode);
	pNodeHierarchy->pAiNode				= pNode;
	pNodeHierarchy->matScale			= INIT_MATRIX;
	pNodeHierarchy->matRotate			= INIT_MATRIX;
	pNodeHierarchy->matTrans			= INIT_MATRIX;
	pNodeHierarchy->matBoneTransform	= Convert_AiToMat4(pNode->mTransformation);

	// 1. BoneNameMap안에 각 Node의 Name이 어디에 들어있는지 판단.
	for (_uint i = 0; i < m_vecBoneNameMap.size(); ++i)
	{
		if (m_vecBoneNameMap[i].find(strNodeName) != m_vecBoneNameMap[i].end())
		{
			// 속해이는 BoneNameMap의 Index 정보.
			pNodeHierarchy->vecBoneMapIdx.emplace_back(i);
		}
	}

	// 2. Animation Index별로 각 Node에 aiNodeAnim가 들어있는지 탐색.
	for (_uint i = 0; i < m_uiNumAnimation; ++i)
	{
		const aiAnimation*	pAnimation		= m_pScene->mAnimations[i];
		const aiNodeAnim*	pNodeAnimation	= Find_NodeAnimation(pAnimation, strNodeName);

		pNodeHierarchy->mapNodeAnim.emplace(i, pNodeAnimation);
	}
	

	m_mapNodeHierarchy.emplace(strNodeName, pNodeHierarchy);

#ifdef ENGINE_LOG
	COUT_STR("Hierarchy Node Name : " << strNodeName);
#endif

	/*__________________________________________________________________________________________________________
	[ 모든 자식 노드에 대해 재귀호출 ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Ready_NodeHierarchy(pNode->mChildren[i]);

}

void CAniCtrl::Update_NodeHierarchy(_float fAnimationTime,
									const aiNode * pNode, 
									const _matrix & matParentTransform)
{
	/*__________________________________________________________________________________________________________
	[ Update_NodeHierarchy ]
	- Bone의 Transform을 구하는 함수.
	- 내부적으로 재귀호출하여 BONE_DESC배열의 matFinalTransform변수에 데이터를 채운다.
	- 재귀호출을 통해 자식 노드로 내려가면서, 각각 매칭된 boneTransformation을 저장.
	____________________________________________________________________________________________________________*/
	string strNodeName(pNode->mName.data);
	HIERARCHY_DESC*		pHierarchyInfo		= m_mapNodeHierarchy[strNodeName];

	const aiNodeAnim*	pNodeAnimation		= pHierarchyInfo->mapNodeAnim[m_uiCurAniIndex];
	const aiNodeAnim*	pNewNodeAnimation	= pHierarchyInfo->mapNodeAnim[m_uiNewAniIdx];

	pHierarchyInfo->matParentTransform		= matParentTransform;

	/*__________________________________________________________________________________________________________
	- 애니메이션 정보가 있는 노드일 경우.
	____________________________________________________________________________________________________________*/
	if (pNodeAnimation)
	{
		/*__________________________________________________________________________________________________________
		- 주어진 KeyFrame의 정보와 AnimationTime정보를 이용해 Interpolation(보간)을 하고 값을 저장.
		____________________________________________________________________________________________________________*/
		// Scale
		const aiVector3D&	vScale	= Calc_InterPolatedValue_From_Key(fAnimationTime, 
																	  pNodeAnimation->mNumScalingKeys, 
																	  pNodeAnimation->mScalingKeys, 
																	  pNewNodeAnimation->mNumScalingKeys, 
																	  pNewNodeAnimation->mScalingKeys);
		// Rotation
		const aiQuaternion& vRotate	= Calc_InterPolatedValue_From_Key(fAnimationTime, 
																	  pNodeAnimation->mNumRotationKeys, 
																	  pNodeAnimation->mRotationKeys, 
																	  pNewNodeAnimation->mNumRotationKeys, 
																	  pNewNodeAnimation->mRotationKeys);
		// Trans
		const aiVector3D&	vTrans	= Calc_InterPolatedValue_From_Key(fAnimationTime,
																	  pNodeAnimation->mNumPositionKeys,
																	  pNodeAnimation->mPositionKeys, 
																	  pNewNodeAnimation->mNumPositionKeys, 
																	  pNewNodeAnimation->mPositionKeys);

		/*__________________________________________________________________________________________________________
		- 각각의 vector와 quaternion은 matrix로 변환되고, 이동/회전/크기 변환을 통해 NodeTransform(Bone Transform)이 완성.
		- 이 NodeTransform은 Bone Space에서 정의되었던 정점들을 Parent Bone Space에서 정의되도록 하는 변환임과 동시에,
		Parent Bone Space에서 정의된 정점들에게 취하는 변환이다.
		- 예를 들어 상완의 좌표계에서 특정 정점들에 변환을 취해서 전완에 위치하도록 만드는 것.
		____________________________________________________________________________________________________________*/
		// Scale * Rotation * Trans
		pHierarchyInfo->matScale	= XMMatrixScaling(vScale.x, vScale.y, vScale.z);
		pHierarchyInfo->matRotate	= Convert_AiToMat3(vRotate.GetMatrix());
		pHierarchyInfo->matTrans	= XMMatrixTranslation(vTrans.x, vTrans.y, vTrans.z);

		pHierarchyInfo->matBoneTransform = pHierarchyInfo->matScale * pHierarchyInfo->matRotate * pHierarchyInfo->matTrans;
	}

	/*__________________________________________________________________________________________________________
	[ 구한 Transformation을 이용해서 Bone Transformation을 설정하고, 자식 Node에 전파하는 부분 ]
	- matGlobalTransform	: Bone Space에서 정의되었던 정점들을 Model Space에서 정의도도록 함.
	- matParentTransform	: 부모 Bone Space에서 정의되었던 정점들을 Model Space에서 정의되도록 함.
	- matBoneTransform		: Bone Space에서 정의되었던 정점들을 부모 Bone Space에서 정의되도록 함.
	____________________________________________________________________________________________________________*/
	pHierarchyInfo->matGlobalTransform = (pHierarchyInfo->matBoneTransform) * (pHierarchyInfo->matParentTransform);

	/*__________________________________________________________________________________________________________
	- Bone이 있는 노드에 대해서만 Bone Transform을 저장.
	- m_vecBoneNameMap은 map<string, _uint>타입으로, bone의 이름과 index를 저장.
	- mapBone을 살펴보는 이유는, 모든 Bone은 Node이지만, 모든 Node가 Bone은 아니기 때문.
	____________________________________________________________________________________________________________*/
	for (_uint& iIdx : pHierarchyInfo->vecBoneMapIdx)
	{
		_uint uiBoneIdx = m_vecBoneNameMap[iIdx][strNodeName];
		
		/*__________________________________________________________________________________________________________
		- matBoneOffset은 Model Space에서 정의되었던 정점들을 Bone Space에서 정의되도록 만드는 것.
		- matGlobalTransform = matBoneTransform * matParentTransform -> ParentTransform을 통해 다시 Model Space에서 정의.
		- GlobalTransform을 따로 저장하는 이유는 자식 노드에 ParentTransform으로 보내주기 위함.
		____________________________________________________________________________________________________________*/
		(*m_vecSkinningMatrix)[iIdx][uiBoneIdx].matBoneOffset		= (*m_vecBoneDesc)[iIdx][uiBoneIdx].matBoneOffset;
		(*m_vecSkinningMatrix)[iIdx][uiBoneIdx].matBoneScale		= pHierarchyInfo->matScale;
		(*m_vecSkinningMatrix)[iIdx][uiBoneIdx].matBoneRotation		= pHierarchyInfo->matRotate;
		(*m_vecSkinningMatrix)[iIdx][uiBoneIdx].matBoneTrans		= pHierarchyInfo->matTrans;
		(*m_vecSkinningMatrix)[iIdx][uiBoneIdx].matParentTransform	= pHierarchyInfo->matParentTransform;
		(*m_vecSkinningMatrix)[iIdx][uiBoneIdx].matRootTransform	= Convert_AiToMat4(m_pScene->mRootNode->mTransformation);
	}

	/*__________________________________________________________________________________________________________
	[ 모든 자식 노드에 대해 재귀호출 ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Update_NodeHierarchy(fAnimationTime, pNode->mChildren[i], pHierarchyInfo->matGlobalTransform);
	}

}

aiNodeAnim * CAniCtrl::Find_NodeAnimation(const aiAnimation * pAnimation, 
										  const string strNodeName)
{
	for (_uint i = 0; i < pAnimation->mNumChannels; ++i)
	{
		if (strNodeName == pAnimation->mChannels[i]->mNodeName.data)
			return pAnimation->mChannels[i];
	}

	return nullptr;
}


/*__________________________________________________________________________________________________________
- Key Frame이란, 애니메이션을 표현하는 Frame들 중 몇 개만 추려서 만든 Frame을 뜻한다.

- Animation의 모든 Frame들을 만들기에는 메모리 낭비도 심하고, 시간도 오래걸리기 때문에,
Key Frame만 만들고 자연스러운 움직임을 위해 그 사이의 값들은 보간해서 얻어낸다.

- Translation과 Scale은 vector 형태로 되어있고, 방향을 뜻하는 Rotation은 quaternion 형태로 되어있다.
- vector와 quaternion은 보간 방식이 다르기 때문에, 함수를 오버로딩하여 구현.
____________________________________________________________________________________________________________*/
aiVector3D CAniCtrl::Calc_InterPolatedValue_From_Key(const _float & fAnimationTime, 
													 const _uint & uiNumKeys, 
													 const aiVectorKey * pVectorKey,
													 const _uint& uiNewNumKeys,
													 const aiVectorKey* pNewVectorKey)
{
	aiVector3D ret1;
	aiVector3D ret2;

	if (1 == uiNumKeys)
	{
		ret1 = pVectorKey[0].mValue;
		return ret1;
	}

	_uint uiKeyIndex		= Find_KeyIndex(fAnimationTime, uiNumKeys, pVectorKey);
	_uint uiNextKeyIndex	= uiKeyIndex + 1;

	/*__________________________________________________________________________________________________________
	[ Key Frame 사이를 보간하여 특정 시간의 Node의 Transformation을 구하는 부분 ]
	____________________________________________________________________________________________________________*/
	_float fTimeDelta	= (_float)(pVectorKey[uiNextKeyIndex].mTime - pVectorKey[uiKeyIndex].mTime);
	_float fFactor		= (fAnimationTime - (_float)pVectorKey[uiKeyIndex].mTime) / fTimeDelta;

	const aiVector3D& StartValue	= pVectorKey[uiKeyIndex].mValue;
	const aiVector3D& EndValue		= pVectorKey[uiNextKeyIndex].mValue;

	ret1.x = StartValue.x + (EndValue.x - StartValue.x) * fFactor;
	ret1.y = StartValue.y + (EndValue.y - StartValue.y) * fFactor;
	ret1.z = StartValue.z + (EndValue.z - StartValue.z) * fFactor;


	if (m_uiCurAniIndex != m_uiNewAniIdx)
	{
		uiKeyIndex		= Find_KeyIndex(0, uiNewNumKeys, pNewVectorKey);
		uiNextKeyIndex	= uiKeyIndex + 1;

		_float fTimeDelta	= (_float)(pNewVectorKey[uiNextKeyIndex].mTime - pNewVectorKey[uiKeyIndex].mTime);
		_float fFactor		= (0.0f - (_float)pNewVectorKey[uiKeyIndex].mTime) / fTimeDelta;

		assert(fFactor >= 0.0f && fFactor <= 1.0f);

		const aiVector3D& StartValue	= pNewVectorKey[uiKeyIndex].mValue;
		const aiVector3D& EndValue		= pNewVectorKey[uiNextKeyIndex].mValue;

		ret2.x = StartValue.x + (EndValue.x - StartValue.x) * fFactor;
		ret2.y = StartValue.y + (EndValue.y - StartValue.y) * fFactor;
		ret2.z = StartValue.z + (EndValue.z - StartValue.z) * fFactor;

		ret1 = ret1 * m_fBlendingTime + ret2 * (1.f - m_fBlendingTime);
	}

	return ret1;
}

aiQuaternion CAniCtrl::Calc_InterPolatedValue_From_Key(const _float & fAnimationTime, 
													   const _uint & uiNumKeys,
													   const aiQuatKey * pQuatKey,
													   const _uint& uiNewNumKeys,
													   const aiQuatKey* pNewQuatKey)
{
	aiQuaternion ret1;
	aiQuaternion ret2;

	if (uiNumKeys == 1)
	{
		ret1 = pQuatKey[0].mValue;
		return ret1;
	}

	_uint uiKeyIndex		= Find_KeyIndex(fAnimationTime, uiNumKeys, pQuatKey);
	_uint uiNextKeyIndex	= uiKeyIndex + 1;

	assert(uiNextKeyIndex < uiNumKeys);

	_float fTimeDelta	= (_float)(pQuatKey[uiNextKeyIndex].mTime - pQuatKey[uiKeyIndex].mTime);
	_float fFactor		= (fAnimationTime - (_float)pQuatKey[uiKeyIndex].mTime) / fTimeDelta;

	assert(fFactor >= 0.0f && fFactor <= 1.0f);

	const aiQuaternion& StartValue	= pQuatKey[uiKeyIndex].mValue;
	const aiQuaternion& EndValue	= pQuatKey[uiNextKeyIndex].mValue;

	aiQuaternion::Interpolate(ret1, StartValue, EndValue, fFactor);

	ret1 = ret1.Normalize();


	if (m_uiCurAniIndex != m_uiNewAniIdx)
	{
		_uint uiKeyIndex		= Find_KeyIndex(0, uiNewNumKeys, pNewQuatKey);
		_uint uiNextKeyIndex	= uiKeyIndex + 1;

		assert(uiNextKeyIndex < uiNumKeys);
		
		_float fTimeDelta	= (_float)(pNewQuatKey[uiNextKeyIndex].mTime - pNewQuatKey[uiKeyIndex].mTime);
		_float fFactor		= (0 - (_float)pNewQuatKey[uiKeyIndex].mTime) / fTimeDelta;

		assert(fFactor >= 0.0f && fFactor <= 1.0f);

		const aiQuaternion& StartValue	= pNewQuatKey[uiKeyIndex].mValue;
		const aiQuaternion& EndValue	= pNewQuatKey[uiNextKeyIndex].mValue;

		aiQuaternion::Interpolate(ret2, StartValue, EndValue, fFactor);
		ret2 = ret2.Normalize();

		aiQuaternion::Interpolate(ret1, ret2, ret1, m_fBlendingTime);

		ret1 = ret1.Normalize();
	}

	return ret1;
}

_uint CAniCtrl::Find_KeyIndex(const _float & fAnimationTime, 
							  const _uint & uiNumKey, 
							  const aiVectorKey * pVectorKey)
{
	assert(uiNumKey > 0);

	for (_uint i = 0; i < uiNumKey - 1; ++i)
	{
		if (fAnimationTime < (_float)pVectorKey[i + 1].mTime)
			return i;
	}

	assert(0);

	return S_OK;
}

_uint CAniCtrl::Find_KeyIndex(const _float & fAnimationTime, 
							  const _uint & uiNumKey,
							  const aiQuatKey * pQuatKey)
{
	assert(uiNumKey > 0);

	for (_uint i = 0; i < uiNumKey - 1; ++i)
	{
		if (fAnimationTime < (_float)pQuatKey[i + 1].mTime)
			return i;
	}

	assert(0);

	return S_OK;
}

_matrix CAniCtrl::Convert_AiToMat4(const aiMatrix4x4 & m)
{
	_matrix matResult;

	matResult.m[0][0] = m[0][0];	matResult.m[0][1] = m[1][0];	matResult.m[0][2] = m[2][0];	matResult.m[0][3] = m[3][0];
	matResult.m[1][0] = m[0][1];	matResult.m[1][1] = m[1][1];	matResult.m[1][2] = m[2][1];	matResult.m[1][3] = m[3][1];
	matResult.m[2][0] = m[0][2];	matResult.m[2][1] = m[1][2];	matResult.m[2][2] = m[2][2];	matResult.m[2][3] = m[3][2];
	matResult.m[3][0] = m[0][3];	matResult.m[3][1] = m[1][3];	matResult.m[3][2] = m[2][3];	matResult.m[3][3] = m[3][3];

	return matResult;
}

_matrix CAniCtrl::Convert_AiToMat3(const aiMatrix3x3 & m)
{
	_matrix matResult;

	matResult.m[0][0] = m[0][0];	matResult.m[0][1] = m[1][0];	matResult.m[0][2] = m[2][0];	matResult.m[0][3] = 0;
	matResult.m[1][0] = m[0][1];	matResult.m[1][1] = m[1][1];	matResult.m[1][2] = m[2][1];	matResult.m[1][3] = 0;
	matResult.m[2][0] = m[0][2];	matResult.m[2][1] = m[1][2];	matResult.m[2][2] = m[2][2];	matResult.m[2][3] = 0;
	matResult.m[3][0] = 0;			matResult.m[3][1] = 0;			matResult.m[3][2] = 0;			matResult.m[3][3] = 1.0f;

	return matResult;
}

CComponent * CAniCtrl::Clone()
{
	return new CAniCtrl(*this);
}

CAniCtrl * CAniCtrl::Create(const aiScene * pScece)
{
	CAniCtrl* pInstance = new CAniCtrl(pScece);

	if (FAILED(pInstance->Ready_AniCtrl()))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CAniCtrl::Free()
{
	for (auto& BoneMap : m_vecBoneNameMap)
		BoneMap.clear();
	
	m_vecBoneNameMap.clear();
	m_vecBoneNameMap.shrink_to_fit();


	m_vecBoneDesc->clear();
	m_vecBoneDesc->shrink_to_fit();
	Safe_Delete(m_vecBoneDesc);

	m_vecBoneTransform->clear();
	m_vecBoneTransform->shrink_to_fit();
	Safe_Delete(m_vecBoneTransform);

	m_vecSkinningMatrix->clear();
	m_vecSkinningMatrix->shrink_to_fit();
	Safe_Delete(m_vecSkinningMatrix);

	if (!m_bIsClone)
	{
		for (auto& MyPair : m_mapNodeHierarchy)
		{
			Safe_Delete(MyPair.second);
		}
		m_mapNodeHierarchy.clear();

		m_pScene = nullptr;
	}

}
