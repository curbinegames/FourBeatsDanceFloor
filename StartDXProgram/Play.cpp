
#include <vector>
#include <queue>

#include <DxLib.h>
#include <dirent.h>

#include <dxcur.h>
#include <dxdraw.h>
#include <strcur.h>

#include <main.h>
#include <system.h>
#include <fbdf_cutin.h>
#include <mapenc.h>

#include <play.h>
#include <Result.h>

#define D_JUDGE_WIDTH 17
#define JUDGE_WIDTH 200
#define CRIT_SCORE (JUDGE_WIDTH - D_JUDGE_WIDTH)

#define SAVE_SCORE_WIDTH 100

#define NOTE_COLOR_1 0xFF7FD5FD
#define NOTE_COLOR_2 0xFF00E600
#define NOTE_COLOR_3 0xFFFF62FB
#define NOTE_COLOR_4 0xFFFFFF00

#define ISNOTE(c) ((c) == '-' || (c) == '.')

#define IS_NUMBER_CHAR(x) ((x)==_T('0')||(x)==_T('1')||(x)==_T('2')||(x)==_T('3')||(x)==_T('4')||(x)==_T('5')||(x)==_T('6')||(x)==_T('7')||(x)==_T('8')||(x)==_T('9'))

#define NOTE_HEIGHT 15
#define NOTE_SPEED   4

typedef enum FBDF_judge_mat_e {
	JUDGE_CRIT,
	JUDGE_HIT,
	JUDGE_SAVE,
	JUDGE_MISS,
	JUDGE_NONE,
} FBDF_judge_mat_t;

typedef enum FBDF_map_format_type_e {
	 ERROR_MAP_TYPE = -1,
	NORMAL_MAP_TYPE,
	RUNNER_MAP_TYPE,
	NATURE_MAP_TYPE,
	   OSU_MAP_TYPE,
	 TAIKO_MAP_TYPE,
} FBDF_map_format_type_t;

#if 1 /* struct */

typedef struct FBDF_push_key_s {
	int D = 0;
	int F = 0;
	int J = 0;
	int K = 0;
	int alltap = 0;
} FBDF_push_key_t;

typedef struct FBDF_score_s {
	uint crit  = 0;
	uint hit   = 0;
	uint save  = 0;
	uint drop  = 0;
	uint point = 0; /* 理論値 = 184 * ノーツ数 */
	uint chain = 0;
	uint chain_point = 0; /* 理論値 = 1~ノーツ数までの和 */
} FBDF_score_t;

typedef struct FBDF_judge_event_s {
	FBDF_judge_mat_t mat = JUDGE_MISS;
	 int gap = 0;
	uint tip = 1; /* 1,2,3,4 */
	uint len = 4;
	uint mtime = 0;
	uint score = 0;
} FBDF_judge_event_t;

typedef struct FBDF_judge_pic_s {
	DxPic_t just = LoadGraph(_T("pic/judge-just.png"));
	DxPic_t good = LoadGraph(_T("pic/judge-good.png"));
	DxPic_t safe = LoadGraph(_T("pic/judge-safe.png"));
	DxPic_t miss = LoadGraph(_T("pic/judge-miss.png"));
} FBDF_judge_pic_t;

typedef struct FBDT_hit_snd_s {
	int SE1Data = LoadSoundMem(_T("SE/SE1.wav"));
	int SE2Data = LoadSoundMem(_T("SE/SE2.wav"));
} FBDT_hit_snd_t;

#endif

class FBDF_judge_c {
private:
	int Xsize = 0;
	int Ysize = 0;
	DxTime_t Jtime = 0;
	DxPic_t Npic = DXLIB_PIC_NULL;
	FBDF_judge_mat_t Jmat = JUDGE_MISS;
	FBDF_judge_pic_t pic;

public:
	FBDF_judge_c() {
		GetGraphSize(this->pic.just, &this->Xsize, &this->Ysize);
	}

	~FBDF_judge_c() {
		DeleteGraph(this->pic.just);
		DeleteGraph(this->pic.good);
		DeleteGraph(this->pic.safe);
		DeleteGraph(this->pic.miss);
	}

	void DrawJudge(int x, int y) {
		double zoom = 1.0;
		if (this->Jtime + 750 < GetNowCount()) { return; }
		zoom = lins(0, 1.25, 100, 1.0, betweens(0, GetNowCount() - this->Jtime, 100));
		DrawDeformationPic(x, y, zoom, zoom, 0, this->Npic);
	}

	void SetJudge(FBDF_judge_mat_t mat) {
		this->Jtime = GetNowCount();
		this->Jmat = mat;
		switch (this->Jmat) {
		case JUDGE_CRIT:
			this->Npic = this->pic.just;
			break;
		case JUDGE_HIT:
			this->Npic = this->pic.good;
			break;
		case JUDGE_SAVE:
			this->Npic = this->pic.safe;
			break;
		case JUDGE_MISS:
			this->Npic = this->pic.miss;
			break;
		}
		return;
	}
};

class FBDF_dancer_c {
private:
	 int len = 0; // -1:miss 0:idle, 1~4:tip, 5~:long
	uint btn = 1; // 1-4
	uint mtime = 0;
	 int Stime = 0;
	uint bpm = 120;

	void DrawDanceString(int x, int y) const {
		if (this->len == 0) {
			DrawString(x, y, _T("state: idle"), COLOR_WHITE);
			return;
		}
		if (this->len <= -1) {
			DrawString(x, y, _T("state: miss"), COLOR_WHITE);
			return;
		}
		if (5 <= this->len) {
			DrawFormatString(x, y, COLOR_WHITE, _T("state: long(%d)"), this->len);
			return;
		}
		if (this->len == 1) {
			DrawFormatString(x, y, COLOR_WHITE, _T("state: move 1-%d"), this->btn);
			return;
		}
		if (this->len == 2) {
			DrawFormatString(x, y, COLOR_WHITE, _T("state: move 2-%d"), (int)((this->btn - 1) / 2) + 1);
			return;
		}
		DrawFormatString(x, y, COLOR_WHITE, _T("state: move %d"), this->len);
		return;
	}

	void DrawDanceWaitTime(int x, int y) const {
		int drawx2 = x + 100;
		int drawy2 = y + 10;
		DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, FALSE);
		if (this->len == 0) {
			return;
		}
		if (this->len < 0) {
			drawx2 = lins_scale(0, drawx2, 500, x, GetNowCount() - this->Stime);
			DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
			return;
		}
		if (5 <= this->len) {
			drawx2 = lins_scale(0, drawx2, 750, x, GetNowCount() - this->Stime);
			DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
			return;
		}
		drawx2 = lins_scale(this->mtime, drawx2, this->mtime + JUDGE_WIDTH, x, GetNowCount() - this->Stime);
		DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
		return;
	}

	void DrawDanceMotionTime(int x, int y) const {
		int drawx2 = x + 100;
		int drawy2 = y + 10;
		int buf = 0;
		DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, FALSE);
		buf = lins(0, x, 100, drawx2, 25);
		DrawLine(buf, y, buf, drawy2, COLOR_WHITE);
		buf = lins(0, x, 100, drawx2, 50);
		DrawLine(buf, y, buf, drawy2, COLOR_WHITE);
		buf = lins(0, x, 100, drawx2, 75);
		DrawLine(buf, y, buf, drawy2, COLOR_WHITE);
		if (this->len < 0) {
			return;
		}
		if (5 <= this->len) {
			return;
		}
		buf = lins(0, x, 4, drawx2, len);
		drawx2 = lins_scale(0, buf, this->mtime, x, GetNowCount() - this->Stime);
		DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
		return;
	}

public:
	void DrawDance(int x, int y) const {
		DrawString(x, y - 20, "ここでダンサーが踊る", COLOR_WHITE);
		this->DrawDanceString(x, y);
		this->DrawDanceWaitTime(x, y + 22);
		this->DrawDanceMotionTime(x, y + 42);
	}

	void UpdateState(void) {
		/* miss->idle */
		if (this->len < 0) {
			if (500 + this->Stime <= GetNowCount()) {
				this->len = 0;
			}
			return;
		}
		/* long->idle */
		if (4 < this->len) {
			if (750 + this->Stime <= GetNowCount()) {
				this->len = 0;
			}
			return;
		}
		/* motion->idle */
		if (this->mtime + JUDGE_WIDTH + this->Stime <= GetNowCount()) {
			this->len = 0;
		}
		return;
	}

	/**
	 * a_lenに0未満をセットするとmissモーションにできる。
	 * a_lenに0をセットするとidleモーションにできる。
	 */
	void SetState(uint a_btn, int a_len, uint a_mtime) {
		this->btn = a_btn;
		this->len = a_len;
		this->mtime = a_mtime;
		this->Stime = GetNowCount();
	}

	void SetBpm(uint val) {
		this->bpm = val;
	}
};

class FBDF_score_bar_c {
private:
	double score_70  =  70.0;
	double score_90  =  90.0;
	double score_96  =  96.0;
	double score_98  =  98.5;
	double score_99  =  99.1;
	double score_ave = 100.0;

	int Stime = 0;
	int Etime = 60000;

	FBDF_score_bar_st graph[FBDF_RESULT_SCORE_GRAPH_COUNT]; /* リザルト用 */
	int graphNo = 0; /* 次に入れる場所 */

	void set_graph(void) {
		if (this->graphNo >= FBDF_RESULT_SCORE_GRAPH_COUNT) { return; }
		this->graph[this->graphNo].bar_70 = this->score_70;
		this->graph[this->graphNo].bar_90 = this->score_90;
		this->graph[this->graphNo].bar_96 = this->score_96;
		this->graph[this->graphNo].bar_98 = this->score_98;
		this->graph[this->graphNo].bar_99 = this->score_99;
		this->graphNo++;
	}

public:
	void update_score(const FBDF_score_t *score, uint noteN) {
		uint hit_notes = score->crit + score->hit + score->drop;
		uint remain_notes = noteN - hit_notes;
		this->score_70 = 100 * (score->point + (70.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_90 = 100 * (score->point + (90.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_96 = 100 * (score->point + (96.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_98 = 100 * (score->point + (98.5 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_99 = 100 * (score->point + (99.1 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_ave = DIV_AVOID_ZERO(100 * score->point, hit_notes * CRIT_SCORE, 100.00);
	}

	void update_graph(int Ntime) {
		if (this->graphNo >= FBDF_RESULT_SCORE_GRAPH_COUNT) { return; }
		if (((Etime * this->graphNo + Stime * (FBDF_RESULT_SCORE_GRAPH_COUNT - this->graphNo - 1)) / (FBDF_RESULT_SCORE_GRAPH_COUNT - 1)) < Ntime)
		{
			this->set_graph();
		}
	}

	void set_time(int Stime, int Etime) {
		this->Stime = Stime;
		this->Etime = Etime;
	}

	void fill_graph_force(void) {
		for (int i = this->graphNo; i < FBDF_RESULT_SCORE_GRAPH_COUNT; i++) {
			this->set_graph();
		}
	}

	void get_graph(FBDF_score_bar_st *dest) const {
		for (int i = 0; i < FBDF_RESULT_SCORE_GRAPH_COUNT; i++) {
			dest[i] = this->graph[i];
		}
	}

	void draw_bar(int x1, int y1, int x2, int y2) const {
		FBDF_score_bar_st score_bar;
		score_bar.bar_70 = this->score_70;
		score_bar.bar_90 = this->score_90;
		score_bar.bar_96 = this->score_96;
		score_bar.bar_98 = this->score_98;
		score_bar.bar_99 = this->score_99;
		FBDF_DrawScoreBarHori(score_bar, x1, y1, x2, y2);
	}

	double GetScore_ave(void) const {
		return this->score_ave;
	}
};

class FBDF_gap_bar_c {
private:
	const int len    =   3; /* バーの長さ[px] */
	const int thick  =   1; /* バーの太さ[px] */
	const int height = 100; /* 表示gap[ms] */
	std::vector<int> queue;

	int sum   = 0;
	int count = 0;

public:
	void DrawBar(int x1, int y1, int x2, int y2) const {
		int Yborder = (y1 + y2) / 2;
		DrawLine(x1, Yborder, x2, Yborder, COLOR_RED);
		for (int in = 0; in < this->queue.size(); in++) {
			int Drawx1 = lins_scale(        0, x1, 49            , x2, in + 50 - this->queue.size());
			int Drawx2 = lins_scale(this->len, x1, 49 + this->len, x2, in + 50 - this->queue.size());
			int Drawy  = lins_scale(   height, y1,        -height, y2, this->queue[in]);
			DrawLine(Drawx1, Drawy, Drawx2, Drawy, COLOR_WHITE);
		}
	}

	void SetVal(int val) {
		this->queue.push_back(val);
		if (50 <= this->queue.size()) {
			this->queue.erase(this->queue.begin());
		}

		if (0 < val) {
			if ((INT_MAX - val) < this->sum) {
				this->sum /= 2;
				this->count /= 2;
			}
		}
		if (val < 0) {
			if (this->sum < (INT_MIN - val)) {
				this->sum /= 2;
				this->count /= 2;
			}
		}

		this->sum += val;
		this->count++;

		return;
	}

	double GetAve(void) const {
		return DIV_AVOID_ZERO(this->sum, this->count, 0);
	}
};

typedef struct FBDF_play_class_set_s {
	FBDF_judge_c judge_class;
	FBDF_dancer_c dancer_class;
	FBDF_score_bar_c score_bar_class;
	FBDF_gap_bar_c gap_bar_class;
} FBDF_play_class_set_t;

#if 1 /* Draw系 */

static void DrawNotes(const FBDF_map_t *map) {
	for (int in = map->noteNo; in < map->noteN; in++) {
		int BaseYpos = 0;
		uint cr = 0xffffffff;
		switch (map->note[in].btn) {
		case 1:
			cr = NOTE_COLOR_1;
			break;
		case 2:
			cr = NOTE_COLOR_2;
			break;
		case 3:
			cr = NOTE_COLOR_3;
			break;
		case 4:
			cr = NOTE_COLOR_4;
			break;
		}
		BaseYpos = 570 - NOTE_HEIGHT - ((sint)map->note[in].time - (sint)map->Ntime - 16) * (sint)map->bpm * NOTE_SPEED / 950;
		DrawBox(42, NOTE_HEIGHT + BaseYpos, 42 + 110, BaseYpos, cr, TRUE);
	}
	return;
}

static void DrawLamp(const FBDF_push_key_t *pkey) {
	static const int baseX = 165;
	static const int baseY = 575;
	static const int sizeX =  60;
	static const int sizeY =  15;
	static const int   gap =  10;
	if (IS_BETWEEN(1, pkey->D, 15)) {
		DrawBox(baseX                      , baseY, baseX +     sizeX          , baseY + sizeY, NOTE_COLOR_1, TRUE);
	}
	if (IS_BETWEEN(1, pkey->F, 15)) {
		DrawBox(baseX +     sizeX +     gap, baseY, baseX + 2 * sizeX +     gap, baseY + sizeY, NOTE_COLOR_2, TRUE);
	}
	if (IS_BETWEEN(1, pkey->J, 15)) {
		DrawBox(baseX + 2 * sizeX + 2 * gap, baseY, baseX + 3 * sizeX + 2 * gap, baseY + sizeY, NOTE_COLOR_3, TRUE);
	}
	if (IS_BETWEEN(1, pkey->K, 15)) {
		DrawBox(baseX + 3 * sizeX + 3 * gap, baseY, baseX + 4 * sizeX + 3 * gap, baseY + sizeY, NOTE_COLOR_4, TRUE);
	}
	return;
}

#endif

/* キー押し検出後の1ノーツ判定 */
static void FBDF_Play_OneNoteJudgeAfterKeyDetect(FBDF_judge_event_t &buf, bool &key_detect, const FBDF_map_t *map) {
	if (key_detect) {
		key_detect = false;
		buf.gap = map->note[map->noteNo].time - map->Ntime;
		if (buf.gap <= 0) {
			buf.score = betweens(0, JUDGE_WIDTH + buf.gap, CRIT_SCORE);
		}
		else {
			buf.score = betweens(0, JUDGE_WIDTH - buf.gap, CRIT_SCORE);
		}
		if (buf.score == CRIT_SCORE) {
			buf.mat = JUDGE_CRIT;
		}
		else if (SAVE_SCORE_WIDTH <= buf.score) {
			buf.mat = JUDGE_HIT;
		}
		else {
			buf.mat = JUDGE_SAVE;
		}
	}
	else {
		buf.mat = JUDGE_MISS;
	}
	return;
}

static void NoteJudge(
	FBDF_play_class_set_t *play_class, FBDF_score_t *score, FBDF_map_t *map,
	const FBDF_push_key_t *pkey, const FBDT_hit_snd_t *se)
{
	FBDF_judge_c     *judge_class     = &play_class->judge_class;
	FBDF_dancer_c    *dancer_class    = &play_class->dancer_class;
	FBDF_score_bar_c *score_bar_class = &play_class->score_bar_class;
	FBDF_gap_bar_c   *gap_bar_class   = &play_class->gap_bar_class;

	std::queue<FBDF_judge_event_t>judge_event;
	FBDF_judge_event_t buf;

	bool key_detect_d = (pkey->D == 1);
	bool key_detect_f = (pkey->F == 1);
	bool key_detect_j = (pkey->J == 1);
	bool key_detect_k = (pkey->K == 1);

	if (1 <= pkey->alltap) {
		while (map->note[map->noteNo].time != 0 &&
			map->note[map->noteNo].time - JUDGE_WIDTH < map->Ntime &&
			(key_detect_d || key_detect_f || key_detect_j || key_detect_k))
		{
			buf.tip = map->note[map->noteNo].btn;
			buf.len = map->note[map->noteNo].len;
			buf.mtime = map->note[map->noteNo].mtime;
			switch (map->note[map->noteNo].btn) {
			case FBDF_PLAY_NOTE_BTN_1:
				FBDF_Play_OneNoteJudgeAfterKeyDetect(buf, key_detect_d, map);
				break;
			case FBDF_PLAY_NOTE_BTN_2:
				FBDF_Play_OneNoteJudgeAfterKeyDetect(buf, key_detect_f, map);
				break;
			case FBDF_PLAY_NOTE_BTN_3:
				FBDF_Play_OneNoteJudgeAfterKeyDetect(buf, key_detect_j, map);
				break;
			case FBDF_PLAY_NOTE_BTN_4:
				FBDF_Play_OneNoteJudgeAfterKeyDetect(buf, key_detect_k, map);
				break;
			}
			judge_event.push(buf);
			if (buf.mat != JUDGE_NONE) { map->noteNo++; }
		}
	}

	if (map->note[map->noteNo].time != 0 &&
		map->note[map->noteNo].time + JUDGE_WIDTH < map->Ntime)
	{
		buf.mat = JUDGE_MISS;
		judge_event.push(buf);
		map->noteNo++;
	}

	bool note_judged = !judge_event.empty();
	while (!judge_event.empty()) {
		buf = judge_event.front();
		judge_event.pop();

		if (buf.mat == JUDGE_NONE) { continue; }

		judge_class->SetJudge(buf.mat);

		/* 判定数追加 */
		switch (buf.mat) {
		case JUDGE_CRIT:
			score->crit++;
			break;
		case JUDGE_HIT:
			score->hit++;
			break;
		case JUDGE_SAVE:
			score->save++;
			break;
		case JUDGE_MISS:
			score->drop++;
			break;
		}

		/* 効果音再生 */
		if (buf.mat != JUDGE_MISS) {
			switch (buf.tip) {
			case 1:
				PlaySoundMem(se->SE1Data, DX_PLAYTYPE_BACK);
				break;
			case 2:
			case 3:
			case 4:
				PlaySoundMem(se->SE2Data, DX_PLAYTYPE_BACK);
				break;
			}
		}

		/* コンボ計算 */
		if (buf.mat != JUDGE_MISS) { score->chain++; }

		/* スコア計算 */
		score->point += buf.score;
		score->chain_point += score->chain;

		/* キャラモーション変更 */
		if (buf.mat == JUDGE_MISS) {
			dancer_class->SetState(buf.tip, -1, buf.mtime);
		}
		else {
			dancer_class->SetState(buf.tip, buf.len, buf.mtime);
		}

		/* gap追加 */
		if (buf.mat != JUDGE_MISS) {
			gap_bar_class->SetVal(buf.gap);
		}
	}

	if (note_judged) {
		score_bar_class->update_score(score, map->noteN);
	}

	return;
}

/* 残っているすべてのノーツをdropにする */
static void NoteTrash(FBDF_play_class_set_t *play_class, FBDF_score_t *score, FBDF_map_t *map) {
	size_t remain_notes = 0;
	FBDF_judge_c     *judge_class     = &play_class->judge_class;
	FBDF_dancer_c    *dancer_class    = &play_class->dancer_class;
	FBDF_score_bar_c *score_bar_class = &play_class->score_bar_class;

	FBDF_judge_event_t buf;

	while (map->note[map->noteNo].time != 0) {
		remain_notes++;
		map->noteNo++;
	}

	if (0 < remain_notes) {
		judge_class->SetJudge(JUDGE_MISS);
		score->drop += remain_notes;
		dancer_class->SetState(0, -1, 0);
		score_bar_class->update_score(score, map->noteN);
	}

	return;
}

/* リザルト用のデータを作成 */
static void FBDF_Play_MakeResultData(FBDF_result_data_t *result_data, const FBDF::play_choose_music_st *nex_music,
	const FBDF_map_t &map, const FBDF_score_t &score, const FBDF_play_class_set_t &play_class
) {
	result_data->name        = nex_music->folder_name;
	result_data->artist      = map.artist;
	result_data->folder_name = nex_music->folder_name;
	result_data->level       = 0; /* meta.binから取ってくる */
	result_data->score       = score.point + score.chain_point;
	result_data->acc         = play_class.score_bar_class.GetScore_ave();
	result_data->crit        = score.crit;
	result_data->hit         = score.hit;
	result_data->save        = score.save;
	result_data->drop        = score.drop;
	result_data->gap_ave     = play_class.gap_bar_class.GetAve();
	result_data->charaNo     = 0; /* まだなんもイラスト描いてない */
	play_class.score_bar_class.get_graph(result_data->score_graph);
	result_data->dif_type    = nex_music->dif_type;
	return;
}

/* 譜面を読み込む */
static bool FBDF_Play_MapLoad(FBDF_map_t &map, const TCHAR *folder_name, const TCHAR *map_file_name) {
	std::string path = "music/";
	path += folder_name;
	path += '/';
	path += map_file_name;
	if (MapLoadOne(&map, path.c_str()) != 0) { return false; }
	map.noteNo = 0;
	map.Stime = GetNowCount();
	return true;
}

/* 曲の音声ファイルを読み込む */
static DxSnd_t FBDF_Play_Loadmusic(const TCHAR *folder_name, const TCHAR *music_file_name) {
	std::string path = "music/";
	path += folder_name;
	path += '/';
	path += music_file_name;
	return LoadSoundMem(path.c_str());
}

/* キー入力関係 */
static void FBDF_Play_KeyCheck(
	FBDF_push_key_t &pkey, FBDF_play_class_set_t &play_class,
	FBDF_score_t &score, FBDF_map_t &map, bool auto_fg, fbdf_cutin_c &cutin
) {
	int keybox[1] = { KEY_INPUT_RETURN };

	int hitkey = keycur(keybox, 1);

	if (!cutin.IsClosing() && (hitkey == KEY_INPUT_RETURN)) {
		NoteTrash(&play_class, &score, &map);
		play_class.score_bar_class.fill_graph_force();
		cutin.SetIo(CUT_FRAG_IN);
	}

	if (auto_fg) {
		pkey.D = (IS_BETWEEN(1, pkey.D, 5)) ? (pkey.D + 1) : (0);
		pkey.F = (IS_BETWEEN(1, pkey.F, 5)) ? (pkey.F + 1) : (0);
		pkey.J = (IS_BETWEEN(1, pkey.J, 5)) ? (pkey.J + 1) : (0);
		pkey.K = (IS_BETWEEN(1, pkey.K, 5)) ? (pkey.K + 1) : (0);

		if (map.note[map.noteNo].time <= 8 + map.Ntime) {
			switch (map.note[map.noteNo].btn) {
			case FBDF_PLAY_NOTE_BTN_1:
				pkey.D = 1;
				pkey.F = 0;
				pkey.J = 0;
				pkey.K = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_2:
				pkey.D = 0;
				pkey.F = 1;
				pkey.J = 0;
				pkey.K = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_3:
				pkey.D = 0;
				pkey.F = 0;
				pkey.J = 1;
				pkey.K = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_4:
				pkey.D = 0;
				pkey.F = 0;
				pkey.J = 0;
				pkey.K = 1;
				break;
			}
		}
	}
	else {
		pkey.D = (CheckHitKey(KEY_INPUT_D) == 1) ? (pkey.D + 1) : (0);
		pkey.F = (CheckHitKey(KEY_INPUT_F) == 1) ? (pkey.F + 1) : (0);
		pkey.J = (CheckHitKey(KEY_INPUT_J) == 1) ? (pkey.J + 1) : (0);
		pkey.K = (CheckHitKey(KEY_INPUT_K) == 1) ? (pkey.K + 1) : (0);
	}

	pkey.alltap = 0;
	pkey.alltap += (pkey.D == 1);
	pkey.alltap += (pkey.F == 1);
	pkey.alltap += (pkey.J == 1);
	pkey.alltap += (pkey.K == 1);
	return;
}

/* 返り値: 次のシーンの番号 */
view_num_t FirstPlayView(FBDF_result_data_t *result_data, const FBDF::play_choose_music_st *nex_music) {
	FBDF_map_t map;
	FBDF_score_t score;
	FBDF_push_key_t pkey;

	FBDF_play_class_set_t play_class;
	fbdf_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	int backPic = LoadGraph(_T("pic/PlayBack.png"));

	DxSnd_t musicData = 0;
	FBDT_hit_snd_t se;

	DxTime_t FinishTime = 0;

	if (FBDF_Play_MapLoad(map, nex_music->folder_name.c_str(), nex_music->map_file_name.c_str()) == false) { return VIEW_SELECT; }

	musicData = FBDF_Play_Loadmusic(nex_music->folder_name.c_str(), map.music_file);
	PlaySoundMem(musicData, DX_PLAYTYPE_BACK);

	map.noteNo = 0;
	map.Stime = GetNowCount();

	play_class.score_bar_class.set_time(map.offset, map.Etime);

	cutin.SetIo(CUT_FRAG_OUT);

	while (1) {
		if (cutin.IsEndAnim()) { break; }

		map.Ntime = GetNowCount() - map.Stime; /* 時間更新 */

		FBDF_Play_KeyCheck(pkey, play_class, score, map, false, cutin); /* autoにしたいなら5番目をtrueに */

		/* ノーツ全処理判定 */
		if ((FinishTime == 0) && (map.noteN == map.noteNo)) { FinishTime = map.Ntime; }

		/* 譜面終了判定 */
		if (!cutin.IsClosing() && (FinishTime != 0) && ((FinishTime + 2000) <= map.Ntime) && (CheckSoundMem(musicData))) {
			cutin.SetIo(CUT_FRAG_IN);
		}

		NoteJudge(&play_class, &score, &map, &pkey, &se);
		play_class.dancer_class.UpdateState();
		play_class.score_bar_class.update_graph(map.Ntime);

		cutin.update();

		ClearDrawScreen(); /* 作画エリアここから */ {
			DrawGraph(0, 0, backPic, TRUE);
			DrawFormatString(400,   5, COLOR_WHITE, _T("%d"), map.noteN);
			DrawFormatString(400,  25, COLOR_WHITE, _T("%d"), map.noteNo);
			DrawFormatString(400,  45, COLOR_WHITE, _T("%d"), score.crit);
			DrawFormatString(400,  65, COLOR_WHITE, _T("%d"), score.hit);
			DrawFormatString(400,  85, COLOR_WHITE, _T("%d"), score.save);
			DrawFormatString(400, 105, COLOR_WHITE, _T("%d"), score.drop);
			
			DrawLamp(&pkey);
			DrawNotes(&map);
			DrawFormatStringToHandle(710, 35, COLOR_WHITE, FBDF_font_DSEG7Modern, _T("%7d"), score.point + score.chain_point); /* スコア描画 */
			play_class.judge_class.DrawJudge(230, 530);
			play_class.score_bar_class.draw_bar(167, 600, 928, 650);
			DrawFormatString(166, 663, COLOR_WHITE, _T("%s"), nex_music->folder_name.c_str());
			play_class.dancer_class.DrawDance(500, 300);
			play_class.gap_bar_class.DrawBar(40, 576, 155, 691);

			cutin.DrawCut();

		} ScreenFlip(); /* 作画エリアここまで */

		if (GetWindowUserCloseFlag(TRUE)) { // 閉じるボタンが押された
			StopSoundMem(musicData);
			DeleteSoundMem(musicData);
			return VIEW_EXIT;
		}
		WaitTimer(10); // ループウェイト
	}

	StopSoundMem(musicData);
	DeleteSoundMem(musicData);

	FBDF_Play_MakeResultData(result_data, nex_music, map, score, play_class);

	return VIEW_RESULT;
}
