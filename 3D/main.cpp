#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <stdio.h>
#include "main.h"
#include "dinput.h"
#include "Collision.h"
#include "LightScatteringSimulation.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#define THING_AMOUNT 3
//�}�E�X���x

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pDevice;
FLOAT fCameraX = 0, fCameraY = 1.0f, fCameraZ = -15.0f,
fCameraHeading = 0, fCameraPitch = 0;

//���z�̊p�x
float SunRotation = 45.0f;

//���z�̔��a
float SunRadius = 350.0f;
//���U���V�~�����[�V�����N���X�̐錾
LSS* m_pLSS = NULL;

LPD3DXFONT pFont;
FLOAT fLookX = 0, fLookY = 1.0f, fLookZ = -3.0f;
FLOAT movabs = 0.0f;

THING Thing[4];
KEYSTATE Key[KEYMAX];
bool mouse_activate = false;
float mouse_sens = 5.0f;
//
//
HRESULT InitThing(THING *pThing, LPSTR szXFileName, D3DXVECTOR3* pvecPosition)
{
	// ���b�V���̏����ʒu
	memcpy(&pThing->vecPosition, pvecPosition, sizeof(D3DXVECTOR3));
	// X�t�@�C�����烁�b�V�������[�h����	
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX(szXFileName, D3DXMESH_SYSTEMMEM,
		pDevice, NULL, &pD3DXMtrlBuffer, NULL,
		&pThing->dwNumMaterials, &pThing->pMesh)))
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", szXFileName, MB_OK);
		return E_FAIL;
	}
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	pThing->pMeshMaterials = new D3DMATERIAL9[pThing->dwNumMaterials];
	pThing->pMeshTextures = new LPDIRECT3DTEXTURE9[pThing->dwNumMaterials];

	for (DWORD i = 0; i<pThing->dwNumMaterials; i++)
	{
		pThing->pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		pThing->pMeshMaterials[i].Ambient = pThing->pMeshMaterials[i].Diffuse;
		pThing->pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFile(pDevice,
				d3dxMaterials[i].pTextureFilename,
				&pThing->pMeshTextures[i])))
			{
				//MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
			}
		}
	}
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

//
//VOID FreeDx()
// �쐬����DirectX�I�u�W�F�N�g�̊J��
VOID FreeDx()
{
	for (DWORD i = 0; i<THING_AMOUNT; i++)
	{
		SAFE_RELEASE(Thing[i].pMesh);
	}
	SAFE_RELEASE(pDevice);
	SAFE_RELEASE(pD3d);
}
HRESULT InitD3d(HWND hWnd)
{
	// �uDirect3D�v�I�u�W�F�N�g�̍쐬
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3D�̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}
	// �uDIRECT3D�f�o�C�X�v�I�u�W�F�N�g�̍쐬
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_MIXED_VERTEXPROCESSING,
		&d3dpp, &pDevice)))
	{
		MessageBox(0, "HAL���[�h��DIRECT3D�f�o�C�X���쐬�ł��܂���\nREF���[�h�ōĎ��s���܂�", NULL, MB_OK);
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&d3dpp, &pDevice)))
		{
			MessageBox(0, "DIRECT3D�f�o�C�X�̍쐬�Ɏ��s���܂���", NULL, MB_OK);
			return E_FAIL;
		}
	}

	// X�t�@�C�����Ƀ��b�V�����쐬����
	InitThing(&Thing[0], "Ground.x", &D3DXVECTOR3(0, -0.5, 0));
	InitThing(&Thing[1], "OneMeshTank.x", &D3DXVECTOR3(0, 1, 0));
	InitThing(&Thing[2], "OneMeshLauncher.x", &D3DXVECTOR3(7, 1, 1));

	InitSphere(pDevice, &Thing[1],-4.0);
	InitSphere(pDevice, &Thing[2],-1.3f);

	// Z�o�b�t�@�[������L���ɂ���
	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	// ���C�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	// �A���r�G���g���C�g�i�����j��ݒ肷��
	pDevice->SetRenderState(D3DRS_AMBIENT, 0x00111111);
	// �X�y�L�����i���ʔ��ˁj��L���ɂ���
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	
	//�X�t�B�A�𓧖��Ƀ����_�����O�������̂ŃA���t�@�u�����f�B���O��ݒ肷��
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//���z���Œ�@�\�p�C�v���C���Ń����_�����O
	//pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	return S_OK;
}

//
//
//
VOID RenderThing(THING* pThing)
{
	//���[���h�g�����X�t�H�[���i��΍��W�ϊ��j
	D3DXMATRIXA16 matWorld, matPosition;
	D3DXMatrixIdentity(&matWorld);
	m_pLSS->Begin();

	//���z�̈ʒu���擾
	D3DXVECTOR4 LightPos, LightDir;
	//���z�̈ʒu���v�Z
	LightPos = D3DXVECTOR4(0.0f, SunRadius * sinf(D3DXToRadian(SunRotation)), SunRadius * cosf(D3DXToRadian(SunRotation)), 0.0f);
	//���z�̕����x�N�g�����v�Z
	LightDir = D3DXVECTOR4(-LightPos.x, -LightPos.y, -LightPos.z, LightPos.w);
	//���z�̕����x�N�g���𐳋K��
	D3DXVec3Normalize((D3DXVECTOR3*)&LightDir, (D3DXVECTOR3*)&LightDir);
	m_pLSS->SetMatrix(&matWorld, &LightDir);
	m_pLSS->SetAmbient(0.1f);
	//�t�H�O�̃p�����[�^��ݒ�
	m_pLSS->SetParameters(20.0f, 1.0f);
	//�t�H�O�̐F��ݒ�
	m_pLSS->SetFogColor(1.0f);
	D3DXMatrixTranslation(&matPosition, pThing->vecPosition.x, pThing->vecPosition.y,
		pThing->vecPosition.z);
	D3DXMatrixMultiply(&matWorld, &matWorld, &matPosition);

	pDevice->SetTransform(D3DTS_WORLD, &matWorld);
	// �r���[�g�����X�t�H�[���i���_���W�ϊ��j
	D3DXMATRIXA16 matView, matCameraPosition, matHeading, matPitch;
	D3DXVECTOR3 vecEyePt(fCameraX, fCameraY, fCameraZ); //�J�����i���_�j�ʒu
	D3DXVECTOR3 vecLookatPt(fLookX, fLookY, fLookZ + 5);//�����ʒu
	D3DXVECTOR3 vecUpVec(0.0f, 1.0f, 0.0f);//����ʒu  
	D3DXMatrixIdentity(&matView);
	D3DXMatrixRotationY(&matHeading, fCameraHeading);
	D3DXMatrixRotationX(&matPitch, fCameraPitch);

	D3DXMatrixLookAtLH(&matCameraPosition, &vecEyePt, &vecLookatPt, &vecUpVec);
	D3DXMatrixMultiply(&matView, &matView, &matCameraPosition);
	D3DXMatrixMultiply(&matView, &matView, &matHeading);
	D3DXMatrixMultiply(&matView, &matView, &matPitch);
	pDevice->SetTransform(D3DTS_VIEW, &matView);
	// �v���W�F�N�V�����g�����X�t�H�[���i�ˉe�ϊ��j
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	//D3DXMatrixPerspectiveFovLH(&matProj,
	//	D3DX_PI / 4.0f,
	//	4.0f / 3.0f,
	//	30.0f, 1100.0f);
	pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// ���C�g�����Ă� ���F�ŋ��ʔ��˂���ɐݒ�
	D3DXVECTOR3 vecDirection(1, 1, 1);
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;
	light.Specular.r = 1.0f;
	light.Specular.g = 1.0f;
	light.Specular.b = 1.0f;
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDirection);
	light.Range = 200.0f;
	pDevice->SetLight(0, &light);
	pDevice->LightEnable(0, TRUE);
	// �����_�����O	 
	for (DWORD i = 0; i<pThing->dwNumMaterials; i++)
	{
		pDevice->SetMaterial(&pThing->pMeshMaterials[i]);
		pDevice->SetTexture(0, pThing->pMeshTextures[i]);
	m_pLSS->BeginPass(1);
		pThing->pMesh->DrawSubset(i);
	m_pLSS->EndPass();
	}
	m_pLSS->End();
}

void RenderSphere(THING* pThing)
{
	pDevice->SetMaterial(pThing->pSphereMeshMaterials);
	pThing->pSphereMesh->DrawSubset(0);
}
VOID RenderString(LPSTR szStr, INT iX, INT iY)
{
	RECT rect = { iX, iY, 0, 0 };
	//������̃T�C�Y���v�Z
	pFont->DrawText(NULL, szStr, -1, &rect, DT_CALCRECT, NULL);
	// ���̃T�C�Y�Ń����_�����O
	pFont->DrawText(NULL, szStr, -1, &rect, DT_LEFT | DT_BOTTOM, 0xff000000);

}

VOID Render()
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	if (SUCCEEDED(pDevice->BeginScene()))
	{
		//for (DWORD i = 0; i<THING_AMOUNT; i++)
		//{
		//	RenderThing(&Thing[i]);
		//}
		RenderThing(&Thing[0]);
		RenderThing(&Thing[1]);
		//RenderSphere(&Thing[1]);
		//RenderThing(&Thing[2]);
		//RenderSphere(&Thing[2]);
		if (CollisionCheck(&Thing[1], &Thing[2]))
		{
			RenderString("�Փ˂��Ă��܂�\n���E�{�����[�����d�Ȃ��Ă���Ƃ������Ƃł�", 10, 80);
		}
		char disp[1024];
		sprintf(disp,"�}�E�X�̊��x %f",mouse_sens);
		RenderString(disp, 10, 10);

		if (mouse_activate)
		{
			RenderString("mouse activate", 10, 40);
		}
		else
		{
			RenderString("mouse none activate", 10, 40);
		}
		pDevice->EndScene();
	}
	pDevice->Present(NULL, NULL, NULL, NULL);
}

//
//
//LRESULT CALLBACK WndProc(HWND hWnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
// �E�B���h�E�v���V�[�W���֐�
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch ((CHAR)wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

void Control()
{
	static int time_count = 0;
	KeyCheck_Dinput(&Key[LEFT],DIK_LEFT);
	KeyCheck_Dinput(&Key[UP], DIK_UP);
	KeyCheck_Dinput(&Key[RIGHT], DIK_RIGHT);
	KeyCheck_Dinput(&Key[DOWN], DIK_DOWN);

	KeyCheck_Dinput(&Key[W], DIK_W);
	KeyCheck_Dinput(&Key[S], DIK_S);
	KeyCheck_Dinput(&Key[A], DIK_A);
	KeyCheck_Dinput(&Key[D], DIK_D);

	KeyCheck_Dinput(&Key[Q], DIK_Q);
	KeyCheck_Dinput(&Key[E], DIK_E);

	KeyCheck_Dinput(&Key[R], DIK_R);
	KeyCheck_Dinput(&Key[T], DIK_T);

	KeyCheck_Dinput(&Key[C], DIK_C);

	static POINT mouse;
	GetCursorPos(&mouse);
	static POINT old_mouse = mouse;
	
	float mouse_x = (float)mouse.x - old_mouse.x;
	float mouse_y = (float)mouse.y - old_mouse.y;
	old_mouse = mouse;

	if (Key[C] == PUSH)
	{
		mouse_activate = !mouse_activate;
	}


	if (Key[R] == ON && mouse_sens > 0.0f)
	{
		mouse_sens -= 0.5f;
	}
	if (Key[T] == ON && mouse_sens < 30.0f)
	{
		mouse_sens += 0.5f;
	}

	if (mouse_x != 0 && mouse_sens != 0.0f)
	{
		mouse_x = mouse_x / mouse_sens;
	}
	if (mouse_y != 0 && mouse_sens != 0.0f)
	{
		mouse_y = mouse_y / mouse_sens;
	}
	if (Key[LEFT] == ON)
	{
		Thing[1].vecPosition.x -= 0.2f;
	}
	if (Key[RIGHT] == ON)
	{
		Thing[1].vecPosition.x += 0.2f;
	}
	if (Key[UP] == ON)
	{
		Thing[1].vecPosition.z += 0.2f;
	}
	if (Key[DOWN] == ON)
	{
		Thing[1].vecPosition.z -= 0.2f;
	}

	//�����̌����Ă�������Ői�܂��悤�Ƃ������ǁA�����o���Ȃ������B
	//fLookX += movabs * cos(D3DXToRadian(mouse_x));
	//fLookY += 0.0f;
	//fLookZ -= movabs * sin(D3DXToRadian(mouse_x));
	
	if (mouse_activate)
	{
		fCameraHeading -= D3DXToRadian(mouse_x);
		fCameraPitch -= D3DXToRadian(mouse_y);
	}
	if (Key[A] == ON)
	{
		fCameraX -= 0.5f;
		fLookX -= 0.5f;
	}
	if (Key[W] == ON)
	{
		fCameraZ += 0.5f;
		fLookZ += 0.5f;
	}
	if (Key[D] == ON)
	{
		fCameraX += 0.5f;
		fLookX += 0.5f;
	}
	if (Key[S] == ON)
	{
		fCameraZ -= 0.5f;
		fLookZ -= 0.5f;
	}

	if (Key[Q] == ON)
	{
		fCameraY += 0.2f;
		fLookY += 0.2f;
	}
	if (Key[E] == ON)
	{
		fCameraY -= 0.2f;
		fLookY -= 0.2f;
	}

}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	HWND hWnd = NULL;
	MSG msg;
	// �E�B���h�E�̏�����
	static char szAppName[] = "3DView";
	WNDCLASSEX  wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		0, 0, 1024, 768, NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	// �_�C���N�g�RD�̏������֐����Ă�
	if (FAILED(InitD3d(hWnd)))
	{
		return 0;
	}

	//�_�C���N�g�C���v�b�g�̏������֐����Ă�
	if (FAILED(InitDinput()))
	{
		return 0;
	}
	m_pLSS = new LSS(pDevice);
	m_pLSS->Load("CLUTSky.jpg", "CLUTLight.jpg");

	//dinput�̃L�[�{�[�h������
	if (FAILED(InitDinput_Key(hWnd)))
	{
		return 0;
	}

	//dinput�̃}�E�X������
	if (FAILED(InitDinput_Mouse(hWnd)))
	{
		return 0;
	}

	//�����񃌃��_�����O�̏�����
	if (FAILED(D3DXCreateFont(pDevice, 0, 8, FW_REGULAR, NULL, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "tahoma", &pFont))) return E_FAIL;
	DWORD SyncOld = timeGetTime();	//	�V�X�e�����Ԃ��擾
	DWORD SyncNow;

	timeBeginPeriod(1);

	// ���b�Z�[�W���[�v
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		Sleep(1);
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			SyncNow = timeGetTime();
			if (SyncNow - SyncOld >= 1000 / 60) {	//	1�b�Ԃ�60�񂱂̒��ɓ���͂�
				Control();
				Render();
				SyncOld = SyncNow;
			}
		}
	}
	timeEndPeriod(1);

	// ���b�Z�[�W���[�v���甲������I�u�W�F�N�g��S�ĊJ������
	FreeDx();
	// OS�ɖ߂�i�A�v���P�[�V�������I������j
	return (INT)msg.wParam;
}

