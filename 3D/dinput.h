#include <windows.h>
#include <mmsystem.h>
#include <dinput.h>

/**
*@file dinput_lib.h
*@brief DirectInput関係のライブラリ
*@author 小谷慶太朗
*/


typedef struct
{
	int dim_x;
	int dim_y;
	int dim_z;
	bool LEFT_mouse;
	bool RIGHT_mouse;
}MOUSEKIND;

/**
*@brief KEYの状態を分かり易くするためにenumで列挙している。
*/
enum KEYSTATE {
	PUSH,
	RELEASE,
	ON,
	OFF
};


/**
*@brief KEYの入力を分かり易くするためにenumで列挙している。
*/
enum KEYKIND
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	FIRE,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	SPACE,
	ESC,
	KEYMAX
};


extern LPDIRECTINPUTDEVICE8 pKeyDevice;
extern LPDIRECTINPUT8 pDinput;
extern LPDIRECTINPUTDEVICE8 g_pMouse;

/**
*@brief キーの入力状態を管理する関数
*/
void KeyCheck_Dinput(KEYSTATE* Key, int DIK);

/**
*@brief DirectInputのデバイス作成
*/
HRESULT InitDinput();

/**
*@brief マウスのボタンと座標のチェック
*/
void Mouse_Check(MOUSEKIND* Mouse);

/**
*@brief DirectInputのキーボードデバイス作成
*/HRESULT InitDinput_Key(HWND hWnd);

/**
*@brief DirectInputのマウスデバイス作成
*/
HRESULT InitDinput_Mouse(HWND hWnd);
