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
#define TEXTURE_WIDTH_ENEMY				(96)																// �L�����T�C�Y
#define TEXTURE_HEIGHT_ENEMY			(64)																// 
#define TEXTURE_MAX						(2)																	// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X_ENEMY	(8)																	// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_ENEMY	(1)																	// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_ENEMY			(TEXTURE_PATTERN_DIVIDE_X_ENEMY*TEXTURE_PATTERN_DIVIDE_Y_ENEMY)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_ENEMY					(6)		// �A�j���[�V�����̐؂�ւ��Wait�l



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


static BOOL		g_Load = FALSE;			// ���������s�������̃t���O
static ENEMY	g_Enemy[ENEMY_MAX];		// �G�l�~�[�\����


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
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = TRUE;
		g_Enemy[i].pos = D3DXVECTOR3(1090.0f, 800.0f, 0.0f);	// ���S�_����\��
		g_Enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].w   = TEXTURE_WIDTH_ENEMY;
		g_Enemy[i].h   = TEXTURE_HEIGHT_ENEMY;
		g_Enemy[i].texNo = ENEMY_SNAIL_TEXTURE_SHOW;
		g_Enemy[i].dir = DIR_LEFT;
		g_Enemy[i].mapChipListNum = 0;

		g_Enemy[i].countAnim = 0;
		g_Enemy[i].patternAnim = 0;

		g_Enemy[i].move = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
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
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)	// ���̃G�l�~�[���g���Ă���H
		{							// Yes
			// �n�`�Ƃ̓����蔻��p�ɍ��W�̃o�b�N�A�b�v������Ă���
			D3DXVECTOR3 pos_old = g_Enemy[i].pos;

			// �A�j���[�V����  
			g_Enemy[i].countAnim++;
			if ((g_Enemy[i].countAnim % ANIM_WAIT_ENEMY) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM_ENEMY;
			}

			// player��mapchip�̔��菈��
			if (CheckMapList2(g_Enemy[i], TEXTURE_WIDTH_ENEMY / 4, TEXTURE_HEIGHT_ENEMY / 2) == TRUE)
			{
				g_Enemy[i].gravity.y = 0;
				g_Enemy[i].vel.y = 0;
				g_Enemy[i].mapChipListNum = MapChipListNum(g_Enemy[i], TEXTURE_WIDTH_ENEMY / 4, TEXTURE_HEIGHT_ENEMY / 2);
			}

			g_Enemy[i].pos += g_Enemy[i].gravity;
			g_Enemy[i].gravity.y += 0.2f;

			if (g_Enemy[i].gravity.y > 30)
			{
				g_Enemy[i].gravity.y = 30;
			}

			// �ړ����� 

			{
				/*if (g_Enemy[i].dir == DIR_RIGHT)
				{
					g_Enemy[i].pos.x += g_Enemy->move.x;
					g_Enemy[i].texNo = 1;
				}
				else if (g_Enemy[i].dir == DIR_LEFT)
				{
					g_Enemy[i].pos.x -= g_Enemy->move.x;
					g_Enemy[i].texNo = 0;
				}

				if (g_Enemy[i].mapChipListNum == 6)
				{
					g_Enemy[i].dir = DIR_RIGHT;
					g_Enemy[i].pos.x += g_Enemy->move.x;
				}
				else if (g_Enemy[i].mapChipListNum == 7)
				{
					g_Enemy[i].dir = DIR_LEFT;
					g_Enemy[i].pos.x -= g_Enemy->move.x;
				}*/


				// �ړ����I�������v���C���[�Ƃ̓����蔻��
				{
					PLAYER *player = GetPlayer();

					// �G�l�~�[�̐��������蔻����s��
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// �����Ă�G�l�~�[�Ɠ����蔻�������
						if (player[j].use == TRUE && player->noDamageMod == FALSE)
						{
							BOOL ans = CollisionBB(g_Enemy[i].pos, g_Enemy[i].w -10.0f, g_Enemy[i].h,
								player[j].pos, player[j].w -10.0f, player[j].h);
							// �������Ă���H
							if (ans == TRUE)
							{
								// �����������̏���
								//player->hp -= 1;
								player->isTakeDamage = TRUE;
								player->countAnim = 0;
								player->patternAnim = 0;
								//g_Enemy[i].use = FALSE;
								AddScore(-500);
								PlaySound(SOUND_LABEL_SE_defend000);
							}
						}
					}
				}


				// MAP�O�`�F�b�N
				MAP *map = GetMap();

				if (g_Enemy[i].pos.x < 0.0f)
				{
					g_Enemy[i].pos.x = 0.0f;
				}

				if (g_Enemy[i].pos.x > map->w)
				{
					g_Enemy[i].pos.x = map->w;
				}

				if (g_Enemy[i].pos.y < 0.0f)
				{
					g_Enemy[i].pos.y = 0.0f;
				}

				if (g_Enemy[i].pos.y > map->h)
				{
					g_Enemy[i].pos.y = map->h;
				}



			}
		}
		else
		{
			count++;
		}
		//if (count == ENEMY_MAX)
		//{
		//	for (int i = 0; i < ENEMY_MAX; i++)
		//	{
		//		g_Enemy[i].use = TRUE;
		//		g_Enemy[i].pos = D3DXVECTOR3(i + rand() % SCREEN_WIDTH, 0.0f, 0.0f);	// ���S�_����\��
		//		g_Enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//		g_Enemy[i].w = TEXTURE_WIDTH_ENEMY;
		//		g_Enemy[i].h = TEXTURE_HEIGHT_ENEMY;
		//		g_Enemy[i].texNo = 0;
		//		g_Enemy[i].dir = DIR_LEFT;
		//		g_Enemy[i].mapChipListNum = 0;

		//		g_Enemy[i].countAnim = 0;
		//		g_Enemy[i].patternAnim = 0;

		//		g_Enemy[i].move = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
		//	}
		//}
			

#ifdef _DEBUG
		// �f�o�b�O�\��
		PrintDebugProc("Enemy No%d  X:%f Y:%f texno:%d\n", i, g_Enemy[i].pos.x, g_Enemy[i].pos.y,g_Enemy[i].mapChipListNum);
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		// ���̃G�l�~�[���g���Ă���H
		{								// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//�G�l�~�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Enemy[i].pos.x - map->pos.x;	// �G�l�~�[�̕\���ʒuX
			float py = g_Enemy[i].pos.y - map->pos.y;	// �G�l�~�[�̕\���ʒuY
			float pw = g_Enemy[i].w;					// �G�l�~�[�̕\����
			float ph = g_Enemy[i].h;					// �G�l�~�[�̕\������

			// �A�j���[�V�����p
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_ENEMY * g_Enemy[i].dir;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_ENEMY;					// �e�N�X�`���̍���
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_ENEMY) * tw * g_Enemy[i].dir;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_ENEMY) * th;					// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// �G�l�~�[�\���̂̐擪�A�h���X���擾
//=============================================================================
ENEMY *GetEnemy(void)
{

	return &g_Enemy[0];
}




