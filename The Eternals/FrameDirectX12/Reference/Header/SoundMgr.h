#ifndef __SOUNDMGR_H__
#define __SOUNDMGR_H__

#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

private:
	CSoundMgr(void);
	~CSoundMgr(void);

public:
	HRESULT		Ready_SoundMgr(void);
	HRESULT		Load_Sound(const char* pFilePath);
	void		Update_Sound(void);

public:
	void		Play_Sound(const _tchar* pSoundTag, SOUNDID eID, const _float& fVolume = 1.0f);
	void		Play_BGM(const _tchar* pSoundTag, const _float& fVolume = 1.0f);

public:
	void		Stop_Sound(SOUNDID eID);
	void		Stop_AllSound(void);

private:
	FMOD::Sound*		Find_Sound(const _tchar* pSoundTag);

private:
	FMOD::System*				m_pSystem;
	list<FMOD::Channel*>		m_ChannelLst[SOUND_END];
	
private:
	map<wstring, FMOD::Sound*>		m_MapSound;

private:
	void		Free(void);

};

END

#endif // !__SOUNDMGR_H__


