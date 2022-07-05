//=============================================================================
//
// バレットの爆発処理 [bullet_explosion.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "bullet.h"
#include "bullet_explosion.h"
#include "sprite.h"
#include "sound.h"
//#include "bg.h"
#include "map.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(1)		// テクスチャの数

// bullet_explosion
#define TEXTURE_WIDTH_EXPLOSION		(150)
#define TEXTURE_HEIGHT_EXPLOSION	(150)

#define TEXTURE_PATTERN_DIVIDE_X_EX	(4)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y_EX	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM_EX			(TEXTURE_PATTERN_DIVIDE_X_EX * TEXTURE_PATTERN_DIVIDE_Y_EX)	// アニメーションパターン数
#define ANIM_WAIT_EX				(2)		// アニメーションの切り替わるWait値


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/bullet/fireball_explosion.png",
};

static BOOL	  g_Load = FALSE;		// 初期化を行ったかのフラグ


static BULLET_EXPLOSION g_Bullet_Explosion[BULLET_MAX];


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet_Explosion(void)
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


	// バレットの爆発構造体の初期化
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet_Explosion[i].use = FALSE;
		g_Bullet_Explosion[i].w = TEXTURE_WIDTH_EXPLOSION;
		g_Bullet_Explosion[i].h = TEXTURE_HEIGHT_EXPLOSION;
		g_Bullet_Explosion[i].pos = D3DXVECTOR3(300.0f, 300.0f, 0.0f);
		g_Bullet_Explosion[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet_Explosion[i].texNo = 0;

		g_Bullet_Explosion[i].countAnim = 0;
		g_Bullet_Explosion[i].patternAnim = 0;

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet_Explosion(void)
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
void UpdateBullet_Explosion(void)
{
	MAP *map = GetMap();

	

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet_Explosion[i].use == TRUE)
		{
			// アニメーション  
			g_Bullet_Explosion[i].countAnim++;
			if ((g_Bullet_Explosion[i].countAnim % ANIM_WAIT_EX) == 0)
			{
				// パターンの切り替え
				g_Bullet_Explosion[i].patternAnim = (g_Bullet_Explosion[i].patternAnim + 1) % ANIM_PATTERN_NUM_EX;

			}
			if (g_Bullet_Explosion[i].patternAnim == ANIM_PATTERN_NUM_EX - 1)
			{
				g_Bullet_Explosion[i].use = FALSE;
				g_Bullet_Explosion[i].patternAnim = 0;
				g_Bullet_Explosion[i].countAnim = 0;
			}
			

		}
//#ifdef _DEBUG
//		// デバッグ表示
//		PrintDebugProc("Bullet_Ex No%d  X:%f Y:%f\n use:%d", i, g_Bullet_Explosion[i].pos.x, g_Bullet_Explosion[i].pos.y,g_Bullet_Explosion[i].use);
//#endif
	}




}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet_Explosion(void)
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
		if (g_Bullet_Explosion[i].use == TRUE)		// このバレットが使われている？
		{									// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet_Explosion[i].texNo]);

			//バレットの位置やテクスチャー座標を反映
			float px = g_Bullet_Explosion[i].pos.x - map->pos.x;	// バレットの表示位置X
			float py = g_Bullet_Explosion[i].pos.y - map->pos.y;	// バレットの表示位置Y
			float pw = g_Bullet_Explosion[i].w;		// バレットの表示幅
			float ph = g_Bullet_Explosion[i].h;		// バレットの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X_EX;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y_EX;	// テクスチャの高さ
			float tx = (float)(g_Bullet_Explosion[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X_EX) * tw;	// テクスチャの左上X座標
			float ty = (float)(g_Bullet_Explosion[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X_EX) * th;	// テクスチャの左上Y座標


			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet_Explosion[i].rot.z);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// バレット構造体の先頭アドレスを取得
//=============================================================================
//BULLET *GetBullet(void)
//{
//	return &g_Bullet[0];
//}


//=============================================================================
// バレットの発射設定
//=============================================================================
void SetBullet_Explosion(D3DXVECTOR3 pos)
{
	// もし未使用の弾が無かったら発射しない( =これ以上撃てないって事 )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet_Explosion[i].use == FALSE)		// 未使用状態のバレットを見つける
		{
			g_Bullet_Explosion[i].use = TRUE;			// 使用状態へ変更する
			g_Bullet_Explosion[i].pos = pos;			// 座標をセット
			//PlaySound(SOUND_LABEL_SE_shot000);
			return;							// 1発セットしたので終了する
		}
	}
}

