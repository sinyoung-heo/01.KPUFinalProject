#pragma once


// CTabPathFinder 대화 상자

class CTabPathFinder : public CDialogEx
{
	DECLARE_DYNAMIC(CTabPathFinder)

public:
	CTabPathFinder(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTabPathFinder();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CTabPathFinder };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnBnClickedRadio_Mesh();
	afx_msg void OnBnClickedRadio_Texture();
	_bool m_bIsClickedRadio_Mesh	= false;
	_bool m_bIsClickedRadio_Texture	= false;

	afx_msg void OnBnClickedButton_Save();

public:
	/*__________________________________________________________________________________________________________
	[ Controler ]
	____________________________________________________________________________________________________________*/
	CListBox	m_ListBoxPath;
	CButton		m_Radio_ResourceType[2];

	/*__________________________________________________________________________________________________________
	[ Value ]
	____________________________________________________________________________________________________________*/
	list<MESHPATH*> m_lstMeshFileInfo;
	_int m_iMeshPathCnt = 0;

	list<IMGPATH*>	m_lstTextureFileInfo;
	_int m_iTexturePathCnt = 0;


	list<MESH_TREECTRL_INFO*> m_lstMeshTreeCtrlInfo;
	list<TEX_TREECTRL_INFO*> m_lstTexTreeCtrlInfo;
};
