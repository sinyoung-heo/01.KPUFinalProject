// CTabPathFinder.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabPathFinder.h"
#include "afxdialogex.h"


// CTabPathFinder 대화 상자

IMPLEMENT_DYNAMIC(CTabPathFinder, CDialogEx)

CTabPathFinder::CTabPathFinder(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabPathFinder, pParent)
{;
}

CTabPathFinder::~CTabPathFinder()
{
	for_each(m_lstMeshFileInfo.begin(), m_lstMeshFileInfo.end(), Engine::Safe_Delete<MESHPATH*>);
	for_each(m_lstTextureFileInfo.begin(), m_lstTextureFileInfo.end(), Engine::Safe_Delete<IMGPATH*>);
}

void CTabPathFinder::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, m_ListBoxPath);
	DDX_Control(pDX, IDC_RADIO2, m_Radio_ResourceType[0]);
	DDX_Control(pDX, IDC_RADIO3, m_Radio_ResourceType[1]);

}


BEGIN_MESSAGE_MAP(CTabPathFinder, CDialogEx)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_RADIO2, &CTabPathFinder::OnBnClickedRadio_Mesh)
	ON_BN_CLICKED(IDC_RADIO3, &CTabPathFinder::OnBnClickedRadio_Texture)
	ON_BN_CLICKED(IDC_BUTTON1, &CTabPathFinder::OnBnClickedButton_Save)
END_MESSAGE_MAP()


// CTabPathFinder 메시지 처리기
BOOL CTabPathFinder::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_Radio_ResourceType[0].SetCheck(TRUE);
	m_bIsClickedRadio_Mesh = true;


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabPathFinder::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTabPathFinder::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialogEx::OnDropFiles(hDropInfo);
	UpdateData(TRUE);

	m_ListBoxPath.ResetContent();

	for_each(m_lstMeshFileInfo.begin(), m_lstMeshFileInfo.end(), Engine::Safe_Delete<MESHPATH*>);
	m_lstMeshFileInfo.clear();

	for_each(m_lstTextureFileInfo.begin(), m_lstTextureFileInfo.end(), Engine::Safe_Delete<IMGPATH*>);
	m_lstTextureFileInfo.clear();


	int		iCount = DragQueryFile(hDropInfo, -1, nullptr, 0);
	TCHAR	szFullPath[MAX_STR] = L"";


	if (m_bIsClickedRadio_Mesh)
	{
		for (int i = 0; i < iCount; ++i)
		{
			DragQueryFile(hDropInfo, i, szFullPath, MAX_STR);
			CToolFileInfo::DirInfoMeshExtraction(szFullPath, m_lstMeshFileInfo);
		}

		wstring wstrCombine			= L"";
		TCHAR	szCount[MIN_STR]	= L"";

		for (auto& pMeshPath : m_lstMeshFileInfo)
		{
			wstrCombine = pMeshPath->wstrMeshTag + L"|"
						+ pMeshPath->wstrFileName + L"|"
						+ pMeshPath->wstrPath;

			m_ListBoxPath.AddString(wstrCombine.c_str());
		}

		m_iMeshPathCnt = (_int)m_lstMeshFileInfo.size();
	}

	else if (m_bIsClickedRadio_Texture)
	{
		for (int i = 0; i < iCount; ++i)
		{
			DragQueryFile(hDropInfo, i, szFullPath, MAX_STR);
			CToolFileInfo::DirInfoExtractionDDS(szFullPath, m_lstTextureFileInfo);
		}

		wstring wstrCombine		= L"";
		TCHAR szCount[MIN_STR]	= L"";

		for (auto& pImgPath : m_lstTextureFileInfo)
		{
			/*____________________________________________________________________
			_itow_s: 정수 -> 문자열로 변환.
			______________________________________________________________________*/
			_itow_s(pImgPath->iTexSize, szCount, 10); // 현재 10진수로 변환.


			wstrCombine = pImgPath->wstrTextureTag + L"|"
						+ szCount + L"|"
						+ pImgPath->wstrPath;

			m_ListBoxPath.AddString(wstrCombine.c_str());
		}

		m_iTexturePathCnt = (_int)m_lstTextureFileInfo.size();
	}


	UpdateData(FALSE);
}


void CTabPathFinder::OnBnClickedRadio_Mesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_Radio_ResourceType[0].SetCheck(true);
	m_Radio_ResourceType[1].SetCheck(false);

	if (m_Radio_ResourceType[0].GetCheck())
	{
		m_bIsClickedRadio_Mesh		= true;
		m_bIsClickedRadio_Texture	= false;
	}	

	UpdateData(FALSE);
}


void CTabPathFinder::OnBnClickedRadio_Texture()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_Radio_ResourceType[0].SetCheck(false);
	m_Radio_ResourceType[1].SetCheck(true);

	if (m_Radio_ResourceType[1].GetCheck())
	{
		m_bIsClickedRadio_Mesh		= false;
		m_bIsClickedRadio_Texture	= true;
	}

	UpdateData(FALSE);
}


void CTabPathFinder::OnBnClickedButton_Save()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_bIsClickedRadio_Mesh)
	{
		// Mesh File Path
		wofstream fout{ L"../../Bin/ToolData/PathFind_Mesh.txt" };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		for (auto& pMeshPath : m_lstMeshFileInfo)
			fout << pMeshPath->wstrMeshTag << L"|" << pMeshPath->wstrFileName << "|" << pMeshPath->wstrPath << endl;

		fout.close();
		WinExec("notepad.exe ../../Bin/ToolData/PathFind_Mesh.txt", SW_SHOW);

		// Mesh File Path Count
		fout.open(L"../../Bin/ToolData/PathFind_Mesh_Count.txt");

		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		fout << m_iMeshPathCnt << endl;

		fout.close();
	}

	else if (m_bIsClickedRadio_Texture)
	{
		// Mesh File Path
		wofstream fout{ L"../../Bin/ToolData/PathFind_Texture.txt" };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		for (auto& pImgPath : m_lstTextureFileInfo)

			fout << pImgPath->wstrTextureTag << L"|" << pImgPath->iTexSize << "|" << pImgPath->wstrPath << endl;
		fout.close();
		WinExec("notepad.exe ../../Bin/ToolData/PathFind_Texture.txt", SW_SHOW);

		// Texture File Path Count
		fout.open(L"../../Bin/ToolData/PathFind_Texture_Count.txt");

		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		fout << m_iTexturePathCnt << endl;
	}


	UpdateData(FALSE);
}
