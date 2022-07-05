//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "particle.h"
#include "player.h"
#include "sprite.h"
//#include "bg.h"
#include "map.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(1)			// テクスチャの数

#define	PARTICLE_SIZE_X		(40.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y		(40.0f)		// 頂点サイズ
#define	VALUE_MOVE_PARTICLE	(5.0f)		// 移動速度

#define	MAX_PARTICLE		(512)		// パーティクル最大数


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3		pos;			// 位置
	D3DXVECTOR3		rot;			// 回転
	D3DXVECTOR3		scale;			// スケール
	D3DXVECTOR3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	BOOL			bUse;			// 使用しているかどうか

} PARTICLE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo;				// テクスチャ番号

static PARTICLE					g_aParticle[MAX_PARTICLE];		// パーティクルワーク
static D3DXVECTOR3				g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static BOOL						g_bPause = FALSE;				// ポーズON/OFF
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static char *g_TextureName[] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// パーティクルワークの初期化
	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		g_aParticle[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[i].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[i].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_aParticle[i].material, sizeof(g_aParticle[i].material));
		g_aParticle[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		g_aParticle[i].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[i].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[i].nIdxShadow = -1;
		g_aParticle[i].nLife = 0;
		g_aParticle[i].bUse = FALSE;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	PLAYER *pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

	if (g_bPause == FALSE)
	{
		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (g_aParticle[i].bUse)
			{// 使用中
				g_aParticle[i].pos.x += g_aParticle[i].move.x;
				g_aParticle[i].pos.z += g_aParticle[i].move.z;

				g_aParticle[i].pos.y += g_aParticle[i].move.y;
				if (g_aParticle[i].pos.y <= g_aParticle[i].fSizeY / 2)
				{// 着地した
					g_aParticle[i].pos.y = g_aParticle[i].fSizeY / 2;
					g_aParticle[i].move.y = g_aParticle[i].move.y * 0.75f;
				}

				g_aParticle[i].move.x += (0.0f - g_aParticle[i].move.x) * 0.015f;
				g_aParticle[i].move.y += 0.25f;
				g_aParticle[i].move.z += (0.0f - g_aParticle[i].move.z) * 0.015f;

				g_aParticle[i].nLife--;
				if (g_aParticle[i].nLife <= 0)
				{
					g_aParticle[i].bUse = FALSE;
					g_aParticle[i].nIdxShadow = -1;
				}
				else
				{
					if (g_aParticle[i].nLife <= 80)
					{
						g_aParticle[i].material.Diffuse.r = 0.8f - (float)(80 - g_aParticle[i].nLife) / 80 * 0.8f;
						g_aParticle[i].material.Diffuse.g = 0.1f - (float)(80 - g_aParticle[i].nLife) / 80 * 0.1f;
						g_aParticle[i].material.Diffuse.b = 0.2f - (float)(80 - g_aParticle[i].nLife) / 80 * 0.2f;
					}

					if (g_aParticle[i].nLife <= 20)
					{
						// α値設定
						g_aParticle[i].material.Diffuse.a -= 0.05f;
						if (g_aParticle[i].material.Diffuse.a < 0.0f)
						{
							g_aParticle[i].material.Diffuse.a = 0.0f;
						}
					}
				}
			}
		}

		// パーティクル発生
//		if((rand() % 2) == 0)
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;

			pos = g_posBase;

			fAngle = (float)(rand() % 628 - 314) / 100.0f;
			fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;
			move.x = sinf(fAngle) * fLength;
			move.y = rand() % 300 / 100.0f - g_fHeightBase;
			move.z = cosf(fAngle) * fLength;

			nLife = rand() % 100 + 150;

			fSize = (float)(rand() % 30 + 20);

			pos.y = fSize / 2;

			// ビルボードの設定
			SetParticle(pos, move, D3DXCOLOR(0.8f, 0.1f, 0.2f, 0.85f), fSize, fSize, nLife);
		}
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	MAP *map = GetMap();

	// ライティングを無効に
	SetLightEnable(FALSE);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(g_aParticle[i].bUse)
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = g_aParticle[i].pos.x - map->pos.x;
			float dy = g_aParticle[i].pos.y - map->pos.y;

			SetSpriteColor(g_VertexBuffer, dx, dy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// マテリアル設定
			SetMaterial(g_aParticle[i].material);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	//// ライティングを有効に
	//SetLightEnable(TRUE);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(TRUE);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
		vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	g_aParticle[nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int i = 0; i < MAX_PARTICLE; i++)
	{
		if(!g_aParticle[i].bUse)
		{
			g_aParticle[i].pos = pos;
			g_aParticle[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[i].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[i].move = move;
			g_aParticle[i].material.Diffuse = col;
			g_aParticle[i].fSizeX = fSizeX;
			g_aParticle[i].fSizeY = fSizeY;
			g_aParticle[i].nLife = nLife;
			g_aParticle[i].bUse = TRUE;

			nIdxParticle = i;
			break;
		}
	}

	return nIdxParticle;
}
