//=============================================================================
//
// �G�l�~�[���� [enemy.h]
// Author : GP11B132 13 ����
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ENEMY_SNAIL_MAX		(1)						// �G�l�~�[��snail��Max��
#define	ENEMY_MAX			(ENEMY_SNAIL_MAX)		// �G�l�~�[��Max��

#define ENEMY_SNAIL_HP_MAX	(1)

enum
{
	DIR_ENEMY_LEFT  =  1,
	DIR_ENEMY_RIGHT = -1,
};

enum
{
	ENEMY_SNAIL_TEXTURE_SHOW,
	ENEMY_SNAIL_TEXTURE_WALK,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct ENEMY
{
	D3DXVECTOR3		pos;				// �|���S���̍��W
	D3DXVECTOR3		rot;				// �|���S���̉�]��
	BOOL			use;				// true:�g���Ă���  false:���g�p
	float			w, h;				// ���ƍ���
	int				countAnim;			// �A�j���[�V�����J�E���g
	int				patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;				// �e�N�X�`���ԍ�
	D3DXVECTOR3		move;				// �ړ����x
	BOOL			isShow;				// �G�l�~�[�̓o��A�j��
	BOOL			isWalk;				// �G�l�~�[�̈ړ��A�j��
	BOOL			isDead;				// �G�l�~�[�̎��S�A�j��

	D3DXVECTOR3		gravity;			// �d��
	D3DXVECTOR3		vel;				// ��т̍��x
	int				dir;				// �����i0:�E 1:���j

	int				hp;					// 

	int				mapChipListNum;		// ��������mapchip��texno���L�^����
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);



