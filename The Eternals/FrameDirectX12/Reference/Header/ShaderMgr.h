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
	CB_SHADER_INFORMATION& Get_ShaderInfo() { return m_pCB_ShaderInformation; }
public:
	void Set_DOF(int Mode) { m_pCB_ShaderInformation.DepthOfField = Mode; }

private:
	CB_SHADER_INFORMATION m_pCB_ShaderInformation;

private:
	virtual void Free();
};

END

