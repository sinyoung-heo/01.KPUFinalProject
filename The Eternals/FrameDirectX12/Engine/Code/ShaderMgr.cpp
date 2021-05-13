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
	}
	if (KEY_DOWN(DIK_NUMPAD1))
	{
		m_pCB_ShaderInformation.Ssao == 0.f ? m_pCB_ShaderInformation.Ssao = 1.f
			: m_pCB_ShaderInformation.Ssao = 0.f;
	}
	if (KEY_DOWN(DIK_NUMPAD2))
	{
		m_pCB_ShaderInformation.Shade == 0.f ? m_pCB_ShaderInformation.Shade = 1.f
			: m_pCB_ShaderInformation.Shade = 0.f;
	}
	if (KEY_DOWN(DIK_NUMPAD3))
	{
		m_pCB_ShaderInformation.ToneMapping == 0.f ? m_pCB_ShaderInformation.ToneMapping = 1.f
			: m_pCB_ShaderInformation.ToneMapping = 0.f;
	}
	if (KEY_DOWN(DIK_NUMPAD4))
	{
		m_pCB_ShaderInformation.Specular == 0.f ? m_pCB_ShaderInformation.Specular = 1.f
			: m_pCB_ShaderInformation.Specular = 0.f;
	}
	if (KEY_DOWN(DIK_NUMPAD5))
	{
		m_pCB_ShaderInformation.TreeAlphaTest == 0.f ? m_pCB_ShaderInformation.TreeAlphaTest = 1.f
			: m_pCB_ShaderInformation.TreeAlphaTest = 0.f;
	}
}

void CShaderMgr::SetUp_ShaderInfo()
{
	m_pCB_ShaderInformation.DepthOfField  = 0.f;
	m_pCB_ShaderInformation.Ssao          = 0.f;
	m_pCB_ShaderInformation.Shade         = 0.f;
	m_pCB_ShaderInformation.ToneMapping   = 0.f;
	m_pCB_ShaderInformation.Specular      = 1.f;
	m_pCB_ShaderInformation.TreeAlphaTest = 1.f;
}

void CShaderMgr::Free()
{
}
