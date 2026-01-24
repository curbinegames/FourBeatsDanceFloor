#pragma once

typedef struct FBDF_score_bar_s {
	double bar_70 = 70.0;
	double bar_90 = 90.0;
	double bar_96 = 96.0;
	double bar_98 = 98.5;
	double bar_99 = 99.1;
} FBDF_score_bar_st;

/* プレイ用、右向き */
extern void FBDF_DrawScoreBarHori(const FBDF_score_bar_st &score_bar, int x1, int y1, int x2, int y2);
extern void FBDF_DrawScoreBarVert(const FBDF_score_bar_st &score_bar, int pos_left, int pos_up, int pos_right, int pos_down);
extern void FBDF_DrawScoreBarVertQuad(const FBDF_score_bar_st &score_bar1, const FBDF_score_bar_st &score_bar2,
    int pos_left, int pos_up, int pos_right, int pos_down
);
