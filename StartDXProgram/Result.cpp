
#include <DxLib.h>

#include <dxcur.h>

#include <main.h>
#include <system.h>
#include <save.h>
#include <Play.h>

#include <Result.h>

#define VIEW_MARGIN 10
#define SCORE_GRAPH_X_SIZE 400
#define SCORE_GRAPH_Y_SIZE (SCORE_GRAPH_X_SIZE * 5 / 8)

#define SCORE_FINAL_BAR_XSIZE 15

/**
 * @brief スコアバーの推移を書く
 * @param[in] src スコアバーの推移データ
 * @param[in] rank スコアランク
 * @return なし
 */
void FBDF_result_DrawScoreGraph(const FBDF_score_bar_st *src, const char *rank) {
	int all_left  = VIEW_MARGIN;
	int all_up    = VIEW_MARGIN;
	int all_right = VIEW_MARGIN + SCORE_GRAPH_X_SIZE;
	int all_down  = VIEW_MARGIN + SCORE_GRAPH_Y_SIZE;

	int all_y_middle = (all_down + all_up) / 2;

	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		FBDF_DrawScoreBarVertQuad(src[i], src[i + 1], xpos1, all_up, xpos2, all_down);
	}

	DrawLine(all_left, all_y_middle, all_right, all_y_middle, COLOR_RED);
	DrawBox(all_left, all_up, all_right, all_down, COLOR_WHITE, FALSE);
	DrawFormatString(2 * VIEW_MARGIN, 2 * VIEW_MARGIN, COLOR_WHITE, _T("scoreRank: %s"), rank);
	return;
}

/**
 * @brief 最終精度のバーを書く
 * @param[in] acc 最終精度
 * @return なし
 */
static void FBDF_result_DrawFinalBar(double acc) {
	int all_left  = 2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE;
	int all_up    =     VIEW_MARGIN;
	int all_right = 2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE;
	int all_down  =     VIEW_MARGIN + SCORE_GRAPH_Y_SIZE;

	int all_y_middle = (all_down + all_up) / 2;

	FBDF_score_bar_st acc_buf;
	acc_buf.bar_70 = acc;
	acc_buf.bar_90 = acc;
	acc_buf.bar_96 = acc;
	acc_buf.bar_98 = acc;
	acc_buf.bar_99 = acc;

	FBDF_DrawScoreBarVert(acc_buf, all_left, all_up, all_right, all_down);

	DrawLine(all_left, all_y_middle, all_right, all_y_middle, COLOR_RED);
	DrawBox(
		2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE                        , VIEW_MARGIN                     ,
		2 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN + SCORE_GRAPH_Y_SIZE,
		COLOR_WHITE, FALSE
	);
}

/**
 * @brief リザルト画面のベース
 * @param[in] data プレイデータ
 * @return view_num_t 次の画面
 */
static view_num_t FBDF_Result_View(const FBDF_result_data_t *data) {
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

		/* キャラ描画、イラスト描いてない */

		ScreenFlip(); // 作画エリアここまで
		if (GetWindowUserCloseFlag(TRUE)) { break; } // 閉じるボタンが押された
		WaitTimer(10); // ループウェイト
	}
	return VIEW_EXIT;
}

/**
 * @brief クリアタイプを計算する
 * @param[in] data プレイデータ
 * @return FBDF_clear_type_et クリアタイプ
 */
static FBDF_clear_type_et FBDF_Resule_JudgeClearType(const FBDF_result_data_t *data) {
	if (data->acc < 70.0) { return FBDF_CLEAR_TYPE_FAILED;    }
	if (30 < data->drop)  { return FBDF_CLEAR_TYPE_CLEARED;   }
	if ( 5 < data->drop)  { return FBDF_CLEAR_TYPE_CAKEWALK;  }
	if ( 0 < data->drop)  { return FBDF_CLEAR_TYPE_MISSLESS;  }
	if ( 0 < data->save)  { return FBDF_CLEAR_TYPE_FULLCOMBO; }
	return FBDF_CLEAR_TYPE_PERFECT;
}

/**
 * @brief スコアを保存する
 * @param[in] data プレイデータ
 * @return bool true=成功, false=失敗
 */
static void FBDF_Result_SaveMusicScore(const FBDF_result_data_t *data) {
	FBDF_file_music_score_st this_time_score;
	this_time_score.acc        = data->acc;
	this_time_score.clear_type = FBDF_Resule_JudgeClearType(data);
	this_time_score.score      = data->score;
	FBDF_Save_UpdateScoreOneDif(&this_time_score, data->folder_name.c_str(), data->dif_type);
}

/**
 * @brief リザルト画面の準備
 * @param[in] data プレイ画面から渡されたデータ
 * @return view_num_t 次の画面
 */
view_num_t FirstResultView(const FBDF_result_data_t *data) {
	FBDF_Result_SaveMusicScore(data);
	return FBDF_Result_View(data);
}
