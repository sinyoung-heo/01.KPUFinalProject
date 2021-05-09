#include "ShaderMesh.h"
#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderMesh::CShaderMesh(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderMesh::CShaderMesh(const CShaderMesh& rhs)
	: CShader(rhs)
{
	/*__________________________________________________________________________________________________________
	- Texture Buffer의 경우, 객체마다 사용하는 텍스처 이미지와 개수가 다르기 때문에,
	GameObject::LateInit_GameObject()에서 Set_Shader_Texture() 함수 호출을 통해
	Create_DescriptorHeaps(pvecTexDiffuse), Create_ConstantBuffer()를 실행.
	____________________________________________________________________________________________________________*/
}

HRESULT CShaderMesh::SetUp_ShaderConstantBuffer(const _uint& uiNumSubsetMesh, const _uint& iAfterImgSize)
{
	m_uiSubsetMeshSize = uiNumSubsetMesh;
	m_uiAfterImgSize = iAfterImgSize;

	m_pCB_ShaderMesh = CUploadBuffer<CB_SHADER_MESH>::Create(m_pGraphicDevice);
	NULL_CHECK_RETURN(m_pCB_ShaderMesh, E_FAIL);

	// SubsetMesh 개수만큼 만들어준다.
	m_pCB_SkinningMatrix = CUploadBuffer<CB_SKINNING_MATRIX>::Create(m_pGraphicDevice, uiNumSubsetMesh);
	NULL_CHECK_RETURN(m_pCB_SkinningMatrix, E_FAIL);

	// AfterImage
	if (m_uiAfterImgSize)
	{
		m_pCB_AFShaderMesh = CUploadBuffer<CB_SHADER_MESH>::Create(m_pGraphicDevice, m_uiAfterImgSize);
		NULL_CHECK_RETURN(m_pCB_AFShaderMesh, E_FAIL);

		m_pCB_AFSkinningMatrix = CUploadBuffer<CB_SKINNING_MATRIX>::Create(m_pGraphicDevice, uiNumSubsetMesh * m_uiAfterImgSize);
		NULL_CHECK_RETURN(m_pCB_AFSkinningMatrix, E_FAIL);
	}
	return S_OK;
}

HRESULT CShaderMesh::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	return S_OK;
}

void CShaderMesh::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint& iSubMeshIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(iSubMeshIdx + (m_uiSubsetMeshSize * TEX_DIFFUSE), m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
		SRV_TexDiffuseDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(iSubMeshIdx + (m_uiSubsetMeshSize * TEX_NORMAL), m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
		SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(iSubMeshIdx + (m_uiSubsetMeshSize * TEX_SPECULAR), m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
		SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(m_uiSubsetMeshSize * TEXTURE_END + 0, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexDepth
		SRV_TexShadowDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDissolveDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDissolveDescriptorHandle.Offset(m_uiSubsetMeshSize * TEXTURE_END + 1, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexDissolve
		SRV_TexDissolveDescriptorHandle);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(7,	// RootParameter Index
		m_pCB_SkinningMatrix->Resource()->GetGPUVirtualAddress() +
		m_pCB_SkinningMatrix->GetElementByteSize() * iSubMeshIdx);
}

void CShaderMesh::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, ID3D12DescriptorHeap* pTexNormalDescriptorHeap, 
	_uint uiTexnormalIdx, _uint uiPatternMapIdx, const _uint& iSubMeshIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);


	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexNormalDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(0, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
		SRV_TexDiffuseDescriptorHandle);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(uiTexnormalIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
		SRV_TexShadowDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(uiPatternMapIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
		SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(2, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
		SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDissolveHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDissolveHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexShadowDepth
		SRV_TexDissolveHandle);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(7,	// RootParameter Index
		m_pCB_SkinningMatrix->Resource()->GetGPUVirtualAddress() +
		m_pCB_SkinningMatrix->GetElementByteSize() * iSubMeshIdx);

}

void CShaderMesh::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
	const _int& iContextIdx,
	ID3D12DescriptorHeap* pTexDescriptorHeap,
	const _uint& iSubMeshIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

	// Set RootSignature.
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexDescriptorHeap };
	pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(iSubMeshIdx + (m_uiSubsetMeshSize * TEX_DIFFUSE), m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
		SRV_TexDiffuseDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(iSubMeshIdx + (m_uiSubsetMeshSize * TEX_NORMAL), m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
		SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(iSubMeshIdx + (m_uiSubsetMeshSize * TEX_SPECULAR), m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
		SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(m_uiSubsetMeshSize * TEXTURE_END, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
		SRV_TexShadowDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexEmissiveDescriptorHandle(pTexDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexEmissiveDescriptorHandle.Offset(m_uiSubsetMeshSize * TEXTURE_END + 1, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexDissolve
		SRV_TexEmissiveDescriptorHandle);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(7,	// RootParameter Index
		m_pCB_SkinningMatrix->Resource()->GetGPUVirtualAddress() +
		m_pCB_SkinningMatrix->GetElementByteSize() * iSubMeshIdx);

}

void CShaderMesh::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
	const _int& iContextIdx,
	ID3D12DescriptorHeap* pTexDescriptorHeap,
	ID3D12DescriptorHeap* pTexNormalDescriptorHeap,
	_uint uiTexnormalIdx, _uint uiPatternMapIdx,
	const _uint& iSubMeshIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

	// Set RootSignature.
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexNormalDescriptorHeap };
	pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(0, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
		SRV_TexDiffuseDescriptorHandle);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(uiTexnormalIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
		SRV_TexShadowDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(uiPatternMapIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
		SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(2, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
		SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDissolveHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDissolveHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexShadowDepth
		SRV_TexDissolveHandle);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(7,	// RootParameter Index
		m_pCB_SkinningMatrix->Resource()->GetGPUVirtualAddress() +
		m_pCB_SkinningMatrix->GetElementByteSize() * iSubMeshIdx);

}

void CShaderMesh::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
	const _int& iContextIdx,
	ID3D12DescriptorHeap* pTexDescriptorHeap,
	ID3D12DescriptorHeap* pTexNormalDescriptorHeap,
	_uint AlbedoIdx,
	const _uint& iSubMeshIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

	// Set RootSignature.
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexNormalDescriptorHeap };
	pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(AlbedoIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
		SRV_TexDiffuseDescriptorHandle);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthDescriptorHandle.Offset(1, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
		SRV_TexShadowDepthDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(2, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
		SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(3, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
		SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDissolveHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDissolveHandle.Offset(4, m_uiCBV_SRV_UAV_DescriptorSize);
	pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexDissolve
		SRV_TexDissolveHandle);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

}
void CShaderMesh::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
	const _int& iContextIdx,
	const _uint& iSubMeshIdx,
	const _uint& iAfterImgIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

	// Set RootSignature.
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_AFShaderMesh->Resource()->GetGPUVirtualAddress() +
		m_pCB_AFShaderMesh->GetElementByteSize() * iAfterImgIdx);

	pCommandList->SetGraphicsRootConstantBufferView(7,	// RootParameter Index
		m_pCB_AFSkinningMatrix->Resource()->GetGPUVirtualAddress() +
		m_pCB_AFSkinningMatrix->GetElementByteSize() * (iSubMeshIdx + m_uiSubsetMeshSize * iAfterImgIdx));
}

void CShaderMesh::Begin_Shader(const _int& iSubMeshIdx, const _int& AfterIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);

	// Set RootSignature.
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ CBV를 루트 서술자에 묶는다 ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_AFShaderMesh->Resource()->GetGPUVirtualAddress() +
		m_pCB_AFShaderMesh->GetElementByteSize() * AfterIdx);

	m_pCommandList->SetGraphicsRootConstantBufferView(7,	// RootParameter Index
		m_pCB_AFSkinningMatrix->Resource()->GetGPUVirtualAddress() +
		m_pCB_AFSkinningMatrix->GetElementByteSize() * (iSubMeshIdx + m_uiSubsetMeshSize * AfterIdx));
}

HRESULT CShaderMesh::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV를 담는 서술자 테이블을 생성 ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[5];

	// TexDiffuse
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
		1,								// 서술자의 개수 - Texture2D의 개수.
		0,								// 셰이더 인수들의 기준 레지스터 번호. (register t0)
		0);								// 레지스터 공간.

// TexNormal
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
		1,								// 서술자의 개수 - Texture2D의 개수.
		1,								// 셰이더 인수들의 기준 레지스터 번호. (register t1)
		0);								// 레지스터 공간.
// Specular
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
		1,								// 서술자의 개수 - Texture2D의 개수.
		2,								// 셰이더 인수들의 기준 레지스터 번호. (register t2)
		0);								// 레지스터 공간.

// ShadowDepth
	SRV_Table[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
		1,								// 서술자의 개수 - Texture2D의 개수.
		3,								// 셰이더 인수들의 기준 레지스터 번호. (register t3)
		0);								// 레지스터 공간.

// Dissolve
	SRV_Table[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// 서술자의 종류 - Shader Resource View.
		1,								// 서술자의 개수 - Texture2D의 개수.
		4,								// 셰이더 인수들의 기준 레지스터 번호. (register t4)
		0);								// 레지스터 공간.
/*__________________________________________________________________________________________________________
- 루트 매개변수는 테이블이거나, 루트 서술자 또는 루트 상수이다.
____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[8];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[3].InitAsDescriptorTable(1, &SRV_Table[3], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[4].InitAsDescriptorTable(1, &SRV_Table[4], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[5].InitAsConstantBufferView(0);		// register b0.
	RootParameter[6].InitAsConstantBufferView(1);		// register b1.
	RootParameter[7].InitAsConstantBufferView(2);		// register b2.

	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),	// 루트 파라미터 개수.
		RootParameter,
		(UINT)StaticSamplers.size(),	// 샘플러 개수.
		StaticSamplers.data(),		// 샘플러 데이터.
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	/*__________________________________________________________________________________________________________
	[ RootSignature를 생성 ]
	____________________________________________________________________________________________________________*/
	ID3DBlob* pSignatureBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	FAILED_CHECK_RETURN(D3D12SerializeRootSignature(&RootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignatureBlob,
		&pErrorBlob), E_FAIL);

	if (nullptr != pErrorBlob)
	{
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		return E_FAIL;
	}

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateRootSignature(0,
		pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_pRootSignature)),
		E_FAIL);
	Safe_Release(pSignatureBlob);
	Safe_Release(pErrorBlob);

	return S_OK;
}

HRESULT CShaderMesh::Create_PipelineState()
{
	/*__________________________________________________________________________________________________________
	[ PipelineState 기본 설정 ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState* pPipelineState = nullptr;
	vector<D3D12_INPUT_ELEMENT_DESC>	vecInputLayout;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	PipelineStateDesc;
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	/*__________________________________________________________________________________________________________
	[ 0번 PipelineState Pass ]
	- "VS_MAIN"
	- "PS_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 6;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target

	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(false, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE,
		D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, TRUE);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 1번 PipelineState Pass ]
	- "VS_SHADOW_MAIN"
	- "PS_SHADOW_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 6;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target

	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_SHADOW_MAIN", "PS_SHADOW_MAIN");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(false, D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, D3D12_BLEND_ONE,
		D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD, TRUE);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*[2번 PipelineState Pass]
		- FILL_MODE_SOLID
		- CULL_MODE_BACK
		- Blend(X)
		- Z Write(O)*/

	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target

	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_DISTORTION");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*[3번 PipelineState Pass]
		- FILL_MODE_SOLID
		- CULL_MODE_BACK
		- Blend(X)
		- Z Write(O)*/

	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 2;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_UNORM;		// Crossfilter Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R16G16B16A16_UNORM;		// Crossfilter Target

	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_CROSSFILTER");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//4번
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Edge Target

	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_EDGE");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 5번 PipelineState Pass ]
	- "VS_AFTERIMAGE"
	- "PS_AFTERIMAGE"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target


	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_AFTERIMAGE", "PS_AFTERIMAGE");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(true,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ZERO,
		D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();



	//6번
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 6;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target

	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_TERRAIN_MAIN", "PS_TERRAIN_MAIN");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	/*__________________________________________________________________________________________________________
	[ 7번 PipelineState Pass ]
	- "VS_SHADOW_MAIN"
	- "PS_SHADOW_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 6;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_SHADOW_MAIN", "PS_SHADOW_DISSOLVE");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	/*__________________________________________________________________________________________________________
	[ 8번 PipelineState Pass ]
	- "VS_SHADOW_MAIN"
	- "PS_SHADOW_MAIN"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(X)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 6;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Normal Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Specular Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;	// Depth Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.RTVFormats[5] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Emissive Target
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_WATERFALL", "PS_WATERFALL");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState();
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();

	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	
	//9qjs
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets =1;								// PS에서 사용할 RenderTarget 개수.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_RAINDROP");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.BlendState = Create_BlendState(true,
		D3D12_BLEND_SRC_ALPHA,
		D3D12_BLEND_INV_SRC_ALPHA,
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ZERO,
		D3D12_BLEND_OP_ADD);
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();


	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderMesh::Create_InputLayout(string VS_EntryPoint,
	string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderMesh.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderMesh.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec2), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	uiOffset += sizeof(_vec4), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderMesh::Create_BlendState(const _bool& bIsBlendEnable,
	const D3D12_BLEND& SrcBlend,
	const D3D12_BLEND& DstBlend,
	const D3D12_BLEND_OP& BlendOp,
	const D3D12_BLEND& SrcBlendAlpha,
	const D3D12_BLEND& DstBlendAlpha,
	const D3D12_BLEND_OP& BlendOpAlpha,
	const _bool& bIsAlphaTest)
{
	D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// 블렌드 설정.
	ZeroMemory(&BlendDesc, sizeof(D3D12_BLEND_DESC));
	BlendDesc.AlphaToCoverageEnable = bIsAlphaTest;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = bIsBlendEnable;
	BlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	BlendDesc.RenderTarget[0].SrcBlend = SrcBlend;
	BlendDesc.RenderTarget[0].DestBlend = DstBlend;
	BlendDesc.RenderTarget[0].BlendOp = BlendOp;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = SrcBlendAlpha;
	BlendDesc.RenderTarget[0].DestBlendAlpha = DstBlendAlpha;
	BlendDesc.RenderTarget[0].BlendOpAlpha = BlendOpAlpha;
	BlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return BlendDesc;
}


CComponent* CShaderMesh::Clone()
{
	return new CShaderMesh(*this);
}

CShader* CShaderMesh::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CShaderMesh* pInstance = new CShaderMesh(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderMesh::Free()
{
	CShader::Free();

	Safe_Delete(m_pCB_ShaderMesh);
	Safe_Delete(m_pCB_SkinningMatrix);
	if (m_uiAfterImgSize)
	{
		Safe_Delete(m_pCB_AFShaderMesh);
		Safe_Delete(m_pCB_AFSkinningMatrix);
	}
}