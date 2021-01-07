#pragma once
class CToolFileInfo
{
public:
	explicit CToolFileInfo() = default;
	~CToolFileInfo() = default;

public:
	// 상대경로 변환
	static CString ConvertRelativePath(CString strFullPath);

	// TextureDDS PathFind
	static void DirInfoExtractionDDS(const wstring& wstrPath, list<IMGPATH*>& rPathInfoLst);
	// Mesh PathFind
	static void DirInfoMeshExtraction(const wstring& wstrPath, list<MESHPATH*>& rPathInfoLst);
	// 현재 경로에 들어있는 이미지 파일의 개수
	static _int DirFileCount(const wstring& wstrPath);
};

