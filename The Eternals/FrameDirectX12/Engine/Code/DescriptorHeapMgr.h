#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CDescriptorHeapMgr : public CBase
{
	DECLARE_SINGLETON(CDescriptorHeapMgr)

private:
	explicit CDescriptorHeapMgr();
	virtual ~CDescriptorHeapMgr() = default;

public:
	HRESULT					Add_DescriptorHeap(wstring wstrHeapTag, ID3D12DescriptorHeap* pDescriptorHeap);
	ID3D12DescriptorHeap*	Find_DescriptorHeap(wstring wstrHeapTag);

private:
	map<wstring, ID3D12DescriptorHeap*> m_mapResourceDescriptorHeap;

private:
	virtual void Free();
};

END