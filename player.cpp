//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
//#include "bg.h"
#include "map.h"
#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "file.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH_PLAYER			(100)	// キャラサイズ
#define TEXTURE_HEIGHT_PLAYER			(150)	// 
#define TEXTURE_MAX						(3)		// テクスチャの数
#define PLAYER_START_X					()
#define PLAYER_START_Y					()

// runのテクスチャ
#define TEXTURE_PATTERN_DIVIDE_X_RUN	(1)																	// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_RUN	(8)																	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_RUN			(TEXTURE_PATTERN_DIVIDE_X_RUN * TEXTURE_PATTERN_DIVIDE_Y_RUN)		// アニメーションパターン数
#define ANIM_WAIT_RUN					(4)																	// アニメーションの切り替わるWait値

// idleのテクスチャ
#define TEXTURE_PATTERN_DIVIDE_X_IDLE	(1)																	// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_IDLE	(6)																	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_IDLE			(TEXTURE_PATTERN_DIVIDE_X_IDLE * TEXTURE_PATTERN_DIVIDE_Y_IDLE)		// アニメーションパターン数
#define ANIM_WAIT_IDLE					(12)																// アニメーションの切り替わるWait値

// ダメージを受けるのテクスチャ
#define TEXTURE_PATTERN_DIVIDE_X_DAMAGE	(1)																	// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_DAMAGE	(3)																	// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_DAMAGE			(TEXTURE_PATTERN_DIVIDE_X_DAMAGE * TEXTURE_PATTERN_DIVIDE_Y_DAMAGE)	// アニメーションパターン数
#define ANIM_WAIT_DAMAGE				(3)																	// アニメーションの切り替わるWait値
#define ANIM_DAMAGE_CYCLE				(3)																	// アニメーションの繰り返しの回数

// プレイヤーの画面内配置座標
#define PLAYER_DISP_X					(SCREEN_WIDTH /2)
#define PLAYER_DISP_Y					(SCREEN_HEIGHT/2 + TEXTURE_HEIGHT_PLAYER)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawPlayerOffset(int no);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;					// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };		// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/player/witch_idle.png",
	"data/TEXTURE/player/witch_run.png",
	"data/TEXTURE/player/witch_take_damage.png",
};


static BOOL		g_Load = FALSE;			// 初期化を行ったかのフラグ
static PLAYER	g_Player[PLAYER_MAX];	// プレイヤー構造体



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
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


	// プレイヤー構造体の初期化
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		// テクスチャの初期化
		g_Player[i].use = TRUE;
		g_Player[i].pos = D3DXVECTOR3(50.0f, 100.0f, 0.0f);			// 中心点から表示
		g_Player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].w   = TEXTURE_WIDTH_PLAYER;
		g_Player[i].h   = TEXTURE_HEIGHT_PLAYER;
		g_Player[i].texNo = PLAYER_TEXTURE_IDLE;

		// アニメの初期化
		g_Player[i].countAnim = 0;
		g_Player[i].patternAnim = 0;
		g_Player[i].aniCycle_damage = 0;

		// 移動の初期化
		g_Player[i].move	= D3DXVECTOR3(4.0f, 0.0f, 0.0f);		// 移動量
		g_Player[i].dir		= DIR_PLAYER_RIGHT;							// 初期化は右向き
		g_Player[i].keyOn			= FALSE;						// Key押されているかフラグ(BGの動き)
		g_Player[i].isTakeDamage	= FALSE;
		g_Player[i].canBGmove		= FALSE;
		g_Player[i].isRush			= FALSE;
		g_Player[i].noDamageMod		= FALSE;

		// 重力の初期化
		g_Player[i].gravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].vel		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Player[i].isFlying = FALSE;								// 垂直方向飛ぶと、BGは動かない


		// プレーヤーの属性の初期化
		g_Player[i].hp = PLAYER_HP_MAX;
		g_Player[i].mp = PLAYER_MP_MAX;

		// 分身用
		for (int j = 0; j < PLAYER_OFFSET_CNT; j++)
		{
			g_Player[i].offset[j] = g_Player[i].pos;
		}
	}

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	
	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].hp == 0)
		{
			g_Player[i].use = FALSE;
		}
		
		// 生きてるプレイヤーだけ処理をする
		if (g_Player[i].use == TRUE)
		{
			// 地形との当たり判定用に座標のバックアップを取っておく
			D3DXVECTOR3 pos_old = g_Player[i].pos;

			// 分身用
			for (int j = PLAYER_OFFSET_CNT-1; j > 0; j--)
			{
				g_Player[i].offset[j] = g_Player[i].offset[j-1];
			}
			g_Player[i].offset[0] = pos_old;

			// アニメーション 
			{

				// ダメージを受けると、ダメージを受ける動画をする
				if (g_Player[i].isTakeDamage == TRUE && g_Player[i].isRush == FALSE)
				{				
					g_Player[i].countAnim += 1;

					if (g_Player[i].countAnim > ANIM_WAIT_DAMAGE)
					{
						g_Player[i].countAnim = 0;
						// パターンの切り替え
						g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_DAMAGE;

						if (g_Player[i].patternAnim == ANIM_PATTERN_NUM_DAMAGE - 1)
						{
							g_Player[i].aniCycle_damage += 1;
						}

						// ダメージを受ける動画を3回ループする
						if (g_Player[i].aniCycle_damage == 3)
						{
							g_Player[i].isTakeDamage = FALSE;
							g_Player[i].patternAnim = 0;
							g_Player[i].countAnim = 0;
							g_Player[i].aniCycle_damage = 0;
						}
					}
				}

				// ラッシュ
				if (g_Player[i].isRush == TRUE)
				{
					g_Player[i].countAnim += 1;

					if (g_Player[i].countAnim > ANIM_WAIT_RUN)
					{
						g_Player[i].countAnim = 0;
						// パターンの切り替え
						g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_RUN;

						// 一回だけで、動画をする
						if (g_Player[i].patternAnim == 3)
						{
							g_Player[i].isRush = FALSE;
							g_Player[i].noDamageMod = FALSE;
							g_Player[i].patternAnim = 0;
							g_Player[i].countAnim = 0;
							g_Player[i].keyOn = FALSE;
						}
					}
				}

				// 走ると立てる動画のループ
				if (g_Player[i].isTakeDamage	== FALSE &&
					g_Player[i].isRush			== FALSE)
				{
					if (g_Player[i].keyOn == TRUE)
					{
						// 走る動画のループ
						g_Player[i].countAnim += 1;

						if (g_Player[i].countAnim > ANIM_WAIT_RUN)
						{
							g_Player[i].countAnim = 0;
							// パターンの切り替え
							g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_RUN;
						}
					}
					else
					{
						// 立てる動画のループ
						g_Player[i].countAnim += 1;

						if (g_Player[i].countAnim > ANIM_WAIT_IDLE)
						{
							g_Player[i].countAnim = 0;
							// パターンの切り替え
							g_Player[i].patternAnim = (g_Player[i].patternAnim + 1) % ANIM_PATTERN_NUM_IDLE;
						}
					}
				}
				
			}


			// ダメージを受けると、強制的な後退の設定と、違うの方向のダメージ動画の設定
			if (g_Player[i].isTakeDamage == TRUE && g_Player[i].isRush == FALSE)
			{
				g_Player[i].keyOn = FALSE;
				

				if (g_Player[i].dir == DIR_PLAYER_RIGHT)
				{
					g_Player[i].pos.x -= g_Player[i].move.x;
				}
				else
				{
					g_Player[i].pos.x += g_Player[i].move.x;
				}
			}

			// ラッシュ
			if (g_Player[i].isRush == TRUE)
			{
				if (g_Player[i].dir == DIR_PLAYER_RIGHT)
				{
					g_Player[i].pos.x += g_Player[i].move.x * 5.0f;
					g_Player[i].canBGmove = TRUE;
				}
				else
				{
					g_Player[i].pos.x -= g_Player[i].move.x * 5.0f;
					g_Player[i].canBGmove = TRUE;
				}
			}
		

			// playerとmapchipの判定処理(重力の制御)
			if (CheckMapList(g_Player[i]) == TRUE)
			{
				g_Player[i].gravity.y = 0.0f;
				g_Player[i].vel.y = 0.0f;
				g_Player[i].isFlying = FALSE;
			}



			// 重力
			if (g_Player[i].isRush == FALSE)
			{
				g_Player[i].pos += g_Player[i].gravity;
				g_Player[i].gravity.y += 0.5f;
			}
			


			// キー入力で移動 
			{
				if (g_Player[i].isTakeDamage == FALSE && g_Player[i].isRush == FALSE)
				{
					// キーボード入力の初期化
					g_Player[i].keyOn = FALSE;

					// テクスチャの初期化
					g_Player[i].texNo = PLAYER_TEXTURE_IDLE;

					// 背景移動の初期化
					g_Player[i].canBGmove = FALSE;

					// 飛ぶの制御
					if (GetKeyboardPress(DIK_SPACE))
					{
						g_Player[i].keyOn = TRUE;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].isFlying = TRUE;
						g_Player[i].gravity = D3DXVECTOR3(0.0f, -0.8f, 0.0f);
						g_Player[i].vel = D3DXVECTOR3(0.0f, -3.5f, 0.0f);
						g_Player[i].pos += g_Player[i].vel;
					}

					// 走ると立てるの制御
					float speed = g_Player[i].move.x;

					// →の移動
					if (GetKeyboardPress(DIK_D))
					{
						
						g_Player[i].pos.x += speed;
						g_Player[i].keyOn = TRUE;
						g_Player[i].dir = DIR_PLAYER_RIGHT;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].isFlying = FALSE;
						g_Player[i].canBGmove = TRUE;
					}
					// ←の移動
					if (GetKeyboardPress(DIK_A))
					{
						g_Player[i].pos.x -= speed;
						g_Player[i].keyOn = TRUE;
						g_Player[i].dir = DIR_PLAYER_LEFT;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].isFlying = FALSE;
						g_Player[i].canBGmove = TRUE;
					}

					// ラッシュ
					if (GetKeyboardTrigger(DIK_K))
					{
						g_Player[i].isRush = TRUE;
						g_Player[i].texNo = PLAYER_TEXTURE_RUN;
						g_Player[i].keyOn = TRUE;
						g_Player[i].noDamageMod = TRUE;
						g_Player[i].countAnim = 0;
						g_Player[i].patternAnim = 0;
					}

					// 弾発射
					if (GetKeyboardTrigger(DIK_J))
					{
						if (g_Player[i].mp > 0)
						{
							g_Player[i].mp -= 1;
							D3DXVECTOR3 pos = g_Player[i].pos;
							if (g_Player[i].dir == DIR_PLAYER_RIGHT)
							{
								pos.x = g_Player[i].pos.x + 20;
								SetBullet(pos, DIR_PLAYER_RIGHT);		// １発目
							}
							if (g_Player[i].dir == DIR_PLAYER_LEFT)
							{
								pos.x = g_Player[i].pos.x - 20;
								SetBullet(pos, DIR_PLAYER_LEFT);		// １発目
							}
						}
						
					}
					
				}
				
				

				// MAP外チェック
				MAP *map = GetMap();

				if (g_Player[i].pos.x < 0.0f)
				{
					g_Player[i].pos.x = 0.0f;
				}

				if (g_Player[i].pos.x > map->w)
				{
					g_Player[i].pos.x = map->w;
				}

				if (g_Player[i].pos.y < 0.0f)
				{
					g_Player[i].pos.y = 0.0f;
				}

				if (g_Player[i].pos.y > map->h)
				{
					g_Player[i].pos.y = map->h;
				}

				// プレイヤーの立ち位置からMAPのスクロール座標を計算する
				map->pos.x = g_Player[i].pos.x - PLAYER_DISP_X;

				if (map->pos.x < 0)
				{
					map->pos.x = 0;
				}

				if (map->pos.x > map->w - SCREEN_WIDTH)
				{
					map->pos.x = map->w - SCREEN_WIDTH;
				}

				map->pos.y = g_Player[i].pos.y - PLAYER_DISP_Y;

				if (map->pos.y < 0) 
				{
					map->pos.y = 0;
				}

				if (map->pos.y > map->h - SCREEN_HEIGHT) 
				{
					map->pos.y = map->h - SCREEN_HEIGHT;
				}

				
				// 移動が終わったらエネミーとの当たり判定
				if (g_Player[i].noDamageMod == FALSE)
				{
					ENEMY *enemy = GetEnemy();

					// エネミーの数分当たり判定を行う
					for (int j = 0; j < ENEMY_MAX; j++)
					{
						// 生きてるエネミーと当たり判定をする
						if (enemy[j].use == TRUE)
						{
							BOOL ans = CollisionBB(g_Player[i].pos, g_Player[i].w -10.0f, g_Player[i].h, 
												   enemy[j].pos,    enemy[j].w -10.0f,    enemy[j].h);
							// 当たっている？
							if (ans == TRUE)
							{
								// 当たった時の処理
								//enemy[j].use = FALSE;
								g_Player[i].hp -= 1;
								g_Player[i].isTakeDamage = TRUE;
								g_Player[i].texNo = PLAYER_TEXTURE_TAKEDAMAGE;
								g_Player[i].canBGmove = TRUE;
								AddScore(-500);
								PlaySound(SOUND_LABEL_SE_defend000);
							}
						}
					}
				}	
			}
		}

#ifdef _DEBUG
		// デバッグ表示
		PrintDebugProc("Player %d Pos  X:%f Y:%f\n", i, g_Player[i].pos.x, g_Player[i].pos.y);
		PrintDebugProc("Player isTakeDamage %d\n",g_Player[i].isTakeDamage);
		PrintDebugProc("Player isRush %d\n", g_Player[i].isRush);
		PrintDebugProc("PlayerTexNo:%d\n", g_Player[i].texNo);
		PrintDebugProc("Playerpattern:%d\n", g_Player[i].patternAnim);
		PrintDebugProc("Playercount:%d\n", g_Player[i].countAnim);
		// 現状をセーブする
		if (GetKeyboardTrigger(DIK_T))
		{
			SaveData();
		}

#endif

	}
	
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (g_Player[i].use == TRUE)		// プレーヤーが生きていると
		{					
			// ダメージを受ける描画処理
			if (g_Player[i].isTakeDamage == TRUE && g_Player[i].isRush == FALSE)
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_Player[i].pos.x - map->pos.x;	// プレイヤーの表示位置X
				float py = g_Player[i].pos.y - map->pos.y;	// プレイヤーの表示位置Y
				float pw = g_Player[i].w;		// プレイヤーの表示幅
				float ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_DAMAGE * g_Player[i].dir;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_DAMAGE;	// テクスチャの高さ
				float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_DAMAGE) * tw * g_Player[i].dir;	// テクスチャの左上X座標
				float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_DAMAGE) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// ラッシュ描画処理
			if (g_Player[i].isRush == TRUE)
			{
				// プレイヤーの分身を描画
				DrawPlayerOffset(i);

				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

				//プレイヤーの位置やテクスチャー座標を反映
				float px = g_Player[i].pos.x - map->pos.x;	// プレイヤーの表示位置X
				float py = g_Player[i].pos.y - map->pos.y;	// プレイヤーの表示位置Y
				float pw = g_Player[i].w;		// プレイヤーの表示幅
				float ph = g_Player[i].h;		// プレイヤーの表示高さ

				// アニメーション用
				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_RUN * g_Player[i].dir;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_RUN;	// テクスチャの高さ
				float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_RUN) * tw * g_Player[i].dir;	// テクスチャの左上X座標
				float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_RUN) * th;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
					g_Player[i].rot.z);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}


			// 走ると立てる描画処理
			if (g_Player[i].isTakeDamage == FALSE && g_Player[i].isRush == FALSE)
			{
				if (g_Player[i].keyOn == TRUE)
				{
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

					//プレイヤーの位置やテクスチャー座標を反映
					float px = g_Player[i].pos.x - map->pos.x;	// プレイヤーの表示位置X
					float py = g_Player[i].pos.y - map->pos.y;	// プレイヤーの表示位置Y
					float pw = g_Player[i].w;		// プレイヤーの表示幅
					float ph = g_Player[i].h;		// プレイヤーの表示高さ

					// アニメーション用
					float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_RUN * g_Player[i].dir;	// テクスチャの幅
					float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_RUN;	// テクスチャの高さ
					float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_RUN) * tw * g_Player[i].dir;	// テクスチャの左上X座標
					float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_RUN) * th;	// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}
				else
				{
					// テクスチャ設定
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[i].texNo]);

					//プレイヤーの位置やテクスチャー座標を反映
					float px = g_Player[i].pos.x - map->pos.x;	// プレイヤーの表示位置X
					float py = g_Player[i].pos.y - map->pos.y;	// プレイヤーの表示位置Y
					float pw = g_Player[i].w;		// プレイヤーの表示幅
					float ph = g_Player[i].h;		// プレイヤーの表示高さ

					// アニメーション用
					float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_IDLE * g_Player[i].dir;	// テクスチャの幅
					float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_IDLE;	// テクスチャの高さ
					float tx = (float)(g_Player[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_IDLE) * tw * g_Player[i].dir;	// テクスチャの左上X座標
					float ty = (float)(g_Player[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_IDLE) * th;	// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
						g_Player[i].rot.z);

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
				}
			}
			
		}
	}
}


//=============================================================================
// プレイヤー構造体の先頭アドレスを取得
//=============================================================================
PLAYER *GetPlayer(void)
{

	return &g_Player[0];
}


//=============================================================================
// プレイヤーの分身を描画
//=============================================================================
void DrawPlayerOffset(int no)
{
	MAP *map = GetMap();
	float alpha = 0.0f;

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Player[no].texNo]);
	
	for (int j = PLAYER_OFFSET_CNT-1; j >= 0; j--)
	{
		//プレイヤーの位置やテクスチャー座標を反映
		float px = g_Player[no].offset[j].x - map->pos.x;	// プレイヤーの表示位置X
		float py = g_Player[no].offset[j].y - map->pos.y;	// プレイヤーの表示位置Y
		float pw = g_Player[no].w;		// プレイヤーの表示幅
		float ph = g_Player[no].h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_RUN * g_Player[no].dir;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_RUN;	// テクスチャの高さ
		float tx = (float)(g_Player[no].patternAnim % TEXTURE_PATTERN_DIVIDE_X_RUN) * tw  * g_Player[no].dir;	// テクスチャの左上X座標
		float ty = (float)(g_Player[no].patternAnim / TEXTURE_PATTERN_DIVIDE_X_RUN) * th;	// テクスチャの左上Y座標


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha),
			g_Player[no].rot.z);

		alpha += (1.0f / PLAYER_OFFSET_CNT);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}



