
#include <string>
#include <vector>
#include <queue>
#include <stack>

#include <DxLib.h>
#include <dirent.h>

#include <strcur.h>
#include <datacur.h>
#include <dxcur.h>
#include <stdcur.h>
#include <UTF8_conv.h>

#include <main.h>
#include <fbdf_cutin.h>
#include <mapenc.h>
#include <CalDif.h>
#include <option.h>

#include <Play.h>

#define NOTE_COLOR_DARK_1 0xFF3E5BCF
#define NOTE_COLOR_DARK_2 0xFF008A00
#define NOTE_COLOR_DARK_3 0xFFA640A3
#define NOTE_COLOR_DARK_4 0xFFC79500

#define NOTE_COLOR_1 0xFF7FD5FD
#define NOTE_COLOR_2 0xFF00E600
#define NOTE_COLOR_3 0xFFFF62FB
#define NOTE_COLOR_4 0xFFFFFF00

typedef enum FBDF_music_list_bar_color_e {
	BLUE_MUSIC_LIST_BAR,
	PINK_MUSIC_LIST_BAR,
	GREEN_MUSIC_LIST_BAR,
	YELLOW_MUSIC_LIST_BAR,
	GRAY_MUSIC_LIST_BAR,
} FBDF_music_list_bar_color_t;

#if 1 /* struct */

typedef struct FBDF_music_dif_s {
	double notes = 0;
	double color = 0;
	double trick = 0;
	double all = 0;
} FBDF_music_dif_t;

typedef struct FBDF_music_detail_s {
	std::string folder_name;
	std::string map_file_name;
	std::string music_name;
	std::string artist;
	uint Length = 0;
	FBDF_music_dif_t auto_cal_dif;
	int user_dif = 0;
	FBDF_dif_type_ec dif_type = FBDF_dif_type_ec::LIGHT;
	FBDF_music_most_colorpat_t most_colorpat[MOST_COLORPAT_NUM];
	FBDF_music_colorcount_t color_count;
	FBDF_file_music_score_st user_highscore;
} FBDF_music_detail_t;

typedef struct FBDF_music_detail_base_s {
	std::string name;
	std::string artist;
	double BPM = 120;
	int offset = 0;
	int preview = 20000;
	std::string map_path;
	int level = -1;
} FBDF_music_detail_base_st;

#endif /* struct */

class FBDF_select_back_pic_c {
private:
	dxcur_pic_c back[3] = {
		dxcur_pic_c(_T("pic/select/back1.png")),
		dxcur_pic_c(_T("pic/select/back2.png")),
		dxcur_pic_c(_T("pic/select/back3.png"))
	};

	DxTime_t Stime = GetNowCount();
	int FrontNo = 1;

public:
	void UpdateState(void) {
		DxTime_t Ntime = GetNowCount();
		if (4000 + this->Stime <= Ntime) {
			this->FrontNo = (this->FrontNo + 1) % 3;
			this->Stime = Ntime;
		}
	}

	void DrawPic(void) const {
		int backNo = (this->FrontNo + 3 - 1) % 3;
		DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, this->back[backNo].handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA,
			lins_scale(0, 0, 4000, 255, (GetNowCount() - this->Stime)));
		DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, this->back[this->FrontNo].handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		return;
	}
};

class FBDF_music_list_c {
public:
	std::vector<FBDF_music_detail_t>detail;
	std::vector<uint>sort;

public: /* 並び替え系 */
	/**
	 * @brief 現在の this->sort の内容を難易度順に並び替える
	 * @param なし
	 * @return なし
	 */
	void SortByDif(void) {
		if (this->sort.empty()) { return; }
		for (int is = 0; is + 1 < (this->sort.size()); is++) {
			for (int ie = is + 1; ie < this->sort.size(); ie++) {
				if (this->detail[this->sort[is]].auto_cal_dif.all >
					this->detail[this->sort[ie]].auto_cal_dif.all)
				{
					uint temp = this->sort[is];
					this->sort[is] = this->sort[ie];
					this->sort[ie] = temp;
				}
			}
		}
	}

public: /* 絞り込み系 */
	void Search(bool (*filter_func)(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type), FBDF_dif_type_ec view_dif) {
		if (filter_func == nullptr) { return; }
		this->sort.clear();
		for (size_t i = 0; i < detail.size(); i++) {
			if (filter_func(this->detail[i], view_dif)) {
				this->sort.push_back(i);
			}
		}
	}

public: /* 番地検索系 */
	FBDF_music_detail_t& operator[](int n) {
		return this->detail[sort[n]];
	}

	const FBDF_music_detail_t& operator[](int n) const {
		return this->detail[sort[n]];
	}

	FBDF_music_detail_t& at(int n) {
		return this->detail[sort[n]];
	}

	const FBDF_music_detail_t& at(int n) const {
		return this->detail[sort[n]];
	}
};

class FBDF_select_view_string_c {
private:
	std::vector<std::string> folder_str;
	std::vector<FBDF_music_list_bar_color_t> folder_color;
	struct {
		dxcur_pic_c gray   = dxcur_pic_c(_T("pic/music_bar_gray.png"));
		dxcur_pic_c blue   = dxcur_pic_c(_T("pic/music_bar_blue.png"));
		dxcur_pic_c pink   = dxcur_pic_c(_T("pic/music_bar_pink.png"));
		dxcur_pic_c green  = dxcur_pic_c(_T("pic/music_bar_green.png"));
		dxcur_pic_c yellow = dxcur_pic_c(_T("pic/music_bar_yellow.png"));
	} pic;

	void DrawOne(const char *name, int offset, FBDF_music_list_bar_color_t bar_color) const {
		int     DrawX = WINDOW_SIZE_X / 2 - 30;
		DxPic_t DrawP = DXLIB_PIC_NULL;
		if (offset != 0) { DrawX += 50; }

		switch (bar_color) {
		case GRAY_MUSIC_LIST_BAR:
			DrawP = this->pic.gray.handle();
			break;
		case BLUE_MUSIC_LIST_BAR:
			DrawP = this->pic.blue.handle();
			break;
		case GREEN_MUSIC_LIST_BAR:
			DrawP = this->pic.green.handle();
			break;
		case PINK_MUSIC_LIST_BAR:
			DrawP = this->pic.pink.handle();
			break;
		case YELLOW_MUSIC_LIST_BAR:
			DrawP = this->pic.yellow.handle();
			break;
		}

		/* bar_colorで色を変える */
		DrawGraph(
			DrawX     , WINDOW_SIZE_Y / 2 - 13 + 45 * offset, DrawP, TRUE);
		DrawFormatString(
			DrawX + 15, WINDOW_SIZE_Y / 2      + 45 * offset, 0xffffffff, _T("%s"), name);
		return;
	}

public:
	void DrawList(int command) const {
		if (this->folder_str.empty()) {
			/* フォルダ内に項目がない。曲フォルダである場合が多い */
			/* 上下の空きスペースに何か置きたい。イラストとか */
			this->DrawOne("該当する曲がありません", 0, GRAY_MUSIC_LIST_BAR);
			return;
		}

		/* 選択中 */
		this->DrawOne(this->folder_str[command].c_str(), 0, this->folder_color[command]);
		/* 選択から下 */
		for (int i = 1; ; i++) {
			int DrawY = WINDOW_SIZE_Y / 2 + i * 45;
			if (WINDOW_SIZE_Y < DrawY) { break; }

			int DrawT = (command + i) % this->size();
			this->DrawOne(this->folder_str[DrawT].c_str(), i, this->folder_color[DrawT]);
		}
		/* 選択から上 */
		for (int i = -1; ; i--) {
			int DrawY = WINDOW_SIZE_Y / 2 + i * 45;
			if (DrawY < 0) { break; }

			int DrawT = command + i;
			while (DrawT < 0) { DrawT += this->size(); }
			this->DrawOne(this->folder_str[DrawT].c_str(), i, this->folder_color[DrawT]);
		}
	}

	void clear(void) {
		this->folder_str.clear();
		this->folder_color.clear();
	}

	void push_back(std::string val, FBDF_music_list_bar_color_t color) {
		this->folder_str.push_back(val);
		this->folder_color.push_back(color);
	}

	size_t size(void) const {
		return this->folder_str.size();
	}
};

#if 1 /* 曲フォルダ―関連 */

typedef struct FBDF_music_folder_node_s FBDF_music_folder_node_st;
struct FBDF_music_folder_node_s {
	std::string name;
	bool is_music_folder = false;
	FBDF_music_list_bar_color_t color = BLUE_MUSIC_LIST_BAR;
	bool (*filter_func)(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) = nullptr;
	std::vector<FBDF_music_folder_node_st*> children;
};

template<typename FolderNode = int>
class folder_manager_c {
private:
	std::stack<FolderNode*> folder_stack;
	std::stack<uint32_t> cmd_stack;

public:
	folder_manager_c(FolderNode *root) {
		this->folder_stack.push(root);
		this->cmd_stack.push(0);
	}

	FolderNode *NowFolder(void) const {
		return this->folder_stack.top();
	}

	bool PushFolder(int cmd) {
		const FolderNode *current = this->folder_stack.top();

		if (!IS_BETWEEN_RIGHT_LESS(0, cmd, current->children.size())) {
			return false;
		}

		this->folder_stack.push(current->children[cmd]);
		this->cmd_stack.push(cmd);
		return true;
	}

	bool PopFolder(void) {
		if (this->folder_stack.size() <= 1) { return false; }
		this->folder_stack.pop();
		this->cmd_stack.pop();
		return true;
	}

	std::stack<uint32_t> GetCmdStack(void) const {
		return this->cmd_stack;
	}
};

#if 1 /* フォルダーのフィルター関連 */

static bool FBDF_Select_FolderFilterAll(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.dif_type == view_dif_type); /* 難易度フィルタのみ */
}

#if 1 /* レベルフィルター */

static bool FBDF_Select_FolderFiltetLevel0(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.auto_cal_dif.all < 1);
}

static bool FBDF_Select_FolderFiltetLevel1(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(1, detail.auto_cal_dif.all, 2));
}

static bool FBDF_Select_FolderFiltetLevel2(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(2, detail.auto_cal_dif.all, 3));
}

static bool FBDF_Select_FolderFiltetLevel3(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(3, detail.auto_cal_dif.all, 4));
}

static bool FBDF_Select_FolderFiltetLevel4(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(4, detail.auto_cal_dif.all, 5));
}

static bool FBDF_Select_FolderFiltetLevel5(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(5, detail.auto_cal_dif.all, 6));
}

static bool FBDF_Select_FolderFiltetLevel6(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(6, detail.auto_cal_dif.all, 7));
}

static bool FBDF_Select_FolderFiltetLevel7(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(7, detail.auto_cal_dif.all, 8));
}

static bool FBDF_Select_FolderFiltetLevel8(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(8, detail.auto_cal_dif.all, 9));
}

static bool FBDF_Select_FolderFiltetLevel9(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(9, detail.auto_cal_dif.all, 10));
}

static bool FBDF_Select_FolderFiltetLevel10(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (10 <= detail.auto_cal_dif.all);
}

#endif /* レベルフィルター */

#if 1 /* スコアフィルター */

static bool FBDF_Select_FolderFiltetScoreP(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (FBDF_SCORE_RANK_P_BORDER <= detail.user_highscore.acc);
}

static bool FBDF_Select_FolderFiltetScoreXP(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_XP_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_P_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreX(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_X_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_XP_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreSP(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_SP_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_X_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreS(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_S_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_SP_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreAP(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_AP_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_S_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreA(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_A_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_AP_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreB(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_B_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_A_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreC(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_C_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_B_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreD(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_D_BORDER, detail.user_highscore.acc, FBDF_SCORE_RANK_C_BORDER);
}

static bool FBDF_Select_FolderFiltetScoreF(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.acc < FBDF_SCORE_RANK_D_BORDER);
}

#endif /* スコアフィルター */

#if 1 /* クリアタイプフィルター */

static bool FBDF_Select_FolderFiltetClearTypePerfect(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_PERFECT);
}

static bool FBDF_Select_FolderFiltetClearTypeFullChain(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_FULLCOMBO);
}

static bool FBDF_Select_FolderFiltetClearTypeMissLess(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_MISSLESS);
}

static bool FBDF_Select_FolderFiltetClearTypeCakewalk(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_CAKEWALK);
}

static bool FBDF_Select_FolderFiltetClearTypeCleared(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_CLEARED);
}

static bool FBDF_Select_FolderFiltetClearTypeAssist(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_ASSIST);
}

static bool FBDF_Select_FolderFiltetClearTypeFailed(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_FAILED);
}

static bool FBDF_Select_FolderFiltetClearTypeNoPlay(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (detail.user_highscore.clear_type == FBDF_CLEAR_TYPE_NOPLAY);
}

#endif /* クリアタイプフィルター */

#endif /* フォルダーのフィルター関連 */

class FBDF_Select_MusicFolderManager_c {
private:

#if 1 /* フォルダー定義 */

	FBDF_music_folder_node_st fol_cleartype_noplay{   "No Play",     true, GRAY_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetClearTypeNoPlay,    {}};
	FBDF_music_folder_node_st fol_cleartype_failed{   "Failed",      true, GRAY_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetClearTypeFailed,    {}};
	FBDF_music_folder_node_st fol_cleartype_assist{   "Assist",      true, BLUE_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetClearTypeAssist,    {}};
	FBDF_music_folder_node_st fol_cleartype_cleared{  "Cleared",     true, BLUE_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetClearTypeCleared,   {}};
	FBDF_music_folder_node_st fol_cleartype_cakewalk{ "Cakewalk",    true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetClearTypeCakewalk,  {}};
	FBDF_music_folder_node_st fol_cleartype_missless{ "Miss Less",   true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetClearTypeMissLess,  {}};
	FBDF_music_folder_node_st fol_cleartype_fullchain{"Full Chain",  true, YELLOW_MUSIC_LIST_BAR, FBDF_Select_FolderFiltetClearTypeFullChain, {}};
	FBDF_music_folder_node_st fol_cleartype_perfect{  "Perfect",     true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetClearTypePerfect,   {}};
	FBDF_music_folder_node_st fol_cleartype_set{      "Clear Type", false, BLUE_MUSIC_LIST_BAR,   nullptr, {&fol_cleartype_perfect, &fol_cleartype_fullchain, &fol_cleartype_missless, &fol_cleartype_cakewalk, &fol_cleartype_cleared, &fol_cleartype_assist, &fol_cleartype_failed, &fol_cleartype_noplay}};

	FBDF_music_folder_node_st fol_score_f{  "Score F",   true, GRAY_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreF,  {}};
	FBDF_music_folder_node_st fol_score_d{  "Score D",   true, GRAY_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreD,  {}};
	FBDF_music_folder_node_st fol_score_c{  "Score C",   true, YELLOW_MUSIC_LIST_BAR, FBDF_Select_FolderFiltetScoreC,  {}};
	FBDF_music_folder_node_st fol_score_b{  "Score B",   true, YELLOW_MUSIC_LIST_BAR, FBDF_Select_FolderFiltetScoreB,  {}};
	FBDF_music_folder_node_st fol_score_a{  "Score A",   true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetScoreA,  {}};
	FBDF_music_folder_node_st fol_score_ap{ "Score A+",  true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetScoreAP, {}};
	FBDF_music_folder_node_st fol_score_s{  "Score S",   true, BLUE_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreS,  {}};
	FBDF_music_folder_node_st fol_score_sp{ "Score S+",  true, BLUE_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreSP, {}};
	FBDF_music_folder_node_st fol_score_x{  "Score X",   true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreX,  {}};
	FBDF_music_folder_node_st fol_score_xp{ "Score X+",  true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreXP, {}};
	FBDF_music_folder_node_st fol_score_p{  "Score P",   true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetScoreP,  {}};
	FBDF_music_folder_node_st fol_score_set{"Score",    false, BLUE_MUSIC_LIST_BAR,   nullptr, {&fol_score_p, &fol_score_xp, &fol_score_x, &fol_score_sp, &fol_score_s, &fol_score_ap, &fol_score_a, &fol_score_b, &fol_score_c, &fol_score_d, &fol_score_f}};

	FBDF_music_folder_node_st fol_level_10{ "Level over 10",  true, YELLOW_MUSIC_LIST_BAR, FBDF_Select_FolderFiltetLevel10, {}};
	FBDF_music_folder_node_st fol_level_9{  "Level 9",        true, YELLOW_MUSIC_LIST_BAR, FBDF_Select_FolderFiltetLevel9,  {}};
	FBDF_music_folder_node_st fol_level_8{  "Level 8",        true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetLevel8,  {}};
	FBDF_music_folder_node_st fol_level_7{  "Level 7",        true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetLevel7,  {}};
	FBDF_music_folder_node_st fol_level_6{  "Level 6",        true, PINK_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetLevel6,  {}};
	FBDF_music_folder_node_st fol_level_5{  "Level 5",        true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetLevel5,  {}};
	FBDF_music_folder_node_st fol_level_4{  "Level 4",        true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetLevel4,  {}};
	FBDF_music_folder_node_st fol_level_3{  "Level 3",        true, GREEN_MUSIC_LIST_BAR,  FBDF_Select_FolderFiltetLevel3,  {}};
	FBDF_music_folder_node_st fol_level_2{  "Level 2",        true, BLUE_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetLevel2,  {}};
	FBDF_music_folder_node_st fol_level_1{  "Level 1",        true, BLUE_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetLevel1,  {}};
	FBDF_music_folder_node_st fol_level_0{  "Level under 0",  true, GRAY_MUSIC_LIST_BAR,   FBDF_Select_FolderFiltetLevel0,  {}};
	FBDF_music_folder_node_st fol_level_set{"Level",         false, BLUE_MUSIC_LIST_BAR,   nullptr, {&fol_level_0, &fol_level_1, &fol_level_2, &fol_level_3, &fol_level_4, &fol_level_5, &fol_level_6, &fol_level_7, &fol_level_8, &fol_level_9, &fol_level_10}};

	FBDF_music_folder_node_st fol_all{"ALL MUSIC", true, BLUE_MUSIC_LIST_BAR, FBDF_Select_FolderFilterAll, {}};

	FBDF_music_folder_node_st fol_root{"DEFAULT", false, BLUE_MUSIC_LIST_BAR, nullptr, {&fol_all, &fol_level_set, &fol_score_set, &fol_cleartype_set}};

	folder_manager_c<FBDF_music_folder_node_st> folder_manager_class = folder_manager_c<FBDF_music_folder_node_st>(&fol_root);

#endif /* フォルダー定義 */

public:
	bool IsMusicFolderNow(void) const {
		return this->folder_manager_class.NowFolder()->is_music_folder;
	}

	const FBDF_music_folder_node_st *NowFolder(void) const {
		return this->folder_manager_class.NowFolder();
	}

	bool PushFolder(int cmd) {
		return this->folder_manager_class.PushFolder(cmd);
	}

	/**
	 * @param[out] pop 取り出されたコマンド番号
	 * @return bool true=実行した, false=実行しなかった
	 */
	bool PopFolder(size_t &pop) {
		const FBDF_music_folder_node_st *buf = this->NowFolder();
		bool ret = this->folder_manager_class.PopFolder();
		if (ret) {
			pop = 0;
			for (size_t i = 0; i < this->NowFolder()->children.size(); i++) {
				if (this->NowFolder()->children[i] == buf) {
					pop = i;
					break;
				}
			}
		}
		return ret;
	}

	/**
	 * @brief 絞り込み/並び替え条件から譜面リストを作る
	 * @param[out] musiclist 譜面リスト
	 * @param[in] folder_num 今いるゲーム内フォルダー
	 * @param[in] view_dif_type 今の難易度表示
	 * @return なし
	 */
	void MakeMusicList(FBDF_music_list_c &musiclist, FBDF_select_view_string_c &folder_string, FBDF_dif_type_ec view_dif_type) {
		/* 内部リスト操作 */
		if (this->IsMusicFolderNow()) {
			musiclist.Search(this->folder_manager_class.NowFolder()->filter_func, view_dif_type);
			musiclist.SortByDif();
		}

		/* リスト作成 */
		folder_string.clear();
		if (this->IsMusicFolderNow()) {
			for (int is = 0; is < musiclist.sort.size(); is++) {
				std::string buf = musiclist[is].music_name;
				FBDF_music_list_bar_color_t color = BLUE_MUSIC_LIST_BAR;
				switch (musiclist[is].dif_type) {
				case FBDF_dif_type_ec::LIGHT:
					buf += "[light]";
					color = GREEN_MUSIC_LIST_BAR;
					break;
				case FBDF_dif_type_ec::NORMAL:
					buf += "[normal]";
					color = YELLOW_MUSIC_LIST_BAR;
					break;
				case FBDF_dif_type_ec::HYPER:
					buf += "[hyper]";
					color = PINK_MUSIC_LIST_BAR;
					break;
				}
				folder_string.push_back(buf, color);
			}
		}
		else {
			for (size_t i = 0; i < this->folder_manager_class.NowFolder()->children.size(); i++) {
				folder_string.push_back(
					this->folder_manager_class.NowFolder()->children[i]->name,
					this->folder_manager_class.NowFolder()->children[i]->color
				);
			}
		}
		return;
	}

	bool ReadFile(int &cmd, FBDF_dif_type_ec &view_def) {
		bool ret = false;
		FILE *fp;
		std::vector<uint32_t> vec;
		fopen_s(&fp, "save/user/select.dat", "rb");
		if (fp == nullptr) { return false; }
		else {
			ret = ReadFileForVector<uint32_t>(vec, fp);
		}
		fread(&cmd, sizeof(int), 1, fp);
		fread(&view_def, sizeof(FBDF_dif_type_ec), 1, fp);
		fclose(fp);

		if (ret == true) {
			for (size_t i = 1; i < vec.size(); i++) {
				this->PushFolder(vec[i]);
			}
		}
		return ret;
	}

	bool WriteFile(int cmd, FBDF_dif_type_ec view_def) const {
		bool ret = false;
		FILE *fp;
		std::vector<uint32_t> vec;
		fopen_s(&fp, "save/user/select.dat", "wb");
		if (fp == nullptr) { return false; }
		else {
			ret = WriteFileForStack<uint32_t>(folder_manager_class.GetCmdStack(), fp);
		}
		fwrite(&cmd, sizeof(int), 1, fp);
		fwrite(&view_def, sizeof(FBDF_dif_type_ec), 1, fp);
		fclose(fp);
		return ret;
	}
};

#endif /* 曲フォルダ―関連 */

static void FBDF_Select_DrawColorCount(int x, int y, const FBDF_music_colorcount_t &count) {
	int Len = pals_scale(35, 300, 0, 0, count.c1);
	int BaseY = y;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_1, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_1, FALSE);
	Len = pals_scale(35, 300, 0, 0, count.c2);
	BaseY += 15;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_2, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_2, FALSE);
	Len = pals_scale(35, 300, 0, 0, count.c3);
	BaseY += 15;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_3, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_3, FALSE);
	Len = pals_scale(35, 300, 0, 0, count.c4);
	BaseY += 15;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_4, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_4, FALSE);
	return;
}

/* 譜面の長さを計算する */
static uint FBDF_CalMapLength(const FBDF_map_t &map) {
	if (map.note.size() < 3) { return 0; }
	return map.note[map.note.size() - 2].time - map.note[0].time;
}

#if 1 /* 譜面リスト読み込み系 */

/**
 * @brief ファイル名から楽曲を読み込む
 * @param[out] detail 読み込んだリストの保存先
 * @param[in] d_name PCフォルダー名
 * @param[in] dif 難易度タイプ
 * @details d_name を "asd"、file を "map.txt" とすると、"music/asd/map.txt" ファイルから楽曲を読み込む
 * @return なし
 */
static void FBDF_Select_MapLoadMusicGetDetail(
	std::vector<FBDF_music_detail_t> &detail, const char *d_name, FBDF_dif_type_ec dif
) {
	FBDF_mapenc_error_et ret;
	FBDF_map_t map;
	FBDF_music_detail_t buf;

	ret = FBDF_MapLoadOne(map, d_name, dif);
	if (ret != FBDF_MAPENC_ERROR_NONE) {
		/* エラーメッセージか何かを残したい */
		if (ret == FBDF_MAPENC_ERROR_FILE) {
			return;
		}
		else {
			/* 気にせずスルー */
		}
	}

	buf.folder_name        = d_name;
	buf.music_name         = map.music_name;
	buf.artist             = map.artist_name;
	buf.Length             = FBDF_CalMapLength(map);
	buf.auto_cal_dif.notes = FBDF_CalMapNotesDif(map.note);
	buf.auto_cal_dif.color = FBDF_CalMapColorDif(&map);
	buf.auto_cal_dif.trick = FBDF_CalMapTrickDif(&map);
	buf.auto_cal_dif.all   = (buf.auto_cal_dif.notes + buf.auto_cal_dif.color + buf.auto_cal_dif.trick) / 3;
	buf.user_dif           = map.user_level;
	buf.map_file_name      = map.map_file_name;
	buf.dif_type           = dif;
	FBDF_CalMapMostColorPat(buf.most_colorpat, &map);
	FBDF_CountMapColor(&buf.color_count, &map, buf.Length);
	FBDF_Save_ReadScoreOneDif(buf.user_highscore, d_name, dif);

	detail.push_back(buf);
}

/**
 * @brief PCフォルダー内を調べて楽曲のリストを読み込む
 * @param[out] musiclist 譜面リスト
 * @return bool true=成功, false=失敗
 */
static bool FBDF_Select_LoadMusicList(FBDF_music_list_c &musiclist) {
	DIR *dir;
	struct dirent *dirs;
	dir = opendir("music");
	if (dir == NULL) { return false; }

	while (1) {
		dirs = readdir(dir);
		if (dirs == NULL) { break; }
		if (dirs->d_name[0] == '.') { continue; }
		FBDF_Select_MapLoadMusicGetDetail(musiclist.detail, dirs->d_name, FBDF_dif_type_ec::LIGHT );
		FBDF_Select_MapLoadMusicGetDetail(musiclist.detail, dirs->d_name, FBDF_dif_type_ec::NORMAL);
		FBDF_Select_MapLoadMusicGetDetail(musiclist.detail, dirs->d_name, FBDF_dif_type_ec::HYPER );
	}

	closedir(dir);
	return true;
}

#endif /* 譜面リスト読み込み系 */

/**
 * @brief セレクト画面のキー入力を管理する
 * @param[out] folder_manager フォルダーマネージャークラス
 * @param[out] now_misic 選択中の曲名
 * @param[out] command 今のカーソル位置
 * @param[out] option_fg オプション画面のフラグ
 * @param[out] view_dif_type 今の難易度表示
 * @param[out] musiclist 譜面リスト
 * @param[out] cutin カットイン管理クラス
 * @return なし
 */
static void FBDF_Select_KeyCheck(
	FBDF_Select_MusicFolderManager_c &folder_manager,
	std::string &now_music,
	int &command,
	bool &option_fg,
	FBDF_dif_type_ec &view_dif_type,
	FBDF_music_list_c &musiclist,
	FBDF_select_view_string_c &folder_string,
	FBDF_cutin_c &cutin
) {
	size_t list_size = 0;
	size_t poped_cmd = 0;
	if (cutin.IsClosing()) { return; } /* カットイン中なのでキー入力無効 */

	InputAllKeyHold();
	switch (GetKeyPushOnce()) {
	case KEY_INPUT_RETURN:
		if (folder_manager.IsMusicFolderNow()) { /* 曲フォルダである */
			if (!musiclist.sort.empty()) { /* 曲フォルダの中が空じゃない */
				cutin.SetIo(CUT_FRAG_IN);
			}
		}
		else { /* サブフォルダである */
			folder_manager.PushFolder(command);
			folder_manager.MakeMusicList(musiclist, folder_string, view_dif_type);
			command = 0;
			if (folder_manager.IsMusicFolderNow() && !musiclist.sort.empty()) {
				for (size_t i = 0; i < musiclist.sort.size(); i++) {
					if (musiclist[i].music_name == now_music) {
						command = i;
						break;
					}
				}
				now_music = musiclist[command].music_name;
			}
		}
		break;
	case KEY_INPUT_BACK:
		if (folder_manager.PopFolder(poped_cmd)) {
			folder_manager.MakeMusicList(musiclist, folder_string, view_dif_type);
			command = poped_cmd;
		}
		break;
	case KEY_INPUT_UP:
		list_size = folder_string.size();
		command = MOD_AVOID_ZERO((command + list_size - 1), list_size, 0);
		if (folder_manager.IsMusicFolderNow() && !musiclist.sort.empty()) {
			now_music = musiclist[command].music_name;
		}
		break;
	case KEY_INPUT_DOWN:
		list_size = folder_string.size();
		command = MOD_AVOID_ZERO((command + 1), list_size, 0);
		if (folder_manager.IsMusicFolderNow() && !musiclist.sort.empty()) {
			now_music = musiclist[command].music_name;
		}
		break;
	case KEY_INPUT_LEFT:
		--view_dif_type;
		if (folder_manager.NowFolder()->name == "ALL MUSIC") {
			folder_manager.MakeMusicList(musiclist, folder_string, view_dif_type);
			command = 0;
			for (size_t i = 0; i < musiclist.sort.size(); i++) {
				if (musiclist[i].music_name == now_music) {
					command = i;
					break;
				}
			}
			now_music = musiclist[command].music_name;
		}
		break;
	case KEY_INPUT_RIGHT:
		++view_dif_type;
		if (folder_manager.NowFolder()->name == "ALL MUSIC") {
			folder_manager.MakeMusicList(musiclist, folder_string, view_dif_type);
			command = 0;
			for (size_t i = 0; i < musiclist.sort.size(); i++) {
				if (musiclist[i].music_name == now_music) {
					command = i;
					break;
				}
			}
			now_music = musiclist[command].music_name;
		}
		break;
	case KEY_INPUT_Z:
		option_fg = true;
		break;
	}
}

static void FBDF_Select_Draw(const FBDF_Select_MusicFolderManager_c &folder_manager_class,
	const FBDF_music_list_c &musiclist, const FBDF_select_view_string_c &folder_string,
	int cmd, const FBDF_select_back_pic_c &back_pic)
{
	back_pic.DrawPic();
	if (folder_manager_class.IsMusicFolderNow() && !musiclist.sort.empty()) {
		DrawFormatString(5,   5, 0xffffffff, _T("notes: %3.2f"),   musiclist[cmd].auto_cal_dif.notes  );
		DrawFormatString(5,  25, 0xffffffff, _T("color: %3.2f"),   musiclist[cmd].auto_cal_dif.color  );
		DrawFormatString(5,  45, 0xffffffff, _T("trick: %3.2f"),   musiclist[cmd].auto_cal_dif.trick  );
		DrawFormatString(5,  65, 0xffffffff, _T(" mdif: %3.2f"),   musiclist[cmd].auto_cal_dif.all    );
		DrawFormatString(5,  85, 0xffffffff, _T("score: %d"),      musiclist[cmd].user_highscore.score);
		DrawFormatString(5, 105, 0xffffffff, _T("  acc: %6.2f%%"), musiclist[cmd].user_highscore.acc  );
		DrawFormatString(5, 125, 0xffffffff, _T("clear type: %s"), FBDF_ClearTypeToString(musiclist[cmd].user_highscore.clear_type).c_str());
		DrawFormatString(5, 145, 0xffffffff, _T("folder: %s"), "");
		FBDF_Select_DrawColorCount(5, 660, (musiclist[cmd].color_count));
	}
	folder_string.DrawList(cmd);
	/* TODO: 操作方法も描きたい */
}

/**
 * @brief セレクト画面のベース
 * @param[out] nex_music プレイ画面に渡すデータ
 * @return view_num_t 次の画面
 */
view_num_t FBDF_SelectView(FBDF_play_choose_music_st &nex_music) {
	bool option_fg = false;
	int command = 0;
	int option_cmd = 0;

	FBDF_dif_type_ec view_dif_type = FBDF_dif_type_ec::LIGHT;

	std::string now_music;

	FBDF_Select_MusicFolderManager_c folder_manager_class;
	FBDF_select_view_string_c folder_string;
	FBDF_music_list_c musiclist;
	FBDF_select_back_pic_c back_pic;
	FBDF_option_pic_st option_pic;

	dxcur_snd_c backsnd(_T("SE/Starlights.mp3"));

	FBDF_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	FBDF_Option_ReloadPic();
	if (FBDF_Select_LoadMusicList(musiclist) == false) { return VIEW_SELECT; }
	folder_manager_class.ReadFile(command, view_dif_type);
	folder_manager_class.MakeMusicList(musiclist, folder_string, view_dif_type);
	/* view_dif_typeも保存したい */
	PlaySoundMem(backsnd.handle(), DX_PLAYTYPE_LOOP);
	cutin.SetIo(CUT_FRAG_OUT);

	while (!GetWindowUserCloseFlag() && !cutin.IsEndAnim()) {
		if (option_fg) {
			FBDF_Option_KeyAction(option_cmd, option_fg);
		}
		else {
			FBDF_Select_KeyCheck(folder_manager_class, now_music, command, option_fg, view_dif_type, musiclist, folder_string, cutin);
		}

		back_pic.UpdateState();
		cutin.update();

		ClearDrawScreen(); // 作画エリアここから
		FBDF_Select_Draw(folder_manager_class, musiclist, folder_string, command, back_pic);
		if (option_fg) { FBDF_Option_Draw(option_cmd, option_pic); }
		cutin.DrawCut();
		ScreenFlip(); // 作画エリアここまで
		WaitTimer(10); // ループウェイト
	}

	if (GetWindowUserCloseFlag()) { return VIEW_EXIT; }

	folder_manager_class.WriteFile(command, view_dif_type);
	FBDF_Save_WriteOption(&game_option);

	nex_music.folder_name   = musiclist[command].folder_name;
	nex_music.map_file_name = musiclist[command].map_file_name;
	nex_music.music_name    = musiclist[command].music_name;
	nex_music.dif_type      = musiclist[command].dif_type;
	return VIEW_PLAY;
}
