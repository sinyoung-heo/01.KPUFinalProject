#include "stdafx.h"
#include "ToolFileInfo.h"

CString CToolFileInfo::ConvertRelativePath(CString strFullPath)
{
	// 1. ����θ� ������ ���ڿ�.
	_tchar szRelativePath[MAX_STR] = L"";

	// 2. ���� �۾� ��θ� ������ ���ڿ�.
	_tchar szCurrentPath[MAX_STR] = L"";
	GetCurrentDirectory(MAX_STR, szCurrentPath);

	// 3. ����� ���ϱ�.
	PathRelativePathTo(szRelativePath,				// ��� ��θ� �޴� ���ڿ��� ���� ������.
					   szCurrentPath,				// ��� ����� ������ �����ϴ� ��θ� �����ϴ� ������.
					   FILE_ATTRIBUTE_DIRECTORY,	// ������ �Ӽ�.
					   strFullPath,					// ��� ����� ������ �����ϴ� ��θ� �����ϴ� ������.
					   FILE_ATTRIBUTE_DIRECTORY);	// ������ �Ӽ�.

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
