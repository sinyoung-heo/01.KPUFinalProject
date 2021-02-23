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

void CToolFileInfo::DirInfoExtractionDDS(const wstring& wstrPath, 
										 list<IMGPATH*>& rPathInfoLst,
										 list<TEX_TREECTRL_INFO*>& lstTexTreeCtrlInfo)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	// 파일 및 경로 제어 관련 제공. (MFC 클래스)
	CFileFind find;

	// 현재 경로에서 첫 번째 파일 탐색.
	find.FindFile(wstrFilePath.c_str());

	int iContinue = 1;
	while (iContinue)
	{
		iContinue = find.FindNextFileW();

		// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
		if (find.IsDots())
			continue;

		// 찾은 이름이 파일명이 아닌 폴더명이라면 true반환.
		else if (find.IsDirectory())
			DirInfoExtractionDDS(wstring(find.GetFilePath()), rPathInfoLst, lstTexTreeCtrlInfo);

		// find 객체가 찾은 것이 파일 이름일 때.
		else
		{
			if (find.IsSystem())
				continue;

			IMGPATH* pImgPath = new IMGPATH;
			TEX_TREECTRL_INFO* pTreeCtrlInfo = new TEX_TREECTRL_INFO;
			_tchar szCurPath[MAX_STR] = L"";

			/*__________________________________________________________________________________________________________
			[ 현재 경로 저장 ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\Texture\Fire\0.dds
			____________________________________________________________________________________________________________*/
			lstrcpy(szCurPath, find.GetFilePath().GetString());

			/*__________________________________________________________________________________________________________
			[ 파일명과 확장자 제거 ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\Texture\Fire
			____________________________________________________________________________________________________________*/
			PathRemoveFileSpec(szCurPath);

			/*__________________________________________________________________________________________________________
			[ 현재 폴더 내의 파일의 개수를 얻는다. ]
			____________________________________________________________________________________________________________*/
			pImgPath->iTexSize = DirFileCount(szCurPath);

			/*__________________________________________________________________________________________________________
			[ 경로 상에서 온전한 파일명만 얻어온다. ]
			ex) wstrTexturename = 0, 1, 2, 3, 4 ....
			____________________________________________________________________________________________________________*/
			wstring wstrTextureName = find.GetFileTitle().GetString();

			// wstrTextureName = 0.dds
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.length() - 1) + L"%d.dds";

			// szFullPath : D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\Texture\Fire\%d.dds
			_tchar szFullPath[MAX_STR] = L"";
			PathCombine(szFullPath, szCurPath, wstrTextureName.c_str());

			/*__________________________________________________________________________________________________________
			[ 상대 경로 변환. ]
			____________________________________________________________________________________________________________*/
			_tchar wstrOriginPath[MAX_STR] = L"";
			lstrcpy(wstrOriginPath, szCurPath);
			
			pImgPath->wstrPath			= ConvertRelativePath(szFullPath);
			pImgPath->wstrTextureTag	= PathFindFileName(szCurPath);

			rPathInfoLst.push_back(pImgPath);


			/*__________________________________________________________________________________________________________
			[ TreeCtrl Info 저장. ]
			____________________________________________________________________________________________________________*/
			pTreeCtrlInfo->wstrTexTag = pImgPath->wstrTextureTag;

			PathRemoveFileSpec(wstrOriginPath);
			CFileFind ff1;
			ff1.FindFile(wstrOriginPath);

			_int iResult2 = 1;
			while (iResult2)
			{
				iResult2 = ff1.FindNextFileW();
				// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
				if (ff1.IsDots())
					continue;

				// 찾은 이름이 파일명이 아닌 폴더명이라면 true 반환.
				if (ff1.IsDirectory())
				{
					iResult2 = 0;
					pTreeCtrlInfo->wstrRootTag = ff1.GetFileName();
				}
			}


			PathRemoveFileSpec(wstrOriginPath);
			CFileFind ff2;
			ff2.FindFile(wstrOriginPath);

			_int iResult3 = 1;
			while (iResult3)
			{
				iResult3 = ff2.FindNextFileW();
				// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
				if (ff2.IsDots())
					continue;

				// 찾은 이름이 파일명이 아닌 폴더명이라면 true 반환.
				if (ff2.IsDirectory())
				{
					iResult3 = 0;
					pTreeCtrlInfo->wstrTexType = ff2.GetFileName();
				}
			}

			lstTexTreeCtrlInfo.push_back(pTreeCtrlInfo);




			iContinue = 0;
		}
	}
}

void CToolFileInfo::DirInfoMeshExtraction(const wstring& wstrPath, 
										  list<MESHPATH*>& rPathInfoLst,
										  list<MESH_TREECTRL_INFO*>& lstMeshTreeCtrlInfo)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	// 파일 및 경로 제어 관련 제공. (MFC 클래스)
	CFileFind find;

	// 현재 경로에서 첫 번째 파일 탐색.
	find.FindFile(wstrFilePath.c_str());

	int iContinue = 1;
	while (iContinue)
	{
		iContinue = find.FindNextFileW();

		// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
		if (find.IsDots())
			continue;

		// 찾은 이름이 파일명이 아닌 폴더명이라면 true반환.
		else if (find.IsDirectory())
			DirInfoMeshExtraction(wstring(find.GetFilePath()), rPathInfoLst, lstMeshTreeCtrlInfo);

		// find 객체가 찾은 것이 파일 이름일 때.
		else
		{
			if (find.IsSystem())
				continue;

			MESHPATH*			pMeshPath		= new MESHPATH;
			MESH_TREECTRL_INFO* pTreeCtrlInfo	= new MESH_TREECTRL_INFO;
			_tchar szCurPath[MAX_STR] = L"";

			/*__________________________________________________________________________________________________________
			[ 현재 경로 저장 ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\StaticMesh\Sample\LandMark\HD_LandMark_Diff.dds
			____________________________________________________________________________________________________________*/
			lstrcpy(szCurPath, find.GetFilePath().GetString());

			/*__________________________________________________________________________________________________________
			[ 파일명과 확장자 제거 ]
			ex) D:\ ~  The Eternals\FrameDirectX12\Bin\Resource\ResourceStage\StaticMesh\Sample\LandMark\
			____________________________________________________________________________________________________________*/
			PathRemoveFileSpec(szCurPath);

			/*__________________________________________________________________________________________________________
			[ 폴더명을 얻어와서 MeshTag와 MeshFileName 얻어오기 ]
			____________________________________________________________________________________________________________*/
			CFileFind find_directory;
			find_directory.FindFile(szCurPath);

			_int iResult = 1;
			while (iResult)
			{
				iResult = find_directory.FindNextFileW();

				// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
				if (find_directory.IsDots())
					continue;

				// 찾은 이름이 파일명이 아닌 폴더명이라면 true 반환.
				if (find_directory.IsDirectory())
				{
					iResult = 0;

					pMeshPath->wstrMeshTag	= find_directory.GetFileName();
					pMeshPath->wstrFileName = pMeshPath->wstrMeshTag + L".X";
				}
			}

			/*__________________________________________________________________________________________________________
			[ 상대 경로 변환. ]
			____________________________________________________________________________________________________________*/
			_tchar wstrOriginPath[MAX_STR] = L"";
			lstrcpy(wstrOriginPath, szCurPath);
			

			pMeshPath->wstrPath = ConvertRelativePath(szCurPath);
			pMeshPath->wstrPath = pMeshPath->wstrPath + L"\\";
			rPathInfoLst.push_back(pMeshPath);

			/*__________________________________________________________________________________________________________
			[ TreeCtrl Info 저장. ]
			____________________________________________________________________________________________________________*/
			pTreeCtrlInfo->wstrMeshTag = pMeshPath->wstrMeshTag;

			PathRemoveFileSpec(wstrOriginPath);
			CFileFind ff1;
			ff1.FindFile(wstrOriginPath);

			_int iResult2 = 1;
			while (iResult2)
			{
				iResult2 = ff1.FindNextFileW();
				// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
				if (ff1.IsDots())
					continue;

				// 찾은 이름이 파일명이 아닌 폴더명이라면 true 반환.
				if (ff1.IsDirectory())
				{
					iResult2 = 0;
					pTreeCtrlInfo->wstrRootTag = ff1.GetFileName();
				}
			}


			PathRemoveFileSpec(wstrOriginPath);
			CFileFind ff2;
			ff2.FindFile(wstrOriginPath);

			_int iResult3 = 1;
			while (iResult3)
			{
				iResult3 = ff2.FindNextFileW();
				// 현재 찾은 파일 이름이 "." 이나 ".."인지 검사. 맞으면 true 반환.
				if (ff2.IsDots())
					continue;

				// 찾은 이름이 파일명이 아닌 폴더명이라면 true 반환.
				if (ff2.IsDirectory())
				{
					iResult3 = 0;
					pTreeCtrlInfo->wstrMeshType = ff2.GetFileName();
				}
			}

			lstMeshTreeCtrlInfo.push_back(pTreeCtrlInfo);


			iContinue = 0;
		}
	}
}

_int CToolFileInfo::DirFileCount(const wstring& wstrPath)
{
	wstring wstrFilePath = wstrPath + L"\\*.*";

	CFileFind find;
	find.FindFile(wstrFilePath.c_str());

	_int iFileCnt = 0;
	_int iContinue = 1;
	while (iContinue)
	{
		iContinue = find.FindNextFile();

		if (find.IsDots())
			continue;
		else if (find.IsDirectory())
			continue;
		else if (find.IsSystem())
			continue;

		++iFileCnt;
	}

	return iFileCnt;
}
