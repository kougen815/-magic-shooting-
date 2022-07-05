//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "score.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(5)				// テクスチャの数

#define TEXTURE_WIDTH_EARTH			(350)	// 地球サイズ
#define TEXTURE_HEIGHT_EARTH		(350)

#define TEXTURE_WIDTH_BUTTON		(250)	//	ボタンサイズ
#define TEXTURE_HEIGHT_BUTTON		(70)
#define BUTTON_DISTANCE				(60)

#define TEXTURE_WIDTH_TEXT			(70)
#define TEXTURE_HEIGHT_TEXT			(70)
#define TEXT_DISTANCE				(25)

#define TEXTURE_WIDTH_CONFIRM		(50)
#define TEXTURE_HEIGHT_CONFIRM		(50)

#define TEXTURE_PATTERN_DIVIDE_X	(50)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(8)		// アニメーションの切り替わるWait値

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title/background.png",
	"data/TEXTURE/title/earth.png",
	"data/TEXTURE/GameUI/choose.png",
	"data/TEXTURE/title/text_English.png",
	"data/TEXTURE/title/star.png",
};


static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ

static TITLE	g_Title;			// 背景の初期量

static EARTH	g_Earth;			// 地球の初期量

static CHOOSE	g_Choose[3];

static TEXT		g_Text;

static CONFIRM	g_Confirm;

// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
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


	// 変数の初期化
	// backgroundの描画
	g_Title.w     = TEXTURE_WIDTH;
	g_Title.h     = TEXTURE_HEIGHT;
	g_Title.pos   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Title.texNo = 0;

	// 地球の描画の初期化
	g_Earth.w = TEXTURE_WIDTH_EARTH;
	g_Earth.h = TEXTURE_HEIGHT_EARTH;
	g_Earth.pos = D3DXVECTOR3(TEXTURE_WIDTH * 0.5f, TEXTURE_HEIGHT * 0.3f, 0.0f);
	g_Earth.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Earth.texNo = 1;
	g_Earth.countAnim = 0;	//アニメカウント
	g_Earth.patternAnim = 0;//アニメパターン番号

	// 選択ボタンの描画の初期化
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

	// 文字の描画の初期化
	g_Text.w = TEXTURE_WIDTH_TEXT;
	g_Text.h = TEXTURE_HEIGHT_TEXT;
	g_Text.pos = D3DXVECTOR3(g_Choose[0].pos.x, g_Choose[0].pos.y, 0.0f);
	g_Text.texNo = 3;

	// 選択アイコンの描画の初期化
	g_Confirm.w = TEXTURE_WIDTH_CONFIRM;
	g_Confirm.h = TEXTURE_HEIGHT_CONFIRM;
	g_Confirm.pos = D3DXVECTOR3(g_Choose[0].pos.x - 60, g_Choose[0].pos.y + 5, 0.0f);
	g_Confirm.texNo = 4;
	g_Confirm.choose = 0;

	g_Load = TRUE;	// データの初期化を行った
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		// Enter押したら、ステージを切り替える
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
			// セーブデータをロードする
			SetLoadGame(TRUE);
			SetFade(FADE_OUT, MODE_GAME);
		}
		
	}
	// ゲームパッドで入力処理
	else if (IsButtonTriggered(0, BUTTON_START))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}
	else if (IsButtonTriggered(0, BUTTON_B))
	{
		SetFade(FADE_OUT, MODE_GAME);
	}



	// アニメーション  
	g_Earth.countAnim += 1.0f;
	if (g_Earth.countAnim > ANIM_WAIT)
	{
		g_Earth.countAnim = 0.0f;
		// パターンの切り替え
		g_Earth.patternAnim = (g_Earth.patternAnim + 1) % ANIM_PATTERN_NUM;
	}


	// 選択ボタンを移動させる
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
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Title.texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			g_Title.pos.x, g_Title.pos.y, g_Title.w, g_Title.h, 
			0.0f, 0.0f, 15.0f, 10.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 地球を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Earth.texNo]);

		// 地球の位置やテクスチャー座標を反映
		float px = g_Earth.pos.x;	// プレイヤーの表示位置X
		float py = g_Earth.pos.y;	// プレイヤーの表示位置Y
		float pw = g_Earth.w;		// プレイヤーの表示幅
		float ph = g_Earth.h;		// プレイヤーの表示高さ

		// アニメーション用
		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = (float)(g_Earth.patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// テクスチャの左上X座標
		float ty = (float)(g_Earth.patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
			g_Earth.rot.z);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 選択ボタンを描画
	{
		for (int i = 0; i < 3; i++)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Choose[i].texNo]);

			// UIの位置やテクスチャー座標を反映
			float px = g_Choose[i].pos.x;
			float py = g_Choose[i].pos.y;
			float pw = g_Choose[i].w;
			float ph = g_Choose[i].h;

			float tw = 1.0f;
			float th = 1.0f;
			float tx = 1.0f;
			float ty = 1.0f;

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer,
				px, py, pw, ph,
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// 文字(Text)を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Text.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_Text.pos.x;
		float py = g_Text.pos.y;
		float pw = g_Text.w;
		float ph = g_Text.h;

		float tw = 1.0f / 17;
		float th = 1.0f / 12;

		//S
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tw * 16, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE, py, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//A
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, 
			px + TEXT_DISTANCE * 2, py, pw, ph,
			tw * 11, th * 6, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//R
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, 
			px + TEXT_DISTANCE * 3, py, pw, ph,
			tw * 14, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, 
			px + TEXT_DISTANCE * 4, py, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//***************************************************//
		//C
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, g_Choose[1].pos.y, pw, ph,
			tw * 16, th * 6, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//O
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[1].pos.y, pw, ph,
			tw * 8, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//N
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[1].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//I
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 4, g_Choose[1].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//N
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 5, g_Choose[1].pos.y, pw, ph,
			tw * 4, th * 8, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//U
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 6, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//E
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 7, g_Choose[1].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//***************************************************//
		//E
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, g_Choose[2].pos.y, pw, ph,
			tw * 3, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//X
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE, g_Choose[2].pos.y, pw, ph,
			tw * 9, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//I
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 2, g_Choose[2].pos.y, pw, ph,
			tw * 11, th * 7, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		//T
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px + TEXT_DISTANCE * 3, g_Choose[2].pos.y, pw, ph,
			tw * 1, th * 9, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 選択アイコンを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Confirm.texNo]);

		// UIの位置やテクスチャー座標を反映
		float px = g_Confirm.pos.x;
		float py = g_Confirm.pos.y;
		float pw = g_Confirm.w;
		float ph = g_Confirm.h;

		float tw = 1.0f;
		float th = 1.0f;
		float tx = 1.0f;
		float ty = 1.0f;


		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph, 
			tx, ty, tw, th,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

}





