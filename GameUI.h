//=============================================================================
//
// ゲーム中のUI処理 [GameUI.cpp]
// Author : GP11B132 13 黄源
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum
{
	TEXTURE_UIBOX,
	TEXTURE_WEAPON,
	TEXTURE_WEAPOMHIKARI,
	TEXTURE_HP_1,
	TEXTURE_HP_0,
	TEXTURE_MP_1,
	TEXTURE_MP_0,
	TEXTURE_MAX,
};

enum 
{
	BOX_WEAPON,
	BOX_HP,
	BOX_MP,
	BOX_MAX,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct GameUI_Box
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号

};

struct GameUI_Weapon
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号

};


struct GameUI_Hikari
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	D3DXVECTOR3		rot;			// ポリゴンの回転量
	float			w, h;			// 幅と高さ
	float			countAnim;		// アニメーションカウント
	int				patternAnim;	// アニメーションパターンナンバー
	int				texNo;			// テクスチャ番号
};

struct GameUI_HP
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
	int				isUse;			// HPは利用かどうか
};

struct GameUI_MP
{
	D3DXVECTOR3		pos;			// ポリゴンの座標
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号
	int				isUse;			// HPは利用かどうか
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGameUI(void);
void UninitGameUI(void);
void UpdateGameUI(void);
void DrawGameUI(void);

//BG *GetBG(void);
