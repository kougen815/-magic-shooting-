//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "score.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_EARTH			(350)	// �n���T�C�Y
#define TEXTURE_HEIGHT_EARTH		(350)

#define TEXTURE_WIDTH_BUTTON		(250)	//	�{�^���T�C�Y
#define TEXTURE_HEIGHT_BUTTON		(70)
#define BUTTON_DISTANCE				(60)

#define TEXTURE_WIDTH_TEXT			(70)
#define TEXTURE_HEIGHT_TEXT			(70)
#define TEXT_DISTANCE				(25)

#define TEXTURE_WIDTH_CONFIRM		(50)
#define TEXTURE_HEIGHT_CONFIRM		(50)

#define TEXTURE_PATTERN_DIVIDE_X	(50)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(8)		// �A�j���[�V�����̐؂�ւ��Wait�l

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title/background.png",
	"data/TEXTURE/title/earth.png",
	"data/TEXTURE/GameUI/choose.png",
	"data/TEXTURE/title/text_English.png",
	"data/TEXTURE/title/star.png",
};


static BOOL		g_Load = FALSE;		// ���������s�������̃t���O

static TITLE	g_Title;			// �w�i�̏�����

static EARTH	g_Earth;			// �n���̏�����

static CHOOSE	g_Choose[3];

static TEXT		g_Text;

static CONFIRM	g_Confirm;

// ����������
//=============================================================================
HRESULT InitTitle(void)
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
	// background�̕`��
	g_Title.w     = TEXTURE_WIDTH;
	g_Title.h     = TEXTURE_HEIGHT;
	g_Title.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Title.texNo = 0;

	// �n���̕`��̏�����
	g_Earth.w = TEXTURE_WIDTH_EARTH;
	g_Earth.h = TEXTURE_HEIGHT_EARTH;
	g_Earth.pos = D3DXVECTOR3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.3f, 0.0f);
	g_Earth.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Earth.texNo = 1;
	g_Earth.countAnim = 0;	//�A�j���J�E���g
	g_Earth.patternAnim = 0;//�A�j���p�^�[���ԍ�

	// �I���{�^���̕`��̏�����
	g_Choose[0].w = TEXTURE_WIDTH_BUTTON;
	g_Choose[0].h = TEXTURE_HEIGHT_BUTTON;
	g_Choose[0].pos = D3DXVECTOR3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.6f, 0.0f);
	g_Choose[0].texNo = 2;

	g_Choose[1].w = TEXTURE_WIDTH_BUTTON;
	g_Choose[1].h = TEXTURE_HEIGHT_BUTTON;
	g_Choose[1].pos = D3DXVECTOR3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.6f + TEXTURE_HEIGHT_BUTTON, 0.0f);
	g_Choose[1].texNo = 2;

	g_Choose[2].w = TEXTURE_WIDTH_BUTTON;
	g_Choose[2].h = TEXTURE_HEIGHT_BUTTON;
	g_Choose[2].pos = D3DXVECTOR3(TEXTURE_WIDTH * 0.4f, TEXTURE_HEIGHT * 0.6f + TEXTURE_HEIGHT_BUTTON * 2.0f, 0.0f);
	g_Choose[2].texNo = 2;

	// �����̕`��̏�����
	g_Text.w = TEXTURE_WIDTH_TEXT;
	g_Text.h = TEXTURE_HEIGHT_TEXT;
	g_Text.pos = D3DXVECTOR3(g_Choose[0].pos.x, g_Choose[0].pos.y, 0.0f);
	g_Text.texNo = 3;

	// �I���A�C�R���̕`��̏�����
	g_Confirm.w = TEXTURE_WIDTH_CONFIRM;
	g_Confirm.h = TEXTURE_HEIGHT_CONFIRM;
	g_Confirm.pos = D3DXVECTOR3(g_Choose[0].pos.x - 60, g_Choose[0].pos.y + 5, 0.0f);
	g_Confirm.texNo = 4;
	g_Confirm.choose = 0;

	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		// Enter��������A�X�e�[�W��؂�ւ���
		if (g_Confirm.choose == 0)
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
		else if (g_Confirm.choose == 2)
		{
			SetFade(FADE_OUT, MODE_END);
		}
		else if (g_Confirm.choose == 1)
		{
			// �Z�[�u�f�[�^�����[�h����
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}
		
	}
	// �Q�[���p�b�h�œ��͏���
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}



	// �A�j���[�V����  
	g_Earth.countAnim += 1.0f;
	if (g_Earth.countAnim > ANIM_WAIT)
	{
		g_Earth.countAnim = 0.0f;
		// �p�^�[���̐؂�ւ�
		g_Earth.patternAnim = (g_Earth.patternAnim + 1) % ANIM_PATTERN_NUM;
	}


	// �I���{�^�����ړ�������
	int i = g_Confirm.choose;
	
	if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S))
	{
		i = i + 1;
		if (i < 3)
		{
			g_Confirm.pos.y = g_Choose[i].pos.y + 5;
			g_Confirm.choose = i;
		}
		else
		{
			g_Confirm.pos.y = g_Choose[0].pos.y + 5;
			g_Confirm.choose = 0;
		}
	}
	else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W))
	{
		i = i - 1;
		if (i >= 0)
		{
			g_Confirm.pos.y = g_Choose[i].pos.y + 5;
			g_Confirm.choose = i;
		}
		else
		{
			g_Confirm.pos.y = g_Choose[2].pos.y + 5;
			g_Confirm.choose = 2;
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			g_Title.pos.x, g_Title.pos.y, g_Title.w, g_Title.h, 
			0.0f, 0.0f, 15.0f, 10.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �n����`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Earth.texNo]);

		// �n���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Earth.pos.x;	// �v���C���[�̕\���ʒuX
		float py = g_Earth.pos.y;	// �v���C���[�̕\���ʒuY
		float pw = g_Earth.w;		// �v���C���[�̕\����
		float ph = g_Earth.h;		// �v���C���[�̕\������

		// �A�j���[�V�����p
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = (float)(g_Earth.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
		float ty = (float)(g_Earth.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
			g_Earth.rot.z);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �I���{�^����`��
	{
		for (int i = 0; i < 3; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Choose[i].texNo]);

			// UI�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Choose[i].pos.x;
			float py = g_Choose[i].pos.y;
			float pw = g_Choose[i].w;
			float ph = g_Choose[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 1.0f;
			float ty = 1.0f;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ����(Text)��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Text.texNo]);

		// UI�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Text.pos.x;
		float py = g_Text.pos.y;
		float pw = g_Text.w;
		float ph = g_Text.h;

		float tw = 1.0f / 17;
		float th = 1.0f / 12;

		//S
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//T
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//A
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, 
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 11, th * 6, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//R
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, 
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 14, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//T
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, 
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//***************************************************//
		//C
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, g_Choose[1].pos.y, pw, ph,
			tw * 16, th * 6, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//O
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[1].pos.y, pw, ph,
			tw * 8, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//N
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//T
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[1].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//I
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, g_Choose[1].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//N
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//U
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//E
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//***************************************************//
		//E
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, g_Choose[2].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//X
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[2].pos.y, pw, ph,
			tw * 9, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//I
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[2].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		//T
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[2].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �I���A�C�R����`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Confirm.texNo]);

		// UI�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Confirm.pos.x;
		float py = g_Confirm.pos.y;
		float pw = g_Confirm.w;
		float ph = g_Confirm.h;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 1.0f;
		float ty = 1.0f;


		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph, 
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

}





