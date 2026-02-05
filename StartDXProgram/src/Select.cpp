
#include <string>
#include <vector>
#include <queue>
#include <stack>

#include <DxLib.h>
#include <dirent.h>

#include <dxcur.h>
#include <strcur.h>
#include <UTF8_conv.h>

#include <main.h>
#include <fbdf_cutin.h>
#include <mapenc.h>
#include <CalDif.h>

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

typedef enum FBDF_music_folder_num_e {
	DEFAULT_MUSIC_FOLDER,
	ALL_MUSIC_FOLDER,
	LEVEL_SET_MUSIC_FOLDER,
	LEVEL0_MUSIC_FOLDER,
	LEVEL1_MUSIC_FOLDER,
	LEVEL2_MUSIC_FOLDER,
	LEVEL3_MUSIC_FOLDER,
	LEVEL4_MUSIC_FOLDER,
	LEVEL5_MUSIC_FOLDER,
	LEVEL6_MUSIC_FOLDER,
	LEVEL7_MUSIC_FOLDER,
	LEVEL8_MUSIC_FOLDER,
	LEVEL9_MUSIC_FOLDER,
	LEVEL10_MUSIC_FOLDER,
	SCORE_SET_MUSIC_FOLDER,
	SCORE_P_MUSIC_FOLDER,
	SCORE_XP_MUSIC_FOLDER,
	SCORE_X_MUSIC_FOLDER,
	SCORE_SP_MUSIC_FOLDER,
	SCORE_S_MUSIC_FOLDER,
	SCORE_AP_MUSIC_FOLDER,
	SCORE_A_MUSIC_FOLDER,
	SCORE_B_MUSIC_FOLDER,
	SCORE_C_MUSIC_FOLDER,
	SCORE_D_MUSIC_FOLDER,
	SCORE_F_MUSIC_FOLDER,
	CLEARTYPE_SET_MUSIC_FOLDER,
	CLEARTYPE_PERFECT_MUSIC_FOLDER,
	CLEARTYPE_FULLCOMBO_MUSIC_FOLDER,
	CLEARTYPE_MISSLESS_MUSIC_FOLDER,
	CLEARTYPE_CAKEWALK_MUSIC_FOLDER,
	CLEARTYPE_CLEARED_MUSIC_FOLDER,
	CLEARTYPE_FAILED_MUSIC_FOLDER,
	CLEARTYPE_NOPLAY_MUSIC_FOLDER,
} FBDF_music_folder_num_t;

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

typedef struct FBDF_music_ber_pic_s {
	dxcur_pic_c blue   = dxcur_pic_c(_T("pic/music_bar_blue.png"));
	dxcur_pic_c pink   = dxcur_pic_c(_T("pic/music_bar_pink.png"));
	dxcur_pic_c green  = dxcur_pic_c(_T("pic/music_bar_green.png"));
	dxcur_pic_c yellow = dxcur_pic_c(_T("pic/music_bar_yellow.png"));
} FBDF_music_ber_pic_t;

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

static std::vector<std::string> folder_str;

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
	void UpdateState() {
		DxTime_t Ntime = GetNowCount();
		if (4000 + this->Stime <= Ntime) {
			this->FrontNo = (this->FrontNo + 1) % 3;
			this->Stime = Ntime;
		}
	}

	void DrawPic() const {
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

	FBDF_music_detail_t& operator[](int n) {
		return this->detail[sort[n]];
	}

	const FBDF_music_detail_t& operator[](int n) const {
		return this->detail[sort[n]];
	}
};

#if 1 /* 曲フォルダ―関連 */

typedef struct FBDF_music_folder_node_s FBDF_music_folder_node_st;
struct FBDF_music_folder_node_s {
	std::string name;
	bool is_music_folder = false;
	bool (*filter_func)(const FBDF_music_detail_t &detail, FBDF_dif_type_ec view_dif_type) = nullptr;
	std::vector<FBDF_music_folder_node_st*> children;
};

template<typename FolderNode = int>
class folder_manager_c {
private:
	std::stack<FolderNode*> folder_stack;

public:
	folder_manager_c(FolderNode *root) {
		this->folder_stack.push(root);
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
		return true;
	}

	bool PopFolder(void) {
		if (this->folder_stack.size() <= 1) { return false; }
		this->folder_stack.pop();
		return true;
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

	FBDF_music_folder_node_st fol_cleartype_noplay{   "No Play",     true, FBDF_Select_FolderFiltetClearTypeNoPlay,    {}};
	FBDF_music_folder_node_st fol_cleartype_failed{   "Failed",      true, FBDF_Select_FolderFiltetClearTypeFailed,    {}};
	FBDF_music_folder_node_st fol_cleartype_cleared{  "Cleared",     true, FBDF_Select_FolderFiltetClearTypeCleared,   {}};
	FBDF_music_folder_node_st fol_cleartype_cakewalk{ "Cakewalk",    true, FBDF_Select_FolderFiltetClearTypeCakewalk,  {}};
	FBDF_music_folder_node_st fol_cleartype_missless{ "Miss Less",   true, FBDF_Select_FolderFiltetClearTypeMissLess,  {}};
	FBDF_music_folder_node_st fol_cleartype_fullchain{"Full Chain",  true, FBDF_Select_FolderFiltetClearTypeFullChain, {}};
	FBDF_music_folder_node_st fol_cleartype_perfect{  "Perfect",     true, FBDF_Select_FolderFiltetClearTypePerfect,   {}};
	FBDF_music_folder_node_st fol_cleartype_set{      "Clear Type", false, nullptr, {&fol_cleartype_perfect, &fol_cleartype_fullchain, &fol_cleartype_missless, &fol_cleartype_cakewalk, &fol_cleartype_cleared, &fol_cleartype_failed, &fol_cleartype_noplay}};

	FBDF_music_folder_node_st fol_score_f{  "Score F",   true, FBDF_Select_FolderFiltetScoreF,  {}};
	FBDF_music_folder_node_st fol_score_d{  "Score D",   true, FBDF_Select_FolderFiltetScoreD,  {}};
	FBDF_music_folder_node_st fol_score_c{  "Score C",   true, FBDF_Select_FolderFiltetScoreC,  {}};
	FBDF_music_folder_node_st fol_score_b{  "Score B",   true, FBDF_Select_FolderFiltetScoreB,  {}};
	FBDF_music_folder_node_st fol_score_a{  "Score A",   true, FBDF_Select_FolderFiltetScoreA,  {}};
	FBDF_music_folder_node_st fol_score_ap{ "Score A+",  true, FBDF_Select_FolderFiltetScoreAP, {}};
	FBDF_music_folder_node_st fol_score_s{  "Score S",   true, FBDF_Select_FolderFiltetScoreS,  {}};
	FBDF_music_folder_node_st fol_score_sp{ "Score S+",  true, FBDF_Select_FolderFiltetScoreSP, {}};
	FBDF_music_folder_node_st fol_score_x{  "Score X",   true, FBDF_Select_FolderFiltetScoreX,  {}};
	FBDF_music_folder_node_st fol_score_xp{ "Score X+",  true, FBDF_Select_FolderFiltetScoreXP, {}};
	FBDF_music_folder_node_st fol_score_p{  "Score P",   true, FBDF_Select_FolderFiltetScoreP,  {}};
	FBDF_music_folder_node_st fol_score_set{"Score",    false, nullptr, {&fol_score_p, &fol_score_xp, &fol_score_x, &fol_score_sp, &fol_score_s, &fol_score_ap, &fol_score_a, &fol_score_b, &fol_score_c, &fol_score_d, &fol_score_f}};

	FBDF_music_folder_node_st fol_level_10{ "Level over 10",  true, FBDF_Select_FolderFiltetLevel10, {}};
	FBDF_music_folder_node_st fol_level_9{  "Level 9",        true, FBDF_Select_FolderFiltetLevel9,  {}};
	FBDF_music_folder_node_st fol_level_8{  "Level 8",        true, FBDF_Select_FolderFiltetLevel8,  {}};
	FBDF_music_folder_node_st fol_level_7{  "Level 7",        true, FBDF_Select_FolderFiltetLevel7,  {}};
	FBDF_music_folder_node_st fol_level_6{  "Level 6",        true, FBDF_Select_FolderFiltetLevel6,  {}};
	FBDF_music_folder_node_st fol_level_5{  "Level 5",        true, FBDF_Select_FolderFiltetLevel5,  {}};
	FBDF_music_folder_node_st fol_level_4{  "Level 4",        true, FBDF_Select_FolderFiltetLevel4,  {}};
	FBDF_music_folder_node_st fol_level_3{  "Level 3",        true, FBDF_Select_FolderFiltetLevel3,  {}};
	FBDF_music_folder_node_st fol_level_2{  "Level 2",        true, FBDF_Select_FolderFiltetLevel2,  {}};
	FBDF_music_folder_node_st fol_level_1{  "Level 1",        true, FBDF_Select_FolderFiltetLevel1,  {}};
	FBDF_music_folder_node_st fol_level_0{  "Level under 0",  true, FBDF_Select_FolderFiltetLevel0,  {}};
	FBDF_music_folder_node_st fol_level_set{"Level",         false, nullptr, {&fol_level_0, &fol_level_1, &fol_level_2, &fol_level_3, &fol_level_4, &fol_level_5, &fol_level_6, &fol_level_7, &fol_level_8, &fol_level_9, &fol_level_10}};

	FBDF_music_folder_node_st fol_all{"ALL MUSIC", true, FBDF_Select_FolderFilterAll, {}};

	FBDF_music_folder_node_st fol_root{"DEFAULT", false, nullptr, {&fol_all, &fol_level_set, &fol_score_set, &fol_cleartype_set}};

	folder_manager_c<FBDF_music_folder_node_st> folder_manager_class = folder_manager_c<FBDF_music_folder_node_st>(&fol_root);

#endif /* フォルダー定義 */

	/**
	 * @brief 譜面リストを難易度順に並び替える
	 * @param[out] musiclist 譜面リスト
	 * @return なし
	 */
	void SortMusicListDif(FBDF_music_list_c &musiclist) const {
		if (musiclist.sort.empty()) { return; }
		for (int is = 0; is + 1 < (musiclist.sort.size()); is++) {
			for (int ie = is + 1; ie < musiclist.sort.size(); ie++) {
				if (musiclist.detail[musiclist.sort[is]].auto_cal_dif.all >
					musiclist.detail[musiclist.sort[ie]].auto_cal_dif.all)
				{
					uint temp = musiclist.sort[is];
					musiclist.sort[is] = musiclist.sort[ie];
					musiclist.sort[ie] = temp;
				}
			}
		}
	}

	/**
	 * @brief 絞り込み条件から譜面リストを作る
	 * @param[out] musiclist 譜面リスト
	 * @param[in] view_dif_type 今の難易度表示
	 * @return なし
	 */
	void MakeMusicListDetectMusic(FBDF_music_list_c &musiclist, FBDF_dif_type_ec view_dif_type) {
		if (this->folder_manager_class.NowFolder()->is_music_folder == false) { return; }
		for (int i = 0; i < musiclist.detail.size(); i++) {
			if (this->folder_manager_class.NowFolder()->filter_func != nullptr) {
				if (this->folder_manager_class.NowFolder()->filter_func(musiclist.detail[i], view_dif_type)) {
					musiclist.sort.push_back(i);
				}
			}
		}
	}

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
	void MakeMusicList(FBDF_music_list_c &musiclist, FBDF_dif_type_ec view_dif_type) {
		if (this->IsMusicFolderNow()) {
			musiclist.sort.clear();
			folder_str.clear();

			this->MakeMusicListDetectMusic(musiclist, view_dif_type);
			this->SortMusicListDif(musiclist);

			/* リスト作成 */
			for (int is = 0; is < musiclist.sort.size(); is++) {
				std::string buf = musiclist[is].music_name;
				switch (musiclist[is].dif_type) {
				case FBDF_dif_type_ec::LIGHT:
					buf += "[light]";
					break;
				case FBDF_dif_type_ec::NORMAL:
					buf += "[normal]";
					break;
				case FBDF_dif_type_ec::HYPER:
					buf += "[hyper]";
					break;
				}
				folder_str.push_back(buf);
			}
		}
		else {
			folder_str.clear();
			for (size_t i = 0; i < this->folder_manager_class.NowFolder()->children.size(); i++) {
				folder_str.push_back(this->folder_manager_class.NowFolder()->children[i]->name);
			}
		}
		return;
	}
};

#endif /* 曲フォルダ―関連 */

static void FBDF_SelectDrawMusicListOne(const char *name, int offset,
	FBDF_music_list_bar_color_t bar_color, const FBDF_music_ber_pic_t *music_ber_pic)
{
	int DrawX = WINDOW_SIZE_X / 2 - 30;
	if (offset != 0) { DrawX += 50; }

	/* bar_colorで色を変える */
	DrawGraph(
		DrawX     , WINDOW_SIZE_Y / 2 - 13 + 45 * offset, music_ber_pic->blue.handle(), TRUE);
	DrawFormatString(
		DrawX + 15, WINDOW_SIZE_Y / 2      + 45 * offset, 0xffffffff, _T("%s"), name);
	return;
}

static void FBDF_SelectDrawMusicList(int command, const FBDF_music_ber_pic_t *music_ber_pic) {
	if (folder_str.empty()) {
		/* フォルダ内に項目がない。曲フォルダである場合が多い */
		/* 上下の空きスペースに何か置きたい。イラストとか */
		FBDF_SelectDrawMusicListOne("該当する曲がありません", 0, BLUE_MUSIC_LIST_BAR, music_ber_pic);
		return;
	}

	/* 選択中 */
	FBDF_SelectDrawMusicListOne(folder_str[command].c_str(), 0, BLUE_MUSIC_LIST_BAR, music_ber_pic);
	/* 選択から下 */
	for (int i = 1; ; i++) {
		int DrawY = WINDOW_SIZE_Y / 2 + i * 45;
		if (WINDOW_SIZE_Y < DrawY) { break; }

		int DrawT = (command + i) % folder_str.size();
		FBDF_SelectDrawMusicListOne(folder_str[DrawT].c_str(), i, BLUE_MUSIC_LIST_BAR, music_ber_pic);
	}
	/* 選択から上 */
	for (int i = -1; ; i--) {
		int DrawY = WINDOW_SIZE_Y / 2 + i * 45;
		if (DrawY < 0) { break; }

		int DrawT = command + i;
		while (DrawT < 0) { DrawT += folder_str.size(); }
		FBDF_SelectDrawMusicListOne(folder_str[DrawT].c_str(), i, BLUE_MUSIC_LIST_BAR, music_ber_pic);
	}
}

static void FBDF_SelectDrawColorCount(int x, int y, const FBDF_music_colorcount_t *count) {
	int Len = pals_scale(35, 300, 0, 0, count->c1);
	int BaseY = y;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_1, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_1, FALSE);
	Len = pals_scale(35, 300, 0, 0, count->c2);
	BaseY += 15;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_2, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_2, FALSE);
	Len = pals_scale(35, 300, 0, 0, count->c3);
	BaseY += 15;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_3, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_3, FALSE);
	Len = pals_scale(35, 300, 0, 0, count->c4);
	BaseY += 15;
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_4, TRUE);
	DrawBox(x, BaseY, x + Len, BaseY + 10, NOTE_COLOR_DARK_4, FALSE);
	return;
}

/* 譜面の長さを計算する */
static uint FBDF_CalMapLength(const FBDF_map_t *map) {
	if (map->note.size() < 3) { return 0; }
	return map->note[map->note.size() - 2].time - map->note[0].time;
}

#if 1 /* 譜面リスト読み込み系 */

/**
 * @brief ファイル名から楽曲を読み込む
 * @param[out] detail 読み込んだリストの保存先
 * @param[in] music_detail_base 全難易度共通部分のデータ
 * @param[in] d_name PCフォルダー名
 * @param[in] file ファイル名
 * @param[in] dif 難易度タイプ
 * @details d_name を "asd"、file を "map.txt" とすると、"music/asd/map.txt" ファイルから楽曲を読み込む
 * @return なし
 */
static void FBDF_Select_MapLoadMusicGetDetail(
	std::vector<FBDF_music_detail_t> &detail, const FBDF_music_detail_base_st &music_detail_base,
	const char *d_name, const char *file, FBDF_dif_type_ec dif
) {
	FBDF_map_t map;
	FBDF_music_detail_t buf;
	std::string map_path;

	map_path  = "music/";
	map_path += d_name;
	map_path += '/';
	map_path += file;

	if (MapLoadOne(&map, map_path.c_str()) == -1) { return; }

	buf.folder_name        = d_name;
	buf.music_name         = d_name;
	buf.artist             = map.artist;
	buf.Length             = FBDF_CalMapLength(&map);
	buf.auto_cal_dif.notes = FBDF_CalMapNotesDif(&map);
	buf.auto_cal_dif.color = FBDF_CalMapColorDif(&map);
	buf.auto_cal_dif.trick = FBDF_CalMapTrickDif(&map);
	buf.auto_cal_dif.all   = (buf.auto_cal_dif.notes + buf.auto_cal_dif.color + buf.auto_cal_dif.trick) / 3;
	buf.user_dif           = music_detail_base.level;
	buf.map_file_name      = file;
	buf.dif_type           = dif;
	FBDF_CalMapMostColorPat(buf.most_colorpat, &map);
	FBDF_CountMapColor(&buf.color_count, &map, buf.Length);
	FBDF_Save_ReadScoreOneDif(&buf.user_highscore, d_name, dif);

	detail.push_back(buf);
}

/**
 * @brief PCフォルダー名から楽曲のリストを読み込む
 * @param[out] musiclist 譜面リスト
 * @param[in] d_name PCフォルダー名。 "asd" とすると "music/asd" フォルダーから楽曲のリストを読み込む
 * @return なし
 */
static void FBDF_Select_MapLoadMusic(FBDF_music_list_c *musiclist, const char *d_name) {
	char str_buf[256] = "";
	FBDF_map_t map;
	FBDF_music_detail_t buf;

	FBDF_music_detail_base_st detail_base[3];

	FILE *fp = NULL;

	std::string map_path;

	/* base.txtがあるか確認 */ {
		map_path  = "music/";
		map_path += d_name;
		map_path += "/base.txt";
		fopen_s(&fp, map_path.c_str(), "r");
		if (fp != NULL) {
			int write_mode = -1; /* -1:all, 0:light, 1:normal, 2:hyper */
			while (fgets(str_buf, 256, fp) != NULL) {
				if (strands(str_buf, "[all]")) {
					write_mode = -1;
				}
				else if (strands(str_buf, "[light]")) {
					write_mode = 0;
				}
				else if (strands(str_buf, "[normal]")) {
					write_mode = 1;
				}
				else if (strands(str_buf, "[hyper]")) {
					write_mode = 2;
				}
				else if (strands(str_buf, "NAME:")) {
					strmods(str_buf, 5);
					if (write_mode == -1) {
						detail_base[0].name = str_buf;
						/* 改行消し */
						for (int ic = 0; ic < detail_base[0].name.size(); ic++) {
							if (detail_base[0].name[ic] == '\n') {
								detail_base[0].name.pop_back();
							}
						}
						/* 日本語補正 */
						detail_base[0].name = UTF8_converter(detail_base[0].name);
						/* 複製 */
						detail_base[2].name = detail_base[1].name = detail_base[0].name;
					}
					else {
						detail_base[write_mode].name = str_buf;
						/* 改行消し */
						for (int ic = 0; ic < detail_base[write_mode].name.size(); ic++) {
							if (detail_base[write_mode].name[ic] == '\n') {
								detail_base[write_mode].name.pop_back();
							}
						}
						/* 日本語補正 */
						detail_base[write_mode].name = UTF8_converter(detail_base[write_mode].name);
					}
				}
				else if (strands(str_buf, "ARTIST:")) {
					strmods(str_buf, 7);
					if (write_mode == -1) {
						detail_base[0].artist = str_buf;
						/* 改行消し */
						for (int ic = 0; ic < detail_base[0].artist.size(); ic++) {
							if (detail_base[0].artist[ic] == '\n') {
								detail_base[0].artist.pop_back();
							}
						}
						/* 日本語補正 */
						detail_base[0].artist = UTF8_converter(detail_base[0].artist);
						/* 複製 */
						detail_base[2].artist = detail_base[1].artist = detail_base[0].artist;
					}
					else {
						detail_base[write_mode].artist = str_buf;
						/* 改行消し */
						for (int ic = 0; ic < detail_base[write_mode].artist.size(); ic++) {
							if (detail_base[write_mode].artist[ic] == '\n') {
								detail_base[write_mode].artist.pop_back();
							}
						}
						/* 日本語補正 */
						detail_base[write_mode].artist = UTF8_converter(detail_base[write_mode].artist);
					}
				}
				else if (strands(str_buf, "BPM:")) {
					strmods(str_buf, 4);
					if (write_mode == -1) {
						detail_base[2].BPM = detail_base[1].BPM = detail_base[0].BPM = strtod(str_buf, NULL);
					}
					else {
						detail_base[write_mode].BPM = strtod(str_buf, NULL);
					}
				}
				else if (strands(str_buf, "OFFSET:")) {
					strmods(str_buf, 7);
					if (write_mode == -1) {
						detail_base[2].offset = detail_base[1].offset = detail_base[0].offset = strtol(str_buf, NULL, 10);
					}
					else {
						detail_base[write_mode].offset = strtol(str_buf, NULL, 10);
					}
				}
				else if (strands(str_buf, "MAP:")) {
					strmods(str_buf, 4);
					if (write_mode == -1) {
						detail_base[1].map_path = str_buf;
						/* 改行消し */
						for (int ic = 1; ic < detail_base[1].map_path.size(); ic++) {
							if (detail_base[1].map_path[ic] == '\n') {
								detail_base[1].map_path.pop_back();
							}
						}
						/* 日本語補正 */
						detail_base[1].map_path = UTF8_converter(detail_base[1].map_path);
					}
					else {
						detail_base[write_mode].map_path = str_buf;
						/* 改行消し */
						for (int ic = 0; ic < detail_base[write_mode].map_path.size(); ic++) {
							if (detail_base[write_mode].map_path[ic] == '\n') {
								detail_base[write_mode].map_path.pop_back();
							}
						}
						/* 日本語補正 */
						detail_base[write_mode].map_path = UTF8_converter(detail_base[write_mode].map_path);
					}
				}
				else if (strands(str_buf, "LEVEL:")) {
					strmods(str_buf, 6);
					if (write_mode == -1) {
						detail_base[2].level = detail_base[1].level = detail_base[0].level = strtol(str_buf, NULL, 10);
					}
					else {
						detail_base[write_mode].level = strtol(str_buf, NULL, 10);
					}
				}
			}
			fclose(fp);
		}
		else { /* なかったらmap.txtだけ探す */
			detail_base[1].map_path = "map.txt";
		}
		FBDF_Select_MapLoadMusicGetDetail(musiclist->detail, detail_base[0], d_name, detail_base[0].map_path.c_str(), FBDF_dif_type_ec::LIGHT );
		FBDF_Select_MapLoadMusicGetDetail(musiclist->detail, detail_base[1], d_name, detail_base[1].map_path.c_str(), FBDF_dif_type_ec::NORMAL);
		FBDF_Select_MapLoadMusicGetDetail(musiclist->detail, detail_base[2], d_name, detail_base[2].map_path.c_str(), FBDF_dif_type_ec::HYPER );
	}

	return;
}

/**
 * @brief PCフォルダー内を調べて楽曲のリストを読み込む
 * @param[out] musiclist 譜面リスト
 * @return int 0=成功, -1=失敗
 */
static int FBDF_Select_LoadMusicList(FBDF_music_list_c *musiclist) {
	DIR *dir;
	struct dirent *dirs;
	dir = opendir("music");
	if (dir == NULL) { return -1; }

	while (1) {
		dirs = readdir(dir);
		if (dirs == NULL) { break; }
		if (dirs->d_name[0] == '.') { continue; }
		FBDF_Select_MapLoadMusic(musiclist, dirs->d_name);
	}

	closedir(dir);
	return 0;
}

#endif /* 譜面リスト読み込み系 */

/**
 * @brief セレクト画面のキー入力を管理する
 * @param[out] folder_manager フォルダーマネージャークラス
 * @param[out] now_misic 選択中の曲名
 * @param[out] command 今のカーソル位置
 * @param[out] view_dif_type 今の難易度表示
 * @param[out] musiclist 譜面リスト
 * @param[out] cutin カットイン管理クラス
 * @return なし
 */
static void FBDF_select_KeyCheck(
	FBDF_Select_MusicFolderManager_c &folder_manager,
	std::string &now_music,
	int &command,
	FBDF_dif_type_ec &view_dif_type,
	FBDF_music_list_c &musiclist,
	fbdf_cutin_c *cutin
) {
	size_t list_size = 0;
	size_t poped_cmd = 0;
	if (cutin->IsClosing()) { return; } /* カットイン中なのでキー入力無効 */

	InputAllKeyHold();
	switch (GetKeyPushOnce()) {
	case KEY_INPUT_RETURN:
		if (folder_manager.IsMusicFolderNow()) { /* 曲フォルダである */
			if (!musiclist.sort.empty()) { /* 曲フォルダの中が空じゃない */
				cutin->SetIo(CUT_FRAG_IN);
			}
		}
		else { /* サブフォルダである */
			folder_manager.PushFolder(command);
			folder_manager.MakeMusicList(musiclist, view_dif_type);
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
			folder_manager.MakeMusicList(musiclist, view_dif_type);
			command = poped_cmd;
		}
		break;
	case KEY_INPUT_UP:
		list_size = folder_str.size();
		command = MOD_AVOID_ZERO((command + list_size - 1), list_size, 0);
		if (folder_manager.IsMusicFolderNow() && !musiclist.sort.empty()) {
			now_music = musiclist[command].music_name;
		}
		break;
	case KEY_INPUT_DOWN:
		list_size = folder_str.size();
		command = MOD_AVOID_ZERO((command + 1), list_size, 0);
		if (folder_manager.IsMusicFolderNow() && !musiclist.sort.empty()) {
			now_music = musiclist[command].music_name;
		}
		break;
	case KEY_INPUT_LEFT:
		--view_dif_type;
		if (folder_manager.NowFolder()->name == "ALL MUSIC") {
			folder_manager.MakeMusicList(musiclist, view_dif_type);
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
			folder_manager.MakeMusicList(musiclist, view_dif_type);
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
	case KEY_INPUT_X:
		//monoFG ^= 1;
		break;
	}
}

/**
 * @brief セレクト画面のベース
 * @param[out] nex_music プレイ画面に渡すデータ
 * @return view_num_t 次の画面
 */
view_num_t FBDF_SelectView(FBDF::play_choose_music_st *nex_music) {
	int keyBlock = 1;
	int command = 0;
	FBDF_dif_type_ec view_dif_type = FBDF_dif_type_ec::LIGHT;
	//int monoFG = 0;

	bool exit_fg = false;

	std::string now_music;

	FBDF_Select_MusicFolderManager_c folder_manager_class;
	FBDF_music_list_c musiclist;

	FBDF_music_ber_pic_t music_ber_pic;
	FBDF_select_back_pic_c back_pic;

	fbdf_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	dxcur_snd_c backsnd(_T("SE/Starlights.mp3"));

	folder_manager_class.MakeMusicList(musiclist, view_dif_type); /* defaultフォルダで作られる想定 */

	if (FBDF_Select_LoadMusicList(&musiclist) != 0) { return VIEW_EXIT; }

	PlaySoundMem(backsnd.handle(), DX_PLAYTYPE_LOOP);

	cutin.SetIo(CUT_FRAG_OUT);

	while (1) {
		if (cutin.IsEndAnim()) { break; }

		FBDF_select_KeyCheck(folder_manager_class, now_music, command, view_dif_type, musiclist, &cutin);

		if (exit_fg) { break; }

		back_pic.UpdateState();

		cutin.update();

		ClearDrawScreen(); // 作画エリアここから
		back_pic.DrawPic();
		DrawFormatString(5,  25, 0xffffffff, _T("%d"), command);
		DrawFormatString(5,  45, 0xffffffff, _T("%d"), keyBlock);
		if (folder_manager_class.IsMusicFolderNow() && !musiclist.sort.empty()) {
			DrawFormatString(5,  65, 0xffffffff, _T("%3.2f"), musiclist[command].auto_cal_dif.notes);
			DrawFormatString(5,  85, 0xffffffff, _T("%3.2f"), musiclist[command].auto_cal_dif.color);
			DrawFormatString(5, 105, 0xffffffff, _T("%3.2f"), musiclist[command].auto_cal_dif.trick);
			DrawFormatString(5, 125, 0xffffffff, _T("%3.2f"), musiclist[command].auto_cal_dif.all);
			DrawFormatString(5, 145, 0xffffffff, _T("score: %d"), musiclist[command].user_highscore.score);
			DrawFormatString(5, 165, 0xffffffff, _T("acc: %6.2f"), musiclist[command].user_highscore.acc);
			DrawFormatString(5, 185, 0xffffffff, _T("clear type: %d"), musiclist[command].user_highscore.clear_type);
			FBDF_SelectDrawColorCount(5, 660, &(musiclist[command].color_count));
		}
		FBDF_SelectDrawMusicList(command, &music_ber_pic);

		cutin.DrawCut();

		ScreenFlip(); // 作画エリアここまで
		if (GetWindowUserCloseFlag(TRUE)) { // 閉じるボタンが押された
			return VIEW_EXIT;
		}
		WaitTimer(10); // ループウェイト
	}

	nex_music->folder_name   = musiclist[command].folder_name;
	nex_music->map_file_name = musiclist[command].map_file_name;
	nex_music->music_name    = musiclist[command].music_name;
	nex_music->dif_type      = musiclist[command].dif_type;
	return VIEW_PLAY;
}
