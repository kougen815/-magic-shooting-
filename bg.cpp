//=============================================================================
//
// BG��ʏ��� [bg.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "bg.h"
#include "sprite.h"

#include "player.h"
#include "map.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX_BG					(4)															// �e�N�X�`���̐�

#define TEXTURE_WIDTH_BG				(SCREEN_WIDTH * (STAGE_WIDTH / SCREEN_WIDTH) * 3)			// 
#define TEXTURE_HEIGHT_BG				(SCREEN_HEIGHT)												// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX_BG] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX_BG] = {
	"data/TEXTURE/background/background_1.png",
	"data/TEXTURE/background/background_2.png",
	"data/TEXTURE/background/background_3.png",
	"data/TEXTURE/background/background_4.png",
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O
static BG	g_Bg[4];



// ����������
//=============================================================================
HRESULT InitBG(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX_BG; i++)
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


	// �ϐ��̏�����
	for (int i = 0; i < TEXTURE_MAX_BG; i++)
	{
		g_Bg[i].w = TEXTURE_WIDTH_BG;
		g_Bg[i].h = TEXTURE_HEIGHT_BG;
		g_Bg[i].pos = D3DXVECTOR3(-SCREEN_WIDTH, 0.0f, 0.0f);
		g_Bg[i].texNo = i;
	}

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBG(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX_BG; i++)
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
void UpdateBG(void)
{
	PLAYER *player = GetPlayer();
	float speed = player->move.x;

	// MAP�O�`�F�b�N
	MAP *map = GetMap();

	if (player->pos.x < 0.0f)
	{
		player->pos.x = 0.0f;
	}

	if (player->pos.x > map->w)
	{
		player->pos.x = map->w;
	}

	if (player->pos.y < 0.0f)
	{
		player->pos.y = 0.0f;
	}

	if (player->pos.y > map->h)
	{
		player->pos.y = map->h;
	}
	
	if (player->pos.x > 0.0f&&player->pos.x < map->w)
	{
		if (player->canBGmove == TRUE)	// ������ԂƁABG�͓����Ȃ��B������t���邾���ŁABG�͓�����B
		{
			if (player->keyOn == TRUE)
			{
				if (player->dir == DIR_RIGHT)			// �E�����ړ�
				{
					g_Bg[0].pos.x -= speed;
					g_Bg[1].pos.x -= speed - 1.0f;
					g_Bg[2].pos.x -= speed - 2.0f;
					g_Bg[3].pos.x -= speed - 3.0f;
				}
				else if (player->dir == DIR_LEFT)		// �������ړ�
				{
					g_Bg[0].pos.x += speed;
					g_Bg[1].pos.x += speed - 1.0f;
					g_Bg[2].pos.x += speed - 2.0f;
					g_Bg[3].pos.x += speed - 3.0f;
				}
			}
			else if (player->isTakeDamage == TRUE)	// �_���[�W���󂯂�Ƃ̋������(BG�̈ړ��͋t�ɂȂ�)
			{
				if (player->dir == DIR_RIGHT)			// �E�����ړ�
				{
					g_Bg[0].pos.x += speed;
					g_Bg[1].pos.x += speed - 1.0f;
					g_Bg[2].pos.x += speed - 2.0f;
					g_Bg[3].pos.x += speed - 3.0f;
				}
				else if (player->dir == DIR_LEFT)		// �������ړ�
				{
					g_Bg[0].pos.x -= speed;
					g_Bg[1].pos.x -= speed - 1.0f;
					g_Bg[2].pos.x -= speed - 2.0f;
					g_Bg[3].pos.x -= speed - 3.0f;
					
				}
			}

		}
	}
	

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBG(void)
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

	// BG�̔w�i��`��
	{
		for (int i = 0; i < TEXTURE_MAX_BG; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bg[i].texNo]);

			float px = g_Bg[i].pos.x - map->pos.x;
			float py = g_Bg[i].pos.y;
			float pw = g_Bg[i].w;
			float ph = g_Bg[i].h;

			float tx = 0.0f;
			float ty = 0.0f;
			float tw = 1.0f*(TEXTURE_WIDTH_BG / SCREEN_WIDTH);
			float th = 1.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		
	}

}


//=============================================================================
// BG�\���̂̐擪�A�h���X���擾
//=============================================================================
BG *GetBG(void)
{
	return &g_Bg[0];
}





