#include "Font.h"
#include "GraphicDevice.h"

USING(Engine)

CFont::CFont(ID3D12Device * pGraphicDevice, ID3D12GraphicsCommandList * pCommandList)
	: CGameObject(pGraphicDevice, pCommandList)
{
}

CFont::CFont(const CFont & rhs)
	: CGameObject(rhs)
	, m_bIsClone(true)
	, m_pD2D_Context(rhs.m_pD2D_Context)
	, m_pDWriteFactory(rhs.m_pDWriteFactory)
	, m_pTextFormat(rhs.m_pTextFormat)
	, m_fFontSize(rhs.m_fFontSize)
	, m_vPosOffset(rhs.m_vPosOffset)
{
	if (nullptr != m_pTextFormat)
		m_pTextFormat->AddRef();
}

HRESULT CFont::Ready_GameObjectPrototype(wstring wstrFontName,
										 const _float & fSize, 
										 const D2D1::ColorF & Color)
{
	m_pD2D_Context = CGraphicDevice::Get_Instance()->Get_D2DContext();
	NULL_CHECK_RETURN(m_pD2D_Context, E_FAIL);

	m_pDWriteFactory = CGraphicDevice::Get_Instance()->Get_DWriteFactory();
	NULL_CHECK_RETURN(m_pDWriteFactory, E_FAIL);

	m_fFontSize = fSize;

	/*__________________________________________________________________________________________________________
	[ �⺻ D2D �ؽ�Ʈ ��ü ����� ]
	- ���� 3D �������� �������� ID3D12Device, �� ID3D11On12Device�� ���� 12 ����̽��� �����Ǵ� 
	ID2D1Device(2D ������ �������� ����� �� ����)�� ������ �� �� ������ ���� ü������ �������ϵ��� �����˴ϴ�.
	
	- �� ������ ������ �ش� UI�� �������ϴ� �Ͱ� ����� �������, ������ D2D ����̽��� ����Ͽ� 3D ��� ���� �ؽ�Ʈ�� �������մϴ�. 
	- �̸� ���� ���� LoadAssets �޼��忡�� �� ���� �⺻���� D2D ��ü�� ������ �մϴ�.
	____________________________________________________________________________________________________________*/
	FAILED_CHECK_RETURN(m_pD2D_Context->CreateSolidColorBrush(Color, &m_pTextBrush), E_FAIL);

	FAILED_CHECK_RETURN(m_pDWriteFactory->CreateTextFormat(wstrFontName.c_str(),		// Font Name
														   nullptr,						// Font Collection
														   DWRITE_FONT_WEIGHT_NORMAL,
														   DWRITE_FONT_STYLE_NORMAL,
														   DWRITE_FONT_STRETCH_NORMAL,
														   fSize,						// Font Size
														   L"ko-KR",					// Local Name
														   &m_pTextFormat), E_FAIL);

	// �»���� �������� �ؽ�Ʈ ����.
	FAILED_CHECK_RETURN(m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING), E_FAIL);
	FAILED_CHECK_RETURN(m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR), E_FAIL);

	return S_OK;
}

HRESULT CFont::Ready_GameObject(wstring wstrText, 
								const _vec2& vPos,
								const D2D1::ColorF& Color)
{
	// Text ���ڿ� & ��ġ & ����.
	m_wstrText			= wstrText;
	m_wstrPreText		= m_wstrText;
	m_vPos				= vPos;

	FAILED_CHECK_RETURN(m_pD2D_Context->CreateSolidColorBrush(Color, &m_pTextBrush), E_FAIL);
	m_pTextBrush->SetColor(Color);

	// Font�� Rendering�� ����.
	m_TextRect.left	= m_vPos.x;
	m_TextRect.top	= m_vPos.y;

	/*__________________________________________________________________________________________________________
	[ Offset ��� ]
	____________________________________________________________________________________________________________*/
	_tchar szTemp[MAX_STR] = L"";
	lstrcpy(szTemp, m_wstrText.c_str());

	_uint			iCurIdx = 0;
	vector<_vec2>	vecPosOffset;
	vecPosOffset.emplace_back(0.f, 0.f);

	for (_int i = 0; szTemp[i] != '\0'; ++i)
	{
		_tchar ch = szTemp[i];
		
		// ���� ���� - TextRect�� Height ����.
		if (ch == '\n')
		{
			vecPosOffset[0].y += m_fFontSize;

			vecPosOffset.emplace_back(0.f, 0.f);
			++iCurIdx;
		}
		// ���� ���� - TextRect�� Height ����.
		else if (ch == ' ')
			vecPosOffset[iCurIdx].x += m_fFontSize * 0.33f;

		// Tap ���� - TextRect�� Height ����.
		else if (ch == '\t')
			vecPosOffset[iCurIdx].x += m_fFontSize * 3.5f;
		
		// ���ĺ� �빮�� & �ҹ��� - TextRect�� Width ����.
		else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
			vecPosOffset[iCurIdx].x += m_fFontSize * 0.85f;

		// Ư�� ���� - TextRect�� Width ����.
		else if ((ch >= 33 && ch <= 64) || (ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 126))
		{
			if (ch >= 48 && ch <= 57)
				vecPosOffset[iCurIdx].x += (m_fFontSize * 1.0f);
			else
				vecPosOffset[iCurIdx].x += (m_fFontSize * 0.6f);
		}
		// �ѱ� ���� - TextRect�� Width ����.
		else
			vecPosOffset[iCurIdx].x += m_fFontSize;
	}

	/*__________________________________________________________________________________________________________
	[ Offset �߿��� ���� ū X ���� ã�´�. ]
	____________________________________________________________________________________________________________*/
	for (_uint i = 0; i < vecPosOffset.size(); ++i)
	{
		if (m_vPosOffset.x <= vecPosOffset[i].x)
			m_vPosOffset.x = vecPosOffset[i].x;
	}

	m_vPosOffset.y = vecPosOffset[0].y;

	vecPosOffset.clear();
	vecPosOffset.shrink_to_fit();

	/*__________________________________________________________________________________________________________
	[ ���� FontRect�� ũ�� ]
	____________________________________________________________________________________________________________*/
	m_TextRect.right	= m_vPos.x + m_vPosOffset.x;
	m_TextRect.bottom	= m_vPos.y + m_vPosOffset.y;

	return S_OK;

	return S_OK;
}

HRESULT CFont::LateInit_GameObject()
{


	return S_OK;
}

_int CFont::Update_GameObject(const _float & fTimeDelta)
{
	FAILED_CHECK_RETURN(CGameObject::LateInit_GameObject(), E_FAIL);

	/*__________________________________________________________________________________________________________
	[ Change Text - Change Rect Offset ]
	____________________________________________________________________________________________________________*/
	Calc_RectOffset();

	/*__________________________________________________________________________________________________________
	[ Update TextRect ]
	____________________________________________________________________________________________________________*/
	m_TextRect.left		= m_vPos.x;
	m_TextRect.top		= m_vPos.y;
	m_TextRect.right	= m_vPos.x + m_vPosOffset.x;
	m_TextRect.bottom	= m_vPos.y + m_vPosOffset.y;

	/*__________________________________________________________________________________________________________
	[ Renderer - Add Render Group ]
	____________________________________________________________________________________________________________*/
	NULL_CHECK_RETURN(m_pRenderer, -1);
	FAILED_CHECK_RETURN(m_pRenderer->Add_Renderer(CRenderer::RENDER_FONT, this), -1);

	return 1;
}

_int CFont::LateUpdate_GameObject(const _float & fTimeDelta)
{
	return 1;
}

void CFont::Render_GameObject(const _float & fTimeDelta)
{
	if (nullptr != m_pD2D_Context)
	{
		m_pD2D_Context->DrawTextW(m_wstrText.c_str(),			// �׸� ���ڿ�.
								  lstrlen(m_wstrText.c_str()),	// �׸� ���ڿ��� ����.
								  m_pTextFormat,				// ���ڿ� �ؽ�Ʈ ����.
								  &m_TextRect,					// �׸� ����.
								  m_pTextBrush);				// �귯��.
	}

}

HRESULT CFont::Calc_RectOffset()
{
	/*__________________________________________________________________________________________________________
	- ��� ���߿� ���ڿ��� �ٲ� ���, Rect Offset�� �ٽ� ����.
	____________________________________________________________________________________________________________*/
	if (m_wstrPreText != m_wstrText)
	{
		m_wstrPreText = m_wstrText;

		_tchar szTemp[MAX_STR] = L"";
		lstrcpy(szTemp, m_wstrText.c_str());

		_uint			iCurIdx = 0;
		vector<_vec2>	vecPosOffset;
		vecPosOffset.emplace_back(0.f, 0.f);

		for (_int i = 0; szTemp[i] != '\0'; ++i)
		{
			_tchar ch = szTemp[i];
			
			// ���� ���� - TextRect�� Height ����.
			if (ch == '\n')
			{
				vecPosOffset[0].y += m_fFontSize;

				vecPosOffset.emplace_back(0.f, 0.f);
				++iCurIdx;
			}
			// ���� ���� - TextRect�� Height ����.
			else if (ch == ' ')
				vecPosOffset[iCurIdx].x += m_fFontSize * 0.33f;

			// Tap ���� - TextRect�� Height ����.
			else if (ch == '\t')
				vecPosOffset[iCurIdx].x += m_fFontSize * 3.5f;
			
			// ���ĺ� �빮�� & �ҹ��� - TextRect�� Width ����.
			else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
				vecPosOffset[iCurIdx].x += m_fFontSize * 0.85f;

			// Ư�� ���� - TextRect�� Width ����.
			else if ((ch >= 33 && ch <= 64) || (ch >= 91 && ch <= 96) || (ch >= 123 && ch <= 126))
			{
				if (ch >= 48 && ch <= 57)
					vecPosOffset[iCurIdx].x += (m_fFontSize * 1.0f);
				else
					vecPosOffset[iCurIdx].x += (m_fFontSize * 0.6f);
			}
			// �ѱ� ���� - TextRect�� Width ����.
			else
				vecPosOffset[iCurIdx].x += m_fFontSize;
		}

		/*__________________________________________________________________________________________________________
		[ Offset �߿��� ���� ū X ���� ã�´�. ]
		____________________________________________________________________________________________________________*/
		for (_uint i = 0; i < vecPosOffset.size(); ++i)
		{
			if (m_vPosOffset.x <= vecPosOffset[i].x)
				m_vPosOffset.x = vecPosOffset[i].x;
		}

		m_vPosOffset.y = vecPosOffset[0].y;

		vecPosOffset.clear();
		vecPosOffset.shrink_to_fit();

		/*__________________________________________________________________________________________________________
		[ ���� FontRect�� ũ�� ]
		____________________________________________________________________________________________________________*/
		m_TextRect.right	= m_vPos.x + m_vPosOffset.x;
		m_TextRect.bottom	= m_vPos.y + m_vPosOffset.y;
	}


	return S_OK;
}

CGameObject * CFont::Clone_GameObject()
{
	return new CFont(*this);
}

CFont * CFont::Create_Prototype(ID3D12Device * pGraphicDevice,
								ID3D12GraphicsCommandList * pCommandList, 
								wstring wstrFontName, 
								const _float & fSize, 
								const D2D1::ColorF & Color)
{
	CFont* pInstance = new CFont(pGraphicDevice, pCommandList);

	if (FAILED(pInstance->Ready_GameObjectPrototype(wstrFontName, fSize, Color)))
		Safe_Release(pInstance);

	return pInstance;
}

void CFont::Free()
{
	CGameObject::Free();

	Safe_Release(m_pTextBrush);
	Safe_Release(m_pTextFormat);

}
