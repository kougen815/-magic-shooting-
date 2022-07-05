//=============================================================================
//
// BG画面処理 [bg.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "bg.h"
#include "sprite.h"

#include "player.h"
#include "map.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX_BG					(4)															// テクスチャの数

#define TEXTURE_WIDTH_BG				(SCREEN_WIDTH * (STAGE_WIDTH / SCREEN_WIDTH) * 3)			// 
#define TEXTURE_HEIGHT_BG				(SCREEN_HEIGHT)												// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX_BG] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX_BG] = {
	"data/TEXTURE/background/background_1.png",
	"data/TEXTURE/background/background_2.png",
	"data/TEXTURE/background/background_3.png",
	"data/TEXTURE/background/background_4.png",
};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BG	g_Bg[4];



// 初期化処理
//=============================================================================
HRESULT InitBG(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;

	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	for (int i = 0; i < TEXTURE_MAX_BG; i++)
	{
		g_Bg[i].w = TEXTURE_WIDTH_BG;
		g_Bg[i].h = TEXTURE_HEIGHT_BG;
		g_Bg[i].pos = D3DXVECTOR3(-SCREEN_WIDTH, 0.0f, 0.0f);
		g_Bg[i].texNo = i;
	}

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateBG(void)
{
	PLAYER *player = GetPlayer();
	float speed = player->move.x;

	// MAP外チェック
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
		if (player->canBGmove == TRUE)	// 垂直飛ぶと、BGは動かない。方向を付けるだけで、BGは動いる。
		{
			if (player->keyOn == TRUE)
			{
				if (player->dir == DIR_RIGHT)			// 右向き移動
				{
					g_Bg[0].pos.x -= speed;
					g_Bg[1].pos.x -= speed - 1.0f;
					g_Bg[2].pos.x -= speed - 2.0f;
					g_Bg[3].pos.x -= speed - 3.0f;
				}
				else if (player->dir == DIR_LEFT)		// 左向き移動
				{
					g_Bg[0].pos.x += speed;
					g_Bg[1].pos.x += speed - 1.0f;
					g_Bg[2].pos.x += speed - 2.0f;
					g_Bg[3].pos.x += speed - 3.0f;
				}
			}
			else if (player->isTakeDamage == TRUE)	// ダメージを受けるとの強制後退(BGの移動は逆になる)
			{
				if (player->dir == DIR_RIGHT)			// 右向き移動
				{
					g_Bg[0].pos.x += speed;
					g_Bg[1].pos.x += speed - 1.0f;
					g_Bg[2].pos.x += speed - 2.0f;
					g_Bg[3].pos.x += speed - 3.0f;
				}
				else if (player->dir == DIR_LEFT)		// 左向き移動
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
// 描画処理
//=============================================================================
void DrawBG(void)
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

	MAP *map = GetMap();

	// BGの背景を描画
	{
		for (int i = 0; i < TEXTURE_MAX_BG; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bg[i].texNo]);

			float px = g_Bg[i].pos.x - map->pos.x;
			float py = g_Bg[i].pos.y;
			float pw = g_Bg[i].w;
			float ph = g_Bg[i].h;

			float tx = 0.0f;
			float ty = 0.0f;
			float tw = 1.0f*(TEXTURE_WIDTH_BG / SCREEN_WIDTH);
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


//=============================================================================
// BG構造体の先頭アドレスを取得
//=============================================================================
BG *GetBG(void)
{
	return &g_Bg[0];
}





