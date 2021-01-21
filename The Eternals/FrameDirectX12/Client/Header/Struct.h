#pragma once

typedef struct tagFrame
{
	tagFrame()
		: fCurFrame(0.0f), iFrameCnt(1)
		, fCurScene(0.0f), iSceneCnt(1)
		, fFrameSpeed(1.0f)
	{}

	tagFrame(const _int& frame_cnt, const _int& scene_cnt, const _float& frame_speed)
		: fCurFrame(0.0f), iFrameCnt(frame_cnt)
		, fCurScene(0.0f), iSceneCnt(scene_cnt)
		, fFrameSpeed(frame_speed)
	{}

	_int	iFrameCnt	= 1;	// ��������Ʈ �̹����� X�� ����.
	_float	fCurFrame	= 0.0f;
	_int	iSceneCnt	= 1;	// ��������Ʈ �̹����� Y�� ����.
	_float	fCurScene	= 0.0f;

	_float	fFrameSpeed	= 0.0f;	// ��������Ʈ �̹��� ����ӵ�.

} FRAME;