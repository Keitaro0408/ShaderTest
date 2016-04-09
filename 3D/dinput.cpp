#include "dinput.h"


#define BUFFER_SIZE  64
LPDIRECTINPUT8 pDinput = NULL;
LPDIRECTINPUTDEVICE8 g_pMouse = NULL;
LPDIRECTINPUTDEVICE8 pKeyDevice = NULL;

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")

HRESULT InitDinput()
{
	HRESULT hr;

	//「DirectInput」オブジェクトの作成
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDinput, NULL)))
	{
		return hr;
	}

	return S_OK;
}



void KeyCheck_Dinput(KEYSTATE* Key, int DIK)
{
	BYTE diks[256];
	static BYTE old_diks[256] = { OFF };

	HRESULT hr = pKeyDevice->Acquire();
	if ((hr == DI_OK) || (hr == S_FALSE))
	{
		pKeyDevice->GetDeviceState(sizeof(diks), &diks);

		if (diks[DIK] & 0x80)
		{
			if (old_diks[DIK] == OFF)
			{
				*Key = PUSH;
				old_diks[DIK] = ON;
			}
			else
			{
				*Key = ON;
				old_diks[DIK] = ON;
			}
		}
		else
		{
			if (old_diks[DIK] == ON)
			{
				*Key = RELEASE;
				old_diks[DIK] = OFF;
			}
			else
			{
				*Key = OFF;
				old_diks[DIK] = OFF;
			}
		}
	}
}


HRESULT InitDinput_Key(HWND hWnd)
{
	HRESULT hr;

	//キーボードの初期化
	if (FAILED(hr = pDinput->CreateDevice(GUID_SysKeyboard,
		&pKeyDevice, NULL)))
	{
		return hr;
	}

	//データフォーマット
	if (FAILED(hr = pKeyDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return hr;
	}

	//協調レベル
	if (FAILED(hr = pKeyDevice->SetCooperativeLevel(
		hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return hr;
	}


	pKeyDevice->Acquire();
	return S_OK;
}


HRESULT InitDinput_Mouse(HWND hWnd)
{
	HRESULT hr;


	//マウスの初期化
	if (FAILED(hr = pDinput->CreateDevice(GUID_SysMouse,
		&g_pMouse, NULL)))
	{
		return hr;
	}

	//データフォーマット
	if (FAILED(hr = g_pMouse->SetDataFormat(&c_dfDIMouse)))
	{
		return hr;
	}

	//協調レベル
	if (FAILED(hr = g_pMouse->SetCooperativeLevel(hWnd,
		DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		return hr;
	}




	DIPROPDWORD dipdw;
	// the header
	dipdw.diph.dwSize = sizeof(dipdw);
	dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;


	if (FAILED(hr = g_pMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph)))
	{
		return hr;
	}


	// バッファ
	dipdw.dwData = BUFFER_SIZE;

	if (FAILED(hr = g_pMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
	{
		return hr;
	}



	g_pMouse->Acquire();
	return S_OK;
}



void Mouse_Check(MOUSEKIND* Mouse)
{
	DIMOUSESTATE dims;
	g_pMouse->Acquire();
	HRESULT hr = g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &dims);
	if (SUCCEEDED(hr))
	{
		if (dims.rgbButtons[0])
		{
			Mouse->LEFT_mouse = true;
		}
		else
		{
			Mouse->LEFT_mouse = false;
		}

		if (dims.rgbButtons[1])
		{
			Mouse->RIGHT_mouse = true;
		}
		else
		{
			Mouse->RIGHT_mouse = false;
		}

		if (dims.lX)
		{
			Mouse->dim_x = dims.lX;
		}

		if (dims.lY)
		{
			Mouse->dim_y = dims.lY;
		}

		if (dims.lZ)
		{
			Mouse->dim_z = dims.lZ;
		}
	}
}
