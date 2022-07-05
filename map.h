//=============================================================================
//
// map��ʏ��� [map.h]
// Author : GP11B132 13 ����
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define STAGE_WIDTH						(2560)						// �X�e�[�W�̉���
#define STAGE_HEIGHT					(1280)						// �X�e�[�W�̏c��



//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct MAP
{
	D3DXVECTOR3		pos;		// �|���S���̍��W
	float			w, h;		// ���ƍ���
	int				texNo;		// �g�p���Ă���e�N�X�`���ԍ�
};


struct MAPCHIP
{
	D3DXVECTOR3		pos;			// �|���S���̍��W(�}�b�v�`�b�v�v�Z�֗��̂��߂ɁA����̍��W�j
	D3DXVECTOR3		pos_center;		// �|���S���̍��W(�����蔻��̂��߂ɁA���S�̍��W�����߂�)
	float			w, h;			// ���ƍ���
	int				texNo;			// �g�p���Ă���e�N�X�`���ԍ�

	BOOL			isTouch;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitMap(void);
void UninitMap(void);
void UpdateMap(void);
void DrawMap(void);

MAP *GetMap(void);
MAPCHIP *GetMapChip(void);
BOOL CheckMapList(PLAYER player);
BOOL CheckMapList2(ENEMY enemy, float w, float h);

int MapChipListNum(ENEMY enemy, float w, float h);