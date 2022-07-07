//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"
#include "input.h"
#include "map.h"
#include "player.h"
#include "collision.h"
#include "sound.h"
#include "score.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX							(2)																	// �e�N�X�`���̐�

#define TEXTURE_WIDTH_SNAIL					(96)																// �L�����T�C�Y
#define TEXTURE_HEIGHT_SNAIL				(64)																// 



// Snail�̃A�j��
// �o��A�j��
#define TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW	(8)																				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_SNAIL_SHOW	(1)																				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_SNAIL_SHOW			(TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW * TEXTURE_PATTERN_DIVIDE_Y_SNAIL_SHOW)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_SNAIL_SHOW				(6)																				// �A�j���[�V�����̐؂�ւ��Wait�l
// �ړ��A�j��
#define TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK	(8)																				// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_SNAIL_WALK	(1)																				// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_SNAIL_WALK			(TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK * TEXTURE_PATTERN_DIVIDE_Y_SNAIL_WALK)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_SNAIL_WALK				(6)																				// �A�j���[�V�����̐؂�ւ��Wait�l



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;					// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };		// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy/show-Sheet.png",
	"data/TEXTURE/enemy/walk-Sheet.png",
};


static BOOL		g_Load = FALSE;						// ���������s�������̃t���O
static ENEMY	g_Enemy_Snail[ENEMY_SNAIL_MAX];		// �G�l�~�[�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
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

	// �����_���ŃG�l�~�[��pos�𐶐�����
	srand((unsigned)time(NULL));


	// �G�l�~�[�\���̂̏�����
	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
	{
		g_Enemy_Snail[i].use	= TRUE;
		g_Enemy_Snail[i].pos	= D3DXVECTOR3(1090.0f, 800.0f, 0.0f);	// ���S�_����\��
		g_Enemy_Snail[i].rot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy_Snail[i].w		= TEXTURE_WIDTH_SNAIL;
		g_Enemy_Snail[i].h		= TEXTURE_HEIGHT_SNAIL;
		g_Enemy_Snail[i].texNo	= ENEMY_SNAIL_TEXTURE_SHOW;
		g_Enemy_Snail[i].dir	= DIR_ENEMY_LEFT;

		g_Enemy_Snail[i].mapChipListNum = 0;

		g_Enemy_Snail[i].isShow = TRUE;
		g_Enemy_Snail[i].isWalk = FALSE;
		g_Enemy_Snail[i].isDead = FALSE;
		g_Enemy_Snail[i].countAnim = 0;
		g_Enemy_Snail[i].patternAnim = 0;

		g_Enemy_Snail[i].hp = ENEMY_SNAIL_HP_MAX;

		g_Enemy_Snail[i].move = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
	}
	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	int count = 0;
	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
	{
		if (g_Enemy_Snail[i].hp == 0)
		{
			g_Enemy_Snail[i].use = FALSE;
		}

		// �����Ă�v���C���[��������������
		if (g_Enemy_Snail[i].use == TRUE)	
		{
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = g_Enemy_Snail[i].pos;

			// �A�j���[�V����  
			{
				// �o��A�j��
				if (g_Enemy_Snail[i].isShow == TRUE)
				{
					g_Enemy_Snail[i].countAnim += 1;

					if (g_Enemy_Snail[i].countAnim > ANIM_WAIT_SNAIL_SHOW)
					{
						g_Enemy_Snail[i].countAnim = 0;
						// �p�^�[���̐؂�ւ�
						g_Enemy_Snail[i].patternAnim = (g_Enemy_Snail[i].patternAnim + 1) % ANIM_PATTERN_NUM_SNAIL_SHOW;

						if (g_Enemy_Snail[i].patternAnim == ANIM_PATTERN_NUM_SNAIL_SHOW - 1)
						{
							g_Enemy_Snail[i].isShow = FALSE;
							g_Enemy_Snail[i].isWalk = TRUE;
						}
					}
				}

				// �ړ��A�j��
				if (g_Enemy_Snail[i].isWalk == TRUE)
				{
					g_Enemy_Snail[i].countAnim += 1;

					if (g_Enemy_Snail[i].countAnim > ANIM_WAIT_SNAIL_WALK)
					{
						g_Enemy_Snail[i].countAnim = 0;
						// �p�^�[���̐؂�ւ�
						g_Enemy_Snail[i].patternAnim = (g_Enemy_Snail[i].patternAnim + 1) % ANIM_PATTERN_NUM_SNAIL_WALK;
					}
				}
			}
			
			// player��mapchip�̔��菈��
			if (CheckMapList2(g_Enemy_Snail[i], TEXTURE_WIDTH_SNAIL / 4, TEXTURE_HEIGHT_SNAIL / 2) == TRUE)
			{
				g_Enemy_Snail[i].gravity.y = 0;
				g_Enemy_Snail[i].vel.y = 0;
				g_Enemy_Snail[i].mapChipListNum = MapChipListNum(g_Enemy_Snail[i], TEXTURE_WIDTH_SNAIL / 4, TEXTURE_HEIGHT_SNAIL / 2);
			}

			g_Enemy_Snail[i].pos += g_Enemy_Snail[i].gravity;
			g_Enemy_Snail[i].gravity.y += 0.2f;

			if (g_Enemy_Snail[i].gravity.y > 30)
			{
				g_Enemy_Snail[i].gravity.y = 30;
			}

			// �ړ����� 
			{
				if (g_Enemy_Snail[i].isWalk == TRUE)
				{
					g_Enemy_Snail[i].texNo = ENEMY_SNAIL_TEXTURE_WALK;
				}
				/*if (g_Enemy_Snail[i].dir == DIR_RIGHT)
				{
					g_Enemy_Snail[i].pos.x += g_Enemy_Snail->move.x;
					g_Enemy_Snail[i].texNo = 1;
				}
				else if (g_Enemy_Snail[i].dir == DIR_LEFT)
				{
					g_Enemy_Snail[i].pos.x -= g_Enemy_Snail->move.x;
					g_Enemy_Snail[i].texNo = 0;
				}

				if (g_Enemy_Snail[i].mapChipListNum == 6)
				{
					g_Enemy_Snail[i].dir = DIR_RIGHT;
					g_Enemy_Snail[i].pos.x += g_Enemy_Snail->move.x;
				}
				else if (g_Enemy_Snail[i].mapChipListNum == 7)
				{
					g_Enemy_Snail[i].dir = DIR_LEFT;
					g_Enemy_Snail[i].pos.x -= g_Enemy_Snail->move.x;
				}*/


				// �ړ����I�������v���C���[�Ƃ̓����蔻��
				{
					PLAYER *player = GetPlayer();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_SNAIL_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (player[j].use == TRUE && player->noDamageMod == FALSE)
						{
							BOOL ans = CollisionBB(g_Enemy_Snail[i].pos, g_Enemy_Snail[i].w -10.0f, g_Enemy_Snail[i].h,
								player[j].pos, player[j].w -10.0f, player[j].h);
							// �������Ă���H
							if (ans == TRUE)
							{
								// �����������̏���
								//player->hp -= 1;
								player->isTakeDamage = TRUE;
								player->countAnim = 0;
								player->patternAnim = 0;
								//g_Enemy_Snail[i].use = FALSE;
								AddScore(-500);
								PlaySound(SOUND_LABEL_SE_defend000);
							}
						}
					}
				}


				// MAP�O�`�F�b�N
				MAP *map = GetMap();

				if (g_Enemy_Snail[i].pos.x < 0.0f)
				{
					g_Enemy_Snail[i].pos.x = 0.0f;
				}

				if (g_Enemy_Snail[i].pos.x > map->w)
				{
					g_Enemy_Snail[i].pos.x = map->w;
				}

				if (g_Enemy_Snail[i].pos.y < 0.0f)
				{
					g_Enemy_Snail[i].pos.y = 0.0f;
				}

				if (g_Enemy_Snail[i].pos.y > map->h)
				{
					g_Enemy_Snail[i].pos.y = map->h;
				}



			}
		}
		else
		{
			count++;
		}
		//if (count == ENEMY_SNAIL_MAX)
		//{
		//	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
		//	{
		//		g_Enemy_Snail[i].use = TRUE;
		//		g_Enemy_Snail[i].pos = D3DXVECTOR3(i + rand() % SCREEN_WIDTH, 0.0f, 0.0f);	// ���S�_����\��
		//		g_Enemy_Snail[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//		g_Enemy_Snail[i].w = TEXTURE_WIDTH_ENEMY;
		//		g_Enemy_Snail[i].h = TEXTURE_HEIGHT_ENEMY;
		//		g_Enemy_Snail[i].texNo = 0;
		//		g_Enemy_Snail[i].dir = DIR_LEFT;
		//		g_Enemy_Snail[i].mapChipListNum = 0;

		//		g_Enemy_Snail[i].countAnim = 0;
		//		g_Enemy_Snail[i].patternAnim = 0;

		//		g_Enemy_Snail[i].move = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
		//	}
		//}
			

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Enemy No%d  X:%f Y:%f texno:%d\n", i, g_Enemy_Snail[i].pos.x, g_Enemy_Snail[i].pos.y,g_Enemy_Snail[i].mapChipListNum);
		PrintDebugProc("EnemyNum %d\n", count);
#endif

	}
	

}


//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
	{
		if (g_Enemy_Snail[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{										// Yes

			if (g_Enemy_Snail[i].isShow == TRUE)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy_Snail[i].texNo]);

				//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Enemy_Snail[i].pos.x - map->pos.x;	// �G�l�~�[�̕\���ʒuX
				float py = g_Enemy_Snail[i].pos.y - map->pos.y;	// �G�l�~�[�̕\���ʒuY
				float pw = g_Enemy_Snail[i].w;					// �G�l�~�[�̕\����
				float ph = g_Enemy_Snail[i].h;					// �G�l�~�[�̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW * g_Enemy_Snail[i].dir;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_SNAIL_SHOW;					// �e�N�X�`���̍���
				float tx = (float)(g_Enemy_Snail[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW) * tw * g_Enemy_Snail[i].dir;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Enemy_Snail[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW) * th;					// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy_Snail[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			if (g_Enemy_Snail[i].isWalk == TRUE)
			{
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy_Snail[i].texNo]);

				//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = g_Enemy_Snail[i].pos.x - map->pos.x;	// �G�l�~�[�̕\���ʒuX
				float py = g_Enemy_Snail[i].pos.y - map->pos.y;	// �G�l�~�[�̕\���ʒuY
				float pw = g_Enemy_Snail[i].w;					// �G�l�~�[�̕\����
				float ph = g_Enemy_Snail[i].h;					// �G�l�~�[�̕\������

				// �A�j���[�V�����p
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK * g_Enemy_Snail[i].dir;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_SNAIL_WALK;					// �e�N�X�`���̍���
				float tx = (float)(g_Enemy_Snail[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK) * tw * g_Enemy_Snail[i].dir;	// �e�N�X�`���̍���X���W
				float ty = (float)(g_Enemy_Snail[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK) * th;					// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy_Snail[i].rot.z);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
			
		}
	}
}


//=============================================================================
// �G�l�~�[�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY *GetEnemy(void)
{

	return &g_Enemy_Snail[0];
}




