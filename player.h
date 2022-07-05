//=============================================================================
//
// �v���C���[���� [player.h]
// Author : GP11B132 13 ����
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define PLAYER_MAX			(1)		// �v���C���[��Max�l��

#define PLAYER_HP_MAX		(5)		// HP��Max

#define PLAYER_MP_MAX		(5)		// MP��Max

#define	PLAYER_OFFSET_CNT	(8)		// 8���g

enum
{
	DIR_RIGHT = 1,
	DIR_LEFT = -1,
};


enum 
{
	PLAYER_TEXTURE_IDLE,
	PLAYER_TEXTURE_RUN,
	PLAYER_TEXTURE_TAKEDAMAGE,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************

struct PLAYER
{
	D3DXVECTOR3		pos;				// �|���S���̍��W
	D3DXVECTOR3		rot;				// �|���S���̉�]��
	BOOL			use;				// true:�g���Ă���  false:���g�p
	float			w, h;				// ���ƍ���
	int				countAnim;			// �A�j���[�V�����J�E���g
	int				patternAnim;		// �A�j���[�V�����p�^�[���i���o�[
	int				texNo;				// �e�N�X�`���ԍ�
	D3DXVECTOR3		move;				// �ړ����x
	int				aniCycle_damage;	// �_���[�W���󂯂�A�j���̉�



	int				dir;				// �����i0:�E 1:���j
	BOOL			keyOn;				// �L�[�{�[�h����͂��ǂ���

	D3DXVECTOR3		gravity;			// �d��
	D3DXVECTOR3		vel;				// ��т̍��x
	BOOL			isFlying;			// ��т��ǂ����H

	int				hp;
	int				mp;
	BOOL			isTakeDamage;		// �_���[�W���󂯂邩�ǂ���
	BOOL			isAttack_bullet;	// �U��(�o���b�g����)���ǂ���
	BOOL			isRush;				// ���b�V�����ǂ���

	BOOL			canBGmove;			// BG�ړ����ǂ����̔���

	BOOL			noDamageMod;		// ���G���(�d�����)



	D3DXVECTOR3		offset[PLAYER_OFFSET_CNT];		// �c���|���S���̍��W

};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);



