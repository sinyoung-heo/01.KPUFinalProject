#include "stdafx.h"
#include "ToolFileInfo.h"

CString CToolFileInfo::ConvertRelativePath(CString strFullPath)
{
	// 1. 상대경로를 저장할 문자열.
	_tchar szRelativePath[MAX_STR] = L"";

	// 2. 현재 작업 경로를 저장할 문자열.
	_tchar szCurrentPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szCurrentPath);

	// 3. 상대경로 구하기.
	PathRelativePathTo(szRelativePath,				// 상대 경로를 받는 문자열에 대한 포인터.
					   szCurrentPath,				// 상대 경로의 시작을 정의하는 경로를 포함하는 포인터.
					   FILE_ATTRIBUTE_DIRECTORY,	// 파일의 속성.
					   strFullPath,					// 상대 경로의 끝점을 정의하는 경로를 포함하는 포인터.
					   FILE_ATTRIBUTE_DIRECTORY);	// 파일의 속성.

	return szRelativePath;
}

void CToolFileInfo::DirInfoExtractionDDS(const wstring& wstrPath, list<IMGPATH*>& rPathInfoLst)
{
}

void CToolFileInfo::DirInfoMeshExtraction(const wstring& wstrPath, list<MESHPATH*>& rPathInfoLst)
{
}

_int CToolFileInfo::DirFileCount(const wstring& wstrPath)
{
	return _int();
}
