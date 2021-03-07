#pragma once
#include "Include.h"

class CToolFileInfo
{
public:
	explicit CToolFileInfo() = default;
	~CToolFileInfo() = default;

public:
	// ����� ��ȯ
	static CString ConvertRelativePath(CString strFullPath);

	// TextureDDS PathFind
	static void DirInfoExtractionDDS(const wstring& wstrPath, 
									 list<IMGPATH*>& rPathInfoLst,
									 list<TEX_TREECTRL_INFO*>& lstTexTreeCtrlInfo);

	// Mesh PathFind
	static void DirInfoMeshExtraction(const wstring& wstrPath,
									  list<MESHPATH*>& rPathInfoLst,
									  list<MESH_TREECTRL_INFO*>& lstMeshTreeCtrlInfo);

	// ���� ��ο� ����ִ� �̹��� ������ ����
	static _int DirFileCount(const wstring& wstrPath);
};

