//=============================================================================
//
// �Q�[������UI���� [GameUI.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "GameUI.h"
#include "sprite.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH_BOX					(150.0f)
#define TEXTURE_HEIGHT_BOX					(150.0f)

#define TEXTURE_WIDTH_WEAPON				(150/2)
#define TEXTURE_HEIGHT_WEAPON				(166/2)
#define TEXTURE_MAX							(TEXTURE_MAX)

#define TEXTURE_WIDTH_HIKARI				(200.0f)
#define TEXTURE_HEIGHT_HIKARI				(200.0f)

#define TEXTURE_WIDTH_HP					(50.0f)
#define TEXTURE_HEIGHT_HP					(50.0f)

#define TEXTURE_WIDTH_MP					(50)
#define TEXTURE_HEIGHT_MP					(50)

#define TEXTURE_PATTERN_DIVIDE_X			(89)													// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y			(1)														// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM					(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)		// �A�j���[�V�����p�^�[����
#define ANIM_WAIT							(1)														// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;					// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };		// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/GameUI/UIbar.png",
	"data/TEXTURE/GameUI/weapon.png",
	"data/TEXTURE/GameUI/weapon_hikari.png",
	"data/TEXTURE/GameUI/hp_1.png",
	"data/TEXTURE/GameUI/hp_0.png",
	"data/TEXTURE/GameUI/mp_1.png",
	"data/TEXTURE/GameUI/mp_0.png",
};

static BOOL	g_Load = FALSE;		// ���������s�������̃t���O

static GameUI_Box			g_UIBox[BOX_MAX];

static GameUI_Weapon		g_UIWeapon;

static GameUI_Hikari		g_UIHikari;

static GameUI_HP			g_UIHP[PLAYER_HP_MAX];

static GameUI_MP			g_UIMP[PLAYER_MP_MAX];

// ����������
//=============================================================================
HRESULT InitGameUI(void)
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


	// �ϐ��̏�����

	// �{�b�N�X
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_UIBox[i].w = TEXTURE_HEIGHT_BOX;
		g_UIBox[i].h = TEXTURE_WIDTH_BOX;
		g_UIBox[i].pos = D3DXVECTOR3(i * TEXTURE_WIDTH_BOX, SCREEN_HEIGHT - 30, 0.0f);
		g_UIBox[i].texNo = TEXTURE_UIBOX;
	}
	
	// ����
	g_UIWeapon.w = TEXTURE_WIDTH_WEAPON;
	g_UIWeapon.h = TEXTURE_HEIGHT_WEAPON;
	g_UIWeapon.pos = D3DXVECTOR3(0.0f, g_UIBox[BOX_WEAPON].pos.y - TEXTURE_HEIGHT_WEAPON, 0.0f);
	g_UIWeapon.texNo = TEXTURE_WEAPON;

	// ������̌�
	g_UIHikari.w = TEXTURE_WIDTH_HIKARI;
	g_UIHikari.h = TEXTURE_HEIGHT_HIKARI;
	g_UIHikari.pos = D3DXVECTOR3(g_UIWeapon.pos.x + TEXTURE_WIDTH_WEAPON * 0.5f, g_UIWeapon.pos.y + 110, 0.0f);
	g_UIHikari.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_UIHikari.texNo = TEXTURE_WEAPOMHIKARI;
	g_UIHikari.countAnim = 0;	
	g_UIHikari.patternAnim = 0;

	// HP
	for (int i = 0; i < PLAYER_HP_MAX; i++)
	{
		g_UIHP[i].w = TEXTURE_WIDTH_HP;
		g_UIHP[i].h = TEXTURE_HEIGHT_HP;
		g_UIHP[i].pos = D3DXVECTOR3(g_UIBox[BOX_HP].pos.x + i * TEXTURE_WIDTH_HP * 0.5f,
									g_UIBox[BOX_HP].pos.y - TEXTURE_HEIGHT_HP,
									0.0f);
		g_UIHP[i].texNo = TEXTURE_HP_1;
		g_UIHP[i].isUse = FALSE;
	}
	
	// MP
	for (int i = 0; i < PLAYER_MP_MAX; i++)
	{
		g_UIMP[i].w = TEXTURE_WIDTH_MP;
		g_UIMP[i].h = TEXTURE_HEIGHT_MP;
		g_UIMP[i].pos = D3DXVECTOR3(g_UIBox[BOX_MP].pos.x + i * TEXTURE_WIDTH_MP * 0.5f,
			g_UIBox[BOX_MP].pos.y - TEXTURE_HEIGHT_MP,
			0.0f);
		g_UIMP[i].texNo = TEXTURE_MP_1;
		g_UIMP[i].isUse = FALSE;
	}

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameUI(void)
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
void UpdateGameUI(void)
{
	// ������̌��̃A�j��
	{
		// �A�j���[�V����  
		g_UIHikari.countAnim += 1.0f;
		if (g_UIHikari.countAnim > ANIM_WAIT)
		{
			g_UIHikari.countAnim = 0.0f;
			// �p�^�[���̐؂�ւ�
			g_UIHikari.patternAnim = (g_UIHikari.patternAnim + 1) % ANIM_PATTERN_NUM;
		}
	}


	PLAYER *player = GetPlayer();
	// HP�̍X�V
	{
		for (int i = PLAYER_HP_MAX - 1; i > player->hp - 1; i--)
		{
			g_UIHP[i].isUse = FALSE;
			g_UIHP[i].texNo = TEXTURE_HP_0;
		}
	}


	// MP�̍X�V
	{
		for (int i = PLAYER_MP_MAX - 1; i > player->mp - 1; i--)
		{
			g_UIMP[i].isUse = FALSE;
			g_UIMP[i].texNo = TEXTURE_MP_0;
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameUI(void)
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

	// waepon�̌���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIHikari.texNo]);

		// �n���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_UIHikari.pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_UIHikari.pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_UIHikari.w;		// �v���C���[�̕\����
		float ph = g_UIHikari.h;		// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = (float)(g_UIHikari.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_UIHikari.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
			g_UIHikari.rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// GameUI��box��`��
	{
		for (int i = 0; i < BOX_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIBox[i].texNo]);

			// UI�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_UIBox[i].pos.x;
			float py = g_UIBox[i].pos.y;
			float pw = g_UIBox[i].w;
			float ph = g_UIBox[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// GameUI��weapon��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIWeapon.texNo]);

		// UI�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_UIWeapon.pos.x;
		float py = g_UIWeapon.pos.y;
		float pw = g_UIWeapon.w;
		float ph = g_UIWeapon.h;

		float tw = 1.0f / 8;
		float th = 1.0f;
		float tx = tw * 3;
		float ty = th;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// HP��`��
	{
		for (int i = 0; i < PLAYER_HP_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIHP[i].texNo]);

			// UI�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_UIHP[i].pos.x;
			float py = g_UIHP[i].pos.y;
			float pw = g_UIHP[i].w;
			float ph = g_UIHP[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		
	}


	// MP��`��
	{
		for (int i = 0; i < PLAYER_MP_MAX; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UIMP[i].texNo]);

			// UI�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_UIMP[i].pos.x;
			float py = g_UIMP[i].pos.y;
			float pw = g_UIMP[i].w;
			float ph = g_UIMP[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 0.0f;
			float ty = 0.0f;

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







