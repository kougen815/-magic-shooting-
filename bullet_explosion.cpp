//=============================================================================
//
// �o���b�g�̔������� [bullet_explosion.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "bullet.h"
#include "bullet_explosion.h"
#include "sprite.h"
#include "sound.h"
//#include "bg.h"
#include "map.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

// bullet_explosion
#define TEXTURE_WIDTH_EXPLOSION		(150)
#define TEXTURE_HEIGHT_EXPLOSION	(150)

#define TEXTURE_PATTERN_DIVIDE_X_EX	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y_EX	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM_EX			(TEXTURE_PATTERN_DIVIDE_X_EX * TEXTURE_PATTERN_DIVIDE_Y_EX)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT_EX				(2)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/bullet/fireball_explosion.png",
};

static BOOL	  g_Load = FALSE;		// ���������s�������̃t���O


static BULLET_EXPLOSION g_Bullet_Explosion[BULLET_MAX];


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet_Explosion(void)
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


	// �o���b�g�̔����\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet_Explosion[i].use = FALSE;
		g_Bullet_Explosion[i].w = TEXTURE_WIDTH_EXPLOSION;
		g_Bullet_Explosion[i].h = TEXTURE_HEIGHT_EXPLOSION;
		g_Bullet_Explosion[i].pos = D3DXVECTOR3(300.0f, 300.0f, 0.0f);
		g_Bullet_Explosion[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet_Explosion[i].texNo = 0;

		g_Bullet_Explosion[i].countAnim = 0;
		g_Bullet_Explosion[i].patternAnim = 0;

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet_Explosion(void)
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
void UpdateBullet_Explosion(void)
{
	MAP *map = GetMap();

	

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet_Explosion[i].use == TRUE)
		{
			// �A�j���[�V����  
			g_Bullet_Explosion[i].countAnim++;
			if ((g_Bullet_Explosion[i].countAnim % ANIM_WAIT_EX) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bullet_Explosion[i].patternAnim = (g_Bullet_Explosion[i].patternAnim + 1) % ANIM_PATTERN_NUM_EX;

			}
			if (g_Bullet_Explosion[i].patternAnim == ANIM_PATTERN_NUM_EX - 1)
			{
				g_Bullet_Explosion[i].use = FALSE;
				g_Bullet_Explosion[i].patternAnim = 0;
				g_Bullet_Explosion[i].countAnim = 0;
			}
			

		}
//#ifdef _DEBUG
//		// �f�o�b�O�\��
//		PrintDebugProc("Bullet_Ex No%d  X:%f Y:%f\n use:%d", i, g_Bullet_Explosion[i].pos.x, g_Bullet_Explosion[i].pos.y,g_Bullet_Explosion[i].use);
//#endif
	}




}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet_Explosion(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet_Explosion[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet_Explosion[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet_Explosion[i].pos.x - map->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Bullet_Explosion[i].pos.y - map->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Bullet_Explosion[i].w;		// �o���b�g�̕\����
			float ph = g_Bullet_Explosion[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_EX;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_EX;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet_Explosion[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_EX) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet_Explosion[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_EX) * th;	// �e�N�X�`���̍���Y���W


			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet_Explosion[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
//BULLET *GetBullet(void)
//{
//	return &g_Bullet[0];
//}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBullet_Explosion(D3DXVECTOR3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet_Explosion[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet_Explosion[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bullet_Explosion[i].pos = pos;			// ���W���Z�b�g
			//PlaySound(SOUND_LABEL_SE_shot000);
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}

