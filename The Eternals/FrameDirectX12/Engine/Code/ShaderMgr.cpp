#include "ShaderMgr.h"
#include "DirectInput.h"
USING(Engine)
IMPLEMENT_SINGLETON(CShaderMgr)


CShaderMgr::CShaderMgr()
{
	ZeroMemory(&m_pCB_ShaderInformation, sizeof(CB_SHADER_INFORMATION));
}

void CShaderMgr::Input_ShaderKey()
{
	if (KEY_DOWN(DIK_NUMPAD0))
	{
		m_pCB_ShaderInformation.DepthOfField == 0.f ? m_pCB_ShaderInformation.DepthOfField = 1.f
			: m_pCB_ShaderInformation.DepthOfField = 0.f;

		m_mapIsOnShader[L"DOF"] = !m_mapIsOnShader[L"DOF"];
	}
	if (KEY_DOWN(DIK_NUMPAD1))
	{
		m_pCB_ShaderInformation.Ssao == 0.f ? m_pCB_ShaderInformation.Ssao = 1.f
			: m_pCB_ShaderInformation.Ssao = 0.f;

		m_mapIsOnShader[L"SSAO"] = !m_mapIsOnShader[L"SSAO"];
	}
	if (KEY_DOWN(DIK_NUMPAD2))
	{
		m_pCB_ShaderInformation.Shade == 0.f ? m_pCB_ShaderInformation.Shade = 1.f
			: m_pCB_ShaderInformation.Shade = 0.f;

		m_mapIsOnShader[L"Shade"] = !m_mapIsOnShader[L"Shade"];
	}
	if (KEY_DOWN(DIK_NUMPAD3))
	{
		m_pCB_ShaderInformation.ToneMapping == 0.f ? m_pCB_ShaderInformation.ToneMapping = 1.f
			: m_pCB_ShaderInformation.ToneMapping = 0.f;

		m_mapIsOnShader[L"HDRTone"] = !m_mapIsOnShader[L"HDRTone"];
	}
	if (KEY_DOWN(DIK_NUMPAD4))
	{
		m_pCB_ShaderInformation.Specular == 0.f ? m_pCB_ShaderInformation.Specular = 1.f
			: m_pCB_ShaderInformation.Specular = 0.f;

		m_mapIsOnShader[L"Specular"] = !m_mapIsOnShader[L"Specular"];
	}
	if (KEY_DOWN(DIK_NUMPAD5))
	{
		m_pCB_ShaderInformation.TreeAlphaTest == 0.f ? m_pCB_ShaderInformation.TreeAlphaTest = 1.f
			: m_pCB_ShaderInformation.TreeAlphaTest = 0.f;
	}
}

void CShaderMgr::Set_IsOnShader(wstring wstrTag, const _bool& bIsOn)
{
	auto iter_find = m_mapIsOnShader.find(wstrTag);

	if (iter_find == m_mapIsOnShader.end())
		return;

	iter_find->second = bIsOn;
}

void CShaderMgr::SetUp_ShaderInfo()
{
	m_mapIsOnShader[L"DOF"]      = false;
	m_mapIsOnShader[L"SSAO"]     = true;
	m_mapIsOnShader[L"Shade"]    = true;
	m_mapIsOnShader[L"HDRTone"]  = true;
	m_mapIsOnShader[L"Specular"] = true;
	m_mapIsOnShader[L"Shadow"]   = true;

	m_pCB_ShaderInformation.DepthOfField  = 0.f;
	m_pCB_ShaderInformation.Ssao          = 0.f;
	m_pCB_ShaderInformation.Shade         = 0.f;
	m_pCB_ShaderInformation.ToneMapping   = 0.f;
	m_pCB_ShaderInformation.Specular      = 1.f;
	m_pCB_ShaderInformation.TreeAlphaTest = 1.f;
}

const _bool& CShaderMgr::Find_IsOnShader(wstring wstrTag)
{
	auto iter_find = m_mapIsOnShader.find(wstrTag);

	if (iter_find == m_mapIsOnShader.end())
		return false;

	return iter_find->second;
}

void CShaderMgr::Free()
{
}
