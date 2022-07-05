//=============================================================================
//
// map画面処理 [map.h]
// Author : GP11B132 13 黄源
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STAGE_WIDTH						(2560)						// ステージの横幅
#define STAGE_HEIGHT					(1280)						// ステージの縦幅



//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct MAP
{
	D3DXVECTOR3		pos;		// ポリゴンの座標
	float			w, h;		// 幅と高さ
	int				texNo;		// 使用しているテクスチャ番号
};


struct MAPCHIP
{
	D3DXVECTOR3		pos;			// ポリゴンの座標(マップチップ計算便利のために、左上の座標）
	D3DXVECTOR3		pos_center;		// ポリゴンの座標(当たり判定のために、中心の座標も求める)
	float			w, h;			// 幅と高さ
	int				texNo;			// 使用しているテクスチャ番号

	BOOL			isTouch;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMap(void);
void UninitMap(void);
void UpdateMap(void);
void DrawMap(void);

MAP *GetMap(void);
MAPCHIP *GetMapChip(void);
BOOL CheckMapList(PLAYER player);
BOOL CheckMapList2(ENEMY enemy, float w, float h);

int MapChipListNum(ENEMY enemy, float w, float h);