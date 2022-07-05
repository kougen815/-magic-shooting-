//=============================================================================
//
// main���� [main.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"

#include "title.h"

#include "bg.h"
#include "map.h"
#include "GameUI.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "bullet_explosion.h"
#include "score.h"

#include "result.h"

#include "sound.h"

#include "fade.h"
#include "file.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME			"AppClass"				// �E�C���h�E�̃N���X��
#define WINDOW_NAME			"�}�W�b�N �V���[�e�B���O(Magic Shooting)"			// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void CheckGameClear(void);


//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;


#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

int g_Mode = MODE_TITLE;					// �N�����̉�ʂ�ݒ�

BOOL g_LoadGame = FALSE;					// NewGame


//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	//UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// �E�B���h�E�̍����W
						CW_USEDEFAULT,																		// �E�B���h�E�̏���W
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// �E�B���h�E����
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
						NULL,
						NULL,
						hInstance,
						NULL);

	// �E�B���h�E���[�h���t���X�N���[�����[�h���̏���
	bool mode = true;

	int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// Yes�Ȃ�Window���[�h�ŋN��
		mode = true;
		break;
	case IDNO:		// No�Ȃ�t���X�N���[�����[�h�ŋN��
		mode = false;
		break;
	case IDCANCEL:	// CANCEL�Ȃ�I��
	default:
		return -1;
		break;
	}

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, mode)))
	{
		return -1;
	}

	//// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	//if(FAILED(Init(hInstance, hWnd, true)))
	//{
	//	return -1;
	//}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;
	
	// �E�C���h�E�̕\��(Init()�̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// �V�X�e���������擾

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;		// �����񐔂̃J�E���g�����Z
			}
		}
	}
	
	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_�����O�����̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	// �J���������̏�����
	InitCamera();

	// ���͏����̏�����
	InitInput(hInstance, hWnd);

	// �T�E���h�����̏�����
	InitSound(hWnd);

	// �t�F�[�h�����̏�����
	InitFade();

	// �ŏ��̃��[�h���Z�b�g���āA���̒���Init����
	SetMode(g_Mode);	// ������SetMode�̂܂܂ŁI

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	// �Q�[���I�ȕ��͂��̒���Uninit����
	SetMode(MODE_END);	// ������SetMode�̂܂܂ŁI

	// �t�F�[�h�̏I������
	UninitFade();

	// �T�E���h�̏I������
	UninitSound();

	// ���͂̏I������
	UninitInput();

	// �J�����̏I������
	UninitCamera();

	// �����_�����O�̏I������
	UninitRenderer();

}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	// ���͂̍X�V����
	UpdateInput();

	// �J�����̍X�V����
	UpdateCamera();

	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̍X�V

		// �^�C�g���̍X�V����
		UpdateTitle();

		break;

	case MODE_GAME:			// �Q�[����ʂ̍X�V

		// BG�̍X�V����
		UpdateBG();

		// �}�b�v�̍X�V����
		UpdateMap();

		// �Q�[������UI�̍X�V����
		UpdateGameUI();

		// �v���[���[�̍X�V����
		UpdatePlayer();

		// �G�l�~�[�̍X�V����
		UpdateEnemy();

		// �o���b�g�̍X�V����
		UpdateBullet();

		// �o���b�g�̔����̓���̍X�V����
		UpdateBullet_Explosion();

		// �X�R�A�̍X�V����
		UpdateScore();

		CheckGameClear();	// �Q�[���N���A���`�F�b�N����

		break;

	case MODE_RESULT:		// ���U���g��ʂ̍X�V

		// ���U���g�̍X�V����
		UpdateResult();

		break;

	}

	UpdateFade();			// �t�F�[�h�̍X�V����
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// �J�������Z�b�g
	SetCamera();

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);
	
	// ���[�h�ɂ���ď����𕪂���
	switch (g_Mode)
	{
	case MODE_TITLE:		// �^�C�g����ʂ̕`��

		// �^�C�g���̕`�揈��
		DrawTitle();

		break;

	case MODE_GAME:			// �Q�[����ʂ̕`��

		// BG�̕`�揈��
		DrawBG();

		// �}�b�v�̕`�揈��
		DrawMap();

		// �Q�[������UI�̕`�揈��
		DrawGameUI();

		// �o���b�g�̕`�揈��
		DrawBullet();
		
		// �G�l�~�[�̕`�揈��
		DrawEnemy();

		// �v���[���[�̕`�揈��
		DrawPlayer();
		
		// �o���b�g�̔����̓���̕`�揈��
		DrawBullet_Explosion();

		// �X�R�A�̕`�揈��
		DrawScore();

		break;

	case MODE_RESULT:		
		
		// ���U���g��ʂ̕`��
		DrawResult();

		break;

	}

	DrawFade();				// �t�F�[�h��ʂ̕`��


#ifdef _DEBUG
	// �f�o�b�O�\��
	DrawDebugProc();
#endif

	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();

}


//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(int mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ
	StopSound();			// �܂��Ȃ��~�߂�

	// �^�C�g���̏I������
	UninitTitle();

	// �}�b�v�̏I������
	UninitMap();

	// BG�̏I������
	UninitBG();

	// �Q�[������UI�̏I������
	UninitGameUI();

	// �v���C���[�̏I������
	UninitPlayer();

	// �G�l�~�[�̏I������
	UninitEnemy();

	// �o���b�g�̏I������
	UninitBullet();

	// �o���b�g�̓���̏I������
	UninitBullet_Explosion();

	// �X�R�A�̏I������
	UninitScore();

	// ���U���g�̏I������
	UninitResult();

	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:

		// �^�C�g����ʂ̏�����
		InitTitle();

		PlaySound(SOUND_LABEL_BGM_sample000);

		break;

	case MODE_GAME:

		// �Q�[����ʂ̏�����
		InitBG();

		// �}�b�v�̏�����
		InitMap();

		// �Q�[������UI�̏�����
		InitGameUI();

		// �v���[���[�̏�����
		InitPlayer();

		// �G�l�~�[�̏�����
		InitEnemy();

		// �o���b�g�̏�����
		InitBullet();

		// �o���b�g�̔����̓���̏�����
		InitBullet_Explosion();

		// �X�R�A�̏�����
		InitScore();

		// ���[�h�Q�[���������炷�ׂĂ̏��������I�������ɃZ�[�u�f�[�^��ǂݍ���
		if (g_LoadGame == TRUE)
		{
			LoadData();
			g_LoadGame = FALSE;		// ���[�h��������t���O��Clear����
		}

		PlaySound(SOUND_LABEL_BGM_sample001);

		break;

	case MODE_RESULT:

		// ���U���g��ʂ̏�����
		InitResult();

		PlaySound(SOUND_LABEL_BGM_sample002);

		break;

	case MODE_END:

		// �Q�[���I�����̏���
		StopSound();

		break;
	}
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


//=============================================================================
// �}�E�X���W�̎擾
//=============================================================================
long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char *GetDebugStr(void)
{
	return g_DebugStr;
}
#endif


//=============================================================================
// �Q�[���N���A���`�F�b�N����
//=============================================================================
void CheckGameClear(void)
{
	PLAYER *player = GetPlayer();

	ENEMY *enemy = GetEnemy();

	int count;

	SetResult( GetScore() );	// ���U���g��ʂœ_��\��������p

	// �v���C���[�������Ă邩�`�F�b�N
	for (int i = count = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == TRUE)
		{
			count++;
		}
	}

	if (count < 1)	// �v���C���[���S�łȂ�V�[���J�ڂ���
	{
		SetFade(FADE_OUT, MODE_RESULT);
		return;
	}


	// �G�l�~�[�������Ă邩�`�F�b�N
	//for (int i = count = 0; i < ENEMY_MAX; i++)
	//{
	//	if (enemy[i].use == TRUE)
	//	{
	//		count++;
	//	}
	//}

	//if (count < 1)	// �G�l�~�[���S�łȂ�V�[���J�ڂ���
	//{
	//	SetFade(FADE_OUT, MODE_RESULT);
	//	return;
	//}

}


//=============================================================================
// �j���[�Q�[�������[�h�Q�[�������Z�b�g����
//=============================================================================
void SetLoadGame(BOOL flg)
{
	g_LoadGame = flg;
}





