#include "VIMesh.h"

#include "GraphicDevice.h"
#include "ShaderMesh.h"
#include "ShaderShadow.h"
#include "AniCtrl.h"
#include "Renderer.h"
#include "RenderTarget.h"

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

D3D12_VERTEX_BUFFER_VIEW CVIMesh::Get_VertexBufferView(const _uint & iIndex) const
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;

	VertexBufferView.BufferLocation = m_vecVB_GPU[iIndex]->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes	= m_uiVertexByteStride;
	VertexBufferView.SizeInBytes	= m_vecVB_ByteSize[iIndex];

	return VertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW CVIMesh::Get_IndexBufferView(const _uint & iIndex) const
{
	D3D12_INDEX_BUFFER_VIEW IndexBufferView;

	IndexBufferView.BufferLocation	= m_vecIB_GPU[iIndex]->GetGPUVirtualAddress();
	IndexBufferView.Format			= m_IndexFormat;
	IndexBufferView.SizeInBytes		= m_vecIB_ByteSize[iIndex];

	return IndexBufferView;
}

HRESULT CVIMesh::Ready_Component(const aiScene * pScene, wstring wstrPath)
{
	m_pScene		= pScene;
	m_wstrFilePath	= wstrPath;
	NULL_CHECK_RETURN(m_pScene, E_FAIL);

	CGraphicDevice::Get_Instance()->Begin_ResetCmdList(CMDID::CMD_LOADING);

	/*__________________________________________________________________________________________________________
	- Scene�� ����ִ� SubsetMesh�� ������ŭ MeshEntry�� ũ�⸦ ��´�.
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
		- Scene���� Material, Vertex, Index ������ ���´�.
		m_pScene->mMeshes[i]	: SubsetMesh ����.
		____________________________________________________________________________________________________________*/
		m_vecMeshEntry[i].MaterialIndex	= m_pScene->mMeshes[i]->mMaterialIndex;
		m_vecMeshEntry[i].NumIndices	= m_pScene->mMeshes[i]->mNumFaces * 3;
		m_vecMeshEntry[i].BaseVertex	= 0;
		m_vecMeshEntry[i].BaseIndex		= 0;

		_int iNumVertices				= m_pScene->mMeshes[i]->mNumVertices;
		_int iNumIndices				= m_vecMeshEntry[i].NumIndices;

		/*__________________________________________________________________________________________________________
		- Vertex Buffer�� Index Buffer�� �����Ѵ�.
		____________________________________________________________________________________________________________*/
		vector<VTXMESH> vecVertex;
		vector<_uint>	vecIndex;

		// aiMesh : assimp�� Mesh ����ü.
		const aiMesh* pAiMesh = m_pScene->mMeshes[i];

		FAILED_CHECK_RETURN(Ready_Mesh(pAiMesh, vecVertex, vecIndex), E_FAIL);

		// vector ������ �ʱ�ȭ.
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
		[ Vertex & Index BUffer ���� ]
		____________________________________________________________________________________________________________*/
		FAILED_CHECK_RETURN(D3DCreateBlob(uiVB_ByteSize, &m_vecVB_CPU[i]), E_FAIL);
		CopyMemory(m_vecVB_CPU[i]->GetBufferPointer(), vecVertex.data(), uiVB_ByteSize);

		FAILED_CHECK_RETURN(D3DCreateBlob(uiIB_ByteSize, &m_vecIB_CPU[i]), E_FAIL);
		CopyMemory(m_vecIB_CPU[i]->GetBufferPointer(), vecIndex.data(), uiIB_ByteSize);

		/*__________________________________________________________________________________________________________
		[ Vertex & Index Buffer ���� ]
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

	CGraphicDevice::Get_Instance()->Wait_ForGpuComplete();
	CGraphicDevice::Get_Instance()->End_ResetCmdList(CMDID::CMD_LOADING);

	// Create Texture DescriptorHeap.
	FAILED_CHECK_RETURN(Create_TextureDescriptorHeap(), E_FAIL);

	return S_OK;
}

HRESULT CVIMesh::Ready_Mesh(const aiMesh * pAiMesh, 
							vector<VTXMESH>& vecVertex,
							vector<_uint>& vecIndex)
{
	/*__________________________________________________________________________________________________________
	- Mesh�� vertex�迭�� bone�迭�� ������ �ִ�. 
	- vertex�迭�� �� ���Ҵ� Mesh�� �������� ��ġ, �븻, �ؽ���UV ��ǥ ���� ������ �ִ�.

	- �����ؾ� �� ���� vertex�� ��� ��ġ ������ Local ��ǥ�迡�� ���ǵ� �����̴�. 
	�� �κ��� ���߿� Animation�� �ٷ� �� �߿��ϴ�.

	- bone�� ������ ��ġ�� vertex���� index�� ������ �ִ�. �� �� veretx�� �󸶸�ŭ�� ������ ��ġ������ ����
	����ġ ���� ������ �ִ�.
	____________________________________________________________________________________________________________*/

	/*__________________________________________________________________________________________________________
	[ Vertex Buffer ]
	____________________________________________________________________________________________________________*/
	VTXMESH vtxMesh;
	ZeroMemory(&vtxMesh, sizeof(VTXMESH));


	for (_uint i = 0; i < pAiMesh->mNumVertices; ++i)
	{
		// - Local Position ���� �Է�.
		vtxMesh.vPos = _vec3(pAiMesh->mVertices[i].x, pAiMesh->mVertices[i].y, pAiMesh->mVertices[i].z);

		// - Normal ������ �ִٸ�, Normal�� �Է�.
		if (pAiMesh->HasNormals())
		{
			// �Ӹ��� ������ Normal�� �ݴ���. (�ӽù���)
			if (!strcmp("Popori_F_Face10_Skel", pAiMesh->mName.C_Str()) ||
				!strcmp("Popori_F_Hair12B_Skel", pAiMesh->mName.C_Str()))
			{
				vtxMesh.vNormal = _vec3(pAiMesh->mNormals[i].x, pAiMesh->mNormals[i].y, pAiMesh->mNormals[i].z) * -1.0f;
			}
			else
				vtxMesh.vNormal = _vec3(pAiMesh->mNormals[i].x, pAiMesh->mNormals[i].y, pAiMesh->mNormals[i].z);

		}
		// - Texture ������ �ִٸ�, TexUV�� �Է�.
		if (pAiMesh->HasTextureCoords(0))
			vtxMesh.vTexUV = _vec2(pAiMesh->mTextureCoords[0][i].x, pAiMesh->mTextureCoords[0][i].y);

		vecVertex.emplace_back(vtxMesh);

		// - Center, Min, Max ���� �Է�.
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
	- Mesh�� Bone�� Vertex�� ����.
	- �̸� ������ Vertices �迭�� Assimp���� ���� Bone ������ ä�� �ִ� �ڵ��̴�.
	- for���� ����, Mesh�� Bone ������ŭ ��ȸ�� �ϰ�, �� Bone ���� ������ ��ġ�� ������ ������ŭ ��ȸ�� �Ѵ�.
	- loop - k ���� 8�� ��ȸ�� �ϴ� ���� ���� ���� ������ �޴� Bone�� Index�� ���������� ä��� �����̴�.
	Weight ���� 0�̸� ���� ä���ְ� �ƴϸ� �׳� �Ѿ�µ�, default���� 0�̱� �����̴�.

	- �̷��� ä���� vertices������ ���߿� Rendering�� �� Vertex Shader�� �Ѿ�� �ȴ�.
	____________________________________________________________________________________________________________*/
	_int iNumBone	= 0;

	// Mesh�� ��� Bone�� ��ȸ. Bone ������ ���ٸ� X
	for (_uint i = 0; i < pAiMesh->mNumBones; ++i)
	{
		_uint iBoneIdx = iNumBone;
		iNumBone += 1;

		// Bone�� ����ġ ���. (Bone�� ������ �ִ� �������� ��ȸ).
		for (_uint j = 0; j < pAiMesh->mBones[i]->mNumWeights; ++j)
		{
			_uint	VtxIdx	= pAiMesh->mBones[i]->mWeights[j].mVertexId;
			_float	Weight	= pAiMesh->mBones[i]->mWeights[j].mWeight;	

			// �ϳ��� ������ ������ ���� �� �ִ� ���� ������ �ִ� 8��.
			for (_uint k = 0; k < 8; k++)
			{
				// ������ �޴� ������, �󸶳� ���κ��� ������ �޴��� ����ġ�� �־��ش�.
				_uint VectorIdx		= k / 4;	// vector�� index
				_uint ElementIdx	= k % 4;	// �� vector�� ���� index

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


	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		// aiString strTextureName;
		const aiMaterial* Material = m_pScene->mMaterials[i];

		// Texture ������ ������ �����Ѵ�.
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
	[ SRV ������ �� ]
	- �ؽ�ó �ڿ��� ���������� �����ߴٸ�, �� �������� �ؾ��� ���� �׿� ���� SRV �����ڸ� �����ϴ� ���̴�. 
	�׷��� �ڿ��� ���̴� ���α׷��� ����� ��Ʈ ���� �Ű����� ���Կ� ������ �� �ִ�.
	____________________________________________________________________________________________________________*/
	m_uiSubsetMeshSize = _uint(m_vecDiffResource.size());

	D3D12_DESCRIPTOR_HEAP_DESC SRV_HeapDesc	= {};
	SRV_HeapDesc.NumDescriptors = m_uiSubsetMeshSize * (TEXTURE_END) + 1;	// �ؽ�ó�� ���� ��ŭ ����. (+ 1 = ShadowDepth)
	SRV_HeapDesc.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	SRV_HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateDescriptorHeap(&SRV_HeapDesc,
															   IID_PPV_ARGS(&m_pTexDescriptorHeap)),
															   E_FAIL);

	/*__________________________________________________________________________________________________________
	[ SRV ������ ]
	- SRV ���� ������ �������� ���� �����ڵ��� �����ؾ� �Ѵ�.
	____________________________________________________________________________________________________________*/
	// ���� ������ ����Ű�� �����͸� ��´�.
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

		// ���� ���� �����ڷ� �Ѿ��.
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

		// ���� ���� �����ڷ� �Ѿ��.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	// Tex Specular
	for (_uint i = 0; i < m_uiSubsetMeshSize; ++i)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
		SRV_Desc.Format							= m_vecSpecResource[i]->GetDesc().Format;
		SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
		SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRV_Desc.Texture2D.MostDetailedMip		= 0;
		SRV_Desc.Texture2D.MipLevels			= m_vecSpecResource[i]->GetDesc().MipLevels;
		SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

		m_pGraphicDevice->CreateShaderResourceView(m_vecSpecResource[i].Get(), &SRV_Desc, SRV_DescriptorHandle);

		// ���� ���� �����ڷ� �Ѿ��.
		SRV_DescriptorHandle.Offset(1, CGraphicDevice::Get_Instance()->Get_CBV_SRV_UAV_DescriptorSize());
	}

	// Tex ShadowDepth
	vector<ComPtr<ID3D12Resource>> vecShadowDepthTarget = CRenderer::Get_Instance()->Get_ShadowDepthTarget()->Get_TargetTexture();

	D3D12_SHADER_RESOURCE_VIEW_DESC SRV_Desc = {};
	SRV_Desc.Format							= vecShadowDepthTarget[0]->GetDesc().Format;
	SRV_Desc.ViewDimension					= D3D12_SRV_DIMENSION_TEXTURE2D;
	SRV_Desc.Shader4ComponentMapping		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRV_Desc.Texture2D.MostDetailedMip		= 0;
	SRV_Desc.Texture2D.MipLevels			= vecShadowDepthTarget[0]->GetDesc().MipLevels;
	SRV_Desc.Texture2D.ResourceMinLODClamp	= 0.0f;

	m_pGraphicDevice->CreateShaderResourceView(vecShadowDepthTarget[0].Get(), &SRV_Desc, SRV_DescriptorHandle);

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
	- ���� �⺻ ���� �ڿ��� �����Ѵ�.
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Default_HeapProperties;
	Default_HeapProperties.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	Default_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Default_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Default_HeapProperties.CreationNodeMask     = 1;
	Default_HeapProperties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC Default_ResourceDesc;
	Default_ResourceDesc.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	Default_ResourceDesc.Alignment				= 0;
	Default_ResourceDesc.Width					= uiByteSize;
	Default_ResourceDesc.Height					= 1;
	Default_ResourceDesc.DepthOrArraySize		= 1;
	Default_ResourceDesc.MipLevels				= 1;
	Default_ResourceDesc.Format					= DXGI_FORMAT_UNKNOWN;
	Default_ResourceDesc.SampleDesc.Count		= 1;
	Default_ResourceDesc.SampleDesc.Quality		= 0;
	Default_ResourceDesc.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Default_ResourceDesc.Flags					= D3D12_RESOURCE_FLAG_NONE;
	
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Default_HeapProperties,
																  D3D12_HEAP_FLAG_NONE,
																  &Default_ResourceDesc,
																  D3D12_RESOURCE_STATE_COMMON,
																  nullptr,
																  IID_PPV_ARGS(&pDefaultBuffer)), 
																  NULL);

	/*__________________________________________________________________________________________________________
	- CPU �޸��� �ڷḦ �⺻ ���ۿ� �����Ϸ���, �ӽ� ���ε� ���� ������ �Ѵ�.
	____________________________________________________________________________________________________________*/
	D3D12_HEAP_PROPERTIES Upload_HeapProperties;
	Upload_HeapProperties.Type                 = D3D12_HEAP_TYPE_UPLOAD;
	Upload_HeapProperties.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	Upload_HeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	Upload_HeapProperties.CreationNodeMask     = 1;
	Upload_HeapProperties.VisibleNodeMask      = 1;

	D3D12_RESOURCE_DESC Upload_ResourceDest;
	Upload_ResourceDest.Dimension				= D3D12_RESOURCE_DIMENSION_BUFFER;
	Upload_ResourceDest.Alignment				= 0;
	Upload_ResourceDest.Width					= uiByteSize;
	Upload_ResourceDest.Height					= 1;
	Upload_ResourceDest.DepthOrArraySize		= 1;
	Upload_ResourceDest.MipLevels				= 1;
	Upload_ResourceDest.Format					= DXGI_FORMAT_UNKNOWN;
	Upload_ResourceDest.SampleDesc.Count		= 1;
	Upload_ResourceDest.SampleDesc.Quality		= 0;
	Upload_ResourceDest.Layout					= D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	Upload_ResourceDest.Flags					= D3D12_RESOURCE_FLAG_NONE;
	
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&Upload_HeapProperties,
																  D3D12_HEAP_FLAG_NONE,
																  &Upload_ResourceDest,
																  D3D12_RESOURCE_STATE_GENERIC_READ,
																  nullptr,
																  IID_PPV_ARGS(&pUploadBuffer)), 
																  NULL);

	/*__________________________________________________________________________________________________________
	- �⺻ ���ۿ� ������ �ڷḦ �����Ѵ�.
	____________________________________________________________________________________________________________*/
	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData		= InitData;
	subResourceData.RowPitch	= uiByteSize;
	subResourceData.SlicePitch	= subResourceData.RowPitch;

	/*__________________________________________________________________________________________________________
	�⺻ ���� �ڿ������� �ڷ� ���縦 ��û�Ѵ�.
	�跫������ �����ڸ�, ���� �Լ� UpdateSubresources�� CPU �޸𸮸� �ӽ� ���ε� ���� �����ϰ�,
	ID3D12CommandList::CopySubresourceRegion�� �̿��ؼ� �ӽ� ���ε� ���� �ڷḦ mBuffer�� �����Ѵ�.
	____________________________________________________________________________________________________________*/
	D3D12_RESOURCE_BARRIER ResourceBarrier;
	ResourceBarrier.Type                    = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	ResourceBarrier.Flags                   = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ResourceBarrier.Transition.Subresource  = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	ResourceBarrier.Transition.pResource    = pDefaultBuffer;
	ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COMMON;
	ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_COPY_DEST;
	CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING)->ResourceBarrier(1, &ResourceBarrier);

	UpdateSubresources<1>(CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING), 
						  pDefaultBuffer, 
						  pUploadBuffer, 
						  0, 
						  0,
						  1, 
						  &subResourceData);

	ResourceBarrier.Transition.pResource    = pDefaultBuffer;
	ResourceBarrier.Transition.StateBefore  = D3D12_RESOURCE_STATE_COPY_DEST;
	ResourceBarrier.Transition.StateAfter   = D3D12_RESOURCE_STATE_GENERIC_READ;
	CGraphicDevice::Get_Instance()->Get_CommandList(CMDID::CMD_LOADING)->ResourceBarrier(1, &ResourceBarrier);

	/*__________________________________________________________________________________________________________
	[ ���� ]
	- ���� �Լ� ȣ�� ���Ŀ���, UploadBuffer�� ��� �����ؾ� �Ѵ�.
	- ������ ���縦 �����ϴ� ��� ����� ���� ������� �ʾұ� �����̴�.
	- ���簡 �Ϸ�Ǿ����� Ȯ������ �Ŀ� ȣ���ڰ� UploadBuffer�� �����ϸ� �ȴ�.
	____________________________________________________________________________________________________________*/



	return pDefaultBuffer;
}


void CVIMesh::Render_DynamicMesh(CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = nullptr;

	if (nullptr != m_pAniCtrl)
	{
		pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();
	}

	if (nullptr == pvecSkinningMatrix)
		return;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_DESC tCB_SkinningDesc;
		ZeroMemory(&tCB_SkinningDesc, sizeof(CB_SKINNING_DESC));

		if (0 != (*pvecSkinningMatrix).size())
		{
			for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
			{
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneOffset[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneScale[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneRotation[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneTrans[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans));
				XMStoreFloat4x4(&tCB_SkinningDesc.matParentTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform));
				XMStoreFloat4x4(&tCB_SkinningDesc.matRootTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform));
			}

			static_cast<CShaderMesh*>(pShader)->Get_UploadBuffer_SkinningDesc()->CopyData(i, tCB_SkinningDesc);

		}

		// Texture�� ������ �ǳʶڴ�.
		if (!m_vecMeshEntry[i].blsTexture)
			continue;

		// Begin Shader.
		pShader->Begin_Shader(m_pTexDescriptorHeap, i);

		// Begin_Buffer.
		m_pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

		// Render_Buffer.
		m_pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
											 1,
											 0,
											 0,
											 0);
	}
}

void CVIMesh::Render_StaticMesh(CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		// Texture�� ������ �ǳʶڴ�.
		if (!m_vecMeshEntry[i].blsTexture)
			continue;

		// Begin Shader.
		pShader->Begin_Shader(m_pTexDescriptorHeap, i);

		// Begin Buffer.
		m_pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

		// Render Buffer.
		m_pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
											 1,
											 0,
											 0,
											 0);
	}
}

void CVIMesh::Render_DynamicMeshShadowDepth(CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = nullptr;

	if (nullptr != m_pAniCtrl)
	{
		pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();
	}

	if (nullptr == pvecSkinningMatrix)
		return;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_DESC tCB_SkinningDesc;
		ZeroMemory(&tCB_SkinningDesc, sizeof(CB_SKINNING_DESC));

		if (0 != (*pvecSkinningMatrix).size())
		{
			for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
			{
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneOffset[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneScale[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneRotation[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneTrans[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans));
				XMStoreFloat4x4(&tCB_SkinningDesc.matParentTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform));
				XMStoreFloat4x4(&tCB_SkinningDesc.matRootTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform));
			}

			static_cast<CShaderShadow*>(pShader)->Get_UploadBuffer_SkinningDesc()->CopyData(i, tCB_SkinningDesc);

		}

		// Begin Shader.
		pShader->Begin_Shader(m_pTexDescriptorHeap, i);

		// Begin_Buffer.
		m_pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

		// Render_Buffer.
		m_pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
											 1,
											 0,
											 0,
											 0);
	}
}

void CVIMesh::Render_StaticMeshShadowDepth(CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		// Begin Shader.
		pShader->Begin_Shader(m_pTexDescriptorHeap, i);

		// Begin Buffer.
		m_pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		m_pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		m_pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

		// Render Buffer.
		m_pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
											 1,
											 0,
											 0,
											 0);
	}
}

void CVIMesh::Render_DynamicMesh(ID3D12GraphicsCommandList * pCommandList,
								 const _int& iContextIdx,
								 CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = nullptr;

	if (nullptr != m_pAniCtrl)
	{
		pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();
	}

	if (nullptr == pvecSkinningMatrix)
		return;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_DESC tCB_SkinningDesc;
		ZeroMemory(&tCB_SkinningDesc, sizeof(CB_SKINNING_DESC));

		if (0 != (*pvecSkinningMatrix).size())
		{
			for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
			{
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneOffset[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneScale[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneRotation[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneTrans[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans));
				XMStoreFloat4x4(&tCB_SkinningDesc.matParentTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform));
				XMStoreFloat4x4(&tCB_SkinningDesc.matRootTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform));
			}

			static_cast<CShaderMesh*>(pShader)->Get_UploadBuffer_SkinningDesc()->CopyData(i, tCB_SkinningDesc);

		}

		// Texture�� ������ �ǳʶڴ�.
		if (!m_vecMeshEntry[i].blsTexture)
			continue;

		// Begin Shader.
		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);

		// Begin_Buffer.
		pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);


		// Render_Buffer.
		pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
										   1,
										   0,
										   0,
										   0);
	}
}

void CVIMesh::Render_StaticMesh(ID3D12GraphicsCommandList * pCommandList,
								const _int& iContextIdx,
								CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		// Texture�� ������ �ǳʶڴ�.
		if (!m_vecMeshEntry[i].blsTexture)
			continue;

		// Begin Shader.
		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);

		// Begin Buffer.
		pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

		// Render Buffer
		pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
										   1,
										   0,
										   0,
										   0);
	}
}

void CVIMesh::Render_DynamicMeshShadowDepth(ID3D12GraphicsCommandList * pCommandList, 
											const _int& iContextIdx,
											CShader * pShader)
{
	vector<VECTOR_SKINNING_MATRIX>*	pvecSkinningMatrix = nullptr;

	if (nullptr != m_pAniCtrl)
	{
		pvecSkinningMatrix = m_pAniCtrl->Get_VecSkinningMatrix();
	}

	if (nullptr == pvecSkinningMatrix)
		return;

	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		CB_SKINNING_DESC tCB_SkinningDesc;
		ZeroMemory(&tCB_SkinningDesc, sizeof(CB_SKINNING_DESC));

		if (0 != (*pvecSkinningMatrix).size())
		{
			for (_uint j = 0; j < (*pvecSkinningMatrix)[i].size(); ++j)
			{
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneOffset[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneOffset));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneScale[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneScale));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneRotation[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneRotation));
				XMStoreFloat4x4(&tCB_SkinningDesc.matBoneTrans[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matBoneTrans));
				XMStoreFloat4x4(&tCB_SkinningDesc.matParentTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matParentTransform));
				XMStoreFloat4x4(&tCB_SkinningDesc.matRootTransform[j], XMMatrixTranspose((*pvecSkinningMatrix)[i][j].matRootTransform));
			}

			static_cast<CShaderShadow*>(pShader)->Get_UploadBuffer_SkinningDesc()->CopyData(i, tCB_SkinningDesc);

		}

		// Begin Shader.
		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);

		// Begin_Buffer.
		pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);


		// Render_Buffer.
		pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
										   1,
										   0,
										   0,
										   0);
	}
}

void CVIMesh::Render_StaticMeshShadowDepth(ID3D12GraphicsCommandList * pCommandList,
										   const _int& iContextIdx,
										   CShader * pShader)
{
	for (_int i = 0; i < m_vecMeshEntry.size(); ++i)
	{
		// Begin Shader.
		pShader->Begin_Shader(pCommandList, iContextIdx, m_pTexDescriptorHeap, i);

		// Begin Buffer.
		pCommandList->IASetVertexBuffers(0, 1, &Get_VertexBufferView(i));
		pCommandList->IASetIndexBuffer(&Get_IndexBufferView(i));
		pCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);

		// Rener Buffer.
		pCommandList->DrawIndexedInstanced(m_vecSubMeshGeometry[i].uiIndexCount,
										   1,
										   0,
										   0,
										   0);
	}
}

CComponent * CVIMesh::Clone()
{
	return new CVIMesh(*this);
}

CVIMesh * CVIMesh::Create(ID3D12Device * pGraphicDevice,
						  ID3D12GraphicsCommandList * pCommandList,
						  const aiScene * pScene, 
						  wstring wstrPath)
{
	CVIMesh* pInstance = new CVIMesh(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Component(pScene, wstrPath)))
		Engine::Safe_Release(pInstance);

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


	for (auto& iter : m_vecDiffResource)
		iter->Release();

	for (auto& iter : m_vecNormResource)
		iter->Release();

	for (auto& iter : m_vecSpecResource)
		iter->Release();


	m_pAniCtrl = nullptr;

	if (!m_bIsClone)
	{
		m_pScene = nullptr;
	}

	Safe_Release(m_pTexDescriptorHeap);
}