#pragma once


typedef struct tagFrame
{
	tagFrame()
		: fCurFrame(0.0f), fFrameCnt(1.0f)
		, fCurScene(0.0f), fSceneCnt(1.0f)
		, fFrameSpeed(1.0f)
	{}

	tagFrame(const _float& frame_cnt, const _float& scene_cnt, const _float& frame_speed)
		: fCurFrame(0.0f), fFrameCnt(frame_cnt)
		, fCurScene(0.0f), fSceneCnt(scene_cnt)
		, fFrameSpeed(frame_speed)
	{}

	_float	fFrameCnt	= 1.0f;	// ��������Ʈ �̹����� X�� ����.
	_float	fCurFrame	= 0.0f;
	_float	fSceneCnt	= 1.0f;	// ��������Ʈ �̹����� Y�� ����.
	_float	fCurScene	= 0.0f;

	_float	fFrameSpeed	= 0.0f;	// ��������Ʈ �̹��� ����ӵ�.

} FRAME;

typedef struct tagTexturePath
{
	wstring wstrTextureTag	= L"";	// TextureTag.
	wstring wstrPath		= L"";	// ���� ��� ���.
	_int	iTexSize		= 1;	// Texture ����.

} IMGPATH;

typedef struct tagMeshPath
{
	wstring wstrMeshTag		= L"";	// MeshTag.
	wstring wstrPath		= L"";	// ���� ��� ���.
	wstring wstrFileName	= L"";	// Mesh ���� �̸�.

} MESHPATH;

typedef struct tagMeshTreeCtrlInfo
{
	wstring wstrMeshType	= L"";	// ex) StaticMesh
	wstring wstrRootTag		= L"";	// ex) Stage1
	wstring wstrMeshTag		= L"";	// ex) LandMark

} MESH_TREECTRL_INFO;

typedef struct tagTextureTreeCtrlInfo
{
	wstring wstrTexType = L"";	// ex) Texture
	wstring wstrRootTag = L"";	// ex) UI
	wstring wstrTexTag	= L"";	// ex) S1UI_CharacterWindow

} TEX_TREECTRL_INFO;
