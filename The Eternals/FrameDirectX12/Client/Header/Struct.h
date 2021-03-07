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