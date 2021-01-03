#include "DirectInput.h"

USING(Engine)
IMPLEMENT_SINGLETON(CDirectInput)

CDirectInput::CDirectInput()
{
}



HRESULT CDirectInput::Ready_InputDevice(HINSTANCE hInst, HWND hWnd)
{
	FAILED_CHECK_RETURN(DirectInput8Create(hInst,
										   DIRECTINPUT_VERSION,
										   IID_IDirectInput8,
										   (void**)&m_pInputSDK,
										   nullptr),
										   E_FAIL);

	// Ű���� ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	// ������ Ű���� ��ü�� ���� ������ �����Ѵ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�(Ŭ���̾�Ʈ�� �� �ִ� ���¿��� Ű �Է��� ���� �� ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ� :������ Ű���� Ȱ��ȭ 
	m_pKeyBoard->Acquire();

	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	// DISCL_EXCLUSIVE		: ���콺�� Ŭ���̾�Ʈ ȭ�� �ȿ����� ������. Ŀ�� ������ X
	// DISCL_NONEXCLUSIVE	: ���콺�� Ŭ���̾�Ʈ ȭ�� �����ε� ������. Ŀ�� ������ O
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CDirectInput::SetUp_InputState()
{
	HRESULT hr;

	hr = m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	if (DIERR_INPUTLOST == hr || DIERR_NOTACQUIRED == hr)
		m_pKeyBoard->Acquire();

	hr = m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);
	if (DIERR_INPUTLOST == hr || DIERR_NOTACQUIRED == hr)
		m_pMouse->Acquire();

	memcpy(m_byKeyPressed, m_byKeyPressedTemp, 256);
	memcpy(m_byKeyDown, m_byKeyDownTemp, 256);

	memcpy(&m_MousePressed, &m_MousePressedTemp, sizeof(DIMOUSESTATE));
	memcpy(&m_MouseDown, &m_MouseDownTemp, sizeof(DIMOUSESTATE));
}

bool CDirectInput::Key_Pressing(const byte & byKey)
{
	if (m_byKeyState[byKey])
		return true;

	return false;
}

bool CDirectInput::Key_Up(const byte & byKey)
{
	if (m_byKeyState[byKey])
	{
		m_byKeyPressed[byKey] = m_byKeyState[byKey];
		m_byKeyPressedTemp[byKey] = m_byKeyPressed[byKey];
		return false;
	}
	else if (m_byKeyPressed[byKey])
	{
		if (m_byKeyPressedTemp[byKey])
			m_byKeyPressedTemp[byKey] = m_byKeyState[byKey];

		return true;
	}

	return false;
}

bool CDirectInput::Key_Down(const byte & byKey)
{
	if (m_byKeyState[byKey] && !m_byKeyDown[byKey])
	{
		m_byKeyDownTemp[byKey] = m_byKeyState[byKey];
		return true;
	}
	else if (!m_byKeyState[byKey] && m_byKeyDown[byKey])
	{
		m_byKeyDownTemp[byKey] = m_byKeyState[byKey];
		return false;
	}

	return false;
}

bool CDirectInput::Key_Combine(const byte & byFirstKey, const byte & bySecondKey)
{
	if (m_byKeyState[byFirstKey] && Key_Pressing(bySecondKey))
		return true;

	return false;
}

bool CDirectInput::Key_CombineUp(const byte & byFirstKey, const byte & bySecondKey)
{
	if (m_byKeyState[byFirstKey] && Key_Up(bySecondKey))
		return true;

	return false;
}

bool CDirectInput::Key_CombineDown(const byte & byFirstKey, const byte & bySecondKey)
{
	if (m_byKeyState[byFirstKey] && Key_Down(bySecondKey))
		return true;

	return false;
}

bool CDirectInput::Mouse_KeyPressing(const MOUSEBUTTON & eMouseKey)
{
	if (m_MouseState.rgbButtons[eMouseKey])
		return true;

	return false;
}

bool CDirectInput::Mouse_KeyUp(const MOUSEBUTTON & eMouseKey)
{
	if (m_MouseState.rgbButtons[eMouseKey])
	{
		m_MousePressed.rgbButtons[eMouseKey] = m_MouseState.rgbButtons[eMouseKey];
		m_MousePressedTemp.rgbButtons[eMouseKey] = m_MousePressed.rgbButtons[eMouseKey];
		return false;
	}
	else if (m_MousePressed.rgbButtons[eMouseKey])
	{
		if (m_MousePressedTemp.rgbButtons[eMouseKey])
			m_MousePressedTemp.rgbButtons[eMouseKey] = m_MouseState.rgbButtons[eMouseKey];

		return true;
	}

	return false;
}

bool CDirectInput::Mouse_KeyDown(const MOUSEBUTTON & eMouseKey)
{

	if (m_MouseState.rgbButtons[eMouseKey] && !m_MouseDown.rgbButtons[eMouseKey])
	{
		m_MouseDownTemp.rgbButtons[eMouseKey] = m_MouseState.rgbButtons[eMouseKey];
		return true;
	}
	else if (!m_MouseState.rgbButtons[eMouseKey] && m_MouseDown.rgbButtons[eMouseKey])
	{
		m_MouseDownTemp.rgbButtons[eMouseKey] = m_MouseState.rgbButtons[eMouseKey];
		return false;
	}

	return false;
}


void CDirectInput::Free()
{
#ifdef ENGINE_LOG
	COUT_STR("Destroy DirectInput");
#endif

	Engine::Safe_Release(m_pKeyBoard);
	Engine::Safe_Release(m_pMouse);
	Engine::Safe_Release(m_pInputSDK);
}
