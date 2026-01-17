
#include "DxLib.h"

#include "keycur.h"
#include <dxcur.h>

#include "main.h"
#include "Result.h"
#include <Play.h>

#define VIEW_MARGIN 10
#define SCORE_GRAPH_X_SIZE 400
#define SCORE_GRAPH_Y_SIZE (SCORE_GRAPH_X_SIZE * 5 / 8)

#define SCORE_FINAL_BAR_XSIZE 15

void FBDF_result_DrawScoreGraph(const FBDF_result_score_graph_t *src, const char *rank) {
	int all_left  = VIEW_MARGIN;
	int all_up    = VIEW_MARGIN;
	int all_right = VIEW_MARGIN + SCORE_GRAPH_X_SIZE;
	int all_down  = VIEW_MARGIN + SCORE_GRAPH_Y_SIZE;

	int all_y_middle = (all_down + all_up) / 2;

	/* 70-60 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(70.0, all_y_middle, 60.0, all_up, src[i    ].gr_70), all_down);
		int ypos2 = betweens(all_up, lins(70.0, all_y_middle, 60.0, all_up, src[i + 1].gr_70), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_RED60, TRUE
		);
	}

	/* 60- 0 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(60.0, all_y_middle, 0.0, all_up, src[i    ].gr_70), all_down);
		int ypos2 = betweens(all_up, lins(60.0, all_y_middle, 0.0, all_up, src[i + 1].gr_70), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_RED0, TRUE
		);
	}

	/* 70-80 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(70.0, all_y_middle,80.0, all_up, src[i    ].gr_70), all_down);
		int ypos2 = betweens(all_up, lins(70.0, all_y_middle,80.0, all_up, src[i + 1].gr_70), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_YELLOW70, TRUE
		);
	}

	/* 80-90 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(80.0, all_y_middle, 90.0, all_up, src[i    ].gr_70), all_down);
		int ypos2 = betweens(all_up, lins(80.0, all_y_middle, 90.0, all_up, src[i + 1].gr_70), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_YELLOW80, TRUE
		);
	}

	/* 90-95 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(90.0, all_y_middle, 95.0, all_up, src[i    ].gr_90), all_down);
		int ypos2 = betweens(all_up, lins(90.0, all_y_middle, 95.0, all_up, src[i + 1].gr_90), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_GREEN90, TRUE
		);
	}

	/* 95-97 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(95.0, all_y_middle, 97.0, all_up, src[i    ].gr_90), all_down);
		int ypos2 = betweens(all_up, lins(95.0, all_y_middle, 97.0, all_up, src[i + 1].gr_90), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_GREEN95, TRUE
		);
	}

	/* 97-98 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(97.0, all_y_middle, 98.0, all_up, src[i    ].gr_96), all_down);
		int ypos2 = betweens(all_up, lins(97.0, all_y_middle, 98.0, all_up, src[i + 1].gr_96), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_BLUE97, TRUE
		);
	}

	/* 98-99 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(98.0, all_y_middle, 99.0, all_up, src[i    ].gr_96), all_down);
		int ypos2 = betweens(all_up, lins(98.0, all_y_middle, 99.0, all_up, src[i + 1].gr_96), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_BLUE98, TRUE
		);
	}

	/* 99-100 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(99.0, all_y_middle, 100.0, all_up, src[i    ].gr_98), all_down);
		int ypos2 = betweens(all_up, lins(99.0, all_y_middle, 100.0, all_up, src[i + 1].gr_98), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_PURPLE99, TRUE
		);
	}

	/* 99.5-100 */
	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		int ypos1 = betweens(all_up, lins(99.5, all_y_middle, 100.0, all_up, src[i    ].gr_99), all_down);
		int ypos2 = betweens(all_up, lins(99.5, all_y_middle, 100.0, all_up, src[i + 1].gr_99), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_PURPLE100, TRUE
		);
	}

	DrawLine(all_left, all_y_middle, all_right, all_y_middle, COLOR_RED);
	DrawBox(all_left, all_up, all_right, all_down, COLOR_WHITE, FALSE);
	DrawFormatString(2 * VIEW_MARGIN, 2 * VIEW_MARGIN, COLOR_WHITE, _T("scoreRank: %s"), rank);
	return;
}

void FBDF_result_DrawFinalBar(double acc) {
	int all_left  = 2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE;
	int all_up    =     VIEW_MARGIN;
	int all_right = 2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE;
	int all_down  =     VIEW_MARGIN + SCORE_GRAPH_Y_SIZE;

	int all_y_middle = (all_down + all_up) / 2;

	/* 70-60 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(70.0, all_y_middle, 60.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(70.0, all_y_middle, 60.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_RED60, TRUE
		);
	}

	/* 60- 0 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(60.0, all_y_middle, 0.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(60.0, all_y_middle, 0.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_RED0, TRUE
		);
	}

	/* 70-80 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(70.0, all_y_middle,80.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(70.0, all_y_middle,80.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_YELLOW70, TRUE
		);
	}

	/* 80-90 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(80.0, all_y_middle, 90.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(80.0, all_y_middle, 90.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_YELLOW80, TRUE
		);
	}

	/* 90-95 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(90.0, all_y_middle, 95.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(90.0, all_y_middle, 95.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_GREEN90, TRUE
		);
	}

	/* 95-97 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(95.0, all_y_middle, 97.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(95.0, all_y_middle, 97.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_GREEN95, TRUE
		);
	}

	/* 97-98 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(97.0, all_y_middle, 98.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(97.0, all_y_middle, 98.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_BLUE97, TRUE
		);
	}

	/* 98-99 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(98.0, all_y_middle, 99.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(98.0, all_y_middle, 99.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_BLUE98, TRUE
		);
	}

	/* 99-100 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(99.0, all_y_middle, 100.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(99.0, all_y_middle, 100.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_PURPLE99, TRUE
		);
	}

	/* 99.5-100 */ {
		int xpos1 = all_left;
		int xpos2 = all_right;
		int ypos1 = betweens(all_up, lins(99.5, all_y_middle, 100.0, all_up, acc), all_down);
		int ypos2 = betweens(all_up, lins(99.5, all_y_middle, 100.0, all_up, acc), all_down);
		DrawQuadrangle(
			xpos1, ypos1,
			xpos2, ypos2,
			xpos2, all_down,
			xpos1, all_down,
			FBDF_PLAY_SCOREBAR_COLOR_PURPLE100, TRUE
		);
	}

	DrawLine(all_left, all_y_middle, all_right, all_y_middle, COLOR_RED);
	DrawBox(
		2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE                        , VIEW_MARGIN                     ,
		2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN + SCORE_GRAPH_Y_SIZE,
		COLOR_WHITE, FALSE
	);
}

/* 返り値: 次のシーンの番号 */
view_num_t FirstResultView(const FBDF_result_data_t *data) {
	int keybox[1] = { KEY_INPUT_RETURN };
	int hitkey = 0;

	dxcur_pic_c back(_T("pic/cutinFulldark.png"));

	std::string score_rank_char = "D";

#if 1 /* ランク実装 */
	if (data->acc >= 100) {
		score_rank_char = "P";
	}
	else if (data->acc >= 99.5) {
		score_rank_char = "X+";
	}
	else if (data->acc >= 99.0) {
		score_rank_char = "X";
	}
	else if (data->acc >= 98.0) {
		score_rank_char = "S+";
	}
	else if (data->acc >= 97.0) {
		score_rank_char = "S";
	}
	else if (data->acc >= 95.0) {
		score_rank_char = "A+";
	}
	else if (data->acc >= 90.0) {
		score_rank_char = "A";
	}
	else if (data->acc >= 80.0) {
		score_rank_char = "B";
	}
	else if (data->acc >= 70.0) {
		score_rank_char = "C";
	}
	else if (data->acc >= 60.0) {
		score_rank_char = "D";
	}
	else {
		score_rank_char = "F";
	}
#endif /* ランク実装 */

	while (1) {
		switch (keycur(keybox, 1)) {
		case KEY_INPUT_RETURN:
			return VIEW_SELECT;
			break;
		default:
			break;
		}
		ClearDrawScreen(); // 作画エリアここから
		DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, back.handle(), TRUE);

		/* スコアグラフ */
		FBDF_result_DrawScoreGraph(data->score_graph, score_rank_char.c_str());

		/* スコアグラフの横 */
		FBDF_result_DrawFinalBar(data->acc);
		DrawFormatString(3 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN     , COLOR_WHITE, _T("%s / %s")    , data->name.c_str(), data->artist.c_str());
		DrawFormatString(3 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN + 20, COLOR_WHITE, _T("level: %.2f"), data->level);

		/* スコアグラフの下 */
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 0, COLOR_WHITE, _T("score: %7d")   , data->score);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 1, COLOR_WHITE, _T("acc: %6.2f")   , data->acc);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 2, COLOR_WHITE, _T("crit: %4d")    , data->crit);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 3, COLOR_WHITE, _T(" hit: %4d")    , data->hit);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 4, COLOR_WHITE, _T("save: %4d")    , data->save);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 5, COLOR_WHITE, _T("drop: %4d")    , data->drop);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 6, COLOR_WHITE, _T("ave: %+.2f")   , data->gap_ave);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 7, COLOR_WHITE, _T("chara: %d")    , data->charaNo);

		ScreenFlip(); // 作画エリアここまで
		if (GetWindowUserCloseFlag(TRUE)) { break; } // 閉じるボタンが押された
		WaitTimer(10); // ループウェイト
	}
	return VIEW_EXIT;
}
