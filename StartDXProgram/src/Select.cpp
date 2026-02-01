
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

#include <Play.h>

/* numがtarget±gap以内であればtrueを返すdefine */
#define IS_NEAR_NUM(num, target, gap) (((target) - (gap)) <= (num) && (num) <= ((target) + (gap)))

#define MOST_COLORPAT_NUM 10

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

typedef enum FBDF_color_pat_mat_e {
	COLOR_PAT_NONE = 0,
	COLOR_PAT_111,
	COLOR_PAT_112,
	COLOR_PAT_113,
	COLOR_PAT_114,
	COLOR_PAT_121,
	COLOR_PAT_122,
	COLOR_PAT_123,
	COLOR_PAT_124,
	COLOR_PAT_131,
	COLOR_PAT_132,
	COLOR_PAT_133,
	COLOR_PAT_134,
	COLOR_PAT_141,
	COLOR_PAT_142,
	COLOR_PAT_143,
	COLOR_PAT_144,
	COLOR_PAT_211,
	COLOR_PAT_212,
	COLOR_PAT_213,
	COLOR_PAT_214,
	COLOR_PAT_221,
	COLOR_PAT_222,
	COLOR_PAT_223,
	COLOR_PAT_224,
	COLOR_PAT_231,
	COLOR_PAT_232,
	COLOR_PAT_233,
	COLOR_PAT_234,
	COLOR_PAT_241,
	COLOR_PAT_242,
	COLOR_PAT_243,
	COLOR_PAT_244,
	COLOR_PAT_311,
	COLOR_PAT_312,
	COLOR_PAT_313,
	COLOR_PAT_314,
	COLOR_PAT_321,
	COLOR_PAT_322,
	COLOR_PAT_323,
	COLOR_PAT_324,
	COLOR_PAT_331,
	COLOR_PAT_332,
	COLOR_PAT_333,
	COLOR_PAT_334,
	COLOR_PAT_341,
	COLOR_PAT_342,
	COLOR_PAT_343,
	COLOR_PAT_344,
	COLOR_PAT_411,
	COLOR_PAT_412,
	COLOR_PAT_413,
	COLOR_PAT_414,
	COLOR_PAT_421,
	COLOR_PAT_422,
	COLOR_PAT_423,
	COLOR_PAT_424,
	COLOR_PAT_431,
	COLOR_PAT_432,
	COLOR_PAT_433,
	COLOR_PAT_434,
	COLOR_PAT_441,
	COLOR_PAT_442,
	COLOR_PAT_443,
	COLOR_PAT_444
} FBDF_color_pat_mat_t;

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

typedef struct FBDF_music_colorpat_count_s {
	uint pat111 = 0;
	uint pat112 = 0;
	uint pat113 = 0;
	uint pat114 = 0;
	uint pat121 = 0;
	uint pat122 = 0;
	uint pat123 = 0;
	uint pat124 = 0;
	uint pat131 = 0;
	uint pat132 = 0;
	uint pat133 = 0;
	uint pat134 = 0;
	uint pat141 = 0;
	uint pat142 = 0;
	uint pat143 = 0;
	uint pat144 = 0;
	uint pat211 = 0;
	uint pat212 = 0;
	uint pat213 = 0;
	uint pat214 = 0;
	uint pat221 = 0;
	uint pat222 = 0;
	uint pat223 = 0;
	uint pat224 = 0;
	uint pat231 = 0;
	uint pat232 = 0;
	uint pat233 = 0;
	uint pat234 = 0;
	uint pat241 = 0;
	uint pat242 = 0;
	uint pat243 = 0;
	uint pat244 = 0;
	uint pat311 = 0;
	uint pat312 = 0;
	uint pat313 = 0;
	uint pat314 = 0;
	uint pat321 = 0;
	uint pat322 = 0;
	uint pat323 = 0;
	uint pat324 = 0;
	uint pat331 = 0;
	uint pat332 = 0;
	uint pat333 = 0;
	uint pat334 = 0;
	uint pat341 = 0;
	uint pat342 = 0;
	uint pat343 = 0;
	uint pat344 = 0;
	uint pat411 = 0;
	uint pat412 = 0;
	uint pat413 = 0;
	uint pat414 = 0;
	uint pat421 = 0;
	uint pat422 = 0;
	uint pat423 = 0;
	uint pat424 = 0;
	uint pat431 = 0;
	uint pat432 = 0;
	uint pat433 = 0;
	uint pat434 = 0;
	uint pat441 = 0;
	uint pat442 = 0;
	uint pat443 = 0;
	uint pat444 = 0;
} FBDF_music_colorpat_count_t;

typedef struct FBDF_music_most_colorpat_s {
	FBDF_color_pat_mat_t mat = COLOR_PAT_NONE;
	uint count = 0;
} FBDF_music_most_colorpat_t;

typedef struct FBDF_music_colorcount_s {
	double c1 = 0;
	double c2 = 0;
	double c3 = 0;
	double c4 = 0;
} FBDF_music_colorcount_t;

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

class FBDF_Select_MusicFolderManager_c {
private:
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

#if 1 /* PushFolder系 */

	void PushFolderDefault(int cmd) {
		switch (cmd) {
		case 0:
			this->folder_stack.push(ALL_MUSIC_FOLDER);
			break;
		case 1:
			this->folder_stack.push(LEVEL_SET_MUSIC_FOLDER);
			break;
		case 2:
			this->folder_stack.push(SCORE_SET_MUSIC_FOLDER);
			break;
		case 3:
			this->folder_stack.push(CLEARTYPE_SET_MUSIC_FOLDER);
			break;
		}
	}

	void PushFolderLevel(int cmd) {
		switch (cmd) {
		case 0:
			this->folder_stack.push(LEVEL0_MUSIC_FOLDER);
			break;
		case 1:
			this->folder_stack.push(LEVEL1_MUSIC_FOLDER);
			break;
		case 2:
			this->folder_stack.push(LEVEL2_MUSIC_FOLDER);
			break;
		case 3:
			this->folder_stack.push(LEVEL3_MUSIC_FOLDER);
			break;
		case 4:
			this->folder_stack.push(LEVEL4_MUSIC_FOLDER);
			break;
		case 5:
			this->folder_stack.push(LEVEL5_MUSIC_FOLDER);
			break;
		case 6:
			this->folder_stack.push(LEVEL6_MUSIC_FOLDER);
			break;
		case 7:
			this->folder_stack.push(LEVEL7_MUSIC_FOLDER);
			break;
		case 8:
			this->folder_stack.push(LEVEL8_MUSIC_FOLDER);
			break;
		case 9:
			this->folder_stack.push(LEVEL9_MUSIC_FOLDER);
			break;
		case 10:
			this->folder_stack.push(LEVEL10_MUSIC_FOLDER);
			break;
		}
	}

	void PushFolderScore(int cmd) {
		switch (cmd) {
		case 0:
			this->folder_stack.push(SCORE_P_MUSIC_FOLDER);
			break;
		case 1:
			this->folder_stack.push(SCORE_XP_MUSIC_FOLDER);
			break;
		case 2:
			this->folder_stack.push(SCORE_X_MUSIC_FOLDER);
			break;
		case 3:
			this->folder_stack.push(SCORE_SP_MUSIC_FOLDER);
			break;
		case 4:
			this->folder_stack.push(SCORE_S_MUSIC_FOLDER);
			break;
		case 5:
			this->folder_stack.push(SCORE_AP_MUSIC_FOLDER);
			break;
		case 6:
			this->folder_stack.push(SCORE_A_MUSIC_FOLDER);
			break;
		case 7:
			this->folder_stack.push(SCORE_B_MUSIC_FOLDER);
			break;
		case 8:
			this->folder_stack.push(SCORE_C_MUSIC_FOLDER);
			break;
		case 9:
			this->folder_stack.push(SCORE_D_MUSIC_FOLDER);
			break;
		case 10:
			this->folder_stack.push(SCORE_F_MUSIC_FOLDER);
			break;
		}
	}

	void PushFolderClearType(int cmd) {
		switch (cmd) {
		case 0:
			this->folder_stack.push(CLEARTYPE_PERFECT_MUSIC_FOLDER);
			break;
		case 1:
			this->folder_stack.push(CLEARTYPE_FULLCOMBO_MUSIC_FOLDER);
			break;
		case 2:
			this->folder_stack.push(CLEARTYPE_MISSLESS_MUSIC_FOLDER);
			break;
		case 3:
			this->folder_stack.push(CLEARTYPE_CAKEWALK_MUSIC_FOLDER);
			break;
		case 4:
			this->folder_stack.push(CLEARTYPE_CLEARED_MUSIC_FOLDER);
			break;
		case 5:
			this->folder_stack.push(CLEARTYPE_FAILED_MUSIC_FOLDER);
			break;
		case 6:
			this->folder_stack.push(CLEARTYPE_NOPLAY_MUSIC_FOLDER);
			break;
		}
	}

#endif /* PushFolder系 */

#if 1 /* サブリスト作成系 */

	void MakeListDefault(void) const {
		folder_str.clear();
		folder_str.push_back("all");
		folder_str.push_back("level");
		folder_str.push_back("score");
		folder_str.push_back("clear type");
	}

	void MakeListLevel(void) const {
		folder_str.clear();
		folder_str.push_back("level under 0");
		folder_str.push_back("level 1");
		folder_str.push_back("level 2");
		folder_str.push_back("level 3");
		folder_str.push_back("level 4");
		folder_str.push_back("level 5");
		folder_str.push_back("level 6");
		folder_str.push_back("level 7");
		folder_str.push_back("level 8");
		folder_str.push_back("level 9");
		folder_str.push_back("level over 10");
	}

	void MakeListScore(void) const {
		folder_str.clear();
		folder_str.push_back("score P");
		folder_str.push_back("score X+");
		folder_str.push_back("score X");
		folder_str.push_back("score S+");
		folder_str.push_back("score S");
		folder_str.push_back("score A+");
		folder_str.push_back("score A");
		folder_str.push_back("score B");
		folder_str.push_back("score C");
		folder_str.push_back("score D");
		folder_str.push_back("score F");
	}

	void MakeListClearType(void) const {
		folder_str.clear();
		folder_str.push_back("perfect");
		folder_str.push_back("full chain");
		folder_str.push_back("missless");
		folder_str.push_back("cakewalk");
		folder_str.push_back("cleared");
		folder_str.push_back("failed");
		folder_str.push_back("no play");
	}

#endif /* サブリスト作成系 */

	/**
	 * @brief 絞り込み条件から譜面リストを作る
	 * @param[out] musiclist 譜面リスト
	 * @param[in] view_dif_type 今の難易度表示
	 * @return なし
	 */
	void MakeMusicListDetectMusic(FBDF_music_list_c &musiclist, FBDF_dif_type_ec view_dif_type) {
		for (int i = 0; i < musiclist.detail.size(); i++) {
			bool detect_fg = false;
			switch (this->folder_stack.top()) {
			case ALL_MUSIC_FOLDER:
				detect_fg = (musiclist.detail[i].dif_type == view_dif_type); /* 難易度フィルタのみ */
				break;
			case LEVEL0_MUSIC_FOLDER:
				detect_fg = (musiclist.detail[i].auto_cal_dif.all < 1);
				break;
			case LEVEL1_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(1, musiclist.detail[i].auto_cal_dif.all, 2));
				break;
			case LEVEL2_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(2, musiclist.detail[i].auto_cal_dif.all, 3));
				break;
			case LEVEL3_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(3, musiclist.detail[i].auto_cal_dif.all, 4));
				break;
			case LEVEL4_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(4, musiclist.detail[i].auto_cal_dif.all, 5));
				break;
			case LEVEL5_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(5, musiclist.detail[i].auto_cal_dif.all, 6));
				break;
			case LEVEL6_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(6, musiclist.detail[i].auto_cal_dif.all, 7));
				break;
			case LEVEL7_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(7, musiclist.detail[i].auto_cal_dif.all, 8));
				break;
			case LEVEL8_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(8, musiclist.detail[i].auto_cal_dif.all, 9));
				break;
			case LEVEL9_MUSIC_FOLDER:
				detect_fg = (IS_BETWEEN_RIGHT_LESS(9, musiclist.detail[i].auto_cal_dif.all, 10));
				break;
			case LEVEL10_MUSIC_FOLDER:
				detect_fg = (10 <= musiclist.detail[i].auto_cal_dif.all);
				break;
			case SCORE_P_MUSIC_FOLDER:
				detect_fg = (FBDF_SCORE_RANK_P_BORDER <= musiclist.detail[i].user_highscore.acc);
				break;
			case SCORE_XP_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_XP_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_P_BORDER);
				break;
			case SCORE_X_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_X_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_XP_BORDER);
				break;
			case SCORE_SP_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_SP_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_X_BORDER);
				break;
			case SCORE_S_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_S_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_SP_BORDER);
				break;
			case SCORE_AP_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_AP_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_S_BORDER);
				break;
			case SCORE_A_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_A_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_AP_BORDER);
				break;
			case SCORE_B_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_B_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_A_BORDER);
				break;
			case SCORE_C_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_C_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_B_BORDER);
				break;
			case SCORE_D_MUSIC_FOLDER:
				detect_fg = IS_BETWEEN_RIGHT_LESS(FBDF_SCORE_RANK_D_BORDER, musiclist.detail[i].user_highscore.acc, FBDF_SCORE_RANK_C_BORDER);
				break;
			case SCORE_F_MUSIC_FOLDER:
				detect_fg = (musiclist.detail[i].user_highscore.acc < FBDF_SCORE_RANK_D_BORDER);
				break;
			case CLEARTYPE_PERFECT_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_PERFECT;
				break;
			case CLEARTYPE_FULLCOMBO_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_FULLCOMBO;
				break;
			case CLEARTYPE_MISSLESS_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_MISSLESS;
				break;
			case CLEARTYPE_CAKEWALK_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_CAKEWALK;
				break;
			case CLEARTYPE_CLEARED_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_CLEARED;
				break;
			case CLEARTYPE_FAILED_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_FAILED;
				break;
			case CLEARTYPE_NOPLAY_MUSIC_FOLDER:
				detect_fg = musiclist.detail[i].user_highscore.clear_type == FBDF_CLEAR_TYPE_NOPLAY;
				break;
			default:
				return;
			}
			if (detect_fg) {
				musiclist.sort.push_back(i);
			}
		}
	}

public:
	std::stack<FBDF_music_folder_num_t> folder_stack;

	FBDF_Select_MusicFolderManager_c(void) {
		this->folder_stack.push(DEFAULT_MUSIC_FOLDER);
	}

	bool IsMusicFolderNow(void) const {
		switch (this->folder_stack.top()) {
		case DEFAULT_MUSIC_FOLDER:
		case LEVEL_SET_MUSIC_FOLDER:
		case SCORE_SET_MUSIC_FOLDER:
		case CLEARTYPE_SET_MUSIC_FOLDER:
			return false;
			break;
		case ALL_MUSIC_FOLDER:
		case LEVEL0_MUSIC_FOLDER:
		case LEVEL1_MUSIC_FOLDER:
		case LEVEL2_MUSIC_FOLDER:
		case LEVEL3_MUSIC_FOLDER:
		case LEVEL4_MUSIC_FOLDER:
		case LEVEL5_MUSIC_FOLDER:
		case LEVEL6_MUSIC_FOLDER:
		case LEVEL7_MUSIC_FOLDER:
		case LEVEL8_MUSIC_FOLDER:
		case LEVEL9_MUSIC_FOLDER:
		case LEVEL10_MUSIC_FOLDER:
		case SCORE_P_MUSIC_FOLDER:
		case SCORE_XP_MUSIC_FOLDER:
		case SCORE_X_MUSIC_FOLDER:
		case SCORE_SP_MUSIC_FOLDER:
		case SCORE_S_MUSIC_FOLDER:
		case SCORE_AP_MUSIC_FOLDER:
		case SCORE_A_MUSIC_FOLDER:
		case SCORE_B_MUSIC_FOLDER:
		case SCORE_C_MUSIC_FOLDER:
		case SCORE_D_MUSIC_FOLDER:
		case SCORE_F_MUSIC_FOLDER:
		case CLEARTYPE_PERFECT_MUSIC_FOLDER:
		case CLEARTYPE_FULLCOMBO_MUSIC_FOLDER:
		case CLEARTYPE_MISSLESS_MUSIC_FOLDER:
		case CLEARTYPE_CAKEWALK_MUSIC_FOLDER:
		case CLEARTYPE_CLEARED_MUSIC_FOLDER:
		case CLEARTYPE_FAILED_MUSIC_FOLDER:
		case CLEARTYPE_NOPLAY_MUSIC_FOLDER:
			return true;
			break;
		}
		return false;
	}

	void PushFolder(int cmd) {
		switch (this->folder_stack.top()) {
		case DEFAULT_MUSIC_FOLDER:
			this->PushFolderDefault(cmd);
			break;
		case LEVEL_SET_MUSIC_FOLDER:
			this->PushFolderLevel(cmd);
			break;
		case SCORE_SET_MUSIC_FOLDER:
			this->PushFolderScore(cmd);
			break;
		case CLEARTYPE_SET_MUSIC_FOLDER:
			this->PushFolderClearType(cmd);
			break;
		default:
			break;
		}
	}

	FBDF_music_folder_num_t NowFolder(void) const {
		return this->folder_stack.top();
	}

	/**
	 * @return bool true=実行した, false=実行しなかった
	 */
	bool PopFolder(void) {
		if (this->NowFolder() == DEFAULT_MUSIC_FOLDER) { return false; }
		this->folder_stack.pop();
		return true;
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
			switch (this->folder_stack.top()) {
			case DEFAULT_MUSIC_FOLDER:
				this->MakeListDefault();
				break;
			case LEVEL_SET_MUSIC_FOLDER:
				this->MakeListLevel();
				break;
			case SCORE_SET_MUSIC_FOLDER:
				this->MakeListScore();
				break;
			case CLEARTYPE_SET_MUSIC_FOLDER:
				this->MakeListClearType();
				break;
			default:
				break;
			}
		}
		return;
	}
};

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

#if 1 /* 譜面情報系 */

/* 譜面の長さを計算する */
static uint FBDF_CalMapLength(const FBDF_map_t *map) {
	if (map->note.size() < 3) { return 0; }
	return map->note[map->note.size() - 2].time - map->note[0].time;
}

/* notes難易度を計算する */
static double FBDF_CalMapNotesDif(FBDF_music_detail_t *det, const FBDF_map_t *map) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (map->note.size() < 3) { return 0; }

	for (uint i = 2; i < map->note.size(); i++) {
		int FirstTime  = map->note[i].time     - map->note[i - 1].time;
		int SecondTime = map->note[i - 1].time - map->note[i - 2].time;

		uint BasePoint = 10;

		DxTime_t TimeGap = 0;
		TimeGap = 2000 / maxs_2(1, map->note[i].time - map->note[i - 1].time);
		BasePoint *= TimeGap;
		BasePointQueue.push_back(BasePoint);
		if (50 <= BasePointQueue.size()) {
			BasePointQueue.erase(BasePointQueue.begin());
		}

		double NowDif = 0.0;
		double mlp = 100.0;
		for (int iq = BasePointQueue.size() - 1; 0 <= iq; iq--) {
			NowDif += BasePointQueue[iq] * mlp;
			mlp *= 0.95;
		}

		if (ret < NowDif) {
			ret = NowDif;
		}
	}

	ret = lins(110000, 1, 310000, 10, ret);

	return ret;
}

/* color難易度を計算する */
static double FBDF_CalMapColorDif(FBDF_music_detail_t *det, const FBDF_map_t *map) {
	const int point_stairLen =  2;
	const int point_step_11  = 10;
	const int point_step_13  = 12;
	const int point_step_33  = 13;
	const int point_3_hop1   = 14;
	const int point_stair1   = 17;
	const int point_3_hop3   = 21;
	const int point_3_hop4   = 21;
	const int point_step_22  = 24;
	const int point_step_44  = 24;
	const int point_stair3   = 27;
	const int point_step_24  = 31;
	const int point_3_hop2   = 34;
	const int point_stair2   = 40;
	const int point_stair4   = 40;

	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (map->note.size() < 3) { return 0; }

	for (uint i = 2; i < map->note.size(); i++) {
		int FirstTime  = map->note[i].time     - map->note[i - 1].time;
		int SecondTime = map->note[i - 1].time - map->note[i - 2].time;
		uint BasePoint;
		bool skipFG = false;

		switch (map->note[i].btn) {
		case 1:
			switch (map->note[i - 1].btn) {
			case 1: /* 11ステップ */
				BasePoint = point_step_11;
				break;
			case 2: /* 3空き */
				BasePoint = point_3_hop1;
				break;
			case 3: /* 13ステップ */
				BasePoint = point_step_13;
				break;
			case 4: /* 階段 */
				if (map->note[i - 2].btn != 3) {
					BasePoint = point_stair4;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		case 2:
			switch (map->note[i - 1].btn) {
			case 2: /* 22ステップ */
				BasePoint = point_step_22;
				break;
			case 3: /* 3空き */
				BasePoint = point_3_hop2;
				break;
			case 4: /* 24ステップ */
				BasePoint = point_step_24;
				break;
			case 1: /* 階段 */
				if (map->note[i - 2].btn != 4) {
					BasePoint = point_stair1;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		case 3:
			switch (map->note[i - 1].btn) {
			case 3: /* 33ステップ */
				BasePoint = point_step_33;
				break;
			case 4: /* 3空き */
				BasePoint = point_3_hop3;
				break;
			case 1: /* 13ステップ */
				BasePoint = point_step_13;
				break;
			case 2: /* 階段 */
				if (map->note[i - 2].btn != 3) {
					BasePoint = point_stair2;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		case 4:
			switch (map->note[i - 1].btn) {
			case 4: /* 44ステップ */
				BasePoint = point_step_44;
				break;
			case 1: /* 3空き */
				BasePoint = point_3_hop4;
				break;
			case 2: /* 24ステップ */
				BasePoint = point_step_24;
				break;
			case 3: /* 階段 */
				if (map->note[i - 2].btn != 2) {
					BasePoint = point_stair3;
				}
				else {
					BasePoint = point_stairLen;
				}
				break;
			}
			break;
		default:
			BasePoint = 0;
		}

#if 1
		DxTime_t TimeGap = 0;
		TimeGap = 2000 / maxs_2(1, map->note[i].time - map->note[i - 1].time);
		BasePoint *= TimeGap;
#endif
		BasePointQueue.push_back(BasePoint);
		if (50 <= BasePointQueue.size()) {
			BasePointQueue.erase(BasePointQueue.begin());
		}

		double NowDif = 0.0;
		double mlp = 100.0;
		for (int iq = BasePointQueue.size() - 1; 0 <= iq; iq--) {
			NowDif += BasePointQueue[iq] * mlp;
			mlp *= 0.95;
		}

		if (ret < NowDif) {
			ret = NowDif;
		}
	}

	ret = lins(130000, 1, 570000, 8, ret);

	return ret;
}

/* trick難易度を計算する */
static double FBDF_CalMapTrickDif(FBDF_music_detail_t *det, const FBDF_map_t *map) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (map->note.size() < 3) { return 0; }

	for (uint i = 2; i < map->note.size(); i++) {
		int FirstTime  = map->note[i].time     - map->note[i - 1].time;
		int SecondTime = map->note[i - 1].time - map->note[i - 2].time;
		uint BasePoint;
		float Gap = 0.0;

		if (FirstTime < SecondTime) {
			int temp = FirstTime;
			FirstTime = SecondTime;
			SecondTime = temp;
		}

		Gap = (float)FirstTime / (float)SecondTime;

		/* レコランのtrickから取ってきたけど、リズム難の比重が大きすぎたから下方修正してる */
		if (7 < Gap) {
			BasePoint = 2; /* 2 */
		}
		else if (IS_NEAR_NUM(Gap, 1.000, 0.1)) {
			BasePoint = 5; /* 5 */
		}
		else if (IS_NEAR_NUM(Gap, 2.000, 0.1)) {
			BasePoint = 10; /* 10 */
		}
		else if (IS_NEAR_NUM(Gap, 1.500, 0.1)) {
			BasePoint = 13; /* 13 */
		}
		else if (IS_NEAR_NUM(Gap, 6.000, 0.1)) {
			BasePoint = 14; /* 14 */
		}
		else if (IS_NEAR_NUM(Gap, 3.000, 0.1)) {
			BasePoint = 15; /* 15 */
		}
		else if (IS_NEAR_NUM(Gap, 7.000, 0.1)) {
			BasePoint = 17; /* 17 */
		}
		else if (IS_NEAR_NUM(Gap, 4.000, 0.1)) {
			BasePoint = 19; /* 19 */
		}
		else if (IS_NEAR_NUM(Gap, 1.333, 0.1)) {
			BasePoint = 19; /* 19 */
		}
		else if (IS_NEAR_NUM(Gap, 5.000, 0.1)) {
			BasePoint = 23; /* 23 */
		}
		else if (IS_NEAR_NUM(Gap, 2.500, 0.1)) {
			BasePoint = 23; /* 23 */
		}
		else if (IS_NEAR_NUM(Gap, 1.667, 0.1)) {
			BasePoint = 27; /* 27 */
		}
		else {
			BasePoint = 30; /* 30 */
		}

#if 1
		DxTime_t TimeGap = 0;
		TimeGap = 2000 / maxs_2(1, map->note[i].time - map->note[i - 1].time);
		BasePoint *= TimeGap;
#endif
		BasePointQueue.push_back(BasePoint);
		if (50 <= BasePointQueue.size()) {
			BasePointQueue.erase(BasePointQueue.begin());
		}

		double NowDif = 0.0;
		double mlp = 100.0;
		for (int iq = BasePointQueue.size() - 1; 0 <= iq; iq--) {
			NowDif += BasePointQueue[iq] * mlp;
			mlp *= 0.95;
		}

		if (ret < NowDif) {
			ret = NowDif;
		}
	}

	ret = lins(86000, 1, 240000, 8, ret);

	return ret;
}

/* カラーパターンを数える */
static void FBDF_CountMapColorPat(FBDF_music_colorpat_count_t *pat, const FBDF_map_t *map) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (map->note.size() < 3) { return; }

	for (uint i = 2; i < map->note.size(); i++) {
		switch (map->note[i - 2].btn) {
		case 1:
			switch (map->note[i - 1].btn) {
			case 1:
				switch (map->note[i].btn) {
				case 1:
					pat->pat111++;
					break;
				case 2:
					pat->pat112++;
					break;
				case 3:
					pat->pat113++;
					break;
				case 4:
					pat->pat114++;
					break;
				}
				break;
			case 2:
				switch (map->note[i].btn) {
				case 1:
					pat->pat121++;
					break;
				case 2:
					pat->pat122++;
					break;
				case 3:
					pat->pat123++;
					break;
				case 4:
					pat->pat124++;
					break;
				}
				break;
			case 3:
				switch (map->note[i].btn) {
				case 1:
					pat->pat131++;
					break;
				case 2:
					pat->pat132++;
					break;
				case 3:
					pat->pat133++;
					break;
				case 4:
					pat->pat134++;
					break;
				}
				break;
			case 4:
				switch (map->note[i].btn) {
				case 1:
					pat->pat141++;
					break;
				case 2:
					pat->pat142++;
					break;
				case 3:
					pat->pat143++;
					break;
				case 4:
					pat->pat144++;
					break;
				}
				break;
			}
			break;
		case 2:
			switch (map->note[i - 1].btn) {
			case 1:
				switch (map->note[i].btn) {
				case 1:
					pat->pat211++;
					break;
				case 2:
					pat->pat212++;
					break;
				case 3:
					pat->pat213++;
					break;
				case 4:
					pat->pat214++;
					break;
				}
				break;
			case 2:
				switch (map->note[i].btn) {
				case 1:
					pat->pat221++;
					break;
				case 2:
					pat->pat222++;
					break;
				case 3:
					pat->pat223++;
					break;
				case 4:
					pat->pat224++;
					break;
				}
				break;
			case 3:
				switch (map->note[i].btn) {
				case 1:
					pat->pat231++;
					break;
				case 2:
					pat->pat232++;
					break;
				case 3:
					pat->pat233++;
					break;
				case 4:
					pat->pat234++;
					break;
				}
				break;
			case 4:
				switch (map->note[i].btn) {
				case 1:
					pat->pat241++;
					break;
				case 2:
					pat->pat242++;
					break;
				case 3:
					pat->pat243++;
					break;
				case 4:
					pat->pat244++;
					break;
				}
				break;
			}
			break;
		case 3:
			switch (map->note[i - 1].btn) {
			case 1:
				switch (map->note[i].btn) {
				case 1:
					pat->pat311++;
					break;
				case 2:
					pat->pat312++;
					break;
				case 3:
					pat->pat313++;
					break;
				case 4:
					pat->pat314++;
					break;
				}
				break;
			case 2:
				switch (map->note[i].btn) {
				case 1:
					pat->pat321++;
					break;
				case 2:
					pat->pat322++;
					break;
				case 3:
					pat->pat323++;
					break;
				case 4:
					pat->pat324++;
					break;
				}
				break;
			case 3:
				switch (map->note[i].btn) {
				case 1:
					pat->pat331++;
					break;
				case 2:
					pat->pat332++;
					break;
				case 3:
					pat->pat333++;
					break;
				case 4:
					pat->pat334++;
					break;
				}
				break;
			case 4:
				switch (map->note[i].btn) {
				case 1:
					pat->pat341++;
					break;
				case 2:
					pat->pat342++;
					break;
				case 3:
					pat->pat343++;
					break;
				case 4:
					pat->pat344++;
					break;
				}
				break;
			}
			break;
		case 4:
			switch (map->note[i - 1].btn) {
			case 1:
				switch (map->note[i].btn) {
				case 1:
					pat->pat411++;
					break;
				case 2:
					pat->pat412++;
					break;
				case 3:
					pat->pat413++;
					break;
				case 4:
					pat->pat414++;
					break;
				}
				break;
			case 2:
				switch (map->note[i].btn) {
				case 1:
					pat->pat421++;
					break;
				case 2:
					pat->pat422++;
					break;
				case 3:
					pat->pat423++;
					break;
				case 4:
					pat->pat424++;
					break;
				}
				break;
			case 3:
				switch (map->note[i].btn) {
				case 1:
					pat->pat431++;
					break;
				case 2:
					pat->pat432++;
					break;
				case 3:
					pat->pat433++;
					break;
				case 4:
					pat->pat434++;
					break;
				}
				break;
			case 4:
				switch (map->note[i].btn) {
				case 1:
					pat->pat441++;
					break;
				case 2:
					pat->pat442++;
					break;
				case 3:
					pat->pat443++;
					break;
				case 4:
					pat->pat444++;
					break;
				}
				break;
			}
			break;
		}
	}

	return;
}

/* 頻出のカラーパターンを調べる */
static void FBDF_CalMapMostColorPat(FBDF_music_most_colorpat_t *mostpat, const FBDF_map_t *map) {
	FBDF_music_colorpat_count_t pat;
	FBDF_CountMapColorPat(&pat, map);
	for (int ip = 0; ip < 63; ip++) {
		switch (ip) {

#define cat(num, pattan)                                               \
	case num:                                                          \
		if (mostpat[MOST_COLORPAT_NUM - 1].count <  pat.pat##pattan) { \
			mostpat[MOST_COLORPAT_NUM - 1].count =  pat.pat##pattan;   \
			mostpat[MOST_COLORPAT_NUM - 1].mat = COLOR_PAT_##pattan;   \
		}                                                              \
	break

			//cat( 0, 111);
			cat( 1, 112);
			//cat( 2, 113);
			cat( 3, 114);
			cat( 4, 121);
			cat( 5, 122);
			//cat( 6, 123);
			cat( 7, 124);
			//cat( 8, 131);
			cat( 9, 132);
			//cat(10, 133);
			cat(11, 134);
			cat(12, 141);
			cat(13, 142);
			cat(14, 143);
			cat(15, 144);
			cat(16, 211);
			cat(17, 212);
			cat(18, 213);
			cat(19, 214);
			cat(20, 221);
			cat(21, 222);
			cat(22, 223);
			cat(23, 224);
			cat(24, 231);
			cat(25, 232);
			cat(26, 233);
			//cat(27, 234);
			cat(28, 241);
			cat(29, 242);
			cat(30, 243);
			cat(31, 244);
			//cat(32, 311);
			cat(33, 312);
			//cat(34, 313);
			cat(35, 314);
			cat(36, 321);
			cat(37, 322);
			cat(38, 323);
			cat(39, 324);
			//cat(40, 331);
			cat(41, 332);
			//cat(42, 333);
			cat(43, 334);
			//cat(44, 341);
			cat(45, 342);
			cat(46, 343);
			cat(47, 344);
			cat(48, 411);
			//cat(49, 412);
			cat(50, 413);
			cat(51, 414);
			cat(52, 421);
			cat(53, 422);
			cat(54, 423);
			cat(55, 424);
			cat(56, 431);
			cat(57, 432);
			cat(58, 433);
			cat(59, 434);
			cat(60, 441);
			cat(61, 442);
			cat(62, 443);
			cat(63, 444);
		default:
			break;

#undef cat

		}
		for (int in = MOST_COLORPAT_NUM - 1; 1 <= in; in--) {
			if (mostpat[in].count <= mostpat[in - 1].count) { break; }
			FBDF_music_most_colorpat_t temp = mostpat[in];
			mostpat[in] = mostpat[in - 1];
			mostpat[in - 1] = temp;
		}
	}
	return;
}

/* 色の出現頻度を数える */
static void FBDF_CountMapColor(FBDF_music_colorcount_t *count, const FBDF_map_t *map, uint Length) {
	double ret = 0.0;
	std::vector<uint> BasePointQueue;

	if (map->note.size() < 3) { return; }

	for (uint i = 0; i < map->note.size(); i++) {
		switch (map->note[i].btn) {
		case 1:
			count->c1++;
			break;
		case 2:
			count->c2++;
			break;
		case 3:
			count->c3++;
			break;
		case 4:
			count->c4++;
			break;
		}
	}

	count->c1 = DIV_AVOID_ZERO(count->c1, Length / 10000.0, 0);
	count->c2 = DIV_AVOID_ZERO(count->c2, Length / 10000.0, 0);
	count->c3 = DIV_AVOID_ZERO(count->c3, Length / 10000.0, 0);
	count->c4 = DIV_AVOID_ZERO(count->c4, Length / 10000.0, 0);

	return;
}

#endif /* 譜面情報系 */

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

	buf.folder_name = d_name;
	buf.music_name = d_name;
	buf.artist = map.artist;
	buf.Length = FBDF_CalMapLength(&map);
	buf.auto_cal_dif.notes = FBDF_CalMapNotesDif(&buf, &map);
	buf.auto_cal_dif.color = FBDF_CalMapColorDif(&buf, &map);
	buf.auto_cal_dif.trick = FBDF_CalMapTrickDif(&buf, &map);
	buf.auto_cal_dif.all = (buf.auto_cal_dif.notes + buf.auto_cal_dif.color + buf.auto_cal_dif.trick) / 3;
	buf.user_dif = music_detail_base.level;
	buf.map_file_name = file;
	buf.dif_type = dif;
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
 * @param[out] command 今のカーソル位置
 * @param[out] view_dif_type 今の難易度表示
 * @param[out] musiclist 譜面リスト
 * @param[out] cutin カットイン管理クラス
 * @return なし
 */
static void FBDF_select_KeyCheck(
	FBDF_Select_MusicFolderManager_c &folder_manager,
	int &command,
	FBDF_dif_type_ec &view_dif_type,
	FBDF_music_list_c &musiclist,
	fbdf_cutin_c *cutin
) {
	size_t list_size;
	if (cutin->IsClosing()) { return; } /* カットイン中なのでキー入力無効 */

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
			command = 0; /* 曲検索いる */
		}
		break;
	case KEY_INPUT_BACK:
		if (folder_manager.PopFolder()) {
			folder_manager.MakeMusicList(musiclist, view_dif_type);
			command = 0; /* 曲検索いる */
		}
		break;
	case KEY_INPUT_UP:
		list_size = folder_str.size();
		command = MOD_AVOID_ZERO((command + list_size - 1), list_size, 0);
		break;
	case KEY_INPUT_DOWN:
		list_size = folder_str.size();
		command = MOD_AVOID_ZERO((command + 1), list_size, 0);
		break;
	case KEY_INPUT_LEFT:
		--view_dif_type;
		if (folder_manager.NowFolder() == ALL_MUSIC_FOLDER) {
			folder_manager.MakeMusicList(musiclist, view_dif_type);
			command = 0; /* 曲検索いる */
		}
		break;
	case KEY_INPUT_RIGHT:
		++view_dif_type;
		if (folder_manager.NowFolder() == ALL_MUSIC_FOLDER) {
			folder_manager.MakeMusicList(musiclist, view_dif_type);
			command = 0; /* 曲検索いる */
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

		InputAllKeyHold();
		FBDF_select_KeyCheck(folder_manager_class, command, view_dif_type, musiclist, &cutin);

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
