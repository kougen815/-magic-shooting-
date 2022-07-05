//=============================================================================
//
// �Q�[������UI���� [GameUI.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum
{
	TEXTURE_UIBOX,
	TEXTURE_WEAPON,
	TEXTURE_WEAPOMHIKARI,
	TEXTURE_HP_1,
	TEXTURE_HP_0,
	TEXTURE_MP_1,
	TEXTURE_MP_0,
	TEXTURE_MAX,
};

enum 
{
	BOX_WEAPON,
	BOX_HP,
	BOX_MP,
	BOX_MAX,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct GameUI_Box
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�

};

struct GameUI_Weapon
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�

};


struct GameUI_Hikari
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	D3DXVECTOR3		rot;			// �|���S���̉�]��
	float			w, h;			// ���ƍ���
	float			countAnim;		// �A�j���[�V�����J�E���g
	int				patternAnim;	// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;			// �e�N�X�`���ԍ�
};

struct GameUI_HP
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
	int				isUse;			// HP�͗��p���ǂ���
};

struct GameUI_MP
{
	D3DXVECTOR3		pos;			// �|���S���̍��W
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�
	int				isUse;			// HP�͗��p���ǂ���
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGameUI(void);
void UninitGameUI(void);
void UpdateGameUI(void);
void DrawGameUI(void);

//BG *GetBG(void);
