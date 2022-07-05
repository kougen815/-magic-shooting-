//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP11B132 13 黄源
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLAYER_MAX			(1)		// プレイヤーのMax人数

#define PLAYER_HP_MAX		(5)		// HPのMax

#define PLAYER_MP_MAX		(5)		// MPのMax

#define	PLAYER_OFFSET_CNT	(8)		// 8分身

enum
{
	DIR_RIGHT = 1,
	DIR_LEFT = -1,
};


enum 
{
	PLAYER_TEXTURE_IDLE,
	PLAYER_TEXTURE_RUN,
	PLAYER_TEXTURE_TAKEDAMAGE,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

struct PLAYER
{
	D3DXVECTOR3		pos;				// ポリゴンの座標
	D3DXVECTOR3		rot;				// ポリゴンの回転量
	BOOL			use;				// true:使っている  false:未使用
	float			w, h;				// 幅と高さ
	int				countAnim;			// アニメーションカウント
	int				patternAnim;		// アニメーションパターンナンバー
	int				texNo;				// テクスチャ番号
	D3DXVECTOR3		move;				// 移動速度
	int				aniCycle_damage;	// ダメージを受けるアニメの回数



	int				dir;				// 向き（0:右 1:左）
	BOOL			keyOn;				// キーボードを入力かどうか

	D3DXVECTOR3		gravity;			// 重力
	D3DXVECTOR3		vel;				// 飛びの高度
	BOOL			isFlying;			// 飛びかどうか？

	int				hp;
	int				mp;
	BOOL			isTakeDamage;		// ダメージを受けるかどうか
	BOOL			isAttack_bullet;	// 攻撃(バレット発射)かどうか
	BOOL			isRush;				// ラッシュかどうか

	BOOL			canBGmove;			// BG移動かどうかの判定

	BOOL			noDamageMod;		// 無敵状態(硬直状態)



	D3DXVECTOR3		offset[PLAYER_OFFSET_CNT];		// 残像ポリゴンの座標

};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);



