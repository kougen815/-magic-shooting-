//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "bullet.h"
#include "bullet_explosion.h"

#include "sprite.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "map.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(2)													// テクスチャの数

// bullet
#define TEXTURE_WIDTH				(104)												// キャラサイズ
#define TEXTURE_HEIGHT				(40)												// 

#define TEXTURE_PATTERN_DIVIDE_X	(6)													// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(10)												// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(4)													// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/bullet/fireball_right.png",
	"data/TEXTURE/bullet/fireball_left.png",
};

static BOOL	  g_Load = FALSE;		// 初期化を行ったかのフラグ
static BULLET g_Bullet[BULLET_MAX];	// バレット構造体


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
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


	// バレット構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use		= FALSE;			// 未使用（発射されていない弾）

		// テクスチャの初期化
		g_Bullet[i].w		= TEXTURE_WIDTH;
		g_Bullet[i].h		= TEXTURE_HEIGHT;
		g_Bullet[i].pos		= D3DXVECTOR3(300, 300.0f, 0.0f);
		g_Bullet[i].rot		= D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo	= 0;

		// アニメの初期化
		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		// 移動の初期化
		g_Bullet[i].move = D3DXVECTOR3(BULLET_SPEED, 0.0f, 0.0f);	// 移動量を初期化
		g_Bullet[i].dir = DIR_RIGHT;
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
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
void UpdateBullet(void)
{
	MAP *map = GetMap();

	int bulletCount = 0;				// 処理したバレットの数

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// このバレットが使われている？
		{								// Yes
			// アニメーション  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// バレットの移動処理
			if (g_Bullet[i].dir == DIR_RIGHT)
			{
				g_Bullet[i].pos += g_Bullet[i].move;
				g_Bullet[i].texNo = 0;
			}
			else if (g_Bullet[i].dir == DIR_LEFT)
			{
				g_Bullet[i].pos -= g_Bullet[i].move;
				g_Bullet[i].texNo = 1;
			}

			// 画面外まで進みの判定、画面外でも進む、でも地図の外へ出るとなくなる。
			if (g_Bullet[i].pos.x < map->pos.x + (-g_Bullet[i].w/2))				// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.x > map->pos.x + (STAGE_WIDTH+ g_Bullet[i].w/2))	// 自分の大きさを考慮して画面外か判定している
			{
				g_Bullet[i].use = false;
			}

			// 当たり判定処理
			{
				ENEMY *enemy = GetEnemy();

				// エネミーの数分当たり判定を行う
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// 生きてるエネミーと当たり判定をする
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);
						// 当たっている？
						if (ans == TRUE)
						{
							// 当たった時の処理
							enemy[j].use = FALSE;
							AddScore(1000);
							PlaySound(SOUND_LABEL_SE_hit000);

							// ↓この処理を入れないと貫通弾になってしまう
							g_Bullet[i].use = FALSE;

							// bulletのexplosion処理
							D3DXVECTOR3 pos = enemy[j].pos;
							SetBullet_Explosion(pos);		// １発目

							break;
						}
					}
				}
			}

			bulletCount++;
		}
//#ifdef _DEBUG
//		// デバッグ表示
//		PrintDebugProc("Bullet No%d  X:%f Y:%f\n", i, g_Bullet[i].pos.x, g_Bullet[i].pos.y);
//#endif
	}


}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
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

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet[i].pos.x - map->pos.x;	// バレットの表示位置X
			float py = g_Bullet[i].pos.y - map->pos.y;	// バレットの表示位置Y
			float pw = g_Bullet[i].w;		// バレットの表示幅
			float ph = g_Bullet[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet(D3DXVECTOR3 pos,int dir)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet[i].pos = pos;			// 座標をセット
			g_Bullet[i].dir = dir;
			PlaySound(SOUND_LABEL_SE_laser000);
			return;							// 1発セットしたので終了する
		}
	}
}

