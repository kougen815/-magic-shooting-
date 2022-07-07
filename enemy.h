//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : GP11B132 13 黄源
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_SNAIL_MAX		(1)						// エネミーのsnailのMax数
#define	ENEMY_MAX			(ENEMY_SNAIL_MAX)		// エネミーのMax数

#define ENEMY_SNAIL_HP_MAX	(1)

enum
{
	DIR_ENEMY_LEFT  =  1,
	DIR_ENEMY_RIGHT = -1,
};

enum
{
	ENEMY_SNAIL_TEXTURE_SHOW,
	ENEMY_SNAIL_TEXTURE_WALK,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	D3DXVECTOR3		pos;				// ポリゴンの座標
	D3DXVECTOR3		rot;				// ポリゴンの回転量
	BOOL			use;				// true:使っている  false:未使用
	float			w, h;				// 幅と高さ
	int				countAnim;			// アニメーションカウント
	int				patternAnim;		// アニメーションパターンナンバー
	int				texNo;				// テクスチャ番号
	D3DXVECTOR3		move;				// 移動速度
	BOOL			isShow;				// エネミーの登場アニメ
	BOOL			isWalk;				// エネミーの移動アニメ
	BOOL			isDead;				// エネミーの死亡アニメ

	D3DXVECTOR3		gravity;			// 重力
	D3DXVECTOR3		vel;				// 飛びの高度
	int				dir;				// 向き（0:右 1:左）

	int				hp;					// 

	int				mapChipListNum;		// 当たったmapchipのtexnoを記録する
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);



