//=============================================================================
//
// エネミー処理 [enemy.cpp]
// Author : GP11B132 13 黄源
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH_ENEMY				(96)																// キャラサイズ
#define TEXTURE_HEIGHT_ENEMY			(64)																// 
#define TEXTURE_MAX						(2)																	// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X_ENEMY	(8)																	// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_ENEMY	(1)																	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_ENEMY			(TEXTURE_PATTERN_DIVIDE_X_ENEMY*TEXTURE_PATTERN_DIVIDE_Y_ENEMY)		// アニメーションパターン数
#define ANIM_WAIT_ENEMY					(6)		// アニメーションの切り替わるWait値



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;					// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };		// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/enemy/show-Sheet.png",
	"data/TEXTURE/enemy/walk-Sheet.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static ENEMY	g_Enemy[ENEMY_MAX];		// エネミー構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ランダムでエネミーのposを生成する
	srand((unsigned)time(NULL));


	// エネミー構造体の初期化
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		g_Enemy[i].use = TRUE;
		g_Enemy[i].pos = D3DXVECTOR3(1090.0f, 800.0f, 0.0f);	// 中心点から表示
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
	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	int count = 0;
	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)	// このエネミーが使われている？
		{							// Yes
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = g_Enemy[i].pos;

			// アニメーション  
			g_Enemy[i].countAnim++;
			if ((g_Enemy[i].countAnim % ANIM_WAIT_ENEMY) == 0)
			{
				// パターンの切り替え
				g_Enemy[i].patternAnim = (g_Enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM_ENEMY;
			}

			// playerとmapchipの判定処理
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

			// 移動処理 

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


				// 移動が終わったらプレイヤーとの当たり判定
				{
					PLAYER *player = GetPlayer();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (player[j].use == TRUE && player->noDamageMod == FALSE)
						{
							BOOL ans = CollisionBB(g_Enemy[i].pos, g_Enemy[i].w -10.0f, g_Enemy[i].h,
								player[j].pos, player[j].w -10.0f, player[j].h);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
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


				// MAP外チェック
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
		//		g_Enemy[i].pos = D3DXVECTOR3(i + rand() % SCREEN_WIDTH, 0.0f, 0.0f);	// 中心点から表示
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
		// デバッグ表示
		PrintDebugProc("Enemy No%d  X:%f Y:%f texno:%d\n", i, g_Enemy[i].pos.x, g_Enemy[i].pos.y,g_Enemy[i].mapChipListNum);
		PrintDebugProc("EnemyNum %d\n", count);
#endif

	}
	

}


//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		if (g_Enemy[i].use == TRUE)		// このエネミーが使われている？
		{								// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy[i].texNo]);

			//エネミーの位置やテクスチャー座標を反映
			float px = g_Enemy[i].pos.x - map->pos.x;	// エネミーの表示位置X
			float py = g_Enemy[i].pos.y - map->pos.y;	// エネミーの表示位置Y
			float pw = g_Enemy[i].w;					// エネミーの表示幅
			float ph = g_Enemy[i].h;					// エネミーの表示高さ

			// アニメーション用
			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_ENEMY * g_Enemy[i].dir;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_ENEMY;					// テクスチャの高さ
			float tx = (float)(g_Enemy[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_ENEMY) * tw * g_Enemy[i].dir;	// テクスチャの左上X座標
			float ty = (float)(g_Enemy[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_ENEMY) * th;					// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Enemy[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


//=============================================================================
// エネミー構造体の先頭アドレスを取得
//=============================================================================
ENEMY *GetEnemy(void)
{

	return &g_Enemy[0];
}




