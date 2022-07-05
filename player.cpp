//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
//#include "bg.h"
#include "map.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "file.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH_PLAYER			(100)	// �L�����T�C�Y
#define TEXTURE_HEIGHT_PLAYER			(150)	// 
#define TEXTURE_MAX						(3)		// �e�N�X�`���̐�
#define PLAYER_START_X					()
#define PLAYER_START_Y					()

// run�̃e�N�X�`��
#define TEXTURE_PATTERN_DIVIDE_X_RUN	(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_RUN	(8)																	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_RUN			(TEXTURE_PATTERN_DIVIDE_X_RUN * TEXTURE_PATTERN_DIVIDE_Y_RUN)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_RUN					(4)																	// �A�j���[�V�����̐؂�ւ��Wait�l

// idle�̃e�N�X�`��
#define TEXTURE_PATTERN_DIVIDE_X_IDLE	(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_IDLE	(6)																	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_IDLE			(TEXTURE_PATTERN_DIVIDE_X_IDLE * TEXTURE_PATTERN_DIVIDE_Y_IDLE)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_IDLE					(12)																// �A�j���[�V�����̐؂�ւ��Wait�l

// �_���[�W���󂯂�̃e�N�X�`��
#define TEXTURE_PATTERN_DIVIDE_X_DAMAGE	(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_DAMAGE	(3)																	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_DAMAGE			(TEXTURE_PATTERN_DIVIDE_X_DAMAGE * TEXTURE_PATTERN_DIVIDE_Y_DAMAGE)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_DAMAGE				(3)																	// �A�j���[�V�����̐؂�ւ��Wait�l
#define ANIM_DAMAGE_CYCLE				(3)																	// �A�j���[�V�����̌J��Ԃ��̉�

// �v���C���[�̉�ʓ��z�u���W
#define PLAYER_DISP_X					(SCREEN_WIDTH /2)
#define PLAYER_DISP_Y					(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT_PLAYER)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;					// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };		// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/player/witch_idle.png",
	"data/TEXTURE/player/witch_run.png",
	"data/TEXTURE/player/witch_take_damage.png",
};


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static PLAYER	g_Player[PLAYER_MAX];	// �v���C���[�\����



//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�\���̂̏�����
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// �e�N�X�`���̏�����
		g_Player[i].use = TRUE;
		g_Player[i].pos = D3DXVECTOR3(50.0f, 100.0f, 0.0f);			// ���S�_����\��
		g_Player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].w   = TEXTURE_WIDTH_PLAYER;
		g_Player[i].h   = TEXTURE_HEIGHT_PLAYER;
		g_Player[i].texNo = PLAYER_TEXTURE_IDLE;

		// �A�j���̏�����
		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;
		g_Player[i].aniCycle_damage = 0;

		// �ړ��̏�����
		g_Player[i].move	= D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// �ړ���
		g_Player[i].dir		= DIR_PLAYER_RIGHT;							// �������͉E����
		g_Player[i].keyOn			= FALSE;						// Key������Ă��邩�t���O(BG�̓���)
		g_Player[i].isTakeDamage	= FALSE;
		g_Player[i].canBGmove		= FALSE;
		g_Player[i].isRush			= FALSE;
		g_Player[i].noDamageMod		= FALSE;

		// �d�͂̏�����
		g_Player[i].gravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].vel		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].isFlying = FALSE;								// ����������ԂƁABG�͓����Ȃ�


		// �v���[���[�̑����̏�����
		g_Player[i].hp = PLAYER_HP_MAX;
		g_Player[i].mp = PLAYER_MP_MAX;

		// ���g�p
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].hp == 0)
		{
			g_Player[i].use = FALSE;
		}
		
		// �����Ă�v���C���[��������������
		if (g_Player[i].use == TRUE)
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = g_Player[i].pos;

			// ���g�p
			for (int j = PLAYER_OFFSET_CNT-1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j-1];
			}
			g_Player[i].offset[0] = pos_old;

			// �A�j���[�V���� 
			{

				// �_���[�W���󂯂�ƁA�_���[�W���󂯂铮�������
				if (g_Player[i].isTakeDamage == TRUE && g_Player[i].isRush == FALSE)
				{				
					g_Player[i].countAnim += 1;

					if (g_Player[i].countAnim > ANIM_WAIT_DAMAGE)
					{
						g_Player[i].countAnim = 0;
						// �p�^�[���̐؂�ւ�
						g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_DAMAGE;

						if (g_Player[i].patternAnim == ANIM_PATTERN_NUM_DAMAGE - 1)
						{
							g_Player[i].aniCycle_damage += 1;
						}

						// �_���[�W���󂯂铮���3�񃋁[�v����
						if (g_Player[i].aniCycle_damage == 3)
						{
							g_Player[i].isTakeDamage = FALSE;
							g_Player[i].patternAnim = 0;
							g_Player[i].countAnim = 0;
							g_Player[i].aniCycle_damage = 0;
						}
					}
				}

				// ���b�V��
				if (g_Player[i].isRush == TRUE)
				{
					g_Player[i].countAnim += 1;

					if (g_Player[i].countAnim > ANIM_WAIT_RUN)
					{
						g_Player[i].countAnim = 0;
						// �p�^�[���̐؂�ւ�
						g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_RUN;

						// ��񂾂��ŁA���������
						if (g_Player[i].patternAnim == 3)
						{
							g_Player[i].isRush = FALSE;
							g_Player[i].noDamageMod = FALSE;
							g_Player[i].patternAnim = 0;
							g_Player[i].countAnim = 0;
							g_Player[i].keyOn = FALSE;
						}
					}
				}

				// ����Ɨ��Ă铮��̃��[�v
				if (g_Player[i].isTakeDamage	== FALSE &&
					g_Player[i].isRush			== FALSE)
				{
					if (g_Player[i].keyOn == TRUE)
					{
						// ���铮��̃��[�v
						g_Player[i].countAnim += 1;

						if (g_Player[i].countAnim > ANIM_WAIT_RUN)
						{
							g_Player[i].countAnim = 0;
							// �p�^�[���̐؂�ւ�
							g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_RUN;
						}
					}
					else
					{
						// ���Ă铮��̃��[�v
						g_Player[i].countAnim += 1;

						if (g_Player[i].countAnim > ANIM_WAIT_IDLE)
						{
							g_Player[i].countAnim = 0;
							// �p�^�[���̐؂�ւ�
							g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_IDLE;
						}
					}
				}
				
			}


			// �_���[�W���󂯂�ƁA�����I�Ȍ�ނ̐ݒ�ƁA�Ⴄ�̕����̃_���[�W����̐ݒ�
			if (g_Player[i].isTakeDamage == TRUE && g_Player[i].isRush == FALSE)
			{
				g_Player[i].keyOn = FALSE;
				

				if (g_Player[i].dir == DIR_PLAYER_RIGHT)
				{
					g_Player[i].pos.x -= g_Player[i].move.x;
				}
				else
				{
					g_Player[i].pos.x += g_Player[i].move.x;
				}
			}

			// ���b�V��
			if (g_Player[i].isRush == TRUE)
			{
				if (g_Player[i].dir == DIR_PLAYER_RIGHT)
				{
					g_Player[i].pos.x += g_Player[i].move.x * 5.0f;
					g_Player[i].canBGmove = TRUE;
				}
				else
				{
					g_Player[i].pos.x -= g_Player[i].move.x * 5.0f;
					g_Player[i].canBGmove = TRUE;
				}
			}
		

			// player��mapchip�̔��菈��(�d�͂̐���)
			if (CheckMapList(g_Player[i]) == TRUE)
			{
				g_Player[i].gravity.y = 0.0f;
				g_Player[i].vel.y = 0.0f;
				g_Player[i].isFlying = FALSE;
			}



			// �d��
			if (g_Player[i].isRush == FALSE)
			{
				g_Player[i].pos += g_Player[i].gravity;
				g_Player[i].gravity.y += 0.5f;
			}
			


			// �L�[���͂ňړ� 
			{
				if (g_Player[i].isTakeDamage == FALSE && g_Player[i].isRush == FALSE)
				{
					// �L�[�{�[�h���͂̏�����
					g_Player[i].keyOn = FALSE;

					// �e�N�X�`���̏�����
					g_Player[i].texNo = PLAYER_TEXTURE_IDLE;

					// �w�i�ړ��̏�����
					g_Player[i].canBGmove = FALSE;

					// ��Ԃ̐���
					if (GetKeyboardPress(DIK_SPACE))
					{
						g_Player[i].keyOn = TRUE;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].isFlying = TRUE;
						g_Player[i].gravity = D3DXVECTOR3(0.0f, -0.8f, 0.0f);
						g_Player[i].vel = D3DXVECTOR3(0.0f, -3.5f, 0.0f);
						g_Player[i].pos += g_Player[i].vel;
					}

					// ����Ɨ��Ă�̐���
					float speed = g_Player[i].move.x;

					// ���̈ړ�
					if (GetKeyboardPress(DIK_D))
					{
						
						g_Player[i].pos.x += speed;
						g_Player[i].keyOn = TRUE;
						g_Player[i].dir = DIR_PLAYER_RIGHT;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].isFlying = FALSE;
						g_Player[i].canBGmove = TRUE;
					}
					// ���̈ړ�
					if (GetKeyboardPress(DIK_A))
					{
						g_Player[i].pos.x -= speed;
						g_Player[i].keyOn = TRUE;
						g_Player[i].dir = DIR_PLAYER_LEFT;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].isFlying = FALSE;
						g_Player[i].canBGmove = TRUE;
					}

					// ���b�V��
					if (GetKeyboardTrigger(DIK_K))
					{
						g_Player[i].isRush = TRUE;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].keyOn = TRUE;
						g_Player[i].noDamageMod = TRUE;
						g_Player[i].countAnim = 0;
						g_Player[i].patternAnim = 0;
					}

					// �e����
					if (GetKeyboardTrigger(DIK_J))
					{
						if (g_Player[i].mp > 0)
						{
							g_Player[i].mp -= 1;
							D3DXVECTOR3 pos = g_Player[i].pos;
							if (g_Player[i].dir == DIR_PLAYER_RIGHT)
							{
								pos.x = g_Player[i].pos.x + 20;
								SetBullet(pos, DIR_PLAYER_RIGHT);		// �P����
							}
							if (g_Player[i].dir == DIR_PLAYER_LEFT)
							{
								pos.x = g_Player[i].pos.x - 20;
								SetBullet(pos, DIR_PLAYER_LEFT);		// �P����
							}
						}
						
					}
					
				}
				
				

				// MAP�O�`�F�b�N
				MAP *map = GetMap();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > map->w)
				{
					g_Player[i].pos.x = map->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > map->h)
				{
					g_Player[i].pos.y = map->h;
				}

				// �v���C���[�̗����ʒu����MAP�̃X�N���[�����W���v�Z����
				map->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;

				if (map->pos.x < 0)
				{
					map->pos.x = 0;
				}

				if (map->pos.x > map->w - SCREEN_WIDTH)
				{
					map->pos.x = map->w - SCREEN_WIDTH;
				}

				map->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;

				if (map->pos.y < 0) 
				{
					map->pos.y = 0;
				}

				if (map->pos.y > map->h - SCREEN_HEIGHT) 
				{
					map->pos.y = map->h - SCREEN_HEIGHT;
				}

				
				// �ړ����I�������G�l�~�[�Ƃ̓����蔻��
				if (g_Player[i].noDamageMod == FALSE)
				{
					ENEMY *enemy = GetEnemy();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (enemy[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w -10.0f, g_Player[i].h, 
												   enemy[j].pos,    enemy[j].w -10.0f,    enemy[j].h);
							// �������Ă���H
							if (ans == TRUE)
							{
								// �����������̏���
								//enemy[j].use = FALSE;
								g_Player[i].hp -= 1;
								g_Player[i].isTakeDamage = TRUE;
								g_Player[i].texNo = PLAYER_TEXTURE_TAKEDAMAGE;
								g_Player[i].canBGmove = TRUE;
								AddScore(-500);
								PlaySound(SOUND_LABEL_SE_defend000);
							}
						}
					}
				}	
			}
		}

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Player %d Pos  X:%f Y:%f\n", i, g_Player[i].pos.x, g_Player[i].pos.y);
		PrintDebugProc("Player isTakeDamage %d\n",g_Player[i].isTakeDamage);
		PrintDebugProc("Player isRush %d\n", g_Player[i].isRush);
		PrintDebugProc("PlayerTexNo:%d\n", g_Player[i].texNo);
		PrintDebugProc("Playerpattern:%d\n", g_Player[i].patternAnim);
		PrintDebugProc("Playercount:%d\n", g_Player[i].countAnim);
		// ������Z�[�u����
		if (GetKeyboardTrigger(DIK_T))
		{
			SaveData();
		}

#endif

	}
	
}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	MAP *map = GetMap();

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// �v���[���[�������Ă����
		{					
			// �_���[�W���󂯂�`�揈��
			if (g_Player[i].isTakeDamage == TRUE && g_Player[i].isRush == FALSE)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Player[i].pos.x - map->pos.x;	// �v���C���[�̕\���ʒuX
				float py = g_Player[i].pos.y - map->pos.y;	// �v���C���[�̕\���ʒuY
				float pw = g_Player[i].w;		// �v���C���[�̕\����
				float ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_DAMAGE * g_Player[i].dir;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_DAMAGE;	// �e�N�X�`���̍���
				float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_DAMAGE) * tw * g_Player[i].dir;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_DAMAGE) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			// ���b�V���`�揈��
			if (g_Player[i].isRush == TRUE)
			{
				// �v���C���[�̕��g��`��
				DrawPlayerOffset(i);

				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

				//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Player[i].pos.x - map->pos.x;	// �v���C���[�̕\���ʒuX
				float py = g_Player[i].pos.y - map->pos.y;	// �v���C���[�̕\���ʒuY
				float pw = g_Player[i].w;		// �v���C���[�̕\����
				float ph = g_Player[i].h;		// �v���C���[�̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_RUN * g_Player[i].dir;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_RUN;	// �e�N�X�`���̍���
				float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_RUN) * tw * g_Player[i].dir;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_RUN) * th;	// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}


			// ����Ɨ��Ă�`�揈��
			if (g_Player[i].isTakeDamage == FALSE && g_Player[i].isRush == FALSE)
			{
				if (g_Player[i].keyOn == TRUE)
				{
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

					//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
					float px = g_Player[i].pos.x - map->pos.x;	// �v���C���[�̕\���ʒuX
					float py = g_Player[i].pos.y - map->pos.y;	// �v���C���[�̕\���ʒuY
					float pw = g_Player[i].w;		// �v���C���[�̕\����
					float ph = g_Player[i].h;		// �v���C���[�̕\������

					// �A�j���[�V�����p
					float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_RUN * g_Player[i].dir;	// �e�N�X�`���̕�
					float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_RUN;	// �e�N�X�`���̍���
					float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_RUN) * tw * g_Player[i].dir;	// �e�N�X�`���̍���X���W
					float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_RUN) * th;	// �e�N�X�`���̍���Y���W

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);
				}
				else
				{
					// �e�N�X�`���ݒ�
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

					//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
					float px = g_Player[i].pos.x - map->pos.x;	// �v���C���[�̕\���ʒuX
					float py = g_Player[i].pos.y - map->pos.y;	// �v���C���[�̕\���ʒuY
					float pw = g_Player[i].w;		// �v���C���[�̕\����
					float ph = g_Player[i].h;		// �v���C���[�̕\������

					// �A�j���[�V�����p
					float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_IDLE * g_Player[i].dir;	// �e�N�X�`���̕�
					float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_IDLE;	// �e�N�X�`���̍���
					float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_IDLE) * tw * g_Player[i].dir;	// �e�N�X�`���̍���X���W
					float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_IDLE) * th;	// �e�N�X�`���̍���Y���W

					// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);

					// �|���S���`��
					GetDeviceContext()->Draw(4, 0);
				}
			}
			
		}
	}
}


//=============================================================================
// �v���C���[�\���̂̐擪�A�h���X���擾
//=============================================================================
PLAYER *GetPlayer(void)
{

	return &g_Player[0];
}


//=============================================================================
// �v���C���[�̕��g��`��
//=============================================================================
void DrawPlayerOffset(int no)
{
	MAP *map = GetMap();
	float alpha = 0.0f;

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);
	
	for (int j = PLAYER_OFFSET_CNT-1; j >= 0; j--)
	{
		//�v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Player[no].offset[j].x - map->pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Player[no].offset[j].y - map->pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Player[no].w;		// �v���C���[�̕\����
		float ph = g_Player[no].h;		// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_RUN * g_Player[no].dir;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_RUN;	// �e�N�X�`���̍���
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X_RUN) * tw  * g_Player[no].dir;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X_RUN) * th;	// �e�N�X�`���̍���Y���W


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}



