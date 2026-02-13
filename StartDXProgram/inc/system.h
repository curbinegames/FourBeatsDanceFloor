#pragma once

#include <sancur.h>

/**
 * 初期の案では画像で表示する予定だった。思い出で残してる。
 * 画像処理の方はメンテしてないので動作未定義。
 */
#define FBDF_DANCER_MAT_TYPE 1 /* 0:画像, 1:3D */

#define FBDF_DANCER_UNIOW   1
#define FBDF_DANCER_NEIDA   2
#define FBDF_DANCER_TRIMBA  3
#define FBDF_DANCER_QUATTRO 4

#define FBDF_PLAYSTYLE_ASSIST_PLUS 0
#define FBDF_PLAYSTYLE_ASSIST      1
#define FBDF_PLAYSTYLE_NORMAL      2
#define FBDF_PLAYSTYLE_BLANC       3
#define FBDF_PLAYSTYLE_BLANC_PLUS  4

#define LANGUAGE_CHOOSE(jp, en) ((game_option.language == 0) ? (jp) : (en))

typedef struct FBDF_game_option_s {
	 int empty_val  = 0; /* nullptrの代わり */
	uint chara      = FBDF_DANCER_UNIOW;
	uint play_style = FBDF_PLAYSTYLE_NORMAL;
	bool auto_en    = false;
	uint lane_speed = 10;
	 int note_offset_timing = 0;
	 int note_offset_draw   = 0;
	bool hit_effect_en = true;
	bool judge_draw_en = true;
	bool fast_slow_en  = false;
	bool chain_draw_en = true;
	uint language      = 0; /* 0=日本語, 1=英語 */
} FBDF_game_option_st;

typedef struct FBDF_score_bar_s {
	double bar_70 = 70.0;
	double bar_90 = 90.0;
	double bar_96 = 96.0;
	double bar_98 = 98.5;
	double bar_99 = 99.1;
} FBDF_score_bar_st;

extern FBDF_game_option_st game_option; /* ゲームオプション、option.cpp以外で変更されない、他の場所では読み込み専用 */

/* プレイ用、右向き */
extern void FBDF_DrawScoreBarHori(const FBDF_score_bar_st &score_bar, int x1, int y1, int x2, int y2);
extern void FBDF_DrawScoreBarVert(const FBDF_score_bar_st &score_bar, int pos_left, int pos_up, int pos_right, int pos_down);
extern void FBDF_DrawScoreBarVertQuad(const FBDF_score_bar_st &score_bar1, const FBDF_score_bar_st &score_bar2,
    int pos_left, int pos_up, int pos_right, int pos_down
);
