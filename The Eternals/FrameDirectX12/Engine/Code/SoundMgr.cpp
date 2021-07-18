#include "Engine_Include.h"
#include "SoundMgr.h"

USING(Engine)
USING(FMOD)

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
}


CSoundMgr::~CSoundMgr()
{
	Free();
}

HRESULT CSoundMgr::Ready_SoundMgr(void)
{
	System_Create(&m_pSystem);
	m_pSystem->init(SOUND_END * MAX_SOUNDCHANNEL, FMOD_INIT_NORMAL, nullptr);

	return S_OK;
}

void CSoundMgr::Update_Sound()
{
	if (nullptr != m_pSystem)
		m_pSystem->update();
}

void CSoundMgr::Play_Sound(const _tchar* pSoundTag, SOUNDID eID)
{
	Sound* pSound = Find_Sound(pSoundTag);

	if (nullptr == pSound)
		return;

	Channel* pChannel = nullptr;

	m_pSystem->playSound(pSound, 0, false, &pChannel);
	m_ChannelLst[eID].push_back(pChannel);

	if (MAX_SOUNDCHANNEL < m_ChannelLst[eID].size())
	{
		pChannel = m_ChannelLst[eID].front();

		if (nullptr != pChannel)
			pChannel->stop();

		m_ChannelLst[eID].pop_front();
	}
}

void CSoundMgr::Play_BGM(const _tchar* pSoundTag)
{
	Sound* pSound = Find_Sound(pSoundTag);

	if (nullptr == pSound)
		return;

	Channel* pChannel = nullptr;

	m_pSystem->playSound(pSound, 0, false, &pChannel);
	pChannel->setMode(FMOD_LOOP_NORMAL);

	m_ChannelLst[SOUND_BGM].push_back(pChannel);

	if (MAX_SOUNDCHANNEL < m_ChannelLst[SOUND_BGM].size())
	{
		pChannel = m_ChannelLst[SOUND_BGM].front();

		if (nullptr != pChannel)
			pChannel->stop();

		m_ChannelLst[SOUND_BGM].pop_front();
	}
}

void CSoundMgr::Stop_Sound(SOUNDID eID)
{
	for (auto& pChannel : m_ChannelLst[eID])
	{
		if (nullptr != pChannel)
			pChannel->stop();
	}

	m_ChannelLst[eID].clear();
}

void CSoundMgr::Stop_AllSound()
{
	for (_uint i = 0; i < SOUND_END; ++i)
	{
		for (auto& pChannel : m_ChannelLst[i])
		{
			if (nullptr != pChannel)
				pChannel->stop();
		}

		m_ChannelLst[i].clear();
	}
}

FMOD::Sound* CSoundMgr::Find_Sound(const _tchar * pSoundTag)
{
	auto iter_find = m_MapSound.find(pSoundTag);

	if (iter_find == m_MapSound.end())
		return nullptr;

	return iter_find->second;
}

HRESULT CSoundMgr::Load_Sound(const char* pFilePath)
{
	_finddata_t fd = {};

	// pFilePath: "../Sound/*.*"
	intptr_t handle = _findfirst(pFilePath, &fd);

	if (0 == handle)
		return E_FAIL;

	_int iLength = _int(strlen(pFilePath));

	_int i = 0;

	for (; i < iLength; ++i)
	{
		if ('*' == pFilePath[i])
			break;
	}

	char szRelative[256] = "";
	char szFullPath[256] = "";

	// szRelative: ../Sound/
	memcpy(szRelative, pFilePath, i);

	_int result = 0;

	while (-1 != result)
	{
		strcpy_s(szFullPath, szRelative);
		strcat_s(szFullPath, fd.name);

		Sound* pSound = nullptr;

		FMOD_RESULT res = m_pSystem->createSound(szFullPath,
			FMOD_DEFAULT, nullptr, &pSound);

		if (FMOD_OK == res)
		{
			string strName = fd.name;
			wstring wstrName;
			wstrName.assign(strName.begin(), strName.end());
			// MultiByteToWideChar(CP_ACP, 0, fd.name, iBuffSize, pSoundTag, iBuffSize);
			m_MapSound.emplace(wstrName, pSound);
		}

		result = _findnext(handle, &fd);
	}

	_findclose(handle);

	m_pSystem->update();

	return S_OK;
}

void CSoundMgr::Free()
{
	for (auto& pair : m_MapSound)
		pair.second->release();

	m_MapSound.clear();

	m_pSystem->close();
	m_pSystem->release();
}