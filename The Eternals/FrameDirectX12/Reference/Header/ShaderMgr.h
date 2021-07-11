#pragma once
#include "Engine_Include.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CShaderMgr : public CBase
{
	DECLARE_SINGLETON(CShaderMgr)

private:
	explicit CShaderMgr();
	virtual ~CShaderMgr() = default;

public:
	void Input_ShaderKey();
public:
	CB_SHADER_INFORMATION&	Get_ShaderInfo() { return m_pCB_ShaderInformation; }
	void Set_IsOnShader(wstring wstrTag, const _bool& bIsOn);
	void Set_TreeAlphaTest(const _float& fOnOff) { m_pCB_ShaderInformation.TreeAlphaTest = fOnOff; }
	void SetUp_ShaderInfo();
	const _bool& Find_IsOnShader(wstring wstrTag);
public:
	void Set_DOF(int Mode) { m_pCB_ShaderInformation.DepthOfField = _float(Mode); }

private:
	CB_SHADER_INFORMATION	m_pCB_ShaderInformation;
	map<wstring, _bool>		m_mapIsOnShader;

private:
	virtual void Free();
};

END

