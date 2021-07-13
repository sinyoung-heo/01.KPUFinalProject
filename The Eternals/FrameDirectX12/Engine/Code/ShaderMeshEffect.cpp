#include "ShaderMeshEffect.h"
#include "GraphicDevice.h"
#include "Renderer.h"

USING(Engine)

CShaderMeshEffect::CShaderMeshEffect(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
	: CShader(pGraphicDevice, pCommandList)
{
}

CShaderMeshEffect::CShaderMeshEffect(const CShaderMeshEffect& rhs)
	: CShader(rhs)
{
	/*__________________________________________________________________________________________________________
	- Texture Buffer�� ���, ��ü���� ����ϴ� �ؽ�ó �̹����� ������ �ٸ��� ������,
	GameObject::LateInit_GameObject()���� Set_Shader_Texture() �Լ� ȣ���� ����
	Create_DescriptorHeaps(pvecTexDiffuse), Create_ConstantBuffer()�� ����.
	____________________________________________________________________________________________________________*/
}
HRESULT CShaderMeshEffect::SetUp_ShaderConstantBuffer(const _uint& uiNumSubsetMesh)
{
	m_uiSubsetMeshSize = uiNumSubsetMesh;
	m_pCB_ShaderMesh = CUploadBuffer<CB_SHADER_MESH>::Create(m_pGraphicDevice,uiNumSubsetMesh);
	NULL_CHECK_RETURN(m_pCB_ShaderMesh, E_FAIL);

	return S_OK;
}
HRESULT CShaderMeshEffect::Ready_Shader()
{
	CShader::Ready_Shader();
	FAILED_CHECK_RETURN(Create_RootSignature(), E_FAIL);
	FAILED_CHECK_RETURN(Create_PipelineState(), E_FAIL);

	return S_OK;
}

void CShaderMeshEffect::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, const _uint& iSubMeshIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
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
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());
}

void CShaderMeshEffect::Begin_Shader(ID3D12DescriptorHeap* pTexDescriptorHeap, ID3D12DescriptorHeap* pTexNormalDescriptorHeap, 
	_uint uiDiffuseIdx, _uint uiTexnormalIdx, _uint uiPatternMapIdx, _uint uiShadowDepthIdx, _uint uiDissolveIdx, const _uint& iSubMeshIdx)
{
	CRenderer::Get_Instance()->Set_CurPipelineState(m_pPipelineState);
	m_pCommandList->SetGraphicsRootSignature(m_pRootSignature);


	/*__________________________________________________________________________________________________________
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { pTexNormalDescriptorHeap };
	m_pCommandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDiffuseDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDiffuseDescriptorHandle.Offset(uiDiffuseIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(0,		// RootParameter Index - TexDiffuse
		SRV_TexDiffuseDescriptorHandle);


	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexNormalDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexNormalDescriptorHandle.Offset(uiTexnormalIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(1,		// RootParameter Index - TexNormal
		SRV_TexNormalDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexSpecularDescriptorHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexSpecularDescriptorHandle.Offset(uiPatternMapIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(2,		// RootParameter Index - TexSpecular
		SRV_TexSpecularDescriptorHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexShadowDepthHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexShadowDepthHandle.Offset(uiShadowDepthIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(3,		// RootParameter Index - TexShadowDepth
		SRV_TexShadowDepthHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV_TexDissolveHandle(pTexNormalDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	SRV_TexDissolveHandle.Offset(uiDissolveIdx, m_uiCBV_SRV_UAV_DescriptorSize);
	m_pCommandList->SetGraphicsRootDescriptorTable(4,		// RootParameter Index - TexShadowDepth
		SRV_TexDissolveHandle);

	/*__________________________________________________________________________________________________________
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	m_pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	m_pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress() +
		m_pCB_ShaderMesh->GetElementByteSize() * iSubMeshIdx);


}

void CShaderMeshEffect::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
	const _int& iContextIdx,
	ID3D12DescriptorHeap* pTexDescriptorHeap,
	const _uint& iSubMeshIdx)
{
	// Set PipelineState.
	CRenderer::Get_Instance()->Set_CurPipelineState(pCommandList, m_pPipelineState, iContextIdx);

	// Set RootSignature.
	pCommandList->SetGraphicsRootSignature(m_pRootSignature);

	/*__________________________________________________________________________________________________________
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
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
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

}

void CShaderMeshEffect::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
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
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
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
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

}

void CShaderMeshEffect::Begin_Shader(ID3D12GraphicsCommandList* pCommandList,
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
	[ SRV�� ��Ʈ �����ڿ� ���´� ]
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
	[ CBV�� ��Ʈ �����ڿ� ���´� ]
	____________________________________________________________________________________________________________*/
	pCommandList->SetGraphicsRootConstantBufferView(5,	// RootParameter Index
		m_pCB_CameraProjMatrix->Resource()->GetGPUVirtualAddress());

	pCommandList->SetGraphicsRootConstantBufferView(6,	// RootParameter Index
		m_pCB_ShaderMesh->Resource()->GetGPUVirtualAddress());

}
HRESULT CShaderMeshEffect::Create_RootSignature()
{
	/*__________________________________________________________________________________________________________
	[ SRV�� ��� ������ ���̺��� ���� ]
	____________________________________________________________________________________________________________*/
	CD3DX12_DESCRIPTOR_RANGE SRV_Table[5];

	// TexDiffuse
	SRV_Table[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		0,								// ���̴� �μ����� ���� �������� ��ȣ. (register t0)
		0);								// �������� ����.

// TexNormal
	SRV_Table[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		1,								// ���̴� �μ����� ���� �������� ��ȣ. (register t1)
		0);								// �������� ����.
// Specular
	SRV_Table[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		2,								// ���̴� �μ����� ���� �������� ��ȣ. (register t2)
		0);								// �������� ����.

// ShadowDepth
	SRV_Table[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		3,								// ���̴� �μ����� ���� �������� ��ȣ. (register t3)
		0);								// �������� ����.

// Dissolve
	SRV_Table[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,	// �������� ���� - Shader Resource View.
		1,								// �������� ���� - Texture2D�� ����.
		4,								// ���̴� �μ����� ���� �������� ��ȣ. (register t4)
		0);								// �������� ����.
/*__________________________________________________________________________________________________________
- ��Ʈ �Ű������� ���̺��̰ų�, ��Ʈ ������ �Ǵ� ��Ʈ ����̴�.
____________________________________________________________________________________________________________*/
	CD3DX12_ROOT_PARAMETER RootParameter[7];
	RootParameter[0].InitAsDescriptorTable(1, &SRV_Table[0], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[1].InitAsDescriptorTable(1, &SRV_Table[1], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[2].InitAsDescriptorTable(1, &SRV_Table[2], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[3].InitAsDescriptorTable(1, &SRV_Table[3], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[4].InitAsDescriptorTable(1, &SRV_Table[4], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameter[5].InitAsConstantBufferView(0);		// register b0.
	RootParameter[6].InitAsConstantBufferView(1);		// register b1.


	auto StaticSamplers = Get_StaticSamplers();
	CD3DX12_ROOT_SIGNATURE_DESC RootSignatureDesc(_countof(RootParameter),	// ��Ʈ �Ķ���� ����.
		RootParameter,
		(UINT)StaticSamplers.size(),	// ���÷� ����.
		StaticSamplers.data(),		// ���÷� ������.
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	/*__________________________________________________________________________________________________________
	[ RootSignature�� ���� ]
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

HRESULT CShaderMeshEffect::Create_PipelineState()
{
	/*__________________________________________________________________________________________________________
	[ PipelineState �⺻ ���� ]
	____________________________________________________________________________________________________________*/
	ID3D12PipelineState* pPipelineState = nullptr;
	vector<D3D12_INPUT_ELEMENT_DESC>	vecInputLayout;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	PipelineStateDesc;
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	/*__________________________________________________________________________________________________________
	[ 0�� PipelineState Pass ]
	- "VS_AFTERIMAGE"
	- "PS_AFTERIMAGE"
	- FILL_MODE_SOLID
	- CULL_MODE_BACK
	- Blend		(O)
	- Z Write	(O)
	____________________________________________________________________________________________________________*/
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 5;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_MAGIC_CIRCLE");
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

	//1qjs
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 5;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
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

	//2��
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 5;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_EFFECT_SHPERE");
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

	//3
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_ICESTORM");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//4
	vecInputLayout = Create_InputLayout("VS_ANIUV", "PS_DECAL");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//5
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_MAIN");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();

	//6��
	ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pRootSignature;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 5;								// PS���� ����� RenderTarget ����.
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM;		// Diffuse Target
	PipelineStateDesc.SampleDesc.Count = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? 4 : 1;
	PipelineStateDesc.SampleDesc.Quality = CGraphicDevice::Get_Instance()->Get_MSAA4X_Enable() ? (CGraphicDevice::Get_Instance()->Get_MSAA4X_QualityLevels() - 1) : 0;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_MAIN");
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
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//7
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_ICEDECAL");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//8
	vecInputLayout = Create_InputLayout("VS_ANIUV_X", "PS_MAIN_SPRITE");
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
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	PipelineStateDesc.DepthStencilState = CShader::Create_DepthStencilState();
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//9
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_MAIN_SPRITE2");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//10
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_EFFECT_SHPERE_RED");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//11
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_EFFECT_SHPERE_COLOR");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState();
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//12
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_EFFECT_ICE_M");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//13
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_WARNING_GROUND");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//14
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_RECTDECAL");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	//15
	vecInputLayout = Create_InputLayout("VS_MAIN", "PS_BREATH");
	PipelineStateDesc.InputLayout = { vecInputLayout.data(), (_uint)vecInputLayout.size() };
	PipelineStateDesc.VS = { reinterpret_cast<BYTE*>(m_pVS_ByteCode->GetBufferPointer()), m_pVS_ByteCode->GetBufferSize() };
	PipelineStateDesc.PS = { reinterpret_cast<BYTE*>(m_pPS_ByteCode->GetBufferPointer()), m_pPS_ByteCode->GetBufferSize() };
	PipelineStateDesc.RasterizerState = CShader::Create_RasterizerState(D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE);
	FAILED_CHECK_RETURN(m_pGraphicDevice->CreateGraphicsPipelineState(&PipelineStateDesc, IID_PPV_ARGS(&pPipelineState)), E_FAIL);
	m_vecPipelineState.emplace_back(pPipelineState);
	CRenderer::Get_Instance()->Add_PipelineStateCnt();
	return S_OK;
}

vector<D3D12_INPUT_ELEMENT_DESC> CShaderMeshEffect::Create_InputLayout(string VS_EntryPoint,
	string PS_EntryPoint)
{
	vector<D3D12_INPUT_ELEMENT_DESC> vecInputLayout;

	m_pVS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderMeshEffect.hlsl", nullptr, VS_EntryPoint.c_str(), "vs_5_1");
	m_pPS_ByteCode = Compile_Shader(L"../../Bin/Shader/ShaderMeshEffect.hlsl", nullptr, PS_EntryPoint.c_str(), "ps_5_1");

	_uint uiOffset = 0;
	vecInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, uiOffset += sizeof(_vec3), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	return vecInputLayout;
}

D3D12_BLEND_DESC CShaderMeshEffect::Create_BlendState(const _bool& bIsBlendEnable,
	const D3D12_BLEND& SrcBlend,
	const D3D12_BLEND& DstBlend,
	const D3D12_BLEND_OP& BlendOp,
	const D3D12_BLEND& SrcBlendAlpha,
	const D3D12_BLEND& DstBlendAlpha,
	const D3D12_BLEND_OP& BlendOpAlpha,
	const _bool& bIsAlphaTest)
{
	D3D12_BLEND_DESC BlendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// ���� ����.
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


CComponent* CShaderMeshEffect::Clone()
{
	return new CShaderMeshEffect(*this);
}

CShader* CShaderMeshEffect::Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CShaderMeshEffect* pInstance = new CShaderMeshEffect(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_Shader()))
		Safe_Release(pInstance);

	return pInstance;
}

void CShaderMeshEffect::Free()
{
	CShader::Free();

	Safe_Delete(m_pCB_ShaderMesh);
}