//=============================================================================
//
// map画面処理 [map.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "map.h"
#include "sprite.h"
#include "player.h"
#include "collision.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX_MAP					(8)							// テクスチャの数

#define MAPCHIP_WIDTH					(64)
#define MAPCHIP_HEIGHT					(64)

#define MAP_SIZE_WIDTH					(STAGE_WIDTH  / MAPCHIP_WIDTH )		// MAPの横幅(値はチップ数)
#define MAP_SIZE_HEIGHT					(STAGE_HEIGHT / MAPCHIP_HEIGHT)		// MAPの縦幅(値はチップ数)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;						// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX_MAP] = { NULL };		// テクスチャ情報

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


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ

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


// 初期化処理
//=============================================================================
HRESULT InitMap(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;

	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Map.w = STAGE_WIDTH;
	g_Map.h = STAGE_HEIGHT;
	g_Map.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Map.texNo = 0;


	// MapChipの初期化
	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			// MapChipの座標は左上の点。計算便利のため
			float chip_pos_x = (float)j * MAPCHIP_WIDTH;
			float chip_pos_y = (float)i * MAPCHIP_HEIGHT;

			g_MapChipList[i][j].pos = D3DXVECTOR3(chip_pos_x, chip_pos_y, 0.0f);

			// MapChipの中心の座標。当たり判定のため
			float chip_pos_center_x = chip_pos_x + MAPCHIP_WIDTH * 0.5f;
			float chip_pos_center_y = chip_pos_y + MAPCHIP_HEIGHT * 0.5f;

			g_MapChipList[i][j].pos_center = D3DXVECTOR3(chip_pos_center_x, chip_pos_center_y, 0.0f);

			g_MapChipList[i][j].w = MAPCHIP_WIDTH;
			g_MapChipList[i][j].h = MAPCHIP_HEIGHT;


			g_MapChipList[i][j].texNo = MapChipList1[i][j];
			
			g_MapChipList[i][j].isTouch = FALSE;
;		}
	}


	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateMap(void)
{

	#ifdef _DEBUG
			// デバッグ表示
			PrintDebugProc("MapChipList[9][0] X:%f Y:%f\n", g_MapChipList[9][0].pos.x, g_MapChipList[9][0].pos.y);
			PrintDebugProc("MapChipList[9][24] X:%f Y:%f\n", g_MapChipList[9][39].pos.x, g_MapChipList[9][39].pos.y);
			PrintDebugProc("Map X:%f Y:%f\n", g_Map.pos.x, g_Map.pos.y);
	#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMap(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// mapの背景を描画
	{
		for (int i = 0; i < TEXTURE_MAX_MAP; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Map.texNo]);

			float px = -g_Map.pos.x;
			float py = -g_Map.pos.y;
			float pw = g_Map.w;
			float ph = g_Map.h;

			float tx = 0.0f;
			float ty = 0.0f;
			float tw = 1.0f;
			float th = 1.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}

	}

	// stage1の背景を描画
	{
		for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
		{
			for (int j = 0; j < MAP_SIZE_WIDTH; j++)
			{
				// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_MapChipList[i][j].texNo]);

			float px = g_MapChipList[i][j].pos.x - g_Map.pos.x;
			float py = g_MapChipList[i][j].pos.y - g_Map.pos.y;
			float pw = g_MapChipList[i][j].w;
			float ph = g_MapChipList[i][j].h;

			float tx = 0.0f;
			float ty = 0.0f;
			float tw = 1.0f;
			float th = 1.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));


			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
			}
		}

	}

}

//=============================================================================
// Playerとmapchipの当たり判定
//=============================================================================
BOOL CheckMapList(PLAYER player)
{
	// 初期化
	BOOL IsMapchip = FALSE;
	
	// 当たり判定座標は足元に調整
	D3DXVECTOR3 player_foot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	player_foot.x = player.pos.x;
	player_foot.y = player.pos.y + player.h * 0.3f;

	// 当たり判定範囲は足元に調整
	float p_w_col	= player.w * 0.5f;
	float p_h_col	= player.h * 0.01f;

	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			if (g_MapChipList[i][j].texNo != 0)				// mapchipは必ず物がある
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
// Enemyとmapchipの当たり判定
//=============================================================================
BOOL CheckMapList2(ENEMY enemy, float w, float h)
{
	// 初期化
	BOOL IsMapchip = FALSE;	

	for (int i = 0; i < MAP_SIZE_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_SIZE_WIDTH; j++)
		{
			if (g_MapChipList[i][j].texNo != 0)			//	mapchipは必ず物がある
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
// enemyと当たったmapchipのTexnoを取得
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
// MAP構造体の先頭アドレスを取得
//=============================================================================
MAP *GetMap(void)
{
	return &g_Map;
}


MAPCHIP *GetMapChip(void)
{
	return &g_MapChipList[0][0];
}



