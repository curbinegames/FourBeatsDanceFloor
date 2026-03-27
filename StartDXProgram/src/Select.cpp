
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

#include <Select.h>

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

#if 1 /* class */

class FBDF_select_jacket_viewer_c {
private:
	std::string path;
	dxcur_pic_c pic;

public:
	void draw(void) const {
		int DrawX = 110;
		int DrawY = 160;
		int size  = 300;
		DrawExtendGraph(DrawX, DrawY, DrawX + size, DrawY + size, this->pic.handle(), TRUE);
	}

	void clear(void) {
		DeleteGraph(this->pic.handle());
		this->path = "";
	}

	void update(std::string folder_name, std::string image_name) {
		std::string new_path = "music/";
		new_path += folder_name;
		new_path += '/';
		new_path += image_name;
		if (new_path == this->path) { return; }
		this->path = new_path;
		this->pic.reload(this->path.c_str());
	}
};

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
				if (this->detail[this->sort[is]].user_dif >
					this->detail[this->sort[ie]].user_dif)
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
	FBDF_music_detail_t& operator[](size_t n) {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	const FBDF_music_detail_t& operator[](size_t n) const {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	FBDF_music_detail_t& at(size_t n) {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	const FBDF_music_detail_t& at(size_t n) const {
		return this->detail[sort[betweens(0, n, sort.size() - 1)]];
	}

	size_t size(void) const {
		return this->detail.size();
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
		int     DrawX = WINDOW_SIZE_X - 400;
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
	return (detail.user_dif < 1);
}

static bool FBDF_Select_FolderFiltetLevel1(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(1, detail.user_dif, 2));
}

static bool FBDF_Select_FolderFiltetLevel2(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(2, detail.user_dif, 3));
}

static bool FBDF_Select_FolderFiltetLevel3(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(3, detail.user_dif, 4));
}

static bool FBDF_Select_FolderFiltetLevel4(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(4, detail.user_dif, 5));
}

static bool FBDF_Select_FolderFiltetLevel5(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(5, detail.user_dif, 6));
}

static bool FBDF_Select_FolderFiltetLevel6(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(6, detail.user_dif, 7));
}

static bool FBDF_Select_FolderFiltetLevel7(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(7, detail.user_dif, 8));
}

static bool FBDF_Select_FolderFiltetLevel8(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(8, detail.user_dif, 9));
}

static bool FBDF_Select_FolderFiltetLevel9(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (IS_BETWEEN_RIGHT_LESS(9, detail.user_dif, 10));
}

static bool FBDF_Select_FolderFiltetLevel10(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) {
	return (10 <= detail.user_dif);
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
	FBDF_music_folder_node_st fol_cleartype_set{      "Clear Type", false, BLUE_MUSIC_LIST_BAR,   nullptr, {
		&fol_cleartype_perfect, &fol_cleartype_fullchain, &fol_cleartype_missless, &fol_cleartype_cakewalk,
		&fol_cleartype_cleared, &fol_cleartype_assist, &fol_cleartype_failed, &fol_cleartype_noplay
	}};

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
	FBDF_music_folder_node_st fol_score_set{"Score",    false, BLUE_MUSIC_LIST_BAR,   nullptr, {
		&fol_score_p, &fol_score_xp, &fol_score_x, &fol_score_sp, &fol_score_s, &fol_score_ap,
		&fol_score_a, &fol_score_b, &fol_score_c, &fol_score_d, &fol_score_f
	}};

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
	FBDF_music_folder_node_st fol_level_set{"Level",         false, BLUE_MUSIC_LIST_BAR,   nullptr, {
		&fol_level_0, &fol_level_1, &fol_level_2, &fol_level_3, &fol_level_4, &fol_level_5,
		&fol_level_6, &fol_level_7, &fol_level_8, &fol_level_9, &fol_level_10
	}};

	FBDF_music_folder_node_st fol_all{"ALL MUSIC", true, BLUE_MUSIC_LIST_BAR, FBDF_Select_FolderFilterAll, {}};

	FBDF_music_folder_node_st fol_root{"DEFAULT", false, BLUE_MUSIC_LIST_BAR, nullptr, {
		&fol_all, &fol_level_set, &fol_score_set, &fol_cleartype_set
	}};

	folder_manager_c<FBDF_music_folder_node_st> folder_manager_class{&fol_root};

	std::vector<std::string> folder_path;

#endif /* フォルダー定義 */

public:
	bool IsMusicFolderNow(void) const {
		return this->folder_manager_class.NowFolder()->is_music_folder;
	}

	const FBDF_music_folder_node_st *NowFolder(void) const {
		return this->folder_manager_class.NowFolder();
	}

	bool PushFolder(int cmd) {
		if (!this->folder_manager_class.PushFolder(cmd)) {
			return false;
		}
		folder_path.push_back(this->NowFolder()->name);
		return true;
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
			folder_path.pop_back();
		}
		return ret;
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

	std::string GetFolderPathString(void) const {
		std::string ret = "";
		if (this->folder_path.empty()) {
			ret = "default";
		}
		else {
			for (size_t i = 0; i < this->folder_path.size(); i++) {
				if (0 < i) {
					ret += '/';
				}
				ret += this->folder_path[i];
			}
		}
		return ret;
	}
};

#endif /* 曲フォルダ―関連 */

class FBDF_select_bgm_c {
private:
	int sample_rate = 44100;
	bool preview_now = false;
	int base_Stime = 0;
	int preview_Stime = 0;
	int now_volume = 255;
	std::string now_preview_path = "";

	int reserve_settime = 0;
	int reserve_starttime = 0;
	int reserve_sample_rate = 44100;
	std::string reserve_preview_path = "";

	dxcur_snd_c base_snd{_T("SE/Starlights.mp3")};
	dxcur_snd_c preview_snd;

	void SetPreview(const TCHAR *path, int msec) {
		if (this->now_preview_path == path) { return; }

		StopSoundMem(this->preview_snd.handle());
		preview_snd.reload(path);
		if (preview_snd.handle() == DXLIB_SND_NULL) { return; }
		this->now_preview_path = path;
		this->sample_rate = this->reserve_sample_rate;

		ChangeVolumeSoundMem(0, this->base_snd.handle());
		SetCurrentPositionSoundMem(msec / 1000.0 * this->sample_rate, this->preview_snd.handle());
		PlaySoundMem(this->preview_snd.handle(), DX_PLAYTYPE_BACK, FALSE);
		preview_now = true;
		preview_Stime = GetNowCount();
	}

public:
	/* 最初に呼ぶ */
	void init(void) {
		PlaySoundMem(this->base_snd.handle(), DX_PLAYTYPE_LOOP, TRUE);
	}

	/* 曲を選択したとき呼ぶ */
	void ReservePreview(const FBDF_music_detail_t &detail) {
		this->reserve_preview_path  = "music/";
		this->reserve_preview_path += detail.folder_name;
		this->reserve_preview_path += '/';
		this->reserve_preview_path += detail.music_file_name;
		this->reserve_starttime     = detail.pre_time;
		this->reserve_settime       = GetNowCount();
		this->reserve_sample_rate   = detail.sample_rate;
	}

	/* 曲無しフォルダに移動したとき呼ぶ */
	void ReserveErase(void) {
		this->reserve_preview_path = "";
	}

private: /* update系 */
	void UpdateBaseBgm(void) {
		int Ntime = GetNowCount() - this->base_Stime;
		if (Ntime < 500) {
			this->now_volume = lins_scale(0, 0, 500, 255, Ntime);
			ChangeVolumeSoundMem(this->now_volume, this->base_snd.handle());
		}
		else {
			if (this->now_volume != 255) {
				ChangeVolumeSoundMem(255, this->base_snd.handle());
				this->now_volume = 255;
			}
		}
	}

	void UpdatePreviewBgm(void) {
		int Ntime = GetNowCount() - preview_Stime;
		if (IS_BETWEEN_RIGHT_LESS(0, Ntime, 500)) {
			this->now_volume = lins_scale(0, 0, 500, 255, Ntime);
			ChangeVolumeSoundMem(this->now_volume, this->preview_snd.handle());
		}
		else if (IS_BETWEEN(500, Ntime, 14500)) {
			if (this->now_volume != 255) {
				ChangeVolumeSoundMem(255, this->preview_snd.handle());
				this->now_volume = 255;
			}
		}
		else if (IS_BETWEEN_LESS(14500, Ntime, 15000)) {
			this->now_volume = lins_scale(14500, 255, 15000, 0, Ntime);
			ChangeVolumeSoundMem(this->now_volume, this->preview_snd.handle());
		}
		else if (15000 <= Ntime) {
			StopSoundMem(this->preview_snd.handle());
			this->now_volume = 0;
			this->now_preview_path = "";
			preview_now = false;
			ChangeVolumeSoundMem(0, this->base_snd.handle());
			this->base_Stime = GetNowCount();
		}
	}

public: /* update系 */
	/* 毎フレーム呼ぶ */
	void update(void) {
		int Ntime = GetNowCount() - reserve_settime;
		if (reserve_preview_path != "" && 500 <= Ntime) {
			SetPreview(this->reserve_preview_path.c_str(), this->reserve_starttime);
			reserve_preview_path = "";
		}
		if (!preview_now) {
			UpdateBaseBgm();
		}
		else {
			UpdatePreviewBgm();
		}
	}
};

class FBDF_select_difdraw_c {
private:
	int light = -1;
	int normal = -1;
	int dif_hyper = -1; /* hyperという名前は予約されてたw ( #define hyper __int64 )  */
	FBDF_dif_type_ec now_type = FBDF_dif_type_ec::LIGHT;
	dxcur_pic_c bottom{ _T("pic/select/dif_bottom.png") };
	dxcur_pic_c cursor{ _T("pic/select/dif_cursor.png") };
	dxcur_divpic_c num_pic{ _T("pic/select/dif_num.png"), 12, 4, 3 };

	void DrawCursor(void) const {
		int DrawX = 30;
		int size = 200;
		switch (this->now_type) {
		case FBDF_dif_type_ec::LIGHT:
			DrawX = 5;
			break;
		case FBDF_dif_type_ec::NORMAL:
			DrawX = 175;
			break;
		case FBDF_dif_type_ec::HYPER:
			DrawX = 355;
			break;
		}
		DrawExtendGraph(DrawX, 410, DrawX + size, 410 + size, this->cursor.handle(), TRUE);
	}

	void DrawNumber(void) const {
		/* 元のファイルサイズ:97x81 */
		int sizeX = 150;
		int sizeY = 81 * sizeX / 97;
		int drawN = 0;
		drawN = (this->light != -1) ? this->light : 11;
		DrawExtendGraph( 30, 450,  30 + sizeX, 450 + sizeY, this->num_pic.handle(drawN), TRUE);
		drawN = (this->normal != -1) ? this->normal : 11;
		DrawExtendGraph(200, 450, 200 + sizeX, 450 + sizeY, this->num_pic.handle(drawN), TRUE);
		drawN = (this->dif_hyper != -1) ? this->dif_hyper : 11;
		DrawExtendGraph(380, 450, 380 + sizeX, 450 + sizeY, this->num_pic.handle(drawN), TRUE);
	}

public:
	void draw(void) const {
		this->DrawCursor();
		this->DrawNumber();
		DrawGraph(10, 545, this->bottom.handle(), TRUE);
	}

	void ResetDifNum(void) {
		this->light = -1;
		this->normal = -1;
		this->dif_hyper = -1;
	}

	void SetDifNum(int dif_list[]) {
		this->light = dif_list[0];
		this->normal = dif_list[1];
		this->dif_hyper = dif_list[2];
	}

	void SetDifType(FBDF_dif_type_ec type) {
		this->now_type = type;
	}
};

/* 譜面の長さを計算する */
static uint FBDF_CalMapLength(const FBDF_map_t &map) {
	if (map.note.size() < 3) { return 0; }
	return map.note[map.note.size() - 2].time - map.note[0].time;
}

class FBDF_select_list_set_c {
private:
	/* 今いるフォルダの中に、特定の曲名があるかどうか。あったら番地、なかったら-1を返す。 */
	int FindMusicOnList(const std::string &find_music) const {
		if (!this->folder_manager.IsMusicFolderNow()) { return -1; }
		for (size_t i = 0; i < this->music_list.sort.size(); i++) {
			if (this->music_list[i].music_name == find_music) {
				return i;
			}
		}
		return -1;
	}

public:
	FBDF_music_list_c music_list;
	FBDF_select_view_string_c view_string;
	FBDF_Select_MusicFolderManager_c folder_manager;

	void MakeNexMusic(FBDF_play_choose_music_st &dest, int cmd) const {
		dest.folder_name   = this->music_list[cmd].folder_name;
		dest.map_file_name = this->music_list[cmd].map_file_name;
		dest.music_name    = this->music_list[cmd].music_name;
		dest.dif_type      = this->music_list[cmd].dif_type;
	}

	/**
	 * @brief 絞り込み/並び替え条件から譜面リストを作る
	 * @param[in] view_dif_type 今の難易度表示
	 * @return なし
	 */
	void MakeMusicList(FBDF_dif_type_ec view_dif_type) {
		/* 内部リスト操作 */
		if (this->folder_manager.IsMusicFolderNow()) {
			this->music_list.Search(this->folder_manager.NowFolder()->filter_func, view_dif_type);
			this->music_list.SortByDif();
		}

		/* リスト作成 */
		this->view_string.clear();
		if (this->folder_manager.IsMusicFolderNow()) {
			for (int is = 0; is < this->music_list.sort.size(); is++) {
				FBDF_music_list_bar_color_t color = BLUE_MUSIC_LIST_BAR;
				switch (this->music_list[is].dif_type) {
				case FBDF_dif_type_ec::LIGHT:
					color = GREEN_MUSIC_LIST_BAR;
					break;
				case FBDF_dif_type_ec::NORMAL:
					color = YELLOW_MUSIC_LIST_BAR;
					break;
				case FBDF_dif_type_ec::HYPER:
					color = PINK_MUSIC_LIST_BAR;
					break;
				}
				this->view_string.push_back(this->music_list[is].music_name, color);
			}
		}
		else {
			for (size_t i = 0; i < this->folder_manager.NowFolder()->children.size(); i++) {
				this->view_string.push_back(
					this->folder_manager.NowFolder()->children[i]->name,
					this->folder_manager.NowFolder()->children[i]->color
				);
			}
		}
		return;
	}

	/* 今、曲が存在する曲フォルダの中にいるか */
	bool OnAvailableMusicFolderNow(void) const {
		return (this->folder_manager.IsMusicFolderNow() && !this->music_list.sort.empty());
	}

	/* リスト作って選択中の曲探してコマンドと曲名を取得する */
	void ReloadMusicList(std::string &now_music, int &cmd, FBDF_dif_type_ec view_dif_type) {
		int find_no;
		this->MakeMusicList(view_dif_type);
		cmd = 0;
		if (this->OnAvailableMusicFolderNow()) {
			find_no = this->FindMusicOnList(now_music);
			if (find_no != -1) { cmd = find_no; }
			now_music = this->music_list[cmd].music_name;
		}
	}

	void WriteUserData(int &cmd, FBDF_dif_type_ec &view_def) const {
		this->folder_manager.WriteFile(cmd, view_def);
	}

	void MapToDetail(FBDF_music_detail_t &dest, const FBDF_map_t &map, const char *d_name, FBDF_dif_type_ec dif) const {
		dest.folder_name         = d_name;
		dest.music_name          = map.music_name;
		dest.music_file_name     = map.music_file_name;
		dest.artist              = map.artist_name;
		dest.jucket_name         = map.jacket_file_name;
		dest.Length              = FBDF_CalMapLength(map);
		dest.pre_time            = map.pre_time;
		dest.auto_cal_dif.notes  = FBDF_CalMapNotesDif(map.note);
		dest.auto_cal_dif.color  = FBDF_CalMapColorDif(map.note);
		dest.auto_cal_dif.trick  = FBDF_CalMapTrickDif(map.note);
		dest.auto_cal_dif.length = FBDF_CalMapLengthDif(map.note);
		dest.auto_cal_dif.all    = (dest.auto_cal_dif.notes + dest.auto_cal_dif.color + dest.auto_cal_dif.trick) / 3;
		dest.user_dif            = map.user_level;
		dest.map_file_name       = map.map_file_name;
		dest.dif_type            = dif;
		FBDF_CalMapMostColorPat(dest.most_colorpat, &map);
		FBDF_CountMapColor(&dest.color_count, map.note, dest.Length);
		FBDF_Save_ReadScoreOneDif(dest.user_highscore, d_name, dif);
#if (FBDF_LOG_LEVEL_DEF <= 1)
		{
			std::string buf = "mdif: ";
			buf += std::to_string(dest.auto_cal_dif.length);
			buf += "(";
			buf += std::to_string(
				lins(21000, 1, 28000, 9, dest.auto_cal_dif.length)
			);
			buf += ")";
			buf += " : ";
			buf += dest.folder_name;
			buf += "[";
			switch (dest.dif_type) {
			case FBDF_dif_type_ec::LIGHT:
				buf += "LIGHT";
				break;
			case FBDF_dif_type_ec::NORMAL:
				buf += "NORMAL";
				break;
			case FBDF_dif_type_ec::HYPER:
				buf += "HYPER";
				break;
			}
			buf += "](";
			buf += std::to_string(dest.user_dif);
			buf += ")";
			FBDF_LOG_INFO(buf.c_str());
		}
#endif
	}

	/**
	 * @brief ファイル名から楽曲を読み込む
	 * @param[out] detail 読み込んだリストの保存先
	 * @param[in] d_name PCフォルダー名
	 * @param[in] dif 難易度タイプ
	 * @details d_name を "asd"、file を "map.txt" とすると、"music/asd/map.txt" ファイルから楽曲を読み込む
	 * @return bool true:読み取り成功, false:失敗
	 */
	bool MapLoadMusicGetDetail(const char *d_name, FBDF_music_detail_t &dest, FBDF_dif_type_ec dif) {
		FBDF_mapenc_error_et ret;
		FBDF_map_t map;

		ret = FBDF_MapLoadOne(map, d_name, dif);
		if (ret != FBDF_MAPENC_ERROR_NONE) {
			/* エラーメッセージか何かを残したい */
			if (ret == FBDF_MAPENC_ERROR_FILE) {
				return false;
			}
			else {
				/* 気にせずスルー */
			}
		}
		MapToDetail(dest, map, d_name, dif);
		return true;
	}

	void MapLoadMusicGetDetailAllDif(const char *d_name) {
		bool insert_fg[3] = { false,false,false };
		int dif_list[3] = { -1,-1,-1 };
		FBDF_music_detail_t buf[3];
		insert_fg[0] = this->MapLoadMusicGetDetail(d_name, buf[0], FBDF_dif_type_ec::LIGHT);
		if (insert_fg[0]) {
			dif_list[0] = buf[0].user_dif;
		}
		insert_fg[1] = this->MapLoadMusicGetDetail(d_name, buf[1], FBDF_dif_type_ec::NORMAL);
		if (insert_fg[1]) {
			dif_list[1] = buf[1].user_dif;
		}
		insert_fg[2] = this->MapLoadMusicGetDetail(d_name, buf[2], FBDF_dif_type_ec::HYPER);
		if (insert_fg[2]) {
			dif_list[2] = buf[2].user_dif;
		}
		for (size_t ib = 0; ib < 3; ib++) {
			if (insert_fg[ib]) {
				for (size_t ia = 0; ia < 3; ia++) {
					buf[ib].level_list[ia] = dif_list[ia];
				}
				this->music_list.detail.push_back(buf[ib]);
			}
		}
	}

	/**
	 * @brief PCフォルダー内を調べて楽曲のリストを読み込む
	 * @return bool true=成功, false=失敗
	 */
	bool LoadMusicList(void) {
		DIR *dir;
		struct dirent *dirs;
		dir = opendir("music");
		if (dir == NULL) { return false; }

		while (1) {
			dirs = readdir(dir);
			if (dirs == NULL) { break; }
			if (dirs->d_name[0] == '.') { continue; }
			MapLoadMusicGetDetailAllDif(dirs->d_name);
		}

		closedir(dir);
		return true;
	}

public:
	void DrawColorCount(int x, int y, int cmd) const {
		const FBDF_music_colorcount_t &count = this->music_list[cmd].color_count;
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

	bool IsAllFolder(void) const {
		return this->folder_manager.NowFolder()->name == "ALL MUSIC";
	}
};

/* セレクト画面に関するクラスをまとめたもの */
class FBDF_select_class_set_c {
public:
	FBDF_select_list_set_c list_set;
	FBDF_select_back_pic_c back_pic;
	dxcur_pic_c top_bar{ "pic/select/select_bar.png" };
	FBDF_select_jacket_viewer_c jacket_viewer;
	FBDF_select_difdraw_c dif_pic;
	FBDF_usage_c usage{ "上下キー: 曲選択、左右キー: 難易度選択\nEnterキー: 実行、Backキー: 戻る、Zキー: オプションに進む" };
	FBDF_select_bgm_c bgm;
	dxcur_snd_c se{ "SE/select.mp3" };

	bool Init(int &cmd, FBDF_dif_type_ec &view_def) {
		if (this->list_set.LoadMusicList() == false) { return false; }
		this->list_set.folder_manager.ReadFile(cmd, view_def);
		this->list_set.MakeMusicList(view_def);
		cmd = betweens(0, cmd, this->list_set.music_list.sort.size() - 1);
		this->UpdateJacket(cmd);
		this->bgm.init();
		if (this->list_set.OnAvailableMusicFolderNow()) {
			this->dif_pic.SetDifNum(this->list_set.music_list[cmd].level_list);
			this->dif_pic.SetDifType(this->list_set.music_list[cmd].dif_type);
		}
		else {
			this->dif_pic.ResetDifNum();
			this->dif_pic.SetDifType(view_def);
		}
#if (FBDF_LOG_LEVEL_DEF <= 1)
		{
			std::string log = "読み込んだ譜面数: ";
			log += std::to_string(this->music_list.size());
			FBDF_LOG_INFO(log.c_str());
		}
#endif
		return true;
	}

	void UpdateJacket(int cmd) {
		if (this->list_set.OnAvailableMusicFolderNow()) {
			this->jacket_viewer.update(
				this->list_set.music_list[cmd].folder_name,
				this->list_set.music_list[cmd].jucket_name
			);
		}
		else {
			this->jacket_viewer.clear();
		}
	}

	void Draw(int cmd) const {
		this->back_pic.DrawPic();
		DrawFormatString(
			5, 80, COLOR_WHITE, _T("folder: %s"),
			this->list_set.folder_manager.GetFolderPathString().c_str()
		);
		if (this->list_set.OnAvailableMusicFolderNow()) {
			this->list_set.DrawColorCount(5, 100, cmd);
			this->jacket_viewer.draw();
			DrawFormatString(
				5, WINDOW_SIZE_Y - 110, COLOR_WHITE, _T("artist: %s"),
				this->list_set.music_list[cmd].artist.c_str()
			);
			DrawFormatString(
				5, WINDOW_SIZE_Y - 90, COLOR_WHITE, _T("score: %d / acc: %6.2f%%"),
				this->list_set.music_list[cmd].user_highscore.score,
				this->list_set.music_list[cmd].user_highscore.acc
			);
			DrawFormatString(
				5, WINDOW_SIZE_Y - 70, COLOR_WHITE, _T("clear type: %s"),
				FBDF_ClearTypeToString(this->list_set.music_list[cmd].user_highscore.clear_type).c_str()
			);
		}
		this->list_set.view_string.DrawList(cmd);
		this->dif_pic.draw();
		DrawGraph(0, 0, this->top_bar.handle(), TRUE);
		this->usage.draw(0, WINDOW_SIZE_Y);
	}
};

#endif /* class */

#if 1 /* キー入力関連 */

static void FBDF_Select_DecideFolder(
	FBDF_select_class_set_c &select_class, std::string &now_music, int &cmd,
	FBDF_dif_type_ec view_dif_type, FBDF_cutin_c &cutin
) {
	FBDF_select_list_set_c &list_set = select_class.list_set;
	if (list_set.folder_manager.IsMusicFolderNow()) { /* 曲フォルダである */
		if (!list_set.music_list.sort.empty()) { /* 曲フォルダの中が空じゃない */
			cutin.SetIo(CUT_FRAG_IN);
		}
	}
	else { /* サブフォルダである */
		list_set.folder_manager.PushFolder(cmd);
		list_set.ReloadMusicList(now_music, cmd, view_dif_type);
		select_class.UpdateJacket(cmd);
		if (list_set.OnAvailableMusicFolderNow()) {
			select_class.bgm.ReservePreview(list_set.music_list[cmd]);
			select_class.dif_pic.SetDifNum(list_set.music_list[cmd].level_list);
			select_class.dif_pic.SetDifType(list_set.music_list[cmd].dif_type);
		}
		else {
			select_class.dif_pic.ResetDifNum();
		}
		PlaySoundMem(select_class.se.handle(), DX_PLAYTYPE_BACK);
	}
}

static void FBDF_Select_BackFolder(
	FBDF_select_class_set_c &select_class, int &cmd, FBDF_dif_type_ec view_dif_type
) {
	FBDF_select_list_set_c &list_set = select_class.list_set;
	size_t poped_cmd = 0;
	if (list_set.folder_manager.PopFolder(poped_cmd)) {
		list_set.MakeMusicList(view_dif_type);
		cmd = poped_cmd;
		select_class.UpdateJacket(cmd);
		select_class.bgm.ReserveErase();
		PlaySoundMem(select_class.se.handle(), DX_PLAYTYPE_BACK);
		select_class.dif_pic.ResetDifNum();
	}
}

static void FBDF_Select_KeyVert(
	FBDF_select_class_set_c &select_class, std::string &now_music, int &cmd, bool up
) {
	FBDF_select_list_set_c &list_set = select_class.list_set;
	size_t list_size = list_set.view_string.size();
	if (up) {
		cmd = MOD_AVOID_ZERO((cmd + list_size - 1), list_size, 0);
	}
	else {
		cmd = MOD_AVOID_ZERO((cmd + 1), list_size, 0);
	}
	if (list_set.OnAvailableMusicFolderNow()) {
		now_music = list_set.music_list[cmd].music_name;
		select_class.bgm.ReservePreview(list_set.music_list[cmd]);
		select_class.dif_pic.SetDifNum(list_set.music_list[cmd].level_list);
		select_class.dif_pic.SetDifType(list_set.music_list[cmd].dif_type);
	}
	select_class.UpdateJacket(cmd);
	PlaySoundMem(select_class.se.handle(), DX_PLAYTYPE_BACK);
}

static void FBDF_Select_KeyHori(
	FBDF_select_class_set_c &select_class, FBDF_dif_type_ec &view_dif_type,
	std::string &now_music, int &cmd, bool right
) {
	FBDF_select_list_set_c &list_set = select_class.list_set;
	if (!list_set.IsAllFolder()) { return; } /* TODO: どちらかというと、if(今いるフォルダーに難易度フィルターがあるかどうか) */
	if (right) { ++view_dif_type; } else { --view_dif_type; }
	list_set.ReloadMusicList(now_music, cmd, view_dif_type);
	select_class.UpdateJacket(cmd);
	if (list_set.OnAvailableMusicFolderNow()) {
		select_class.bgm.ReservePreview(list_set.music_list[cmd]);
		select_class.dif_pic.SetDifNum(list_set.music_list[cmd].level_list);
		select_class.dif_pic.SetDifType(list_set.music_list[cmd].dif_type);
	}
	PlaySoundMem(select_class.se.handle(), DX_PLAYTYPE_BACK);
}

/**
 * @brief セレクト画面のキー入力を管理する
 * @param[out] key キークラス
 * @param[out] select_class セレクトクラス
 * @param[out] now_misic 選択中の曲名
 * @param[out] command 今のカーソル位置
 * @param[out] option_fg オプション画面のフラグ
 * @param[out] view_dif_type 今の難易度表示
 * @param[out] cutin カットイン管理クラス
 * @return なし
 */
static void FBDF_Select_KeyCheck(
	dxcur_key_c &key, FBDF_select_class_set_c &select_class,
	std::string &now_music, int &command, bool &option_fg,
	FBDF_dif_type_ec &view_dif_type, FBDF_cutin_c &cutin
) {
	if (cutin.IsClosing()) { return; } /* カットイン中のときはキー入力無効 */

	key.update();
	switch (key.GetKeyPulseOnce()) {
	case KEY_INPUT_RETURN:
		FBDF_Select_DecideFolder(select_class, now_music, command, view_dif_type, cutin);
		break;
	case KEY_INPUT_BACK:
		FBDF_Select_BackFolder(select_class, command, view_dif_type);
		break;
	case KEY_INPUT_UP:
		FBDF_Select_KeyVert(select_class, now_music, command, true);
		break;
	case KEY_INPUT_DOWN:
		FBDF_Select_KeyVert(select_class, now_music, command, false);
		break;
	case KEY_INPUT_LEFT:
		FBDF_Select_KeyHori(select_class, view_dif_type, now_music, command, false);
		break;
	case KEY_INPUT_RIGHT:
		FBDF_Select_KeyHori(select_class, view_dif_type, now_music, command, true);
		break;
	case KEY_INPUT_Z:
		option_fg = true;
		break;
	}
}

#endif /* キー入力関連 */

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
	FBDF_select_class_set_c select_class;
	dxcur_key_c key;
	FBDF_option_pic_st option_pic;
	FBDF_usage_c option_usage("上下キー: 項目選択、左右キー: 設定の変更\nBack/Zキー: 選曲画面に戻る");
	FBDF_cutin_c cutin;

	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	FBDF_Option_ReloadPic();
	if (select_class.Init(command, view_dif_type) == false) { return VIEW_SELECT; }
	cutin.SetIo(CUT_FRAG_OUT);

	while (!GetWindowUserCloseFlag() && !cutin.IsEndAnim()) {
		if (option_fg) {
			FBDF_Option_KeyAction(key, option_cmd, option_fg, select_class.se.handle());
		}
		else {
			FBDF_Select_KeyCheck(
				key, select_class, now_music, command, option_fg, view_dif_type, cutin
			);
		}

		select_class.back_pic.UpdateState();
		select_class.bgm.update();
		cutin.update();

		ClearDrawScreen(); // 作画エリアここから
		select_class.Draw(command);
		if (option_fg) { FBDF_Option_Draw(option_cmd, option_pic, option_usage); }
		cutin.DrawCut();
		ScreenFlip(); // 作画エリアここまで
		WaitTimer(10); // ループウェイト
	}

	if (GetWindowUserCloseFlag()) { return VIEW_EXIT; }

	select_class.list_set.WriteUserData(command, view_dif_type);
	FBDF_Save_WriteOption(&game_option);

	select_class.list_set.MakeNexMusic(nex_music, command);
	return VIEW_PLAY;
}
