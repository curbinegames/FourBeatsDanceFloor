
#include <vector>
#include <queue>

#include <DxLib.h>
#include <dirent.h>

#include <dxcur.h>
#include <dxdraw.h>
#include <keycur.h>
#include <strcur.h>

#include <main.h>
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

	FBDF_result_score_graph_t graph[FBDF_RESULT_SCORE_GRAPH_COUNT]; /* リザルト用 */
	int graphNo = 0; /* 次に入れる場所 */

	void set_graph(void) {
		if (this->graphNo >= FBDF_RESULT_SCORE_GRAPH_COUNT) { return; }
		this->graph[this->graphNo].gr_70 = this->score_70;
		this->graph[this->graphNo].gr_90 = this->score_90;
		this->graph[this->graphNo].gr_96 = this->score_96;
		this->graph[this->graphNo].gr_98 = this->score_98;
		this->graph[this->graphNo].gr_99 = this->score_99;
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

	void get_graph(FBDF_result_score_graph_t *dest) const {
		for (int i = 0; i < FBDF_RESULT_SCORE_GRAPH_COUNT; i++) {
			dest[i] = this->graph[i];
		}
	}

	void draw_bar(int x1, int y1, int x2, int y2) const {
		double drawRight = 167;
		/* 70-60 */
		drawRight = lins_scale(70.0, x1,  60.0, x2, this->score_70);
		DrawBox(x1, y1, drawRight, y2, 0xFFEB3324, TRUE); // 赤
		/* 60- 0 */
		drawRight = lins_scale(60.0, x1,   0.0, x2, this->score_70);
		DrawBox(x1, y1, drawRight, y2, 0xFF962117, TRUE); // 赤
		/* 70-80 */
		drawRight = lins_scale(70.0, x1,  80.0, x2, this->score_70);
		DrawBox(x1, y1, drawRight, y2, 0xFFBDBB3F, TRUE); // 黄色
		/* 80-90 */
		drawRight = lins_scale(80.0, x1,  90.0, x2, this->score_70);
		DrawBox(x1, y1, drawRight, y2, 0xFFEDEB4F, TRUE); // 黄色
		/* 90-95 */
		drawRight = lins_scale(90.0, x1,  95.0, x2, this->score_90);
		DrawBox(x1, y1, drawRight, y2, 0xFF5BC23C, TRUE); // 緑
		/* 95-97 */
		drawRight = lins_scale(95.0, x1,  97.0, x2, this->score_90);
		DrawBox(x1, y1, drawRight, y2, 0xFF78FF4F, TRUE); // 緑
		/* 97-98 */
		drawRight = lins_scale(97.0, x1,  98.0, x2, this->score_96);
		DrawBox(x1, y1, drawRight, y2, 0xFF3282F6, TRUE); // 青
		/* 98-99 */
		drawRight = lins_scale(98.0, x1,  99.0, x2, this->score_96);
		DrawBox(x1, y1, drawRight, y2, 0xFF73FBFD, TRUE); // 青
		/* 99-100 */
		drawRight = lins_scale(99.0, x1, 100.0, x2, this->score_98);
		DrawBox(x1, y1, drawRight, y2, 0xFFA349A4, TRUE); // 紫
		/* 99.5-100 */
		drawRight = lins_scale(99.5, x1, 100.0, x2, this->score_99);
		DrawBox(x1, y1, drawRight, y2, 0xFFEA3FF7, TRUE); // 紫
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

static char *fgetwsOnChar(char *s, size_t size, FILE *fp) {
	unsigned char buf[2] = { 0,0 };
	char num_buf[32] = "";
	char *ret = NULL;
	if (s == NULL) { return NULL; }
	if (fp == NULL) { return NULL; }

	s[0] = '\0';

	while (fread_s(buf, 2, 1, 2, fp) == 2) {
		if (buf[0] == 0xFF && buf[1] == 0xFE) {
			continue;
		}
		if (buf[0] == 0x0D && buf[1] == 0x00) {
			continue;
		}
		if (buf[0] == 0x0A && buf[1] == 0x00) {
			ret = s;
			break;
		}
		ret = s;
		if (buf[1] != '\0') {
			switch (buf[1] * 0x100 + buf[0]) {
			case 0x2200:
				strcat_s(s, size, "∀");
				break;
			case 0x22A5:
				strcat_s(s, size, "⊥");
				break;
			case 0x25B2:
				strcat_s(s, size, "▲");
				break;
			case 0x25CF:
				strcat_s(s, size, "●");
				break;
			case 0x3044:
				strcat_s(s, size, "い");
				break;
			case 0x3046:
				strcat_s(s, size, "う");
				break;
			case 0x3053:
				strcat_s(s, size, "こ");
				break;
			case 0x305A:
				strcat_s(s, size, "ず");
				break;
			case 0x305F:
				strcat_s(s, size, "た");
				break;
			case 0x3060:
				strcat_s(s, size, "だ");
				break;
			case 0x3068:
				strcat_s(s, size, "と");
				break;
			case 0x306E:
				strcat_s(s, size, "の");
				break;
			case 0x3081:
				strcat_s(s, size, "め");
				break;
			case 0x3082:
				strcat_s(s, size, "も");
				break;
			case 0x3086:
				strcat_s(s, size, "ゆ");
				break;
			case 0x308C:
				strcat_s(s, size, "れ");
				break;
			case 0x3093:
				strcat_s(s, size, "ん");
				break;
			case 0x30A2:
				strcat_s(s, size, "ア");
				break;
			case 0x30A3:
				strcat_s(s, size, "ィ");
				break;
			case 0x30AD:
				strcat_s(s, size, "キ");
				break;
			case 0x30B0:
				strcat_s(s, size, "グ");
				break;
			case 0x30B7:
				strcat_s(s, size, "シ");
				break;
			case 0x30B8:
				strcat_s(s, size, "ジ");
				break;
			case 0x30C6:
				strcat_s(s, size, "テ");
				break;
			case 0x30C7:
				strcat_s(s, size, "デ");
				break;
			case 0x30C8:
				strcat_s(s, size, "ト");
				break;
			case 0x30C9:
				strcat_s(s, size, "ド");
				break;
			case 0x30CE:
				strcat_s(s, size, "ノ");
				break;
			case 0x30E1:
				strcat_s(s, size, "メ");
				break;
			case 0x30E3:
				strcat_s(s, size, "ャ");
				break;
			case 0x30E6:
				strcat_s(s, size, "ユ");
				break;
			case 0x30E7:
				strcat_s(s, size, "ョ");
				break;
			case 0x30E9:
				strcat_s(s, size, "ラ");
				break;
			case 0x30EA:
				strcat_s(s, size, "リ");
				break;
			case 0x30EF:
				strcat_s(s, size, "ワ");
				break;
			case 0x30F3:
				strcat_s(s, size, "ン");
				break;
			case 0x30EB:
				strcat_s(s, size, "ル");
				break;
			case 0x30F4:
				strcat_s(s, size, "ヴ");
				break;
			case 0x30FB:
				strcat_s(s, size, "・");
				break;
			case 0x30FC:
				strcat_s(s, size, "ー");
				break;
			case 0x5104:
				strcat_s(s, size, "億");
				break;
			case 0x5411:
				strcat_s(s, size, "向");
				break;
			case 0x56DB:
				strcat_s(s, size, "四");
				break;
			case 0x56FD:
				strcat_s(s, size, "国");
				break;
			case 0x5B87:
				strcat_s(s, size, "宇");
				break;
			case 0x5B99:
				strcat_s(s, size, "宙");
				break;
			case 0x5B9A:
				strcat_s(s, size, "定");
				break;
			case 0x5E1D:
				strcat_s(s, size, "帝");
				break;
			case 0x5E74:
				strcat_s(s, size, "年");
				break;
			case 0x61B6:
				strcat_s(s, size, "憶");
				break;
			case 0x661F:
				strcat_s(s, size, "星");
				break;
			case 0x6D41:
				strcat_s(s, size, "流");
				break;
			case 0x707D:
				strcat_s(s, size, "災");
				break;
			case 0x718A:
				strcat_s(s, size, "熊");
				break;
			case 0x7A7A:
				strcat_s(s, size, "空");
				break;
			case 0x7B1B:
				strcat_s(s, size, "笛");
				break;
			case 0x8A18:
				strcat_s(s, size, "記");
				break;
			case 0x8DE1:
				strcat_s(s, size, "跡");
				break;
			case 0x907A:
				strcat_s(s, size, "遺");
				break;
			case 0x9593:
				strcat_s(s, size, "間");
				break;
			case 0xFF1F:
				strcat_s(s, size, "？");
				break;
			default:
				sprintf_s(num_buf, "[0x%04X]", buf[1] * 0x100 + buf[0]);
				strcat_s(s, size, num_buf);
				break;
			}
		}
		else {
			stradds_2(s, size, buf[0]);
		}
	}

	return ret;
}

#if 1 /* 譜面読み込み系 */

static void MapLoadT_Strfix(char *ret) {
	bool judge = true;
	size_t len = 0;

	/* 2文字チェック */
	while (judge) {
		len = strnlen(ret, 256);
		if (len <= 4) {
			break;
		}
		if (len % 2 != 0) {
			break;
		}
		for (int i = 1; i < len; i += 2) {
			if (ret[i] != '.') {
				judge = false;
				break;
			}
		}
		if (judge) {
			int pos = 1;
			ret[1] = '\0';
			for (int i = 2; i < len; i += 2) {
				ret[pos] = ret[i];
				ret[pos + 1] = '\0';
				pos++;
			}
		}
	}

	/* 3文字チェック */
	judge = true;
	while (judge) {
		len = strnlen(ret, 256);
		if (len <= 3) {
			break;
		}
		if (len % 3 != 0) {
			break;
		}
		for (int i = 2; i < len; i += 3) {
			if (ret[i] != '.') {
				judge = false;
				break;
			}
			if (ret[i - 1] != '.') {
				judge = false;
				break;
			}
		}
		if (judge) {
			int pos = 1;
			ret[1] = '\0';
			for (int i = 3; i < len; i += 3) {
				ret[pos] = ret[i];
				ret[pos + 1] = '\0';
				pos++;
			}
		}
	}
	return;
}

static int MapLoadFromTaiko(FBDF_map_t *map, const char *path) {
	char stack_buf[256] = "";
	char catch_buf[256] = "";

	char *p_cmp = NULL;

	FBDF_map_enc_t option;

	FILE *fp = NULL;

	fopen_s(&fp, path, "r");
	if (fp == NULL) { return -1; }

	while (fgets(catch_buf, 256, fp) != NULL) {
		if (strands(catch_buf, "BPM:")) {
			strmods(catch_buf, 4);
			map->bpm = strsansD(catch_buf, 256);
			option.now_bpm = map->bpm;
		}
		else if (strands(catch_buf, "OFFSET:")) {
			strmods(catch_buf, 7);
			map->offset = (int)(strsansD(catch_buf, 256) * 1000);
			option.now_shuttime = map->offset;
		}
		else if (strands(catch_buf, "WAVE:")) {
			strmods(catch_buf, 5);
			strcpy_s(map->music_file, 256, catch_buf);
			for (int ic = 0; map->music_file[ic] != '\0'; ic++) {
				if (map->music_file[ic] == '\n') {
					map->music_file[ic] = '\0';
					break;
				}
			}
		}
		else if (strands(catch_buf, "#START")) {
			break;
		}
	}

	while (fgets(catch_buf, 256, fp) != NULL) {
		if (strands(catch_buf, "#END")) {
			break;
		}

		if (strands(catch_buf, "#SCROLL")    ||
			strands(catch_buf, "#GOGOSTART") ||
			strands(catch_buf, "#GOGOEND")   ||
			strands(catch_buf, "#BARLINEOFF")||
			strands(catch_buf, "#BARLINEON"))
		{
			continue;
		}

		if (strands(catch_buf, "#BPMCHANGE ")) {
			strmods(catch_buf, 11);
			option.now_bpm = strsansD(catch_buf, 256);
		}
		else if (strands(catch_buf, "#MEASURE ")) {
			strmods(catch_buf, 9);
			option.measure = strtol(catch_buf, NULL, 10);
			strnex_2(catch_buf, 256);
			option.measure_u = strtol(catch_buf, NULL, 10);
		}
		else if (strands(catch_buf, "#DELAY ")) {
			strmods(catch_buf, 7);
			option.now_shuttime += strsansD(catch_buf, 256) * 1000;
		}
		else if (catch_buf[0] == '#') {
			; // 何もしない
		}
		else {
			strcat_s(stack_buf, 256, catch_buf);

			for (int ic = 0;; ic++) {
				if (stack_buf[ic] == '\0') {
					break;
				}
				if (stack_buf[ic] == '/') {
					stack_buf[ic] = '\0';
					break;
				}
				if (stack_buf[ic] == '\n') {
					stack_buf[ic] = '\0';
					break;
				}
				if (stack_buf[ic] == '0' ||
					stack_buf[ic] == '8')
				{
					stack_buf[ic] = '.';
					continue;
				}
				if (stack_buf[ic] == '1' ||
					stack_buf[ic] == '2' ||
					stack_buf[ic] == '3' ||
					stack_buf[ic] == '4' ||
					stack_buf[ic] == '5' ||
					stack_buf[ic] == '6' ||
					stack_buf[ic] == '7' ||
					stack_buf[ic] == '9')
				{
					stack_buf[ic] = '-';
					continue;
				}
				if (stack_buf[ic] == ',') {
					stack_buf[ic] = '\0';
					char div_buf[256] = "";
					size_t len = strnlen(stack_buf, 256);
					uint div_count = len / option.measure;

					if (len == 0) {
						stack_buf[0] = '.';
						stack_buf[1] = '\0';
						len = 1;
						div_count = len / option.measure;
					}

					while (div_count == 0) {
						int pos = 0;
						for (int ic = 0; stack_buf[ic] != '\0'; ic++) {
							div_buf[pos] = stack_buf[ic];
							div_buf[pos + 1] = '.';
							div_buf[pos + 2] = '\0';
							pos += 2;
						}
						strcpy_s(stack_buf, 256, div_buf);
						len = strnlen(stack_buf, 256);
						div_count = len / option.measure;
					}

					for (int ib = 0; ib < option.measure; ib++) {
						strcpy_s(div_buf, 256, stack_buf);
						div_buf[div_count] = '\0';
						MapLoadT_Strfix(div_buf);
						option.now_block = strnlen(div_buf, 256);
						GetNoteLine(map, div_buf, &option);
						strmods_2(stack_buf, 256, div_count);
					}
					stack_buf[0] = '\0';
					break;
				}
			}
		}
	}
	fclose(fp);

	/* ラストノート挿入 */ {
		FBDF_note_t buf_note;
		map->note.push_back(buf_note);
	}

	return 0;
}

static int MapLoadFromNature(FBDF_map_t *map, const char *path) {
	char stack_buf[256] = "";
	char catch_buf[256] = "";

	char *p_cmp = NULL;

	bool bpm_inited = false;

	FBDF_map_enc_t option;

	FILE *fp = NULL;

	fopen_s(&fp, path, "r");
	if (fp == NULL) { return -1; }

	while (fgetwsOnChar(catch_buf, 256, fp) != NULL) {
		if (strands(catch_buf, "#BPM:")) {
			strmods(catch_buf, 5);
			map->bpm = strsansD(catch_buf, 256);
			option.now_bpm = map->bpm;
			bpm_inited = true;
		}
		else if (strands(catch_buf, "#SONGOFFSET:")) {
			strmods(catch_buf, 12);
			map->song_offset = (int)(strsansD(catch_buf, 256));
		}
		else if (strands(catch_buf, "#NOTEOFFSET:")) {
			strmods(catch_buf, 12);
			map->offset = (int)(strsansD(catch_buf, 256));
		}
		else if (strands(catch_buf, "#WAV:")) {
			strmods(catch_buf, 5);
			strcpy_s(map->music_file, 256, catch_buf);
			for (int ic = 0; map->music_file[ic] != '\0'; ic++) {
				if (map->music_file[ic] == '\n') {
					map->music_file[ic] = '\0';
					break;
				}
			}
		}
		else if (strands(catch_buf, "#NOTES:")) {
			break;
		}
	}

	map->offset = map->offset - map->song_offset;
	option.now_shuttime = map->offset;

	while (fgetwsOnChar(catch_buf, 256, fp) != NULL) {
		if (strands(catch_buf, "#END")) {
			break;
		}

		if (strands(catch_buf, "#BPM:")) {
			strmods(catch_buf, 5);
			option.now_bpm = strsansD(catch_buf, 256);
			if (!bpm_inited) {
				map->bpm = option.now_bpm;
				bpm_inited = true;
			}
		}
		else if (strands(catch_buf, "#MEASURE:")) {
			strmods(catch_buf, 9);
			option.measure = strtol(catch_buf, NULL, 10);
			strnex_2(catch_buf, 256);
			option.measure_u = strtol(catch_buf, NULL, 10);
		}
		else if (strands(catch_buf, "#SCROLL:")) {
			strmods(catch_buf, 8);
			option.scrool = strsansD(catch_buf, 256);
		}
		else if (strands(catch_buf, "#SC:")) {
			strmods(catch_buf, 4);
			option.scrool = strsansD(catch_buf, 256);
		}
		else if (strands(catch_buf, "#LETBPM:")) {
			strmods(catch_buf, 8);
			option.now_bpm = strsansD(catch_buf, 256) / (double)option.scrool;
		}
		else if (strands(catch_buf, "#SCROLLBPM:")) {
			strmods(catch_buf, 11);
			option.scrool = strsansD(catch_buf, 256) / (double)option.now_bpm;
		}
		else if (strands(catch_buf, "#SCBPM:")) {
			strmods(catch_buf, 7);
			option.scrool = strtol(catch_buf, NULL, 10) / (double)option.now_bpm;
		}
		else if (strands(catch_buf, "#STOP:")) {
			strmods(catch_buf, 6);
			option.now_shuttime += strsansD(catch_buf, 256);
		}
		else if (strands(catch_buf, "#STOPSTEP:")) {
			int count = 0;
			int length = 0;
			strmods(catch_buf, 10);
			count = strtol(catch_buf, NULL, 10);
			strnex_2(catch_buf, 256);
			length = strtol(catch_buf, NULL, 10);
			option.now_shuttime += 4 * 60000 * count / (length * option.now_bpm * option.scrool);
		}
		else if (catch_buf[0] == '#') {
			; // 何もしない
		}
		else if (catch_buf[0] == '\0') {
			; // 何もしない
		}
		else {
			bool in_note = false;
			bool end_measure = false;
			bool change_long = false;
			for (int ic = 0;; ic++) {
				/* 小節終わり */
				if (catch_buf[ic] == ',') {
					end_measure = true;
					break;
				}
				/* 終端判定 */
				if (catch_buf[ic] == '\0') {
					break;
				}
				if (catch_buf[ic] == '/') {
					break;
				}
				if (catch_buf[ic] == '\n') {
					break;
				}
				/* 色変ロング */
				if (catch_buf[ic] == ':') {
					change_long = true;
					continue;
				}
				/* ノーツ判定 */
				if (!change_long) {
					if (catch_buf[ic] == 'r'||
						catch_buf[ic] == 'R'||
						catch_buf[ic] == 'g'||
						catch_buf[ic] == 'G'||
						catch_buf[ic] == 'b'||
						catch_buf[ic] == 'B'||
						catch_buf[ic] == 'c'||
						catch_buf[ic] == 'C'||
						catch_buf[ic] == 'm'||
						catch_buf[ic] == 'M'||
						catch_buf[ic] == 'y'||
						catch_buf[ic] == 'Y'||
						catch_buf[ic] == 'w'||
						catch_buf[ic] == 'W'||
						catch_buf[ic] == 'f'||
						catch_buf[ic] == 'F')
					{
						in_note = true;
					}
				}
			}

			strcat_s(stack_buf, 256, ((in_note) ? "-" : "."));

			if (end_measure) {
				char div_buf[256] = "";
				size_t len = strnlen(stack_buf, 256);
				uint div_count = len / option.measure;

				if (len == 0) {
					stack_buf[0] = '.';
					stack_buf[1] = '\0';
					len = 1;
					div_count = len / option.measure;
				}

				while (div_count < 3 && len < 127) {
					int pos = 0;
					for (int ic = 0; stack_buf[ic] != '\0'; ic++) {
						div_buf[pos] = stack_buf[ic];
						div_buf[pos + 1] = '.';
						div_buf[pos + 2] = '\0';
						pos += 2;
					}
					strcpy_s(stack_buf, 256, div_buf);
					len = strnlen(stack_buf, 256);
					div_count = len / option.measure;
				}

				for (int ib = 0; ib < option.measure; ib++) {
					strcpy_s(div_buf, 256, stack_buf);
					div_buf[div_count] = '\0';
					MapLoadT_Strfix(div_buf);
					option.now_block = strnlen(div_buf, 256);
					GetNoteLine(map, div_buf, &option);
					strmods_2(stack_buf, 256, div_count);
				}
				stack_buf[0] = '\0';
			}
		}
	}
	fclose(fp);

	/* ラストノート挿入 */ {
		FBDF_note_t buf_note;
		map->note.push_back(buf_note);
	}

	return 0;
}

static FBDF_map_format_type_t JudgeMapMat(TCHAR *map_path, const char *nex_music) {
	char path[256] = "music/";
	DIR *dir = NULL;
	struct dirent *dirs;
	FBDF_map_format_type_t ret = ERROR_MAP_TYPE;

	strcat_s(path, nex_music);
	
	strcpy_s(map_path, 256, path);
	strcat_s(map_path, 256, "/");

	dir = opendir(path);
	if (dir == NULL) {
		return ERROR_MAP_TYPE;
	}

	while (1) {
		dirs = readdir(dir);
		if (dirs == NULL) {
			break;
		}
		if (dirs->d_name[0] == '.') {
			continue;
		}
		if (strcmp(dirs->d_name, "map.txt") == 0) {
			ret = NORMAL_MAP_TYPE;
			break;
		}
		if (strstr(dirs->d_name, ".rrs") != NULL) {
			ret = RUNNER_MAP_TYPE;
			break;
		}
		if (strstr(dirs->d_name, ".nps") != NULL) {
			ret = NATURE_MAP_TYPE;
			break;
		}
		if (strstr(dirs->d_name, ".osu") != NULL) {
			ret = OSU_MAP_TYPE;
			break;
		}
		if (strstr(dirs->d_name, ".tja") != NULL) {
			ret = TAIKO_MAP_TYPE;
			break;
		}
	}
	if (dirs != NULL) {
		strcat_s(map_path, 256, dirs->d_name);
	}
	closedir(dir);

	return ret;
}

#endif

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
			case 1:
				if (key_detect_d) {
					key_detect_d = false;
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
				break;
			case 2:
				if (key_detect_f) {
					key_detect_f = false;
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
				break;
			case 3:
				if (key_detect_j) {
					key_detect_j = false;
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
				break;
			case 4:
				if (key_detect_k) {
					key_detect_k = false;
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

		/* スコア計算 */
		score->point += buf.score;

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
	FBDF_judge_c     *judge_class     = &play_class->judge_class;
	FBDF_dancer_c    *dancer_class    = &play_class->dancer_class;
	FBDF_score_bar_c *score_bar_class = &play_class->score_bar_class;

	std::queue<FBDF_judge_event_t>judge_event;
	FBDF_judge_event_t buf;

	while (map->note[map->noteNo].time != 0) {
		buf.mat = JUDGE_MISS;
		judge_event.push(buf);
		map->noteNo++;
	}

	bool note_judged = !judge_event.empty();
	while (!judge_event.empty()) {
		buf = judge_event.front();
		judge_event.pop();

		judge_class->SetJudge(buf.mat);

		/* 判定数追加 */
		score->drop++;

		/* キャラモーション変更 */
		if (buf.mat == JUDGE_MISS) {
			dancer_class->SetState(buf.tip, -1, buf.mtime);
		}
		else {
			dancer_class->SetState(buf.tip, buf.len, buf.mtime);
		}
	}

	if (note_judged) {
		score_bar_class->update_score(score, map->noteN);
	}

	return;
}

/* 返り値: 次のシーンの番号 */
view_num_t FirstPlayView(FBDF_result_data_t *result_data, const FBDF::play_choose_music_st *nex_music) {
	int keybox[1] = { KEY_INPUT_RETURN };
	int hitkey = 0;
	DxTime_t FinishTime = 0;

	char musicPath[256];
	TCHAR map_path[256];

	FBDF_map_t map;
	FBDF_score_t score;
	FBDF_push_key_t pkey;
	FBDF_play_class_set_t play_class;

	fbdf_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	int backPic = LoadGraph(_T("pic/PlayBack.png"));
	int musicData = 0;

	FBDT_hit_snd_t se;

	{
		std::string map_path;
		map_path  = "music/";
		map_path += nex_music->folder_name;
		map_path += '/';
		map_path += nex_music->map_file_name;
		if (MapLoadOne(&map, map_path.c_str()) != 0) { return VIEW_EXIT; }
	}

#if 0
	switch (JudgeMapMat(map_path, nex_music->music_name)) {
	case NORMAL_MAP_TYPE:
		if (MapLoadOne(&map, nex_music->music_name) != 0) { return VIEW_EXIT; }
		break;
	case RUNNER_MAP_TYPE:
		break;
	case NATURE_MAP_TYPE:
		if (MapLoadFromNature(&map, map_path) != 0) { return VIEW_EXIT; }
		break;
	case    OSU_MAP_TYPE:
		break;
	case  TAIKO_MAP_TYPE:
		if (MapLoadFromTaiko(&map, map_path) != 0) { return VIEW_EXIT; }
		break;
	default:
		return VIEW_EXIT;
	}
#endif

	strcpy_s(musicPath, "music/");
	strcat_s(musicPath, nex_music->folder_name.c_str());
	strcat_s(musicPath, "/");
	strcat_s(musicPath, map.music_file);

	musicData = LoadSoundMem(musicPath);

	PlaySoundMem(musicData, DX_PLAYTYPE_BACK);

	map.noteNo = 0;
	map.Stime = GetNowCount();

	play_class.score_bar_class.set_time(map.offset, map.Etime);

	cutin.SetIo(CUT_FRAG_OUT);

	while (1) {
		if (cutin.IsEndAnim()) { break; }

		/* 時間 */ {
			map.Ntime = GetNowCount() - map.Stime;
		}

		/* キー */ {
			hitkey = keycur(keybox, 1);
			if (!cutin.IsClosing() && (hitkey == KEY_INPUT_RETURN)) {
				NoteTrash(&play_class, &score, &map);
				play_class.score_bar_class.fill_graph_force();
				cutin.SetIo(CUT_FRAG_IN);
			}

#if 0 /* auto */
			if (IS_BETWEEN(1, pkey.D, 5)) {
				pkey.D++;
			}
			else {
				pkey.D = 0;
			}
			if (IS_BETWEEN(1, pkey.F, 5)) {
				pkey.F++;
			}
			else {
				pkey.F = 0;
			}
			if (IS_BETWEEN(1, pkey.J, 5)) {
				pkey.J++;
			}
			else {
				pkey.J = 0;
			}
			if (IS_BETWEEN(1, pkey.K, 5)) {
				pkey.K++;
			}
			else {
				pkey.K = 0;
			}
			if (map.note[map.noteNo].time <= 8 + map.Ntime) {
				switch (map.note[map.noteNo].btn) {
				case 1:
					pkey.D = 1;
					pkey.F = 0;
					pkey.J = 0;
					pkey.K = 0;
					break;
				case 2:
					pkey.D = 0;
					pkey.F = 1;
					pkey.J = 0;
					pkey.K = 0;
					break;
				case 3:
					pkey.D = 0;
					pkey.F = 0;
					pkey.J = 1;
					pkey.K = 0;
					break;
				case 4:
					pkey.D = 0;
					pkey.F = 0;
					pkey.J = 0;
					pkey.K = 1;
					break;
				}
				pkey.alltap = 1;
			}
			else {
				pkey.alltap = 0;
			}
#else /* manual */
			switch (hitkey) {
			default:
				break;
			}

			pkey.D = (CheckHitKey(KEY_INPUT_D) == 1) ? (pkey.D + 1) : (0);
			pkey.F = (CheckHitKey(KEY_INPUT_F) == 1) ? (pkey.F + 1) : (0);
			pkey.J = (CheckHitKey(KEY_INPUT_J) == 1) ? (pkey.J + 1) : (0);
			pkey.K = (CheckHitKey(KEY_INPUT_K) == 1) ? (pkey.K + 1) : (0);
			pkey.alltap = 0;
			pkey.alltap += (pkey.D == 1);
			pkey.alltap += (pkey.F == 1);
			pkey.alltap += (pkey.J == 1);
			pkey.alltap += (pkey.K == 1);
#endif
		}

		/* ノーツ全処理判定 */
		if ((FinishTime == 0) && (map.noteN == map.noteNo)) {
			FinishTime = map.Ntime;
		}

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
			DrawFormatStringToHandle(710, 35, COLOR_WHITE, FBDF_font_DSEG7Modern, _T("%7d"), score.point);
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

	/* リザルト用のデータを作成 */

	result_data->name   = nex_music->folder_name;
	result_data->artist = map.artist;
	result_data->folder_name = nex_music->folder_name;
	result_data->level = 0; /* meta.binから取ってくる */
	result_data->score = score.point;
	result_data->acc  = play_class.score_bar_class.GetScore_ave();
	result_data->crit = score.crit;
	result_data->hit  = score.hit;
	result_data->save = score.save;
	result_data->drop = score.drop;
	result_data->gap_ave = play_class.gap_bar_class.GetAve();
	result_data->charaNo = 0; /* まだなんもイラスト描いてない */
	play_class.score_bar_class.get_graph(result_data->score_graph);
	result_data->dif_type = nex_music->dif_type;

	return VIEW_RESULT;
}
