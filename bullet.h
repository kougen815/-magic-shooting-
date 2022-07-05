//=============================================================================
//
// �o���b�g���� [bullet.h]
// Author : GP11B132 13 ����
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BULLET_MAX		(10)			// �o���b�g��Max��
#define BULLET_SPEED	(10.0f)			// �o���b�g�̈ړ��X�s�[�h

enum
{
	DIR_BULLET_RIGHT =  1,
	DIR_BULLET_LEFT  = -1,
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �o���b�g�\����
struct BULLET
{
	BOOL					use;					// true:�g���Ă���  false:���g�p
	float					w, h;					// ���ƍ���
	D3DXVECTOR3				pos;					// �o���b�g�̍��W
	D3DXVECTOR3				rot;					// �o���b�g�̉�]��
	D3DXVECTOR3				move;					// �o���b�g�̈ړ���
	int						countAnim;				// �A�j���[�V�����J�E���g
	int						patternAnim;			// �A�j���[�V�����p�^�[���i���o�[
	int						texNo;					// ���Ԗڂ̃e�N�X�`���[���g�p����̂�
	int						dir;					// ���˂̕���

};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);
void SetBullet(D3DXVECTOR3 pos,int dir);


