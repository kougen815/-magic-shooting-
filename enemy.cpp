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
#define TEXTURE_MAX							(2)																	// テクスチャの数

#define TEXTURE_WIDTH_SNAIL					(96)																// キャラサイズ
#define TEXTURE_HEIGHT_SNAIL				(64)																// 



// Snailのアニメ
// 登場アニメ
#define TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW	(8)																				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_SNAIL_SHOW	(1)																				// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_SNAIL_SHOW			(TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW * TEXTURE_PATTERN_DIVIDE_Y_SNAIL_SHOW)		// アニメーションパターン数
#define ANIM_WAIT_SNAIL_SHOW				(6)																				// アニメーションの切り替わるWait値
// 移動アニメ
#define TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK	(8)																				// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_SNAIL_WALK	(1)																				// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_SNAIL_WALK			(TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK * TEXTURE_PATTERN_DIVIDE_Y_SNAIL_WALK)		// アニメーションパターン数
#define ANIM_WAIT_SNAIL_WALK				(6)																				// アニメーションの切り替わるWait値



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


static BOOL		g_Load = FALSE;						// 初期化を行ったかのフラグ
static ENEMY	g_Enemy_Snail[ENEMY_SNAIL_MAX];		// エネミー構造体


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
	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
	{
		g_Enemy_Snail[i].use	= TRUE;
		g_Enemy_Snail[i].pos	= D3DXVECTOR3(1090.0f, 800.0f, 0.0f);	// 中心点から表示
		g_Enemy_Snail[i].rot	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy_Snail[i].w		= TEXTURE_WIDTH_SNAIL;
		g_Enemy_Snail[i].h		= TEXTURE_HEIGHT_SNAIL;
		g_Enemy_Snail[i].texNo	= ENEMY_SNAIL_TEXTURE_SHOW;
		g_Enemy_Snail[i].dir	= DIR_ENEMY_LEFT;

		g_Enemy_Snail[i].mapChipListNum = 0;

		g_Enemy_Snail[i].isShow = TRUE;
		g_Enemy_Snail[i].isWalk = FALSE;
		g_Enemy_Snail[i].isDead = FALSE;
		g_Enemy_Snail[i].countAnim = 0;
		g_Enemy_Snail[i].patternAnim = 0;

		g_Enemy_Snail[i].hp = ENEMY_SNAIL_HP_MAX;

		g_Enemy_Snail[i].move = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
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
	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
	{
		if (g_Enemy_Snail[i].hp == 0)
		{
			g_Enemy_Snail[i].use = FALSE;
		}

		// 生きてるプレイヤーだけ処理をする
		if (g_Enemy_Snail[i].use == TRUE)	
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = g_Enemy_Snail[i].pos;

			// アニメーション  
			{
				// 登場アニメ
				if (g_Enemy_Snail[i].isShow == TRUE)
				{
					g_Enemy_Snail[i].countAnim += 1;

					if (g_Enemy_Snail[i].countAnim > ANIM_WAIT_SNAIL_SHOW)
					{
						g_Enemy_Snail[i].countAnim = 0;
						// パターンの切り替え
						g_Enemy_Snail[i].patternAnim = (g_Enemy_Snail[i].patternAnim + 1) % ANIM_PATTERN_NUM_SNAIL_SHOW;

						if (g_Enemy_Snail[i].patternAnim == ANIM_PATTERN_NUM_SNAIL_SHOW - 1)
						{
							g_Enemy_Snail[i].isShow = FALSE;
							g_Enemy_Snail[i].isWalk = TRUE;
						}
					}
				}

				// 移動アニメ
				if (g_Enemy_Snail[i].isWalk == TRUE)
				{
					g_Enemy_Snail[i].countAnim += 1;

					if (g_Enemy_Snail[i].countAnim > ANIM_WAIT_SNAIL_WALK)
					{
						g_Enemy_Snail[i].countAnim = 0;
						// パターンの切り替え
						g_Enemy_Snail[i].patternAnim = (g_Enemy_Snail[i].patternAnim + 1) % ANIM_PATTERN_NUM_SNAIL_WALK;
					}
				}
			}
			
			// playerとmapchipの判定処理
			if (CheckMapList2(g_Enemy_Snail[i], TEXTURE_WIDTH_SNAIL / 4, TEXTURE_HEIGHT_SNAIL / 2) == TRUE)
			{
				g_Enemy_Snail[i].gravity.y = 0;
				g_Enemy_Snail[i].vel.y = 0;
				g_Enemy_Snail[i].mapChipListNum = MapChipListNum(g_Enemy_Snail[i], TEXTURE_WIDTH_SNAIL / 4, TEXTURE_HEIGHT_SNAIL / 2);
			}

			g_Enemy_Snail[i].pos += g_Enemy_Snail[i].gravity;
			g_Enemy_Snail[i].gravity.y += 0.2f;

			if (g_Enemy_Snail[i].gravity.y > 30)
			{
				g_Enemy_Snail[i].gravity.y = 30;
			}

			// 移動処理 
			{
				if (g_Enemy_Snail[i].isWalk == TRUE)
				{
					g_Enemy_Snail[i].texNo = ENEMY_SNAIL_TEXTURE_WALK;
				}
				/*if (g_Enemy_Snail[i].dir == DIR_RIGHT)
				{
					g_Enemy_Snail[i].pos.x += g_Enemy_Snail->move.x;
					g_Enemy_Snail[i].texNo = 1;
				}
				else if (g_Enemy_Snail[i].dir == DIR_LEFT)
				{
					g_Enemy_Snail[i].pos.x -= g_Enemy_Snail->move.x;
					g_Enemy_Snail[i].texNo = 0;
				}

				if (g_Enemy_Snail[i].mapChipListNum == 6)
				{
					g_Enemy_Snail[i].dir = DIR_RIGHT;
					g_Enemy_Snail[i].pos.x += g_Enemy_Snail->move.x;
				}
				else if (g_Enemy_Snail[i].mapChipListNum == 7)
				{
					g_Enemy_Snail[i].dir = DIR_LEFT;
					g_Enemy_Snail[i].pos.x -= g_Enemy_Snail->move.x;
				}*/


				// 移動が終わったらプレイヤーとの当たり判定
				{
					PLAYER *player = GetPlayer();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_SNAIL_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (player[j].use == TRUE && player->noDamageMod == FALSE)
						{
							BOOL ans = CollisionBB(g_Enemy_Snail[i].pos, g_Enemy_Snail[i].w -10.0f, g_Enemy_Snail[i].h,
								player[j].pos, player[j].w -10.0f, player[j].h);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
								//player->hp -= 1;
								player->isTakeDamage = TRUE;
								player->countAnim = 0;
								player->patternAnim = 0;
								//g_Enemy_Snail[i].use = FALSE;
								AddScore(-500);
								PlaySound(SOUND_LABEL_SE_defend000);
							}
						}
					}
				}


				// MAP外チェック
				MAP *map = GetMap();

				if (g_Enemy_Snail[i].pos.x < 0.0f)
				{
					g_Enemy_Snail[i].pos.x = 0.0f;
				}

				if (g_Enemy_Snail[i].pos.x > map->w)
				{
					g_Enemy_Snail[i].pos.x = map->w;
				}

				if (g_Enemy_Snail[i].pos.y < 0.0f)
				{
					g_Enemy_Snail[i].pos.y = 0.0f;
				}

				if (g_Enemy_Snail[i].pos.y > map->h)
				{
					g_Enemy_Snail[i].pos.y = map->h;
				}



			}
		}
		else
		{
			count++;
		}
		//if (count == ENEMY_SNAIL_MAX)
		//{
		//	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
		//	{
		//		g_Enemy_Snail[i].use = TRUE;
		//		g_Enemy_Snail[i].pos = D3DXVECTOR3(i + rand() % SCREEN_WIDTH, 0.0f, 0.0f);	// 中心点から表示
		//		g_Enemy_Snail[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//		g_Enemy_Snail[i].w = TEXTURE_WIDTH_ENEMY;
		//		g_Enemy_Snail[i].h = TEXTURE_HEIGHT_ENEMY;
		//		g_Enemy_Snail[i].texNo = 0;
		//		g_Enemy_Snail[i].dir = DIR_LEFT;
		//		g_Enemy_Snail[i].mapChipListNum = 0;

		//		g_Enemy_Snail[i].countAnim = 0;
		//		g_Enemy_Snail[i].patternAnim = 0;

		//		g_Enemy_Snail[i].move = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
		//	}
		//}
			

#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Enemy No%d  X:%f Y:%f texno:%d\n", i, g_Enemy_Snail[i].pos.x, g_Enemy_Snail[i].pos.y,g_Enemy_Snail[i].mapChipListNum);
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

	for (int i = 0; i < ENEMY_SNAIL_MAX; i++)
	{
		if (g_Enemy_Snail[i].use == TRUE)		// このエネミーが使われている？
		{										// Yes

			if (g_Enemy_Snail[i].isShow == TRUE)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy_Snail[i].texNo]);

				//エネミーの位置やテクスチャー座標を反映
				float px = g_Enemy_Snail[i].pos.x - map->pos.x;	// エネミーの表示位置X
				float py = g_Enemy_Snail[i].pos.y - map->pos.y;	// エネミーの表示位置Y
				float pw = g_Enemy_Snail[i].w;					// エネミーの表示幅
				float ph = g_Enemy_Snail[i].h;					// エネミーの表示高さ

				// アニメーション用
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW * g_Enemy_Snail[i].dir;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_SNAIL_SHOW;					// テクスチャの高さ
				float tx = (float)(g_Enemy_Snail[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW) * tw * g_Enemy_Snail[i].dir;	// テクスチャの左上X座標
				float ty = (float)(g_Enemy_Snail[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_SNAIL_SHOW) * th;					// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy_Snail[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			if (g_Enemy_Snail[i].isWalk == TRUE)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Enemy_Snail[i].texNo]);

				//エネミーの位置やテクスチャー座標を反映
				float px = g_Enemy_Snail[i].pos.x - map->pos.x;	// エネミーの表示位置X
				float py = g_Enemy_Snail[i].pos.y - map->pos.y;	// エネミーの表示位置Y
				float pw = g_Enemy_Snail[i].w;					// エネミーの表示幅
				float ph = g_Enemy_Snail[i].h;					// エネミーの表示高さ

				// アニメーション用
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK * g_Enemy_Snail[i].dir;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_SNAIL_WALK;					// テクスチャの高さ
				float tx = (float)(g_Enemy_Snail[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK) * tw * g_Enemy_Snail[i].dir;	// テクスチャの左上X座標
				float ty = (float)(g_Enemy_Snail[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_SNAIL_WALK) * th;					// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Enemy_Snail[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			
		}
	}
}


//=============================================================================
// エネミー構造体の先頭アドレスを取得
//=============================================================================
ENEMY *GetEnemy(void)
{

	return &g_Enemy_Snail[0];
}




