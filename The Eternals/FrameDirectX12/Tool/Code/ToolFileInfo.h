#pragma once
class CToolFileInfo
{
public:
	explicit CToolFileInfo() = default;
	~CToolFileInfo() = default;

public:
	// ����� ��ȯ
	static CString ConvertRelativePath(CString strFullPath);

	// TextureDDS PathFind
	static void DirInfoExtractionDDS(const wstring& wstrPath, list<IMGPATH*>& rPathInfoLst);
	// Mesh PathFind
	static void DirInfoMeshExtraction(const wstring& wstrPath, list<MESHPATH*>& rPathInfoLst);
	// ���� ��ο� ����ִ� �̹��� ������ ����
	static _int DirFileCount(const wstring& wstrPath);
};

