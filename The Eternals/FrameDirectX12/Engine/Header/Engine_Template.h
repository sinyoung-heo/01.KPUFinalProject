#pragma once
#include "Engine_Typedef.h"
#include "Engine_Macro.h"

/*__________________________________________________________________________________________________________
[ inline �Լ� ]

- �Լ��� �Լ��� ȣ��� ������ �߻��ϴ� �������� ���� ������尡 �ִٴ� ������ �ִ�. 
- �̴� CPU�� �ٸ� �������Ϳ� �Բ� ���� ���� ���� ��ɾ��� �ּҸ� �����ؾ� �ϹǷ�(���߿� ��ȯ�� ��ġ�� �� �� �ֵ���)
��� �Լ� �Ű� ������ �����ؾ� �Ѵ�. 
- �Ҵ�� ���� ����ϸ� ���α׷��� �� ��ġ�� �б�ȴ�. ���ο��� �ۼ��� �ڵ�(�ν���Ʈ �ڵ�)�� �ξ� �� ������.

- ũ�ų� ������ �½�ũ�� �����ϴ� �Լ��� ��� �Լ� ȣ���� �������� �Լ��� ����Ǵ� �� �ɸ��� �ð��� ���� �� �߿����� �ʴ�. 
- �׷��� �Ϲ������� ����ϴ� ���� �Լ��� ���, 
�Լ� ȣ�⿡ �ʿ��� �ð��� ������ �Լ� �ڵ带 �����ϴ� �� �ʿ��� �ð����� �ξ� ���� ��찡 �ִ�. 
- �̷� ���� ����� ���� ���ϰ� �߻��� �� �ִ�.



- C++�� �ζ��� �Լ�(inline function)��� ���ο��� �ۼ��� �ڵ��� �ӵ��� �Լ��� ������ �����ϴ� ����� �����Ѵ�. 
- inline Ű����� �����Ϸ����� �Լ��� �ζ��� �Լ��� ó���ϵ��� ��û�Ѵ�. 
- �����Ϸ��� �ڵ带 �������ϸ� ��� �ζ��� �Լ��� ��-�÷��̽�(in-place) Ȯ��ȴ�. 
- ��, �Լ� ȣ���� �Լ� ��ü�� ���� ���纻���� ��ü�Ǿ� �Լ� ������尡 ���ŵȴ�! 

- ������ �ζ��� �Լ��� ��� �Լ� ȣ�⿡ ���� ������ ��ġ���� Ȯ��ǹǷ� 
�ζ��� �Լ��� ��ų� �ζ��� �Լ��� ���� �� ȣ���ϴ� ��� �����ϵ� �ڵ带 �ణ �� ũ�� ���� �� �ִٴ� ���̴�.

- �ڵ带 ��Ǯ�� ���ɼ��� �����Ƿ� �Լ��� �ζ���ȭ �ϴ� ���� ���� ������ ���� ª�� �Լ��� ���� �����ϴ�. 
- ���� inline Ű����� ���� ������ ���̴�. 
- �����Ϸ��� �ζ��ο� ���� ��û�� �����Ӱ� ������ �� �ִ�. (�� �Լ��� �ζ���ȭ �Ϸ��� �ϸ� ������ ���ɼ��� �ִ�.)

- ����������, ���� �����Ϸ��� �ڵ����� �Լ��� �ζ���ȭ �ϴ� �� �ſ� �پ��. 
- �Լ��� �ζ������� ǥ������ �ʴ��� �����Ϸ��� ������ ���� ������ �����ϴ� �Լ��� �ζ���ȭ �Ѵ�. 
- ���� ��κ��� ��� inline Ű���带 ����� �ʿ䰡 ����.

- �ζ��� �Լ��� �˰� �־�� ������ �ֽ� �����Ϸ��� �Լ��� �����ϰ� �ζ���ȭ �ϹǷ� inline Ű���带 ����� �ʿ䰡 ����.
____________________________________________________________________________________________________________*/

namespace Engine
{
	/*__________________________________________________________________________________________________________
	[ delete pointer ]
	____________________________________________________________________________________________________________*/
	template <typename T>
	inline void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template <typename T>
	inline void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}

	template <typename T>
	inline DWORD Safe_Release(T& pointer)
	{
		DWORD	dwRefCnt = 0;
		if (nullptr != pointer)
		{
			dwRefCnt = pointer->Release();
			if (dwRefCnt == 0)
				pointer = nullptr;
		}
		return dwRefCnt;
	}

	/*__________________________________________________________________________________________________________
	[ Check nullptr ]
	____________________________________________________________________________________________________________*/
	template<typename T>
	inline void NULL_CHECK(T& ptr)
	{
		if (nullptr == ptr)
		{
			__debugbreak();
			return;
		}
	}

	template<typename T1, typename T2>
	inline T2 NULL_CHECK_RETURN(T1& ptr, const T2& return_value)
	{
		if (nullptr == ptr)
		{
			__debugbreak();
			return return_value;
		}

		return S_OK;
	}

	/*__________________________________________________________________________________________________________
	[ Check HRESULT ]
	____________________________________________________________________________________________________________*/
	inline void FAILED_CHECK(const HRESULT& hr)
	{
		if (hr < 0)
		{
			MessageBoxW(NULL, L"Failed", L"System Error", MB_OK);
			__debugbreak();

			return;
		}
	}

	template<typename T1>
	inline T1 FAILED_CHECK_RETURN(const HRESULT& hr, const T1& return_value)
	{
		if (hr < 0)
		{
			MessageBoxW(NULL, L"Failed", L"System Error", MB_OK);
			__debugbreak();

			return return_value;
		}

		return S_OK;
	}


	/*____________________________________________________________________
	[ UploadBuffer ]
	______________________________________________________________________*/
	template<typename T>
	class CUploadBuffer
	{
	public:
		CUploadBuffer(ID3D12Device* pGraphicDevice)
			: m_pGraphicDevice(pGraphicDevice)
		{}
		~CUploadBuffer() { Free(); }

	public:
		ID3D12Resource*	Resource() const { return m_pUploadBuffer; }
		const _uint&	GetElementByteSize() { return m_uiElementByteSize; }
		const _uint&	GetElementCount() { return m_uiElementCount; }
		void			CopyData(_int iElementIndex, const T& data) { memcpy(&m_pMappedData[iElementIndex * m_uiElementByteSize], &data, sizeof(T)); }
	private:
		HRESULT			Ready_UploadBuffer(_uint uiElementCount = 1, _bool bIsConstantBuffer = true)
		{
			m_uiElementCount	= uiElementCount;
			m_bIsConstantBuffer = bIsConstantBuffer;
			m_uiElementByteSize = sizeof(T);

			/*__________________________________________________________________________________________________________
			��� ���� ������ ũ��� �ݵ�� 256����Ʈ�� ����̾�� �Ѵ�.
			�̴� �ϵ��� m * 256����Ʈ �����¿��� �����ϴ� n * 256����Ʈ ������ ��� �ڷḸ �� �� �ֱ� �����̴�.

			typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC
			{
				UINT64 OffsetInBytes;			// 256�� ���.
				UINT   SizeInBytes;				// 256�� ���.
			} D3D12_CONSTANT_BUFFER_VIEW_DESC;
			____________________________________________________________________________________________________________*/
			if (m_bIsConstantBuffer)
				m_uiElementByteSize = (sizeof(T) + 255) & ~255;

			FAILED_CHECK_RETURN(m_pGraphicDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
																		  D3D12_HEAP_FLAG_NONE,
																		  &CD3DX12_RESOURCE_DESC::Buffer(m_uiElementByteSize * m_uiElementCount),
																		  D3D12_RESOURCE_STATE_GENERIC_READ,
																		  nullptr,
																		  IID_PPV_ARGS(&m_pUploadBuffer)), 
																		  E_FAIL);

			FAILED_CHECK_RETURN(m_pUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pMappedData)), E_FAIL);

			/*__________________________________________________________________________________________________________
			�ڿ��� �� ����ϱ� ������ ������ ������ �ʿ䰡 ����.
			�׷���, �ڿ��� GPU�� ����ϴ� �߿��� CPU���� �ڿ��� �������� �ʾƾ� �Ѵ�. (����, �ݵ�� ����ȭ ����� ����ؾ� �Ѵ�.)
			____________________________________________________________________________________________________________*/

			return S_OK;
		}

	public:
		static CUploadBuffer<T>* Create(ID3D12Device* pGraphicDevice, _uint uiElementCount = 1, _bool bIsConstantBuffer = true)
		{
			CUploadBuffer<T>* pInstance = new CUploadBuffer<T>(pGraphicDevice);

			if (FAILED(pInstance->Ready_UploadBuffer(uiElementCount, bIsConstantBuffer)))
				Safe_Delete(pInstance);

			return pInstance;
		}

	private:
		ID3D12Device*	m_pGraphicDevice	= nullptr;
		ID3D12Resource* m_pUploadBuffer		= nullptr;
		BYTE*			m_pMappedData		= nullptr;
											
		_uint			m_uiElementCount	= 0;
		_uint			m_uiElementByteSize = 0;
											
		_bool			m_bIsConstantBuffer = false;

	private:
		void Free()
		{
			if (m_pUploadBuffer != nullptr)
				m_pUploadBuffer->Unmap(0, nullptr);

			m_pMappedData = nullptr;
		}
	};

}