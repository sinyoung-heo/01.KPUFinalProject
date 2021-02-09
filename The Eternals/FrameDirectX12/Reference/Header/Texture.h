#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	wstring							Get_FileName()			{ return m_wstrFileName; }
	vector<ComPtr<ID3D12Resource>>	Get_Texture()			{ return m_vecResource; };
	const _uint&					Get_TexSize()			{ return m_uiTexSize; }
	ID3D12DescriptorHeap*			Get_TexDescriptorHeap() { return m_pTexDescriptorHeap; }

	HRESULT							Ready_Texture(wstring wstrFileName,
												  wstring wstrFilePath,
												  const _uint& iCnt,
												  const TEXTYPE& eType);
	HRESULT							Create_TextureDescriptorHeap(const TEXTYPE& eType = TEXTYPE::TEX_2D);
	virtual void					Release_UploadBuffer();

private:
	vector<ComPtr<ID3D12Resource>>	m_vecResource;
	vector<ComPtr<ID3D12Resource>>	m_vecUpload;

	ID3D12DescriptorHeap*			m_pTexDescriptorHeap = nullptr;
	wstring							m_wstrFileName       = L"";
	_uint							m_uiTexSize		     = 0;

public:
	virtual CComponent* Clone();
public:
	static CTexture*	Create(ID3D12Device* pGraphicDevice, ID3D12GraphicsCommandList* pCommandList,
							   wstring wstrFileName,
							   wstring wstrFilePath, 
							   const _uint& iCnt = 1,
							   const TEXTYPE& eType = TEXTYPE::TEX_2D);
private:
	virtual void Free();
};

END