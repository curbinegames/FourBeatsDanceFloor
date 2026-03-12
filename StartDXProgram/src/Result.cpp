
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

class FBDF_result_scorerank_pic_c {
private:
	dxcur_pic_c pic;

public:
	FBDF_result_scorerank_pic_c(void) = delete;
	FBDF_result_scorerank_pic_c(double acc) {
		if (acc >= 100) {
			this->pic.reload(_T("pic/result/scorerank_P.png"));
		}
		else if (acc >= 99.5) {
			this->pic.reload(_T("pic/result/scorerank_XP.png"));
		}
		else if (acc >= 99.0) {
			this->pic.reload(_T("pic/result/scorerank_X.png"));
		}
		else if (acc >= 98.0) {
			this->pic.reload(_T("pic/result/scorerank_SP.png"));
		}
		else if (acc >= 97.0) {
			this->pic.reload(_T("pic/result/scorerank_S.png"));
		}
		else if (acc >= 95.0) {
			this->pic.reload(_T("pic/result/scorerank_AP.png"));
		}
		else if (acc >= 90.0) {
			this->pic.reload(_T("pic/result/scorerank_A.png"));
		}
		else if (acc >= 80.0) {
			this->pic.reload(_T("pic/result/scorerank_B.png"));
		}
		else if (acc >= 70.0) {
			this->pic.reload(_T("pic/result/scorerank_C.png"));
		}
		else if (acc >= 60.0) {
			this->pic.reload(_T("pic/result/scorerank_D.png"));
		}
		else {
			this->pic.reload(_T("pic/result/scorerank_F.png"));
		}
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

class FBDF_result_chara_pic_c {
private:
	dxcur_pic_c pic;

public:
	FBDF_result_chara_pic_c(void) {
		switch (game_option.chara) {
		case FBDF_DANCER_UNIOW:
			this->pic.reload(_T("pic/result/uniow.png"));
			break;
		case FBDF_DANCER_NEIDA:
			this->pic.reload(_T("pic/result/neida.png"));
			break;
		case FBDF_DANCER_TRIMBA:
			this->pic.reload(_T("pic/result/trimba.png"));
			break;
		case FBDF_DANCER_QUATTRO:
			this->pic.reload(_T("pic/result/quattro.png"));
			break;
		default:
			this->pic.reload(_T("pic/result/uniow.png"));
			break;
		}
	}

	void draw(void) const {
		int drawLeft = WINDOW_SIZE_X / 2;
		int drawUp   = WINDOW_SIZE_Y - drawLeft;
		DrawExtendGraph(drawLeft, drawUp, WINDOW_SIZE_X, WINDOW_SIZE_Y, this->pic.handle(), TRUE);
	}
};

/**
 * @brief スコアバーの推移を書く
 * @param[in] src スコアバーの推移データ
 * @param[in] rank スコアランク
 * @return なし
 */
static void FBDF_Result_DrawScoreGraph(const FBDF_score_bar_st *src, DxPic_t rank_pic) {
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
	DrawGraph(2 * VIEW_MARGIN, 2 * VIEW_MARGIN, rank_pic, TRUE);
	return;
}

/**
 * @brief 最終精度のバーを書く
 * @param[in] acc 最終精度
 * @return なし
 */
static void FBDF_Result_DrawFinalBar(double acc) {
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
static view_num_t FBDF_Result_View(const FBDF_result_data_t &data) {
	int keybox[1] = { KEY_INPUT_RETURN };

	dxcur_pic_c back(_T("pic/cutinFulldark.png"));
	FBDF_result_scorerank_pic_c scorerank_pic(data.acc);
	FBDF_result_chara_pic_c chara_pic;

	while (!GetWindowUserCloseFlag() && (GetKeyPushOnce(true) != KEY_INPUT_RETURN)) {
		ClearDrawScreen(); // 作画エリアここから
		DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, back.handle(), TRUE);

		/* スコアグラフ */
		FBDF_Result_DrawScoreGraph(data.score_graph, scorerank_pic.handle());

		/* スコアグラフの横 */
		FBDF_Result_DrawFinalBar(data.acc);
		DrawFormatString(3 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN     , COLOR_WHITE, _T("%s / %s")    , data.music_name.c_str(), data.artist_name.c_str());
		DrawFormatString(3 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN + 20, COLOR_WHITE, _T("level: %.2f"), data.level);

		/* スコアグラフの下 */
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 0, COLOR_WHITE, _T("score: %7d")   , data.score);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 1, COLOR_WHITE, _T("  acc: %6.2f") , data.acc);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 2, COLOR_WHITE, _T("crit: %4d")    , data.crit);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 3, COLOR_WHITE, _T(" hit: %4d")    , data.hit);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 4, COLOR_WHITE, _T("save: %4d")    , data.save);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 5, COLOR_WHITE, _T("drop: %4d")    , data.drop);
		DrawFormatString(VIEW_MARGIN, 2 * VIEW_MARGIN + SCORE_GRAPH_Y_SIZE + 20 * 6, COLOR_WHITE, _T("ave: %+.2f")   , data.gap_ave);

		chara_pic.draw();

		ScreenFlip(); // 作画エリアここまで
		WaitTimer(10); // ループウェイト
	}

	if (GetWindowUserCloseFlag()) { return VIEW_EXIT; } /* 閉じるボタンが押された */
	return VIEW_SELECT;
}

/**
 * @brief クリアタイプを計算する
 * @param[in] data プレイデータ
 * @return FBDF_clear_type_et クリアタイプ
 */
static FBDF_clear_type_et FBDF_Result_JudgeClearType(const FBDF_result_data_t &data) {
	if (data.acc < 70.0)             { return FBDF_CLEAR_TYPE_FAILED;    } /* acc70%未満 */
	if (game_option.play_style <= FBDF_PLAYSTYLE_ASSIST)
	                                 { return FBDF_CLEAR_TYPE_ASSIST;    } /* acc70%以上 & アシストあり */
	if (30 < data.drop)              { return FBDF_CLEAR_TYPE_CLEARED;   } /* acc70%以上 & アシストなし & (30 < drop数) */
	if ( 5 < data.drop)              { return FBDF_CLEAR_TYPE_CAKEWALK;  } /* acc70%以上 & アシストなし & ( 5 < drop数 <= 30) */
	if ( 0 < data.drop)              { return FBDF_CLEAR_TYPE_MISSLESS;  } /* acc70%以上 & アシストなし & ( 0 < drop数 <=  5) */
	if ( 0 < data.save ||
		game_option.play_style <  FBDF_PLAYSTYLE_BLANC_PLUS)
		                             { return FBDF_CLEAR_TYPE_FULLCOMBO; } /* acc70%以上 & アシストなし & drop数=0 & (save数1以上 or ブラン+モードではない)) */
	return FBDF_CLEAR_TYPE_PERFECT; /* acc70%以上 & アシストなし & drop数=0 & save数=0 & ブラン+モード */
}

/**
 * @brief スコアを保存する
 * @param[in] data プレイデータ
 * @return bool true=成功, false=失敗
 */
static bool FBDF_Result_SaveMusicScore(const FBDF_result_data_t &data) {
	FBDF_file_music_score_st this_time_score;
	this_time_score.acc        = data.acc;
	this_time_score.clear_type = FBDF_Result_JudgeClearType(data);
	this_time_score.score      = data.score;
	return FBDF_Save_UpdateScoreOneDif(this_time_score, data.folder_name.c_str(), data.dif_type);
}

/**
 * @brief リザルト画面の準備
 * @param[in] data プレイ画面から渡されたデータ
 * @return view_num_t 次の画面
 */
view_num_t FirstResultView(const FBDF_result_data_t &data) {
	if (FBDF_Result_SaveMusicScore(data) == false) {
		std::string msg = data.music_name;
		msg += ": スコアの保存に失敗しました。";
		FBDF_LOG_ALERT(msg.c_str());
	}
	return FBDF_Result_View(data);
}
