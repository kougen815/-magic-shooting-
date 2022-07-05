//=============================================================================
//
// map��ʏ��� [map.cpp]
// Author : GP11B132 13 ����
//
//=============================================================================
#include "map.h"
#include "sprite.h"
#include "player.h"
#include "collision.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX_MAP					(8)							// �e�N�X�`���̐�

#define MAPCHIP_WIDTH					(64)
#define MAPCHIP_HEIGHT					(64)

#define MAP_SIZE_WIDTH					(STAGE_WIDTH  / MAPCHIP_WIDTH )		// MAP�̉���(�l�̓`�b�v��)
#define MAP_SIZE_HEIGHT					(STAGE_HEIGHT / MAPCHIP_HEIGHT)		// MAP�̏c��(�l�̓`�b�v��)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;						// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX_MAP] = { NULL };		// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX_MAP] = {
	"data/TEXTURE/map1.png",
	"data/TEXTURE/map/1.png",
	"data/TEXTURE/map/2.png",
	"data/TEXTURE/map/3.png",
	"data/TEXTURE/map/4.png",
	"data/TEXTURE/map/5.png",
	"data/TEXTURE/map/6.png",
	"data/TEXTURE/map/7.png",
};


static BOOL	g_Load = FALSE;		// ���������s�������̃t���O

static MAP	g_Map;

static MAPCHIP	g_MapChipList[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH];


int MapChipList1[MAP_SIZE_HEIGHT][MAP_SIZE_WIDTH]
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//1
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//2
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//3
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//4
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//5
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//6
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,1,1,1,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//7
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//8
	{0,0,0,0,0,0,0,0,0,6,1,1,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//9
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//10
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//11
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//12
	{0,6,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//13
	{0,0,0,0,0,0,0,0,6,1,1,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//14
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,1,1,1,7,0,0,0,0},//15
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//16
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//17
	{6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,7},//18
	{5,4,4,4,5,4,4,4,4,4,4,4,4,4,4,4,5,4,4,5,5,4,4,4,5,4,4,4,4,4,4,4,4,4,4,4,5,4,4,5},//19
	{5,4,4,4,4,4,5,4,4,4,4,4,4,4,4,4,4,5,4,5,5,4,4,4,5,4,4,4,4,4,4,4,4,4,4,4,5,4,4,5},//20
};


// ����������
//=============================================================================
HRESULT InitMap(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX_MAP; i++)
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
	g_Map.w = STAGE_WIDTH;
	g_Map.h = STAGE_HEIGHT;
	g_Map.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Map.texNo = 0;


	// MapChip�̏�����
	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			// MapChip�̍��W�͍���̓_�B�v�Z�֗��̂���
			float chip_pos_x = (float)j * MAPCHIP_WIDTH;
			float chip_pos_y = (float)i * MAPCHIP_HEIGHT;

			g_MapChipList[i][j].pos = D3DXVECTOR3(chip_pos_x, chip_pos_y, 0.0f);

			// MapChip�̒��S�̍��W�B�����蔻��̂���
			float chip_pos_center_x = chip_pos_x + MAPCHIP_WIDTH * 0.5f;
			float chip_pos_center_y = chip_pos_y + MAPCHIP_HEIGHT * 0.5f;

			g_MapChipList[i][j].pos_center = D3DXVECTOR3(chip_pos_center_x, chip_pos_center_y, 0.0f);

			g_MapChipList[i][j].w = MAPCHIP_WIDTH;
			g_MapChipList[i][j].h = MAPCHIP_HEIGHT;


			g_MapChipList[i][j].texNo = MapChipList1[i][j];
			
			g_MapChipList[i][j].isTouch = FALSE;
;		}
	}


	g_Load = TRUE;	// �f�[�^�̏��������s����
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMap(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX_MAP; i++)
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
void UpdateMap(void)
{

	#ifdef _DEBUG
			// �f�o�b�O�\��
			PrintDebugProc("MapChipList[9][0] X:%f Y:%f\n", g_MapChipList[9][0].pos.x, g_MapChipList[9][0].pos.y);
			PrintDebugProc("MapChipList[9][24] X:%f Y:%f\n", g_MapChipList[9][39].pos.x, g_MapChipList[9][39].pos.y);
			PrintDebugProc("Map X:%f Y:%f\n", g_Map.pos.x, g_Map.pos.y);
	#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMap(void)
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

	// map�̔w�i��`��
	{
		for (int i = 0; i < TEXTURE_MAX_MAP; i++)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Map.texNo]);

			float px = -g_Map.pos.x;
			float py = -g_Map.pos.y;
			float pw = g_Map.w;
			float ph = g_Map.h;

			float tx = 0.0f;
			float ty = 0.0f;
			float tw = 1.0f;
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

	// stage1�̔w�i��`��
	{
		for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
		{
			for (int j = 0; j < MAP_SIZE_WIDTH; j++)
			{
				// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_MapChipList[i][j].texNo]);

			float px = g_MapChipList[i][j].pos.x - g_Map.pos.x;
			float py = g_MapChipList[i][j].pos.y - g_Map.pos.y;
			float pw = g_MapChipList[i][j].w;
			float ph = g_MapChipList[i][j].h;

			float tx = 0.0f;
			float ty = 0.0f;
			float tw = 1.0f;
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

}

//=============================================================================
// Player��mapchip�̓����蔻��
//=============================================================================
BOOL CheckMapList(PLAYER player)
{
	// ������
	BOOL IsMapchip = FALSE;
	
	// �����蔻����W�͑����ɒ���
	D3DXVECTOR3 player_foot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	player_foot.x = player.pos.x;
	player_foot.y = player.pos.y + player.h * 0.3f;

	// �����蔻��͈͂͑����ɒ���
	float p_w_col	= player.w * 0.5f;
	float p_h_col	= player.h * 0.01f;

	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			if (g_MapChipList[i][j].texNo != 0)				// mapchip�͕K����������
			{
				if (CollisionBB(player_foot, p_w_col, p_h_col, g_MapChipList[i][j].pos_center, g_MapChipList[i][j].w, g_MapChipList[i][j].h))
				{
					g_MapChipList[i][j].isTouch = TRUE;
					IsMapchip = TRUE;
				}
			}
		}
	}

	return IsMapchip;
}

//=============================================================================
// Enemy��mapchip�̓����蔻��
//=============================================================================
BOOL CheckMapList2(ENEMY enemy, float w, float h)
{
	// ������
	BOOL IsMapchip = FALSE;	

	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			if (g_MapChipList[i][j].texNo != 0)			//	mapchip�͕K����������
			{
				float e_left = enemy.pos.x - w * 0.5f;
				float e_right = enemy.pos.x + w * 0.5f;
				float e_down = enemy.pos.y + h * 0.5f;
				float e_up = enemy.pos.y - h * 0.5f;
				float m_left = g_MapChipList[i][j].pos.x;
				float m_right = g_MapChipList[i][j].pos.x + MAPCHIP_WIDTH;
				float m_up = g_MapChipList[i][j].pos.y;
				float m_down = g_MapChipList[i][j].pos.y + MAPCHIP_HEIGHT;
				if ((e_right > m_left) && (e_left < m_right) && (e_down > m_up) && (e_up < m_down))
				{
					IsMapchip = TRUE;
					g_MapChipList[i][j].isTouch = TRUE;
				}
			}
		}
	}
	return IsMapchip;
}


//=============================================================================
// enemy�Ɠ�������mapchip��Texno���擾
//=============================================================================
int MapChipListNum(ENEMY enemy, float w, float h)
{
	int mapChipListNum;
	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			if (g_MapChipList[i][j].texNo != 0)
			{
				float e_left = enemy.pos.x - w * 0.5f;
				float e_right = enemy.pos.x + w * 0.5f;
				float e_down = enemy.pos.y + h * 0.5f;
				float e_up = enemy.pos.y - h * 0.5f;
				float m_left = g_MapChipList[i][j].pos.x;
				float m_right = g_MapChipList[i][j].pos.x + MAPCHIP_WIDTH;
				float m_up = g_MapChipList[i][j].pos.y;
				float m_down = g_MapChipList[i][j].pos.y + MAPCHIP_HEIGHT;
				if ((e_right > m_left) && (e_left < m_right) && (e_down > m_up) && (e_up < m_down))
				{
					mapChipListNum = g_MapChipList[i][j].texNo;
				}
			}
		}
	}
	return mapChipListNum;
}


//=============================================================================
// MAP�\���̂̐擪�A�h���X���擾
//=============================================================================
MAP *GetMap(void)
{
	return &g_Map;
}


MAPCHIP *GetMapChip(void)
{
	return &g_MapChipList[0][0];
}



