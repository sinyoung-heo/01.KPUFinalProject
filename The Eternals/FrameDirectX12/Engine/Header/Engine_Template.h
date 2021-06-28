#pragma once
#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"

namespace Engine
{
	/*__________________________________________________________________________________________________________
	[ LINEAR INTERPOLATION DESC ]
	____________________________________________________________________________________________________________*/
	template<class T1>
	struct LINEAR_INTERPOLATION_DESC
	{
		LINEAR_INTERPOLATION_DESC()
			: is_start_interpolation(false)
			, linear_ratio(0.0f)
			, interpolation_speed(1.0f)
		{
			ZeroMemory(&v1, sizeof(T1));
			ZeroMemory(&v2, sizeof(T1));
		}


		LINEAR_INTERPOLATION_DESC(const bool& is_start_interpolation, const float& linear_ratio, const float& interpolation_speed, const T1& v1, const T1& v2)
			: is_start_interpolation(is_start_interpolation)
			, linear_ratio(linear_ratio)
			, interpolation_speed(interpolation_speed)
			, v1(v1)
			, v2(v2)
		{}

		bool	is_start_interpolation = false;
		float	linear_ratio           = 0.0f;
		float	interpolation_speed    = 1.0f;
		T1		v1;
		T1		v2;
	};

	const float MAX_LINEAR_RATIO = 1.0f;
	const float MIN_LINEAR_RATIO = 0.0f;

	template<class T1>
	T1 LinearInterpolation(const T1& v1, const T1& v2, float& ratio)
	{
		if (ratio >= MAX_LINEAR_RATIO)
			ratio = MAX_LINEAR_RATIO;

		else if (ratio <= MIN_LINEAR_RATIO)
			ratio = MIN_LINEAR_RATIO;

		return v1 * (1.0f - ratio) + v2 * ratio;
	}

	template<class T1>
	void SetUp_LinearInterpolation(const _float& time_delta ,T1& src, LINEAR_INTERPOLATION_DESC<T1>& linear_desc)
	{
		if (linear_desc.is_start_interpolation)
		{
			linear_desc.linear_ratio += linear_desc.interpolation_speed * time_delta;

			if (linear_desc.linear_ratio >= MAX_LINEAR_RATIO)
			{
				linear_desc.linear_ratio = MAX_LINEAR_RATIO;
				linear_desc.is_start_interpolation = false;
			}

			src = LinearInterpolation<T1>(linear_desc.v1, linear_desc.v2, linear_desc.linear_ratio);
		}
		else
		{
			linear_desc.linear_ratio = 0.0f;
		}
	}


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
			상수 버퍼 원소의 크기는 반드시 256바이트의 배수이어야 한다.
			이는 하드웨어가 m * 256바이트 오프셋에서 시작하는 n * 256바이트 길이의 상수 자료만 볼 수 있기 때문이다.

			typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC
			{
				UINT64 OffsetInBytes;			// 256의 배수.
				UINT   SizeInBytes;				// 256의 배수.
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
			자원을 다 사용하기 전에는 대응을 해제할 필요가 없다.
			그러나, 자원을 GPU가 사용하는 중에는 CPU에서 자원을 갱신하지 않아야 한다. (따라서, 반드시 동기화 기법을 사용해아 한다.)
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