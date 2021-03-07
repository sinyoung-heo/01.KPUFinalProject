#pragma once
#include "Include.h"

class CToolFileInfo
{
public:
	explicit CToolFileInfo() = default;
	~CToolFileInfo() = default;

public:
	// 상대경로 변환
	static CString ConvertRelativePath(CString strFullPath);

	// TextureDDS PathFind
	static void DirInfoExtractionDDS(const wstring& wstrPath, 
									 list<IMGPATH*>& rPathInfoLst,
									 list<TEX_TREECTRL_INFO*>& lstTexTreeCtrlInfo);

	// Mesh PathFind
	static void DirInfoMeshExtraction(const wstring& wstrPath,
									  list<MESHPATH*>& rPathInfoLst,
									  list<MESH_TREECTRL_INFO*>& lstMeshTreeCtrlInfo);

	// 현재 경로에 들어있는 이미지 파일의 개수
	static _int DirFileCount(const wstring& wstrPath);
};

