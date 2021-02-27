// ../Code/TabTexSpriteUV.cpp: 구현 파일
//

#include "stdafx.h"
#include "Tool.h"
#include "TabTexSpriteUV.h"
#include "afxdialogex.h"
#include "GraphicDevice.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "Management.h"
#include "DescriptorHeapMgr.h"
#include "ToolSceneStage.h"
#include "ToolUICanvas.h"
#include "ToolGridLine.h"
#include "ToolGridRect.h"


// CTabTexSpriteUV 대화 상자

IMPLEMENT_DYNAMIC(CTabTexSpriteUV, CDialogEx)

CTabTexSpriteUV::CTabTexSpriteUV(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CTabUI_TexSpriteUV, pParent)
	, m_pComponentMgr(Engine::CComponentMgr::Get_Instance())
	, m_pObjectMgr(Engine::CObjectMgr::Get_Instance())
	, m_pManagement(Engine::CManagement::Get_Instance())
	, m_pDescriptorHeapMgr(Engine::CDescriptorHeapMgr::Get_Instance())
	, m_strTextureTag(_T(""))
	, m_iTextureWidth(0)
	, m_iTextureHeight(0)
	, m_fGridWidth(0.0f)
	, m_fGridHeight(0.0f)
	, m_fFrameCnt(0.0f)
	, m_fCurFrame(0.0f)
	, m_fSceneCnt(0.0f)
	, m_fCurScene(0.0f)
{

}

CTabTexSpriteUV::~CTabTexSpriteUV()
{
}

void CTabTexSpriteUV::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE2000,		m_TexUITreeCtrl);
	DDX_Control(pDX, IDC_EDIT2000,		m_EditTextureTag);
	DDX_Control(pDX, IDC_EDIT2001,		m_EditTextureWidth);
	DDX_Control(pDX, IDC_EDIT2002,		m_EditTexturHeight);
	DDX_Control(pDX, IDC_LIST2000,		m_ListBoxTexIndex);
	DDX_Control(pDX, IDC_EDIT2010,		m_EditCanvasWidth);
	DDX_Control(pDX, IDC_EDIT2011,		m_EditCanvasHeight);
	DDX_Control(pDX, IDC_EDIT2003,		m_EditGridWidth);
	DDX_Control(pDX, IDC_EDIT2004,		m_EditGridHeight);
	DDX_Control(pDX, IDC_BUTTON2000,	m_ButtonSave);
	DDX_Control(pDX, IDC_BUTTON2001,	m_ButtonLoad);
	DDX_Text(pDX, IDC_EDIT2000, m_strTextureTag);
	DDX_Text(pDX, IDC_EDIT2001, m_iTextureWidth);
	DDX_Text(pDX, IDC_EDIT2002, m_iTextureHeight);
	DDX_Text(pDX, IDC_EDIT2010, m_fCanvasWidth);
	DDX_Text(pDX, IDC_EDIT2011, m_fCanvasHeight);
	DDX_Text(pDX, IDC_EDIT2003, m_fGridWidth);
	DDX_Text(pDX, IDC_EDIT2004, m_fGridHeight);
	DDX_Text(pDX, IDC_EDIT2005, m_fFrameCnt);
	DDX_Text(pDX, IDC_EDIT2006, m_fCurFrame);
	DDX_Text(pDX, IDC_EDIT2007, m_fSceneCnt);
	DDX_Text(pDX, IDC_EDIT2008, m_fCurScene);
}


BEGIN_MESSAGE_MAP(CTabTexSpriteUV, CDialogEx)
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY(NM_CLICK, IDC_TREE2000, &CTabTexSpriteUV::OnNMClickTree2000_TreeTextureTag)
	ON_LBN_SELCHANGE(IDC_LIST2000, &CTabTexSpriteUV::OnLbnSelchangeList2000_TextureIndex)
	ON_EN_CHANGE(IDC_EDIT2003, &CTabTexSpriteUV::OnEnChangeEdit2003_GridWidth)
	ON_EN_CHANGE(IDC_EDIT2004, &CTabTexSpriteUV::OnEnChangeEdit2004_GridHeight)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON2000, &CTabTexSpriteUV::OnBnClickedButton2000_SAVE)
	ON_BN_CLICKED(IDC_BUTTON2001, &CTabTexSpriteUV::OnBnClickedButton2001_LOAD)
END_MESSAGE_MAP()


// CTabTexSpriteUV 메시지 처리기


BOOL CTabTexSpriteUV::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_TexUITreeCtrl.EnableWindow(FALSE);
	m_EditTextureTag.EnableWindow(FALSE);
	m_EditTextureWidth.EnableWindow(FALSE);
	m_EditTexturHeight.EnableWindow(FALSE);
	m_ListBoxTexIndex.EnableWindow(FALSE);
	m_EditCanvasWidth.EnableWindow(FALSE);
	m_EditCanvasHeight.EnableWindow(FALSE);
	m_EditGridWidth.EnableWindow(FALSE);
	m_EditGridHeight.EnableWindow(FALSE);
	m_ButtonSave.EnableWindow(FALSE);
	m_ButtonLoad.EnableWindow(FALSE);


	m_fCanvasWidth	= (_float)WINCY;
	m_fCanvasHeight = (_float)WINCY;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTabTexSpriteUV::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CTabTexSpriteUV::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	UpdateData(TRUE);

	RECT rcEdit[4] = { };
	m_EditGridWidth.GetWindowRect(&rcEdit[0]);
	m_EditGridHeight.GetWindowRect(&rcEdit[1]);
	m_EditCanvasWidth.GetWindowRect(&rcEdit[2]);
	m_EditCanvasHeight.GetWindowRect(&rcEdit[3]);

	if (PtInRect(&rcEdit[0], pt))	// Grid Width
	{
		if (zDelta > 0)
		{
			m_fGridWidth += 5.0f;
			if (m_fGridWidth >= m_fCanvasWidth)
				m_fGridWidth = m_fCanvasWidth;
		}
		else if (zDelta < 0.0f)
		{
			m_fGridWidth -= 5.0f;
			if (m_fGridWidth <= GRID_MIN)
				m_fGridWidth = GRID_MIN;
		}

		Create_GridLine(m_fGridWidth, m_fGridHeight);
		Create_GridRect();
	}

	else if (PtInRect(&rcEdit[1], pt))	// Grid Height
	{
		if (zDelta > 0)
		{
			m_fGridHeight += 5.0f;
			if (m_fGridHeight >= m_fCanvasHeight)
				m_fGridHeight = m_fCanvasHeight;
		}
		else if (zDelta < 0.0f)
		{
			m_fGridHeight -= 5.0f;
			if (m_fGridHeight <= GRID_MIN)
				m_fGridHeight = GRID_MIN;
		}

		Create_GridLine(m_fGridWidth, m_fGridHeight);
		Create_GridRect();
	}

	else if (PtInRect(&rcEdit[2], pt) ||	// Canvas Width
			 PtInRect(&rcEdit[3], pt))		// Canvas Height
	{
		if (zDelta > 0)
		{
			m_fCanvasWidth	+= 10.0f;
			m_fCanvasHeight += 10.0f;
		}
		else if (zDelta < 0.0f)
		{
			m_fCanvasWidth	-= 10.0f;
			m_fCanvasHeight -= 10.0f;
			if (m_fCanvasWidth <= CANVAS_MIN)
			{
				m_fCanvasWidth = CANVAS_MIN;
				m_fCanvasHeight = CANVAS_MIN;
			}
		}

		// ToolUICanvas Pos와 Scale 설정.
		m_pToolUICanvas->Get_Transform()->m_vScale	= _vec3(m_fCanvasWidth, m_fCanvasHeight, 1.0f);
		m_pToolUICanvas->Get_Transform()->m_vPos	= _vec3(m_fCanvasWidth / 2, m_fCanvasHeight / 2.0f, 1.0f);
	}

	UpdateData(FALSE);

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

HRESULT CTabTexSpriteUV::Ready_TabTexSpriteUV()
{
	UpdateData(TRUE);

	m_TexUITreeCtrl.EnableWindow(TRUE);
	m_EditTextureTag.EnableWindow(TRUE);
	m_EditTextureWidth.EnableWindow(TRUE);
	m_EditTexturHeight.EnableWindow(TRUE);
	m_ListBoxTexIndex.EnableWindow(TRUE);
	m_EditCanvasWidth.EnableWindow(TRUE);
	m_EditCanvasHeight.EnableWindow(TRUE);
	m_EditGridWidth.EnableWindow(TRUE);
	m_EditGridHeight.EnableWindow(TRUE);
	m_ButtonSave.EnableWindow(TRUE);
	m_ButtonLoad.EnableWindow(TRUE);

	// Ready Mesh TreeControl.
	HTREEITEM h_Texture, h_TextureRoot;

	h_Texture = m_TexUITreeCtrl.InsertItem(L"Texture", NULL, NULL);

	wifstream fin{ L"../../Bin/ToolData/TexTreeCtrlInfo.txt" };
	if (fin.fail())
		return E_FAIL;

	_tchar	szCurTexType[MIN_STR] = L"";
	_tchar	szCurTexRoot[MIN_STR] = L"";
	_tchar	szTexTag[MAX_STR] = L"";

	wstring wstrPreTexType = L"";
	wstring wstrPreTexRoot = L"";

	while (true)
	{
		fin.getline(szCurTexType, MIN_STR, '|');
		fin.getline(szCurTexRoot, MIN_STR, '|');
		fin.getline(szTexTag, MIN_STR);

		if (fin.eof())
			break;

		wstring wstrCurTexType = szCurTexType;
		wstring wstrCurTexRoot = szCurTexRoot;

		if (L"Texture" == wstrCurTexType &&
			L"UI" == wstrCurTexRoot)
		{
			if (wstrPreTexRoot != wstrCurTexRoot)
			{
				wstrPreTexRoot = wstrCurTexRoot;
				h_TextureRoot = m_TexUITreeCtrl.InsertItem(wstrPreTexRoot.c_str(), h_Texture, NULL);
			}

			m_TexUITreeCtrl.InsertItem(szTexTag, h_TextureRoot, NULL);
		}

	}

	fin.close();

	// Texture
	m_TexUITreeCtrl.Expand(h_Texture, TVE_EXPAND);

	HTREEITEM h_Child = m_TexUITreeCtrl.GetNextItem(h_Texture, TVGN_CHILD);
	m_TexUITreeCtrl.Expand(h_Child, TVE_EXPAND);

	while (h_Child != NULL)
	{
		h_Child = m_TexUITreeCtrl.GetNextItem(h_Child, TVGN_NEXT);
		m_TexUITreeCtrl.Expand(h_Child, TVE_EXPAND);
	}

	UpdateData(FALSE);

	return S_OK;
}

void CTabTexSpriteUV::OnNMClickTree2000_TreeTextureTag(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CPoint mouse;
	GetCursorPos(&mouse);
	m_TexUITreeCtrl.ScreenToClient(&mouse);

	_uint iFlag = 0;
	HTREEITEM h_MeshTag = m_TexUITreeCtrl.HitTest(mouse, &iFlag);

	// 클릭한 Tree의 문자열을 얻어온다.
	m_strTextureTag = m_TexUITreeCtrl.GetItemText(h_MeshTag);

	if (m_strTextureTag == L"UI")
		return;

	// Texture의 Width와 Height를 얻어온다.
	Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(wstring(m_strTextureTag), Engine::ID_STATIC));
	m_iSelectTexIndex = 0;
	m_iTextureWidth   = (_int)pTextureCom->Get_Texture().front().Get()->GetDesc().Width;
	m_iTextureHeight  = (_int)pTextureCom->Get_Texture().front().Get()->GetDesc().Height;

	// Texture DescriptorHeap의 Num을 얻어온다.
	m_ListBoxTexIndex.ResetContent();

	_int iNumDescriptor = pTextureCom->Get_TexDescriptorHeap()->GetDesc().NumDescriptors;
	for (_int i = 0; i < iNumDescriptor; ++i)
	{
		_tchar m_szText[MAX_STR] = L"";
		wsprintf(m_szText, L"Idx : %d", i);

		m_ListBoxTexIndex.AddString(m_szText);
	}
	
	// ToolCanvas에 DescriptorHeap, TexIndex 설정.
	m_pToolUICanvas->m_pTexDescriptorHeap = pTextureCom->Get_TexDescriptorHeap();
	m_pToolUICanvas->m_uiTexIdx           = 0;
	//m_fCanvasWidth	                      = (_float)WINCY;
	//m_fCanvasHeight                       = (_float)WINCY;

	// ToolUICanvas Pos와 Scale 설정.
	m_pToolUICanvas->Get_Transform()->m_vScale	= _vec3(m_fCanvasWidth, m_fCanvasHeight, 1.0f);
	m_pToolUICanvas->Get_Transform()->m_vPos	= _vec3(m_fCanvasWidth / 2.0f, m_fCanvasHeight / 2.0f, 1.0f);


	// ToolGridLine 생성.
	Create_GridLine(m_fCanvasWidth, m_fCanvasHeight);
	Create_GridRect();

	*pResult = 0;

	UpdateData(FALSE);
}


void CTabTexSpriteUV::OnLbnSelchangeList2000_TextureIndex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	// 선택한 ListBox의 Index를 얻어온다.
	m_iSelectTexIndex = m_ListBoxTexIndex.GetCaretIndex();

	// 선택한 TexIdx의 Width와 Height를 갱신한다.
	Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(wstring(m_strTextureTag), Engine::ID_STATIC));
	m_iTextureWidth  = (_int)pTextureCom->Get_Texture()[m_iSelectTexIndex].Get()->GetDesc().Width;
	m_iTextureHeight = (_int)pTextureCom->Get_Texture()[m_iSelectTexIndex].Get()->GetDesc().Height;

	// ToolCanvas에 DescriptorHeap, TexIndex 설정.
	m_pToolUICanvas->m_pTexDescriptorHeap = pTextureCom->Get_TexDescriptorHeap();
	m_pToolUICanvas->m_uiTexIdx           = m_iSelectTexIndex;

	// ToolUICanvas Pos와 Scale 설정.
	m_pToolUICanvas->Get_Transform()->m_vScale	= _vec3(m_fCanvasWidth, m_fCanvasHeight, 1.0f);
	m_pToolUICanvas->Get_Transform()->m_vPos	= _vec3(m_fCanvasWidth / 2, m_fCanvasHeight / 2.0f, 1.0f);

	// ToolGridLine 생성.
	//Create_GridLine(m_fCanvasWidth, m_fCanvasHeight);
	//Create_GridRect();

	UpdateData(FALSE);
}


HRESULT CTabTexSpriteUV::Create_GridLine(const _float& fGridWidth, const _float& fGridHeight)
{
	m_pObjectMgr->Clear_OBJLIST(L"Layer_UI", L"UIGridLineWidth");
	m_pObjectMgr->Clear_OBJLIST(L"Layer_UI", L"UIGridLineHeight");
	m_fGridWidth  = fGridWidth;
	m_fGridHeight = fGridHeight;

	Engine::CGameObject* pGridLine = nullptr;

	_vec3 vPos = _vec3(0.0f);

	m_fFrameCnt = m_fCanvasWidth / m_fGridWidth;
	m_fSceneCnt = m_fCanvasHeight / m_fGridHeight;

	for (_int i = 0; i < (_int)(ceil(m_fCanvasWidth / m_fGridWidth)) + 1; ++i)
	{
		vPos.y = m_fCanvasHeight / 2.0f;

		pGridLine  = CToolGridLine::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(), 
										   Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMD_MAIN),
										   _vec3(1.0f, m_fCanvasHeight, 1.0f),
										   vPos,
										   900);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_UI", L"UIGridLineWidth", pGridLine), E_FAIL);

		vPos.x += m_fGridWidth;
	}

	vPos = _vec3(0.0f);
	for (_int i = 0; i < (_int)(ceil(m_fCanvasHeight / m_fGridHeight)) + 1; ++i)
	{
		vPos.x = m_fCanvasWidth / 2.0f;

		pGridLine  = CToolGridLine::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(), 
										   Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMD_MAIN),
										   _vec3(m_fCanvasWidth, 1.0f, 1.0f),
										   vPos,
										   900);
		Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_UI", L"UIGridLineHeight", pGridLine), E_FAIL);

		vPos.y += m_fGridHeight;
	}

	return S_OK;
}

HRESULT CTabTexSpriteUV::Create_GridRect()
{
	Engine::CGameObject* pGridRect = nullptr;
	m_pObjectMgr->Clear_OBJLIST(L"Layer_UI", L"UIGridRect");

	m_iRectSizeX = (_int)(ceil(m_fFrameCnt));
	m_iRectSizeY = (_int)(ceil(m_fSceneCnt));

	_vec3 vScale;
	vScale.x = m_fCanvasWidth / m_fFrameCnt;
	vScale.y = m_fCanvasHeight / m_fSceneCnt;
	vScale.z = 1.0f;

	_vec3 vPos = _vec3(1.0f);

	for (_int i = 0; i < m_iRectSizeY; ++i)
	{
		for (_int j = 0; j < m_iRectSizeX; ++j)
		{
			vPos.x = vScale.x * 0.5f + vScale.x * j;
			vPos.y = vScale.y * 0.5f + vScale.y * i;

			pGridRect = CToolGridRect::Create(Engine::CGraphicDevice::Get_Instance()->Get_GraphicDevice(),
											  Engine::CGraphicDevice::Get_Instance()->Get_CommandList(Engine::CMD_MAIN),
											  vScale,
											  vPos,
											  950);
			Engine::FAILED_CHECK_RETURN(m_pObjectMgr->Add_GameObject(L"Layer_UI", L"UIGridRect", pGridRect), E_FAIL);
		}

	}

	return S_OK;
}


void CTabTexSpriteUV::OnEnChangeEdit2003_GridWidth()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_fGridWidth >= m_fCanvasWidth)
		m_fGridWidth = m_fCanvasWidth;

	if (m_fGridWidth <= GRID_MIN)
		m_fGridWidth = GRID_MIN;


	Create_GridLine(m_fGridWidth, m_fGridHeight);
	Create_GridRect();

	UpdateData(FALSE);
}


void CTabTexSpriteUV::OnEnChangeEdit2004_GridHeight()
{
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if (m_fGridHeight >= m_fCanvasHeight)
		m_fGridHeight = m_fCanvasHeight;

	if (m_fGridHeight <= GRID_MIN)
		m_fGridHeight = GRID_MIN;

	Create_GridLine(m_fGridWidth, m_fGridHeight);
	Create_GridRect();

	UpdateData(FALSE);
}



void CTabTexSpriteUV::OnBnClickedButton2000_SAVE()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileDialog Dlg(FALSE,
					L"texuv",
					L"*.texuv",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.texuv) | *.texuv ||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wofstream fout { Dlg.GetPathName().GetString() };
		if (fout.fail())
		{
			AfxMessageBox(L"Save is Failed");
			return;
		}

		// StaticMesh Data 저장
		fout << wstring(m_strTextureTag)	<< L" "		// TextureTag
			 << m_iSelectTexIndex			<< L" "		// TextureIndex
			 << m_fFrameCnt					<< L" "		// FrameCnt
			 << m_fCurFrame					<< L" "		// CurFrame
			 << m_fSceneCnt					<< L" "		// SceneCnt
			 << m_fCurScene					<< L" "		// CurScene
			 << m_fCanvasWidth				<< L" "		// CanvasWidth
			 << m_fCanvasHeight				<< L" "		// CanvasHeight
			 << m_fGridWidth				<< L" "		// GridWidth
			 << m_fGridHeight				<< L" ";	// GridHeight

		AfxMessageBox(L"Data Save Successed");
	}

	UpdateData(FALSE);
}


void CTabTexSpriteUV::OnBnClickedButton2001_LOAD()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	Engine::OBJLIST* m_plstGridRect = m_pObjectMgr->Get_OBJLIST(L"Layer_UI", L"UIGridRect");
	auto iter_begin = m_plstGridRect->end();
	iter_begin = --iter_begin;

	CFileDialog Dlg(TRUE,
					L"texuv",
					L"*.texuv",
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					L"Data Files(*.texuv)|*.texuv||",
					this);

	_tchar szPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szPath);		// 작업중인 현재 경로.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	PathRemoveFileSpec(szPath);					// 마지막 폴더 삭제.
	lstrcat(szPath, L"\\Bin\\ToolData");
	Dlg.m_ofn.lpstrInitialDir = szPath;

	if (Dlg.DoModal() == IDOK)
	{
		wifstream fin { Dlg.GetPathName().GetString() };
		if (fin.fail())
		{
			AfxMessageBox(L"Load is Failed");
			return;
		}		

		wstring	wstrTextureTag = L"";
		_int	iTextureIndex  = 0;
		_float	fFrameCnt	   = 0.0f;
		_float	fCurFrame	   = 0.0f;
		_float	fSceneCnt	   = 0.0f;
		_float	fCurScene	   = 0.0f;
		_float	fCanvasWidth   = 0.0f;
		_float	fCanvasHeight  = 0.0f;
		_float	fGridWidth     = 0.0f;
		_float	fGridHeight    = 0.0f;

		while (true)
		{

			fin >> wstrTextureTag 	// TextureTag
				>> iTextureIndex	// TextureIndex
				>> fFrameCnt		// FrameCnt
				>> fCurFrame		// CurFrame
				>> fSceneCnt		// SceneCnt
				>> fCurScene		// CurScene
				>> fCanvasWidth		// CanvasWidth
				>> fCanvasHeight	// CanvasHeight
				>> fGridWidth		// GridWidth
				>> fGridHeight;		// GridHeight

			if (fin.eof())
				break;

			m_strTextureTag   = wstrTextureTag.c_str();
			m_iSelectTexIndex = iTextureIndex;
			m_fFrameCnt       = fFrameCnt;
			m_fCurFrame       = fCurFrame;
			m_fSceneCnt       = fSceneCnt;
			m_fCurScene       = fCurScene;
			m_fCanvasWidth    = fCanvasWidth;	// CanvasWidth
			m_fCanvasHeight   = fCanvasHeight;	// CanvasHeight
			m_fGridWidth      = fGridWidth;
			m_fGridHeight     = fGridHeight;


			// 선택한 TexIdx의 Width와 Height를 갱신한다.
			Engine::CTexture* pTextureCom = static_cast<Engine::CTexture*>(m_pComponentMgr->Get_Component(wstring(m_strTextureTag), Engine::ID_STATIC));
			if (m_iSelectTexIndex >= pTextureCom->Get_Texture().size())
				m_iSelectTexIndex = 0;
			m_iTextureWidth  = (_int)pTextureCom->Get_Texture()[m_iSelectTexIndex].Get()->GetDesc().Width;
			m_iTextureHeight = (_int)pTextureCom->Get_Texture()[m_iSelectTexIndex].Get()->GetDesc().Height;

			// ToolCanvas에 DescriptorHeap, TexIndex 설정.
			m_pToolUICanvas->m_pTexDescriptorHeap = pTextureCom->Get_TexDescriptorHeap();
			m_pToolUICanvas->m_uiTexIdx           = m_iSelectTexIndex;

			// ToolUICanvas Pos와 Scale 설정.
			m_pToolUICanvas->Get_Transform()->m_vScale	= _vec3(m_fCanvasWidth, m_fCanvasHeight, 1.0f);
			m_pToolUICanvas->Get_Transform()->m_vPos	= _vec3(m_fCanvasWidth / 2, m_fCanvasHeight / 2.0f, 1.0f);

			// Texture DescriptorHeap의 Num을 얻어온다.
			m_ListBoxTexIndex.ResetContent();

			_int iNumDescriptor = pTextureCom->Get_TexDescriptorHeap()->GetDesc().NumDescriptors;
			for (_int i = 0; i < iNumDescriptor; ++i)
			{
				_tchar m_szText[MAX_STR] = L"";
				wsprintf(m_szText, L"Idx : %d", i);

				m_ListBoxTexIndex.AddString(m_szText);
			}

			// Grid 생성.
			Create_GridLine(m_fGridWidth, m_fGridHeight);
			Create_GridRect();
		}

		AfxMessageBox(L"Data Load Successed");
	}

	_bool bIsStop = false;
	for (_int i = 0; i < ceil(m_fSceneCnt); ++i)
	{
		for (_int j = 0; j < ceil(m_fFrameCnt); ++j, ++iter_begin)
		{
			if (i == m_fCurScene && j == m_fCurFrame)
			{
				static_cast<CToolGridRect*>(*(++iter_begin))->m_bIsSelect = true;
				bIsStop = true;
				break;
			}
		}
		if (bIsStop)
			break;
	}


	UpdateData(FALSE);
}
