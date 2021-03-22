#include "VIMesh.h"

#include "GraphicDevice.h"
#include "ShaderMesh.h"
#include "ShaderShadow.h"
#include "AniCtrl.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "ComponentMgr.h"
#include "DescriptorHeapMgr.h"
#include "TimeMgr.h"
USING(Engine)


CVIMesh::CVIMesh(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CComponent(pGraphicDevice, pCommandList)
{
}

CVIMesh::CVIMesh(const CVIMesh & rhs)
	: CComponent(rhs)
	, m_vecVB_CPU(rhs.m_vecVB_CPU)
	, m_vecIB_CPU(rhs.m_vecIB_CPU)
	, m_vecVB_GPU(rhs.m_vecVB_GPU)
	, m_vecIB_GPU(rhs.m_vecIB_GPU)
	, m_uiVertexByteStride(rhs.m_uiVertexByteStride)
	, m_vecVB_ByteSize(rhs.m_vecVB_ByteSize)
	, m_vecIB_ByteSize(rhs.m_vecIB_ByteSize)
	, m_IndexFormat(rhs.m_IndexFormat)
	, m_vecSubMeshGeometry(rhs.m_vecSubMeshGeometry)
	, m_wstrFileName(rhs.m_wstrFileName)
	, m_wstrFilePath(rhs.m_wstrFilePath)
	, m_vecDiffResource(rhs.m_vecDiffResource)
	, m_vecNormResource(rhs.m_vecNormResource)
	, m_vecSpecResource(rhs.m_vecSpecResource)
	, m_vecMeshEntry(rhs.m_vecMeshEntry)
	, m_pTexDescriptorHeap(rhs.m_pTexDescriptorHeap)
	, m_iSumVertex(rhs.m_iSumVertex)
	, m_vCenter(rhs.m_vCenter)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
{
	m_pScene = rhs.m_pScene;

	for (auto& iter : m_vecVB_CPU)
	{
		if (nullptr != iter)
			iter->AddRef();
	}
	for (auto& iter : m_vecIB_CPU)
	{
		if (nullptr != iter)
			iter->AddRef();
	}

	for (auto& iter : m_vecVB_GPU)
	{
		if (nullptr != iter)
			iter->AddRef();
	}
	for (auto& iter : m_vecIB_GPU)
	{
		if (nullptr != iter)
			iter->AddRef();
	}

	for (auto& iter : m_vecDiffResource)
	{
		if (nullptr != iter)
			iter->AddRef();
	}
	for (auto& iter : m_vecNormResource)
	{
		if (nullptr != iter)
			iter->AddRef();
	}
	for (auto& iter : m_vecSpecResource)
	{
		if (nullptr != iter)
			iter->AddRef();
	}

	if (nullptr != m_pTexDescriptorHeap)
		m_pTexDescriptorHeap->AddRef();

}

D3D12_VERTEX_BUFFER_VIEW CVIMesh::Get_VertexBufferView(const _uint & iSubMeshIdx) const
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
	VertexBufferView.BufferLocation = m_vecVB_GPU[iSubMeshIdx]->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes	= m_uiVertexByteStride;
	VertexBufferView.SizeInBytes	= m_vecVB_ByteSize[iSubMeshIdx];

	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW CVIMesh::Get_IndexBufferView(const _uint & iSubMeshIdx) const
{
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;
	IndexBufferView.BufferLocation	= m_vecIB_GPU[iSubMeshIdx]->GetGPUVirtualAddress();
	IndexBufferView.Format			= m_IndexFormat;
	IndexBufferView.SizeInBytes		= m_vecIB_ByteSize[iSubMeshIdx];

	return IndexBufferView;
}

HRESULT CVIMesh::Ready_Component(const aiScene * pScene, wstring wstrFileName, wstring wstrPath)
{
	m_pScene		= pScene;
	m_wstrFileName	= wstrFileName;
	m_wstrFilePath	= wstrPath;
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	CGraphicDevice::Get_Instance()->Begin_ResetCmdList(CMDID::CMD_LOADING);

	/*__________________________________________________________________________________________________________
	- Scene에 담겨있는 SubsetMesh의 개수만큼 MeshEntry의 크기를 잡는다.
	____________________________________________________________________________________________________________*/
	m_vecMeshEntry.resize(m_pScene->mNumMeshes);

#ifdef ENGINE_LOG
	COUT_STR("");
	wcout << L"[ Mesh File Path : " << wstrPath << L" ]" << endl;
	COUT_STR("Num SubsetMesh  " << m_pScene->mNumMeshes);
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		COUT_STR("Bone Name\t" << m_pScene->mMeshes[i]->mName.C_Str());
		COUT_STR("Num Bone\t" << m_pScene->mMeshes[i]->mNumBones);
		COUT_STR("Num Vertex\t" << m_pScene->mMeshes[i]->mNumVertices);
		COUT_STR("Num Face\t" << m_pScene->mMeshes[i]->mNumFaces);
		COUT_STR("Num Material\t" << m_pScene->mNumMaterials);
		COUT_STR("");
	}
#endif

	for (size_t i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		/*__________________________________________________________________________________________________________
		- Scene에서 Material, Vertex, Index 정보를 얻어온다.
		m_pScene->mMeshes[i]	: SubsetMesh 정보.
		____________________________________________________________________________________________________________*/
		m_vecMeshEntry[i].MaterialIndex	= m_pScene->mMeshes[i]->mMaterialIndex;
		m_vecMeshEntry[i].NumIndices	= m_pScene->mMeshes[i]->mNumFaces * 3;
		m_vecMeshEntry[i].BaseVertex	= 0;
		m_vecMeshEntry[i].BaseIndex		= 0;
		
		_int iNumVertices				= m_pScene->mMeshes[i]->mNumVertices;
		_int iNumIndices				= m_vecMeshEntry[i].NumIndices;

		/*__________________________________________________________________________________________________________
		- Vertex Buffer와 Index Buffer를 생성한다.
		____________________________________________________________________________________________________________*/
		vector<VTXMESH> vecVertex;
		vector<_uint>	vecIndex;

		// aiMesh : assimp의 Mesh 구조체.
		const aiMesh* pAiMesh = m_pScene->mMeshes[i];

		FAILED_CHECK_RETURN(Ready_Mesh(pAiMesh, vecVertex, vecIndex), E_FAIL);

		// vector 사이즈 초기화.
		m_vecIB_ByteSize.resize(m_vecMeshEntry.size());
		m_vecVB_ByteSize.resize(m_vecMeshEntry.size());
		m_vecIB_CPU.resize(m_vecMeshEntry.size());
		m_vecIB_GPU.resize(m_vecMeshEntry.size());
		m_vecVB_CPU.resize(m_vecMeshEntry.size());
		m_vecVB_GPU.resize(m_vecMeshEntry.size());
		m_vecIB_Uploader.resize(m_vecMeshEntry.size());;
		m_vecVB_Uploader.resize(m_vecMeshEntry.size());
		m_vecSubMeshGeometry.resize(m_vecMeshEntry.size());

		const _int uiVB_ByteSize = (_uint)vecVertex.size() * sizeof(VTXMESH);
		const _int uiIB_ByteSize = (_uint)vecIndex.size() * sizeof(_uint);

		/*__________________________________________________________________________________________________________
		[ Vertex & Index BUffer 복사 ]
		____________________________________________________________________________________________________________*/
		FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_vecVB_CPU[i]), E_FAIL);
		CopyMemory(m_vecVB_CPU[i]->GetBufferPointer(), vecVertex.data(), uiVB_ByteSize);

		FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_vecIB_CPU[i]), E_FAIL);
		CopyMemory(m_vecIB_CPU[i]->GetBufferPointer(), vecIndex.data(), uiIB_ByteSize);

		/*__________________________________________________________________________________________________________
		[ Vertex & Index Buffer 생성 ]
		____________________________________________________________________________________________________________*/
		m_vecVB_GPU[i] = Create_DefaultBuffer(vecVertex.data(), uiVB_ByteSize, m_vecVB_Uploader[i]);
		NULL_CHECK_RETURN(m_vecVB_GPU[i], E_FAIL);

		m_vecIB_GPU[i] = Create_DefaultBuffer(vecIndex.data(), uiIB_ByteSize, m_vecIB_Uploader[i]);
		NULL_CHECK_RETURN(m_vecIB_GPU[i], E_FAIL);

		m_uiVertexByteStride	= sizeof(VTXMESH);
		m_vecVB_ByteSize[i]		= uiVB_ByteSize;
		m_vecIB_ByteSize[i]		= uiIB_ByteSize;
		m_IndexFormat			= DXGI_FORMAT_R32_UINT;

		m_vecSubMeshGeometry[i].uiIndexCount			= (_uint)vecIndex.size();
		m_vecSubMeshGeometry[i].uiStartIndexLocation	= 0;
		m_vecSubMeshGeometry[i].iBaseVertexLocation		= 0;
	}

	m_vCenter = m_vCenter / m_iSumVertex;


	FAILED_CHECK_RETURN(Ready_Texture(), E_FAIL);

	// CGraphicDevice::Get_Instance()->Wait_ForGpuComplete();
	CGraphicDevice::Get_Instance()->End_ResetCmdList(CMDID::CMD_LOADING);

	// Create Texture DescriptorHeap.
	FAILED_CHECK_RETURN(Create_TextureDescriptorHeap(), E_FAIL);

	// Add DescriptorHeap
	CDescriptorHeapMgr::Get_Instance()->Add_DescriptorHeap(m_wstrFileName, m_pTexDescriptorHeap);

	return S_OK;
}

HRESULT CVIMesh::Ready_Mesh(const aiMesh * pAiMesh, 
							vector<VTXMESH>& vecVertex,
							vector<_uint>& vecIndex)
{
	/*__________________________________________________________________________________________________________
	- Mesh는 vertex배열과 bone배열을 가지고 있다. 
	- vertex배열의 각 원소는 Mesh의 정점들의 위치, 노말, 텍스쳐UV 좌표 등을 가지고 있다.

	- 주의해야 할 점은 vertex에 담긴 위치 정보가 Local 좌표계에서 정의된 정점이다. 
	이 부분은 나중에 Animation을 다룰 때 중요하다.

	- bone은 영향을 끼치는 vertex들의 index를 가지고 있다. 또 각 veretx에 얼마만큼의 영향을 끼치는지에 대한
	가중치 값을 가지고 있다.
	____________________________________________________________________________________________________________*/

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	VTXMESH vtxMesh;
	ZeroMemory(&vtxMesh, sizeof(VTXMESH));


	for (_uint i = 0; i < pAiMesh->mNumVertices; ++i)
	{
		// - Local Position 정보 입력.
		vtxMesh.vPos = _vec3(pAiMesh->mVertices[i].x, pAiMesh->mVertices[i].y, pAiMesh->mVertices[i].z);

		// - Normal 정보가 있다면, Normal값 입력.
		if (pAiMesh->HasNormals())
		{
			// 머리랑 몸통의 Normal이 반대임. (임시방편)
			if (!strcmp("Popori_F_Face10_Skel", pAiMesh->mName.C_Str()) ||
				!strcmp("Popori_F_Hair12B_Skel", pAiMesh->mName.C_Str()))
			{
				vtxMesh.vNormal = _vec3(pAiMesh->mNormals[i].x, pAiMesh->mNormals[i].y, pAiMesh->mNormals[i].z) * -1.0f;
			}
			else
				vtxMesh.vNormal = _vec3(pAiMesh->mNormals[i].x, pAiMesh->mNormals[i].y, pAiMesh->mNormals[i].z);

			//vtxMesh.vNormal = _vec3(pAiMesh->mNormals[i].x, pAiMesh->mNormals[i].y, pAiMesh->mNormals[i].z);
		}
		// - Texture 정보가 있다면, TexUV값 입력.
		if (pAiMesh->HasTextureCoords(0))
			vtxMesh.vTexUV = _vec2(pAiMesh->mTextureCoords[0][i].x, pAiMesh->mTextureCoords[0][i].y);

		vecVertex.emplace_back(vtxMesh);

		// - Center, Min, Max 정보 입력.
		m_vCenter += vtxMesh.vPos;

		if (m_vMin.x > vtxMesh.vPos.x)
			m_vMin.x = vtxMesh.vPos.x;
		if (m_vMin.y > vtxMesh.vPos.y)
			m_vMin.y = vtxMesh.vPos.y;
		if (m_vMin.z > vtxMesh.vPos.z)
			m_vMin.z = vtxMesh.vPos.z;

		if (m_vMax.x < vtxMesh.vPos.x)
			m_vMax.x = vtxMesh.vPos.x;
		if (m_vMax.y < vtxMesh.vPos.y)
			m_vMax.y = vtxMesh.vPos.y;
		if (m_vMax.z < vtxMesh.vPos.z)
			m_vMax.z = vtxMesh.vPos.z;

	}

	m_iSumVertex += pAiMesh->mNumVertices;


	/*__________________________________________________________________________________________________________
	[ Index Buffer ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < pAiMesh->mNumFaces; ++i)
	{
		aiFace Face = pAiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);

		vecIndex.emplace_back(Face.mIndices[0]);
		vecIndex.emplace_back(Face.mIndices[1]);
		vecIndex.emplace_back(Face.mIndices[2]);
	}

	/*__________________________________________________________________________________________________________
	[ Bone ]
	- Mesh의 Bone과 Vertex를 연결.
	- 미리 정의한 Vertices 배열에 Assimp에서 읽은 Bone 정보를 채워 넣는 코드이다.
	- for문을 보면, Mesh의 Bone 개수만큼 순회를 하고, 각 Bone 마다 영향을 끼치는 정점의 개수만큼 순회를 한다.
	- loop - k 에서 8번 순회를 하는 것은 정점 별로 영향을 받는 Bone의 Index를 순차적으로 채우기 위함이다.
	Weight 값이 0이면 값을 채워넣고 아니면 그냥 넘어가는데, default값이 0이기 때문이다.

	- 이렇게 채워진 vertices정보는 나중에 Rendering할 때 Vertex Shader로 넘어가게 된다.
	____________________________________________________________________________________________________________*/
	_int iNumBone	= 0;

	// Mesh의 모든 Bone을 순회. Bone 정보가 없다면 X
	for (_uint i = 0; i < pAiMesh->mNumBones; ++i)
	{
		_uint iBoneIdx = iNumBone;
		iNumBone += 1;

		// Bone의 가중치 계산. (Bone이 영향을 주는 정점들을 순회).
		for (_uint j = 0; j < pAiMesh->mBones[i]->mNumWeights; ++j)
		{
			_uint	VtxIdx	= pAiMesh->mBones[i]->mWeights[j].mVertexId;
			_float	Weight	= pAiMesh->mBones[i]->mWeights[j].mWeight;	

			// 하나의 정점이 영향을 받을 수 있는 뼈의 개수는 최대 8개.
			for (_uint k = 0; k < 8; k++)
			{
				// 영향을 받는 정점이, 얼마나 뼈로부터 영향을 받는지 가중치를 넣어준다.
				_uint VectorIdx		= k / 4;	// vector의 index
				_uint ElementIdx	= k % 4;	// 각 vector의 원소 index

				if (ElementIdx == 0)
				{
					if (vecVertex[VtxIdx].arrBoneWeight[VectorIdx].x == 0.0f)
					{
						vecVertex[VtxIdx].arrBoneID[VectorIdx].x		= (_float)iBoneIdx;
						vecVertex[VtxIdx].arrBoneWeight[VectorIdx].x	= Weight;
						break;
					}
				}
				if (ElementIdx == 1)
				{
					if (vecVertex[VtxIdx].arrBoneWeight[VectorIdx].y == 0.0f)
					{
						vecVertex[VtxIdx].arrBoneID[VectorIdx].y		= (_float)iBoneIdx;
						vecVertex[VtxIdx].arrBoneWeight[VectorIdx].y	= Weight;
						break;
					}

				}
				if (ElementIdx == 2)
				{
					if (vecVertex[VtxIdx].arrBoneWeight[VectorIdx].z == 0.0f)
					{
						vecVertex[VtxIdx].arrBoneID[VectorIdx].z		= (_float)iBoneIdx;
						vecVertex[VtxIdx].arrBoneWeight[VectorIdx].z	= Weight;
						break;
					}

				}
				if (ElementIdx == 3)
				{
					if (vecVertex[VtxIdx].arrBoneWeight[VectorIdx].w == 0.0f)
					{
						vecVertex[VtxIdx].arrBoneID[VectorIdx].w		= (_float)iBoneIdx;
						vecVertex[VtxIdx].arrBoneWeight[VectorIdx].w	= Weight;
						break;
					}
				}

			}	// loop - k
		}	// loop - j
	}	// loop - i

	return S_OK;
}

HRESULT CVIMesh::Ready_Texture()
{
	/*__________________________________________________________________________________________________________
	[ Texturing ]
	____________________________________________________________________________________________________________*/
	if (!m_pScene->HasMaterials())
		return S_OK;


	for (_uint i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		// aiString strTextureName;
		const aiMaterial* Material = m_pScene->mMaterials[i];

		// Texture 정보가 있음을 설정한다.
		m_vecMeshEntry[i].blsTexture = true;

		// Diffuse
		aiString strDiffuse;
		if (AI_SUCCESS == Material->GetTexture(aiTextureType_DIFFUSE, 0, &strDiffuse))
		{
			wstring wstrTextureName;
			wstring wstrPathName;
			string	strName = strDiffuse.C_Str();

			wstrTextureName.assign(strName.begin(), strName.end());
			wstrPathName = m_wstrFilePath + wstrTextureName;

			ComPtr<ID3D12Resource> pResource	= nullptr;
			ComPtr<ID3D12Resource> pUpload		= nullptr;

			FAILED_CHECK_RETURN(CreateDDSTextureFromFile12(m_pGraphicDevice,
														   CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING), 
														   wstrPathName.c_str(),
														   pResource,
														   pUpload), E_FAIL);

			m_vecDiffResource.emplace_back(pResource);
			m_vecDiffUpload.emplace_back(pUpload);
		}

		// Normal
		aiString strNormal;
		if (AI_SUCCESS == Material->GetTexture(aiTextureType_DIFFUSE, 1, &strNormal))
		{
			wstring wstrTextureName;
			wstring wstrPathName;
			string	strName = strNormal.C_Str();

			wstrTextureName.assign(strName.begin(), strName.end());
			wstrPathName = m_wstrFilePath + wstrTextureName;

			ComPtr<ID3D12Resource> pResource	= nullptr;
			ComPtr<ID3D12Resource> pUpload		= nullptr;

			FAILED_CHECK_RETURN(CreateDDSTextureFromFile12(m_pGraphicDevice,
														   CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING), 
														   wstrPathName.c_str(),
														   pResource,
														   pUpload), E_FAIL);

			m_vecNormResource.emplace_back(pResource);
			m_vecNormUpload.emplace_back(pUpload);
		}

		// Specular
		aiString strSpecular;
		if (AI_SUCCESS == Material->GetTexture(aiTextureType_SPECULAR, 0, &strSpecular))
		{
			wstring wstrTextureName;
			wstring wstrPathName;
			string	strName = strSpecular.C_Str();

			wstrTextureName.assign(strName.begin(), strName.end());
			wstrPathName = m_wstrFilePath + wstrTextureName;

			ComPtr<ID3D12Resource> pResource	= nullptr;
			ComPtr<ID3D12Resource> pUpload		= nullptr;

			FAILED_CHECK_RETURN(CreateDDSTextureFromFile12(m_pGraphicDevice,
														   CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING), 
														   wstrPathName.c_str(),
														   pResource,
														   pUpload), E_FAIL);

			m_vecSpecResource.emplace_back(pResource);
			m_vecSpecUpload.emplace_back(pUpload);
		}

	}

	return S_OK;
}

HRESULT CVIMesh::Create_TextureDescriptorHeap()
{
	/*__________________________________________________________________________________________________________
	[ SRV 서술자 힙 ]
	- 텍스처 자원을 성공적으로 생성했다면, 그 다음으로 해야할 일은 그에 대한 SRV 서술자를 생성하는 것이다. 
	그래야 자원을 셰이더 프로그램이 사용할 루트 서명 매개변수 슬롯에 설정할 수 있다.
	____________________________________________________________________________________________________________*/
	m_uiSubsetMeshSize = _uint(m_vecDiffResource.size());

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc	= {};
	SRV_HeapDesc.NumDescriptors = m_uiSubsetMeshSize * (TEXTURE_END )  + 1 + 1;	// 텍스처의 개수 만큼 설정. (+ 1 = ShadowDepth +1 Dissolve Texture)
	SRV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	/*__________________________________________________________________________________________________________
	[ SRV 서술자 ]
	- SRV 힙을 생성한 다음에는 실제 서술자들을 생성해야 한다.
	____________________________________________________________________________________________________________*/
	// 힙의 시작을 가리키는 포인터를 얻는다.
	CD3DX12_CPU_DESCRIPTOR_HANDLE SRV_DescriptorHandle(m_pTexDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// Tex Diffuse
	for (_uint i = 0; i < m_uiSubsetMeshSize; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format							= m_vecDiffResource[i]->GetDesc().Format;
		SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip		= 0;
		SRV_Desc.Texture2D.MipLevels			= m_vecDiffResource[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(m_vecDiffResource[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	// Tex Normal
	for (_uint i = 0; i < m_uiSubsetMeshSize; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format							= m_vecNormResource[i]->GetDesc().Format;
		SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip		= 0;
		SRV_Desc.Texture2D.MipLevels			= m_vecNormResource[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(m_vecNormResource[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	// Tex Specular
	for (_uint i = 0; i < m_uiSubsetMeshSize; ++i)
	{
		if (m_vecSpecResource.empty())
			m_vecSpecResource = m_vecNormResource;

		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format = m_vecSpecResource[i]->GetDesc().Format;
		SRV_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip = 0;
		SRV_Desc.Texture2D.MipLevels = m_vecSpecResource[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(m_vecSpecResource[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// 힙의 다음 서술자로 넘어간다.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	// Tex ShadowDepth
	vector<ComPtr<ID3D12Resource>> vecShadowDepthTarget = CRenderer::Get_Instance()->Get_TargetShadowDepth()->Get_TargetTexture();

	D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
	SRV_Desc.Format							= vecShadowDepthTarget[0]->GetDesc().Format;
	SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
	SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRV_Desc.Texture2D.MostDetailedMip		= 0;
	SRV_Desc.Texture2D.MipLevels			= vecShadowDepthTarget[0]->GetDesc().MipLevels;
	SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

	m_pGraphicDevice->CreateShaderResourceView(vecShadowDepthTarget[0].Get(), &SRV_Desc, SRV_DescriptorHandle);

	// 힙의 다음 서술자로 넘어간다.
	SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());

	//Dissol
	ComPtr<ID3D12Resource> TexDissolve = static_cast<CTexture*>
		(CComponentMgr::Get_Instance()->Get_Component(L"ResourcePrototype_TextureDissolve", ID_STATIC))->Get_Texture().front();

	SRV_Desc = {};
	SRV_Desc.Format = TexDissolve->GetDesc().Format;
	SRV_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	SRV_Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRV_Desc.Texture2D.MostDetailedMip = 0;
	SRV_Desc.Texture2D.MipLevels = TexDissolve->GetDesc().MipLevels;
	SRV_Desc.Texture2D.ResourceMinLODClamp = 0.0f;

	m_pGraphicDevice->CreateShaderResourceView(TexDissolve.Get(), &SRV_Desc, SRV_DescriptorHandle);

	return S_OK;
}

void CVIMesh::Release_UploadBuffer()
{
	for (auto& pUploadBuffer : m_vecIB_Uploader)
		Safe_Release(pUploadBuffer);

	for (auto& pUploadBuffer : m_vecVB_Uploader)
		Safe_Release(pUploadBuffer);


	for (auto& pUploadBuffer : m_vecDiffUpload)
		Safe_Release(pUploadBuffer);
	
	for (auto& pUploadBuffer : m_vecNormUpload)
		Safe_Release(pUploadBuffer);

	for (auto& pUploadBuffer : m_vecSpecUpload)
		Safe_Release(pUploadBuffer);

}

ID3D12Resource * CVIMesh::Create_DefaultBuffer(const void * InitData,
											   const UINT64 & uiByteSize,
											   ID3D12Resource *& pUploadBuffer)
{
	ID3D12Resource* pDefaultBuffer = nullptr;

	/*__________________________________________________________________________________________________________
	- 실제 기본 버퍼 자원을 생성한다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
																  D3D12_HEAP_FLAG_NONE,
																  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																  D3D12_RESOURCE_STATE_COMMON,
																  nullptr,
																  IID_PPV_ARGS(&pDefaultBuffer)), 
																  NULL);

	/*__________________________________________________________________________________________________________
	- CPU 메모리의 자료를 기본 버퍼에 복사하려면, 임시 업로드 힙을 만들어야 한다.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																  D3D12_HEAP_FLAG_NONE,
																  &CD3DX12_RESOURCE_DESC::Buffer(uiByteSize),
																  D3D12_RESOURCE_STATE_GENERIC_READ,
																  nullptr,
																  IID_PPV_ARGS(&pUploadBuffer)), 
																  NULL);

	/*__________________________________________________________________________________________________________
	- 기본 버퍼에 복사할 자료를 서술한다.
	____________________________________________________________________________________________________________*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData		= InitData;
	subResourceData.RowPitch	= uiByteSize;
	subResourceData.SlicePitch	= subResourceData.RowPitch;

	/*__________________________________________________________________________________________________________
	기본 버퍼 자원으로의 자료 복사를 요청한다.
	계략적으로 말하자면, 보조 함수 UpdateSubresources는 CPU 메모리를 임시 업로드 힙에 복사하고,
	ID3D12CommandList::CopySubresourceRegion을 이용해서 임시 업로드 힙의 자료를 mBuffer에 복사한다.

	[ 주의 ]
	- 위의 함수 호출 이후에도, UploadBuffer를 계속 유지해야 한다.
	- 실제로 복사를 수행하는 명령 목록이 아직 실행되지 않았기 때문이다.
	- 복사가 완료되었음이 확실해진 후에 호출자가 UploadBuffer를 해제하면 된다.
	____________________________________________________________________________________________________________*/
	CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING)->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer,
																																  D3D12_RESOURCE_STATE_COMMON,
																																  D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING), pDefaultBuffer, pUploadBuffer, 0, 0, 1, &subResourceData);

	CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING)->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pDefaultBuffer,
																																  D3D12_RESOURCE_STATE_COPY_DEST,
																																  D3D12_RESOURCE_STATE_GENERIC_READ));

	return pDefaultBuffer;
}


void CVIMesh::Render_DynamicMesh(CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_MATRIX tCB_SkinningMatrix;
		ZeroMemory(&tCB_SkinningMatrix, sizeof(CB_SKINNING_MATRIX));

		for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
		{
			tCB_SkinningMatrix.matBoneOffset[j]		= CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset);
			tCB_SkinningMatrix.matBoneScale[j]		= CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale);
			tCB_SkinningMatrix.matBoneRotation[j]		= CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation);
			tCB_SkinningMatrix.matBoneTrans[j]		= CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans);
			tCB_SkinningMatrix.matParentTransform[j]	= CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform);
			tCB_SkinningMatrix.matRootTransform[j]	= CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform);
		}
		static_cast<CShaderMesh*>(pShader)->Get_UploadBuffer_SkinningMatrix()->CopyData(i, tCB_SkinningMatrix);


		pShader->Begin_Shader(m_pTexDescriptorHeap, i);
		Begin_Buffer(m_pCommandList, i);

		Render_Buffer(m_pCommandList, i);
	}
}



void CVIMesh::Render_StaticMesh(CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		pShader->Begin_Shader(m_pTexDescriptorHeap, i);
		Begin_Buffer(m_pCommandList, i);

		Render_Buffer(m_pCommandList, i);
	}
}

void CVIMesh::Render_DynamicMeshShadowDepth(CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_MATRIX tCB_SkinningMatrix;
		ZeroMemory(&tCB_SkinningMatrix, sizeof(CB_SKINNING_MATRIX));

		for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
		{
			tCB_SkinningMatrix.matBoneOffset[j]      = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset);
			tCB_SkinningMatrix.matBoneScale[j]       = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale);
			tCB_SkinningMatrix.matBoneRotation[j]    = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation);
			tCB_SkinningMatrix.matBoneTrans[j]       = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans);
			tCB_SkinningMatrix.matParentTransform[j] = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform);
			tCB_SkinningMatrix.matRootTransform[j]   = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform);
		}
		static_cast<CShaderShadow*>(pShader)->Get_UploadBuffer_SkinningMatrix()->CopyData(i, tCB_SkinningMatrix);

		pShader->Begin_Shader(m_pTexDescriptorHeap, i);
		Begin_Buffer(m_pCommandList, i);

		Render_Buffer(m_pCommandList, i);
	}
}

void CVIMesh::Render_StaticMeshShadowDepth(CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		pShader->Begin_Shader(m_pTexDescriptorHeap, i);
		Begin_Buffer(m_pCommandList, i);

		Render_Buffer(m_pCommandList, i);
	}
}

void CVIMesh::Render_DynamicMesh(ID3D12GraphicsCommandList * pCommandList,
								 const _int& iContextIdx,
								 CShader * pShader
								)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();

	if (m_fDeltaTime > m_fAfterImgMakeTime)
	{
		m_lstAFAlpha.emplace_back(_rgba(0.f, 0.f, 0.f, 1.f));
	}

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_MATRIX tCB_SkinningMatrix;
		ZeroMemory(&tCB_SkinningMatrix, sizeof(CB_SKINNING_MATRIX));

		for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
		{
			tCB_SkinningMatrix.matBoneOffset[j]      = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset);
			tCB_SkinningMatrix.matBoneScale[j]       = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale);
			tCB_SkinningMatrix.matBoneRotation[j]    = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation);
			tCB_SkinningMatrix.matBoneTrans[j]       = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans);
			tCB_SkinningMatrix.matParentTransform[j] = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform);
			tCB_SkinningMatrix.matRootTransform[j]   = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform);
		}
		static_cast<CShaderMesh*>(pShader)->Get_UploadBuffer_SkinningMatrix()->CopyData(i, tCB_SkinningMatrix);
		
		// Mesh AfterImage
		if (m_uiAfterImgSize)
		{
			if (m_fDeltaTime > m_fAfterImgMakeTime &&
				m_lstAFSkinningMatrix.size() < m_uiAfterImgSize * m_vecMeshEntry.size())
				m_lstAFSkinningMatrix.emplace_back(tCB_SkinningMatrix);
			/*
			if (m_lstAFSkinningMatrix.size() > m_uiAfterImgSize * m_vecMeshEntry.size())
				m_lstAFSkinningMatrix.pop_front();*/
		}

		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);
		Begin_Buffer(pCommandList, i);

		Render_Buffer(pCommandList, i);
	}

	if (m_uiAfterImgSize)
	{
		for (list<_rgba>::iterator& iterFade = m_lstAFAlpha.begin(); iterFade != m_lstAFAlpha.end();)
		{
			(*iterFade).w -= m_fAfterImgSubAlpha;
			if (0 > (*iterFade).w)
			{
				for(int i=0; i< m_vecMeshEntry.size(); ++i)
					m_lstAFSkinningMatrix.pop_front();
				iterFade = m_lstAFAlpha.erase(iterFade);
				continue;
			}
			else
				++iterFade;
		}
	}
}


void CVIMesh::Render_DynamicMeshAfterImage(CShader* pShader, const _uint& iAfterImgIdx)
{
	auto iter_begin = m_lstAFSkinningMatrix.begin();
	for (_int i = 0; i < iAfterImgIdx * m_vecMeshEntry.size(); ++i)
		++iter_begin;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i, ++iter_begin)
	{
		static_cast<CShaderMesh*>(pShader)->Get_UploadBuffer_AFSkinningMatrix()->CopyData(i + (m_vecMeshEntry.size() * iAfterImgIdx), *iter_begin);

		static_cast<CShaderMesh*>(pShader)->Begin_Shader(i,iAfterImgIdx);
		Begin_Buffer(m_pCommandList,i);
		Render_Buffer(m_pCommandList,i);
	}
}
void CVIMesh::Render_DynamicMeshAfterImage(ID3D12GraphicsCommandList* pCommandList, 
										   const _int& iContextIdx, 
										   CShader* pShader,
										   const _uint& iAfterImgIdx)
{
	auto iter_begin = m_lstAFSkinningMatrix.begin();
	for (_int i = 0; i < iAfterImgIdx * m_vecMeshEntry.size(); ++i)
		++iter_begin;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i, ++iter_begin)
	{
		static_cast<CShaderMesh*>(pShader)->Get_UploadBuffer_AFSkinningMatrix()->CopyData(i + (m_vecMeshEntry.size() * iAfterImgIdx), *iter_begin);

		static_cast<CShaderMesh*>(pShader)->Begin_Shader(pCommandList, 
														 iContextIdx,
														 i, 
														 iAfterImgIdx);
		Begin_Buffer(pCommandList, i);
		Render_Buffer(pCommandList, i);
	}
	
}

void CVIMesh::Render_StaticMesh(ID3D12GraphicsCommandList * pCommandList,
								const _int& iContextIdx,
								CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);
		Begin_Buffer(pCommandList, i);

		Render_Buffer(pCommandList, i);
	}
}

void CVIMesh::Render_DynamicMeshShadowDepth(ID3D12GraphicsCommandList * pCommandList, 
											const _int& iContextIdx,
											CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_MATRIX tCB_SkinningMatrix;
		ZeroMemory(&tCB_SkinningMatrix, sizeof(CB_SKINNING_MATRIX));

		for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
		{
			tCB_SkinningMatrix.matBoneOffset[j]      = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset);
			tCB_SkinningMatrix.matBoneScale[j]       = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale);
			tCB_SkinningMatrix.matBoneRotation[j]    = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation);
			tCB_SkinningMatrix.matBoneTrans[j]       = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans);
			tCB_SkinningMatrix.matParentTransform[j] = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform);
			tCB_SkinningMatrix.matRootTransform[j]   = CShader::Compute_MatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform);
		}
		static_cast<CShaderShadow*>(pShader)->Get_UploadBuffer_SkinningMatrix()->CopyData(i, tCB_SkinningMatrix);

		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);
		Begin_Buffer(pCommandList, i);

		Render_Buffer(pCommandList, i);
	}
}

void CVIMesh::Render_StaticMeshShadowDepth(ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx,
										   CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);
		Begin_Buffer(pCommandList, i);

		Render_Buffer(pCommandList, i);
	}
}

void CVIMesh::Begin_Buffer(ID3D12GraphicsCommandList* pCommandList, const _int& iSubMeshIdx)
{
	pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(iSubMeshIdx));
	pCommandList->IASetIndexBuffer(&Get_IndexBufferView(iSubMeshIdx));

	pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
}

void CVIMesh::Render_Buffer(ID3D12GraphicsCommandList* pCommandList, const _int& iSubMeshIdx)
{
	pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[iSubMeshIdx].uiIndexCount,
									   1,
									   0,
									   0,
									   0);
}

CComponent * CVIMesh::Clone()
{
	return new CVIMesh(*this);
}

CVIMesh * CVIMesh::Create(ID3D12Device * pGraphicDevice,
						  ID3D12GraphicsCommandList * pCommandList,
						  const aiScene * pScene, 
						  wstring wstrFileName,
						  wstring wstrPath)
{
	CVIMesh* pInstance = new CVIMesh(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Component(pScene, wstrFileName, wstrPath)))
		Safe_Release(pInstance);

	return pInstance;
}

void CVIMesh::Free()
{
	CComponent::Free();

	for (auto& iter : m_vecVB_CPU)
		Safe_Release(iter);
	for (auto& iter : m_vecIB_CPU)
		Safe_Release(iter);

	for (auto& iter : m_vecVB_GPU)
		Safe_Release(iter);
	for (auto& iter : m_vecIB_GPU)
		Safe_Release(iter);


	//for (auto& iter : m_vecDiffResource)
	//	iter->Release();

	//for (auto& iter : m_vecNormResource)
	//	iter->Release();

	//for (auto& iter : m_vecSpecResource)
	//	iter->Release();


	m_pAniCtrl = nullptr;

	if (!m_bIsClone)
	{
		m_pScene = nullptr;
	}

	Safe_Release(m_pTexDescriptorHeap);
}