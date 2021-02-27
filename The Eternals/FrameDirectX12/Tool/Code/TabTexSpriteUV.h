#pragma once


// CTabTexSpriteUV 대화 상자
namespace Engine
{
	class CComponentMgr;
	class CObjectMgr;
	class CManagement;
	class CDescriptorHeapMgr;
}

class CToolUICanvas;

#define GRID_MIN	10.0f
#define CANVAS_MIN	100.0f

class CTabTexSpriteUV : public CDialogEx
{
	DECLARE_DYNAMIC(CTabTexSpriteUV)

public:
	CTabTexSpriteUV(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabTexSpriteUV();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabUI_TexSpriteUV };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	HRESULT	Ready_TabTexSpriteUV();

public:
	Engine::CManagement*		m_pManagement		= nullptr;
	Engine::CObjectMgr*			m_pObjectMgr		= nullptr;
	Engine::CComponentMgr*		m_pComponentMgr		= nullptr;
	Engine::CDescriptorHeapMgr* m_pDescriptorHeapMgr = nullptr;

	CToolUICanvas* m_pToolUICanvas = nullptr;


	/*__________________________________________________________________________________________________________
	[ Control ]
	____________________________________________________________________________________________________________*/
	afx_msg void	OnNMClickTree2000_TreeTextureTag(NMHDR* pNMHDR, LRESULT* pResult);
	CTreeCtrl		m_TexUITreeCtrl;

	CEdit			m_EditTextureTag;
	CString			m_strTextureTag;

	CEdit			m_EditTextureWidth;
	CEdit			m_EditTexturHeight;
	int				m_iTextureWidth;
	int				m_iTextureHeight;

	afx_msg void	OnLbnSelchangeList2000_TextureIndex();
	CListBox		m_ListBoxTexIndex;
	_uint			m_iSelectTexIndex = 0;

	CEdit			m_EditCanvasWidth;
	CEdit			m_EditCanvasHeight;
	float			m_fCanvasWidth	= (_float)WINCY;
	float			m_fCanvasHeight = (_float)WINCY;

	HRESULT			Create_GridLine(const _float& fGridWidth, const _float& fGridHeight);
	HRESULT			Create_GridRect();
	afx_msg void	OnEnChangeEdit2003_GridWidth();
	afx_msg void	OnEnChangeEdit2004_GridHeight();
	CEdit			m_EditGridWidth;
	CEdit			m_EditGridHeight;
	float			m_fGridWidth;
	float			m_fGridHeight;

	_int			m_iRectSizeX = 0;
	_int			m_iRectSizeY = 0;
	float			m_fFrameCnt;
	float			m_fCurFrame;
	float			m_fSceneCnt;
	float			m_fCurScene;


	CButton			m_ButtonSave;
	CButton			m_ButtonLoad;
	afx_msg void OnBnClickedButton2000_SAVE();
	afx_msg void OnBnClickedButton2001_LOAD();
};
