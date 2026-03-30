
#include <DxLib.h>

#include <dxcur.h>
#include <strcur.h>

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
		std::string path = "pic/result/ScoreRank";
		if (acc >= 100) {
			path += "P";
		}
		else if (acc >= 99.5) {
			path += "XP";
		}
		else if (acc >= 99.0) {
			path += "X";
		}
		else if (acc >= 98.0) {
			path += "SP";
		}
		else if (acc >= 97.0) {
			path += "S";
		}
		else if (acc >= 95.0) {
			path += "AP";
		}
		else if (acc >= 90.0) {
			path += "A";
		}
		else if (acc >= 80.0) {
			path += "B";
		}
		else if (acc >= 70.0) {
			path += "C";
		}
		else if (acc >= 60.0) {
			path += "D";
		}
		else {
			path += "F";
		}
		path += ".png";
		this->pic.reload(path.c_str());
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

/**
 * @brief クリアタイプを計算する
 * @param[in] data プレイデータ
 * @return FBDF_clear_type_et クリアタイプ
 */
static FBDF_clear_type_et FBDF_Result_JudgeClearType(const FBDF_result_data_t &data) {
	if (data.acc <= 0.0) { return FBDF_CLEAR_TYPE_NOPLAY;    } /* acc0% */
	if (data.acc < 70.0) { return FBDF_CLEAR_TYPE_FAILED;    } /* acc70%未満 */
	if (game_option.play_style <= FBDF_PLAYSTYLE_ASSIST)
	{ return FBDF_CLEAR_TYPE_ASSIST;    } /* acc70%以上 & アシストあり */
	if (30 < data.drop)  { return FBDF_CLEAR_TYPE_CLEARED;   } /* acc70%以上 & アシストなし & (30 < drop数) */
	if ( 5 < data.drop)  { return FBDF_CLEAR_TYPE_CAKEWALK;  } /* acc70%以上 & アシストなし & ( 5 < drop数 <= 30) */
	if ( 0 < data.drop)  { return FBDF_CLEAR_TYPE_MISSLESS;  } /* acc70%以上 & アシストなし & ( 0 < drop数 <=  5) */
	if ( 0 < data.save)  { return FBDF_CLEAR_TYPE_FULLCOMBO; } /* acc70%以上 & アシストなし & drop数=0 & save数1以上 */
	return FBDF_CLEAR_TYPE_PERFECT; /* acc70%以上 & アシストなし & drop数=0 & save数=0 */
}

class FBDF_result_cleartype_pic_c {
private:
	dxcur_pic_c pic;

public:
	FBDF_result_cleartype_pic_c(void) = delete;
	FBDF_result_cleartype_pic_c(const FBDF_result_data_t &data) {
		std::string path = "pic/result/ClearType";
		switch (FBDF_Result_JudgeClearType(data)) {
		case FBDF_CLEAR_TYPE_NOPLAY:
			path += "NP";
			break;
		case FBDF_CLEAR_TYPE_FAILED:
			path += "FL";
			break;
		case FBDF_CLEAR_TYPE_ASSIST:
			path += "AS";
			break;
		case FBDF_CLEAR_TYPE_CLEARED:
			path += "CL";
			break;
		case FBDF_CLEAR_TYPE_CAKEWALK:
			path += "CW";
			break;
		case FBDF_CLEAR_TYPE_MISSLESS:
			path += "ML";
			break;
		case FBDF_CLEAR_TYPE_FULLCOMBO:
			path += "FC";
			break;
		case FBDF_CLEAR_TYPE_PERFECT:
			path += "PF";
			break;
		}
		path += ".png";
		this->pic.reload(path.c_str());
	}

	DxPic_t handle(void) const {
		return this->pic.handle();
	}
};

class FBDF_result_chara_pic_c {
private:
	dxcur_pic_c pic;

public:
	FBDF_result_chara_pic_c(void) = delete;
	FBDF_result_chara_pic_c(bool ok) {
		std::string file_path = "pic/result/";
		switch (game_option.chara) {
		case FBDF_DANCER_UNIOW:
			file_path += "uniow";
			break;
		case FBDF_DANCER_NEIDA:
			file_path += "neida";
			break;
		case FBDF_DANCER_TRIMBA:
			file_path += "trimba";
			break;
		case FBDF_DANCER_QUATTRO:
			file_path += "quattro";
			break;
		default:
			file_path += "uniow";
			break;
		}
		file_path += "_";
		if (ok) {
			file_path += "ok";
		}
		else {
			file_path += "ng";
		}
		file_path += ".png";
		this->pic.reload(file_path.c_str());
	}

	void draw(void) const {
		int drawLeft = WINDOW_SIZE_X / 2;
		int drawUp   = WINDOW_SIZE_Y - drawLeft;
		DrawExtendGraph(drawLeft, drawUp, WINDOW_SIZE_X, WINDOW_SIZE_Y, this->pic.handle(), TRUE);
	}
};

class FBDF_result_num_pic_c {
private:
	dxcur_divpic_c pic = dxcur_divpic_c(_T("pic/Cascadia_num.png"), 13, 5, 3);

	const int pointsizeX = 25;
	const int picsizeX = 60;
	const int picsizeY = 86;

	/* x座標は勝手に進むので注意 */
	void DrawNumOnce(int &x, int y, char num, double size) const {
		num = betweens('0', num, '9');
		DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
			this->pic.handle(num - '0'), TRUE);
		x += this->picsizeX * size;
	}

	/* x座標は勝手に進むので注意 */
	void DrawPoint(int &x, int y, double size) const {
		DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
			this->pic.handle(10), TRUE);
		x += this->pointsizeX * size;
	}

	/* x座標は勝手に進むので注意 */
	void DrawPlus(int &x, int y, double size) const {
		DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
			this->pic.handle(11), TRUE);
		x += this->picsizeX * size;
	}

	/* x座標は勝手に進むので注意 */
	void DrawMinus(int &x, int y, double size) const {
		DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
			this->pic.handle(12), TRUE);
		x += this->picsizeX * size;
	}

	uint GetPicSize(uint num, double size) const {
		uint ret = 0;
		if (num == 0) { return this->picsizeX * size; }
		while (num != 0) {
			ret += this->picsizeX;
			num /= 10;
		}
		return (uint)(ret * size);
	}

public:
	void DrawNum(int x, int y, double size, int num, bool sign = false) const {
		char buf[8];
		int DrawX = x;
		int DrawY = y;
		if (num < 0) {
			this->DrawMinus(DrawX, DrawY, size);
		}
		else if (sign){
			this->DrawPlus(DrawX, DrawY, size);
		}
		strnums(buf, num, 8);
		for (size_t i = 0; buf[i] != '\0'; i++) {
			this->DrawNumOnce(DrawX, DrawY, buf[i], size);
		}
	}

	void DrawNumRight(int right, int up, double size, uint num, bool sign = false) const {
		char buf[8];
		int DrawX = right - this->GetPicSize(num, size);
		this->DrawNum(DrawX, up, size, num, sign);
	}

	void DrawFloat(int x, int y, double size, double num, uint under, bool sign = false) const {
		char buf[12];
		int DrawX = x;
		int DrawY = y;
		if (num < 0) {
			this->DrawMinus(DrawX, DrawY, size);
		}
		else if (sign){
			this->DrawPlus(DrawX, DrawY, size);
		}
		strnumsD(buf, num, 12, under);
		for (size_t i = 0; buf[i] != '\0'; i++) {
			if (buf[i] == '.') {
				this->DrawPoint(DrawX, DrawY, size);
			}
			else {
				this->DrawNumOnce(DrawX, DrawY, buf[i], size);
			}
		}
	}
};

/**
 * @brief スコアバーの推移を書く
 * @param[in] src スコアバーの推移データ
 * @param[in] rank スコアランク
 * @return なし
 */
static void FBDF_Result_DrawScoreGraph(
	const FBDF_score_bar_st *src, DxPic_t rank_pic, DxPic_t type_pic
) {
	const int all_left  = VIEW_MARGIN;
	const int all_up    = VIEW_MARGIN;
	const int all_right = VIEW_MARGIN + SCORE_GRAPH_X_SIZE;
	const int all_down  = VIEW_MARGIN + SCORE_GRAPH_Y_SIZE;

	const int all_y_middle = (all_down + all_up) / 2;

	for (int i = 0; i < (FBDF_RESULT_SCORE_GRAPH_COUNT - 1); i++) {
		int xpos1 = lins_scale(0, all_left, 24, all_right, i    );
		int xpos2 = lins_scale(0, all_left, 24, all_right, i + 1);
		FBDF_DrawScoreBarVertQuad(src[i], src[i + 1], xpos1, all_up, xpos2, all_down);
	}

	DrawLine(all_left, all_y_middle, all_right, all_y_middle, COLOR_RED);
	DrawBox(all_left, all_up, all_right, all_down, COLOR_WHITE, FALSE);
	DrawGraph(all_left, all_up, rank_pic, TRUE);
	DrawGraph(all_left, all_down - 56, type_pic, TRUE);
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

static void FBDF_Result_DrawUnderScore(
	const FBDF_result_data_t &data, DxPic_t pic, FBDF_result_num_pic_c &numpic
) {
	const int baseY = 16 + VIEW_MARGIN + SCORE_GRAPH_Y_SIZE;
	const int margin = 56;
	DrawGraph(0, VIEW_MARGIN + SCORE_GRAPH_Y_SIZE, pic, TRUE);
	numpic.DrawNum(
		190      , baseY             , 0.48, data.score
	);
	numpic.DrawFloat(
		190      , baseY + margin    , 0.48, data.acc    , 2
	);
	numpic.DrawNumRight(
		190 + 125, baseY + margin * 2, 0.48, data.crit
	);
	numpic.DrawNumRight(
		190 + 125, baseY + margin * 3, 0.48, data.hit
	);
	numpic.DrawNumRight(
		190 + 125, baseY + margin * 4, 0.48, data.save
	);
	numpic.DrawNumRight(
		190 + 125, baseY + margin * 5, 0.48, data.drop
	);
	numpic.DrawFloat(
		190      , baseY + margin * 6, 0.48, data.gap_ave, 2, true
	);
}

/**
 * @brief リザルト画面のベース
 * @param[in] data プレイデータ
 * @return view_num_t 次の画面
 */
static view_num_t FBDF_Result_View(const FBDF_result_data_t &data) {
	dxcur_pic_c back(_T("pic/cutinFulldark.png"));
	dxcur_pic_c list_str(_T("pic/result/list_str.png"));
	FBDF_result_num_pic_c numpic;
	dxcur_key_c key;
	FBDF_result_scorerank_pic_c scorerank_pic(data.acc);
	FBDF_result_cleartype_pic_c cleartype_pic(data);
	FBDF_result_chara_pic_c chara_pic(FBDF_CLEAR_TYPE_ASSIST <= FBDF_Result_JudgeClearType(data));

	while (!GetWindowUserCloseFlag()) {
		key.update();
		if (key.GetKeyState(KEY_INPUT_RETURN) == 1) { break; }
		ClearDrawScreen(); // 作画エリアここから
		DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, back.handle(), TRUE);

		/* スコアグラフ */
		FBDF_Result_DrawScoreGraph(data.score_graph, scorerank_pic.handle(), cleartype_pic.handle());

		/* スコアグラフの横 */
		FBDF_Result_DrawFinalBar(data.acc);
		DrawFormatString(3 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN     , COLOR_WHITE, _T("%s / %s")    , data.music_name.c_str(), data.artist_name.c_str());
		DrawFormatString(3 * VIEW_MARGIN + SCORE_GRAPH_X_SIZE + SCORE_FINAL_BAR_XSIZE, VIEW_MARGIN + 20, COLOR_WHITE, _T("level: %d"), data.level);

		/* スコアグラフの下 */
		FBDF_Result_DrawUnderScore(data, list_str.handle(), numpic);

		chara_pic.draw();

		ScreenFlip(); // 作画エリアここまで
		WaitTimer(10); // ループウェイト
	}

	if (GetWindowUserCloseFlag()) { return VIEW_EXIT; } /* 閉じるボタンが押された */
	return VIEW_SELECT;
}

/**
 * @brief スコアを保存する
 * @param[in] data プレイデータ
 * @return bool true=成功, false=失敗
 */
static bool FBDF_Result_SaveMusicScore(const FBDF_result_data_t &data) {
	FBDF_file_music_score_st this_time_score;
	this_time_score.score      = data.score;
	if (game_option.play_style <= FBDF_PLAYSTYLE_ASSIST) {
		this_time_score.acc = 0;
	}
	else {
		this_time_score.acc = data.acc;
	}
	if (game_option.play_style <= FBDF_PLAYSTYLE_BLANC_PLUS) {
		this_time_score.clear_type = FBDF_Result_JudgeClearType(data);
	}
	else {
		this_time_score.blanc_clear_type = FBDF_Result_JudgeClearType(data);
	}
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
