
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
#include <motionenc.h>

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

#define NOTE_HEIGHT 15
#define NOTE_SPEED   4

typedef enum FBDF_judge_mat_e {
	JUDGE_CRIT,
	JUDGE_HIT,
	JUDGE_SAVE,
	JUDGE_MISS,
	JUDGE_NONE
} FBDF_judge_mat_et;

typedef enum FBDF_dancer_state_e {
	FBDF_DANCER_STATE_IDLE,
	FBDF_DANCER_STATE_MISS,
	FBDF_DANCER_STATE_AFK,
	FBDF_DANCER_STATE_DANCING_1,
	FBDF_DANCER_STATE_DANCING_2,
	FBDF_DANCER_STATE_DANCING_3,
	FBDF_DANCER_STATE_DANCING_4,
	FBDF_DANCER_STATE_DANCING_LONG
} FBDF_dancer_state_et;

#if 1 /* struct */

typedef struct FBDF_push_key_s {
	int D = 0;
	int F = 0;
	int J = 0;
	int K = 0;
	int alltap = 0;
} FBDF_push_key_st;

typedef struct FBDF_score_s {
	uint crit  = 0;
	uint  hit  = 0;
	uint save  = 0;
	uint drop  = 0;
	uint point = 0; /* 理論値 = 184 * ノーツ数 */
	uint chain = 0;
	uint chain_point = 0; /* 理論値 = 1~ノーツ数までの和 */
} FBDF_score_st;

typedef struct FBDF_judge_event_s {
	FBDF_judge_mat_et mat = JUDGE_MISS; /* ノーツの判定 */
	 int gap = 0; /* 押したタイミングのずれ[ms] */
	uint len = 4; /* 次のノーツまでのブロック数 */
	uint mtime = 0; /* 次のノーツまでの時間、モーション時間[ms] */
	uint score = 0; /* このノーツの点数 */
	FBDF_note_motion_assign_et motion = FBDF_NOTE_MOTION_ASSIGN_NONE;
	FBDF_Play_note_btn_et tip = FBDF_PLAY_NOTE_BTN_1;
} FBDF_judge_event_st;

typedef struct FBDF_Play_note_pic_s {
	dxcur_pic_c one   = dxcur_pic_c("pic/play/NoteOne.png");
	dxcur_pic_c two   = dxcur_pic_c("pic/play/NoteTwo.png");
	dxcur_pic_c three = dxcur_pic_c("pic/play/NoteThree.png");
	dxcur_pic_c four  = dxcur_pic_c("pic/play/NoteFour.png");
	dxcur_pic_c white = dxcur_pic_c("pic/play/NoteWhite.png");
} FBDF_Play_note_pic_st;

typedef struct FBDT_hit_snd_s {
	dxcur_snd_c SE1Data = dxcur_snd_c(_T("SE/SE1.wav"));
	dxcur_snd_c SE2Data = dxcur_snd_c(_T("SE/SE2.wav"));
} FBDT_hit_snd_t;

#endif /* struct */

#if 1 /* class */

class FBDF_judge_c {
private:
	int Xsize = 0;
	int Ysize = 0;
	DxTime_t Jtime = 0;
	DxPic_t Npic = DXLIB_PIC_NULL;
	FBDF_judge_mat_et Jmat = JUDGE_MISS;
	struct {
		dxcur_pic_c crit = dxcur_pic_c(_T("pic/play/JudgeCrit.png"));
		dxcur_pic_c  hit = dxcur_pic_c(_T("pic/play/JudgeHit.png"));
		dxcur_pic_c save = dxcur_pic_c(_T("pic/play/JudgeSave.png"));
		dxcur_pic_c drop = dxcur_pic_c(_T("pic/play/JudgeDrop.png"));
	} pic;

public:
	/**
	 * @brief コンストラクタ、画像データの大きさを取得している
	 * @param なし
	 */
	FBDF_judge_c(void) {
		GetGraphSize(this->pic.crit.handle(), &this->Xsize, &this->Ysize);
	}

	/**
	 * @brief 判定の描画
	 * @param[in] x 描画横位置
	 * @param[in] y 描画縦位置
	 * @return なし
	 */
	void DrawJudge(int x, int y) const {
		double zoom = 1.0;
		if (this->Jtime + 750 < GetNowCount()) { return; }
		zoom = lins(0, 1.25, 100, 1.0, betweens(0, GetNowCount() - this->Jtime, 100)) * 0.8;
		DrawDeformationPic(x, y, zoom, zoom, 0, this->Npic);
	}

	/**
	 * @brief 判定をセットする
	 * @param[in] mat セットする判定
	 * @return なし
	 */
	void SetJudge(FBDF_judge_mat_et mat) {
		this->Jtime = GetNowCount();
		this->Jmat = mat;
		switch (this->Jmat) {
		case JUDGE_CRIT:
			this->Npic = this->pic.crit.handle();
			break;
		case JUDGE_HIT:
			this->Npic = this->pic.hit.handle();
			break;
		case JUDGE_SAVE:
			this->Npic = this->pic.save.handle();
			break;
		case JUDGE_MISS:
			this->Npic = this->pic.drop.handle();
			break;
		}
		return;
	}
};

/**
 * ダンサーを管理するクラス
 * ステート推移
 *            最初--------------> idle
 * anystate --ノーツを叩いた----> dance
 * idle     --ミスった----------> miss
 * dance    --ミスった----------> miss
 * dance    --一定時間経過した--> idle
 */
class FBDF_dancer_c {
private:
	const size_t motion_len = 120;

	 int len = 0; // -1:miss 0:idle, 1~4:tip, 5~:long
	DxTime_t mtime = 0; /* モーション長さ */
	 int Stime = 0; /* モーションスタート絶対時間 */
	 int offset = 0; /* 待機ステップ開始時間 */
	size_t Nmotion_picNo = 0; /* 今のダンスモーション番号 */
	double bpm = 120;
	FBDF_Play_note_btn_et btn = FBDF_PLAY_NOTE_BTN_1;
	FBDF_dancer_state_et Nstate = FBDF_DANCER_STATE_IDLE;

#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
	dxcur_divpic_c idle_pic;
	dxcur_divpic_c miss_pic;
#elif FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
	Dx3Dobj_t  n3Dmodel_handle     = -1;
	Dx3Danim_t n3Dmotion_idle_ath  = -1;
	Dx3Danim_t n3Dmotion_miss_ath  = -1;
	Dx3Danim_t n3Dmotion_dance_ath = -1;
#endif /* 3Dモデル */

	std::vector<size_t> searched_motion;
	std::vector<FBDF_Play_motion_st> motion_data;

public: /* コンストラクタ系 */
	FBDF_dancer_c(void) : FBDF_dancer_c(FBDF_DANCER_UNIOW) {} /* ユニオとして初期化 */

	FBDF_dancer_c(size_t n) {
		std::string chara_name;
		std::string folder_path = "dancer/";
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
		std::string image_path;
#elif FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
		std::string n3Dmodel_path;
#endif /* 3Dモデル */

		switch (n) {
		case FBDF_DANCER_UNIOW:
			chara_name = "uniow";
			break;
		case FBDF_DANCER_NEIDA:
			chara_name = "neida";
			break;
		case FBDF_DANCER_TRIMBA:
			chara_name = "trimba";
			break;
		case FBDF_DANCER_QUATTRO:
			chara_name = "quattro";
			break;
		default:
			chara_name = "uniow";
			break;
		}
		FBDF_DancerMotionEnc(motion_data, chara_name.c_str());

		folder_path += chara_name;
		folder_path += '/';

#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
		image_path  = folder_path;
		image_path += "idle.png";
		idle_pic = dxcur_divpic_c(image_path.c_str(), 8, 4, 2);

		image_path  = folder_path;
		image_path += "miss.png";
		miss_pic = dxcur_divpic_c(image_path.c_str(), 4, 2, 2);
#elif FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
		n3Dmodel_path  = folder_path;
		n3Dmodel_path += "model.mv1";
		this->n3Dmodel_handle = MV1LoadModel(n3Dmodel_path.c_str());
		MV1SetPosition(this->n3Dmodel_handle, VGet(
			lins(0, 0, 2000, WINDOW_SIZE_X, 1000),
			lins(0, 0, 2000, WINDOW_SIZE_Y,  300),
			500
		));
		MV1SetScale(this->n3Dmodel_handle, VGet(2, 2, 2));
		this->n3Dmotion_idle_ath = MV1AttachAnim(this->n3Dmodel_handle, 0);
		this->n3Dmotion_miss_ath = MV1AttachAnim(this->n3Dmodel_handle, 1);
		MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,  1);
		MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,  0);
		MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, 0);
#endif /* 3Dモデル */
	}

#if FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
	~FBDF_dancer_c() {
		MV1DeleteModel(this->n3Dmodel_handle);
	}
#endif /* 3Dモデル */

private:
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
	/* アニメ番号を取得する */
	size_t GetMotionAnimNo(void) const {
		if (this->len == 1) { /* ダンスモーション1 */
			return this->btn - 1;
		}
		else if (this->len == 2) { /* ダンスモーション2 */
			return (size_t)((this->btn - 1) / 2);
		}
		else if (IS_BETWEEN(3, this->len, 4)) { /* ダンスモーション3/4 */
			return min((int)lins(0, 0, this->mtime, this->len, GetNowCount() - this->Stime), this->len);
		}
		else if (5 <= this->len) { /* ロングモーション */
			return lins_scale(0, 0, 750, 8, GetNowCount() - this->Stime);
		}
		else if (this->len < 0) { /* ミスモーション */
			if (5000 <= GetNowCount() - this->Stime) { return 3; } /* ミス放置モーション */
			return lins_scale(0, 0, 100, 2, GetNowCount() - this->Stime);
		}
		else { /* 待機モーション */
			double loop_time = 2 * 60000 / this->bpm; /* 1ループの時間、this->bpmは0以外を保証 */
			int base_time = GetNowCount() - this->Stime - this->offset; /* オフセットからの時間 */
			int now_block = (int)(base_time / loop_time); /* ループ回数、loop_timeは0以外を保証 */
			int in_time = base_time - now_block * loop_time; /* ループ内の時間 */
			if (in_time < 0) { in_time += loop_time; } /* マイナス補正 */
			return (int)lins(0, 0, loop_time, 8, in_time) % 8;
		}
		return 0; /* 通らないけど一応明記 */
	}
#elif FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
	size_t GetIdleMotionAnimNo(void) const {
		double loop_time = 4 * 60000 / this->bpm; /* 1ループの時間、this->bpmは0以外を保証 */
		int base_time = GetNowCount() - this->Stime - this->offset; /* オフセットからの時間 */
		int now_block = (int)(base_time / loop_time); /* ループ回数、loop_timeは0以外を保証 */
		int in_time = base_time - now_block * loop_time; /* ループ内の時間 */
		if (in_time < 0) { in_time += loop_time; } /* マイナス補正 */
		return (int)(lins(0, 0, loop_time, motion_len * 2, in_time)) % (motion_len * 2);
	}

	size_t GetMotionAnimNo(void) const {
		size_t retval = 0;
		size_t start = 0;
		size_t end = 60;
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_DANCING_1:
			switch (this->btn) {
			case FBDF_PLAY_NOTE_BTN_1:
				start = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_2:
				start = this->motion_len / 4;
				break;
			case FBDF_PLAY_NOTE_BTN_3:
				start = this->motion_len / 2;
				break;
			case FBDF_PLAY_NOTE_BTN_4:
				start = this->motion_len * 3 / 4;
				break;
			}
			end = start + this->motion_len / 4;
			retval = lins_scale(0, start, this->mtime, end, GetNowCount() - this->Stime);
			break;
		case FBDF_DANCER_STATE_DANCING_2:
			switch (this->btn) {
			case FBDF_PLAY_NOTE_BTN_1:
			case FBDF_PLAY_NOTE_BTN_2:
				start = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_3:
			case FBDF_PLAY_NOTE_BTN_4:
				start = this->motion_len / 2;
				break;
			}
			end = start + this->motion_len / 2;
			retval = lins_scale(0, start, this->mtime, end, GetNowCount() - this->Stime);
			break;
		case FBDF_DANCER_STATE_DANCING_3:
		case FBDF_DANCER_STATE_DANCING_4:
			retval = lins_scale(0, 0, this->mtime, this->motion_len, GetNowCount() - this->Stime);
			break;
		case FBDF_DANCER_STATE_DANCING_LONG:
			retval = lins_scale(0, 0, min(this->mtime, 750), this->motion_len, GetNowCount() - this->Stime);
			break;
		case FBDF_DANCER_STATE_MISS:
			return lins_scale(0, 0, 500, this->motion_len / 2, GetNowCount() - this->Stime);
			break;
		case FBDF_DANCER_STATE_AFK:
			retval = lins_scale(5000, this->motion_len / 2, 6000, this->motion_len, GetNowCount() - this->Stime);
			break;
		case FBDF_DANCER_STATE_IDLE:
			retval = this->GetIdleMotionAnimNo();
			break;
		}
		return retval;
	}

	void SetShapeAll(void) {
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_MISS:
		case FBDF_DANCER_STATE_AFK:
			/* FIXME: ユニオ専用の設定になってる */
			if (GetNowCount() - this->Stime < 1000 * 20 / 60) {
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("まばたき")), 0.0);
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("笑い")), 0.0);
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("点目")), 1.0);
			}
			else if (GetNowCount() - this->Stime < 5000) {
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("まばたき")), 0.5);
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("笑い")), 0.5);
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("点目")), 0.0);
			}
			else {
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("まばたき")), 0.5);
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("笑い")), lins_scale(5000, 0.5, 5500, 0.0, GetNowCount() - this->Stime));
				MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("点目")), 0.0);
			}
			break;
		default:
			MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("まばたき")), 1 - abs(lins_scale(0, 0.0, 200, 2.0, GetNowCount() % 6000) - 1));
			MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("笑い")), 0.0);
			MV1SetShapeRate(this->n3Dmodel_handle, MV1SearchShape(this->n3Dmodel_handle, _T("点目")), 0.0);
			break;
		}
	}
#endif /* 3Dモデル */

#if 1 /* デバッグ描画 */
	/**
	 * @brief デバッグ用、ステート情報を描く
	 * @param[in] x 描画横位置
	 * @param[in] y 描画縦位置
	 * @return なし
	 */
	void DrawDebugDanceString(int x, int y) const {
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_IDLE:
			DrawString(x, y, _T("state: idle"), COLOR_WHITE);
			break;
		case FBDF_DANCER_STATE_MISS:
			DrawString(x, y, _T("state: miss"), COLOR_WHITE);
			break;
		case FBDF_DANCER_STATE_AFK:
			DrawString(x, y, _T("state: afk"), COLOR_WHITE);
			break;
		case FBDF_DANCER_STATE_DANCING_LONG:
			DrawFormatString(x, y, COLOR_WHITE, _T("state: long(%d)"), this->len);
			break;
		case FBDF_DANCER_STATE_DANCING_1:
			DrawFormatString(x, y, COLOR_WHITE, _T("state: move 1-%d"), static_cast<int>(this->btn));
			break;
		case FBDF_DANCER_STATE_DANCING_2:
			DrawFormatString(x, y, COLOR_WHITE, _T("state: move 2-%d"), (int)(static_cast<int>(this->btn - 1) / 2) + 1);
			break;
		case FBDF_DANCER_STATE_DANCING_3:
		case FBDF_DANCER_STATE_DANCING_4:
			DrawFormatString(x, y, COLOR_WHITE, _T("state: move %d"), this->len);
			break;
		}
		return;
	}

	/**
	 * @brief デバッグ用、ウェイト時間を描く
	 * @param[in] x 描画横位置
	 * @param[in] y 描画縦位置
	 * @return なし
	 */
	void DrawDebugDanceWaitTime(int x, int y) const {
		int drawx2 = x + 100;
		int drawy2 = y + 10;
		DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, FALSE);
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_IDLE:
			/* nothing */
			break;
		case FBDF_DANCER_STATE_MISS:
			if (GetNowCount() - this->Stime < 500) { /* モーション中 */
				drawx2 = lins_scale(0, drawx2, 500, x, GetNowCount() - this->Stime);
			}
			else if (GetNowCount() - this->Stime < 5000) { /* afkまでの待機 */
				drawx2 = lins_scale(5000, drawx2, 500, x, GetNowCount() - this->Stime);
			}
			DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
			break;
		case FBDF_DANCER_STATE_AFK:
			drawx2 = lins_scale(5000, drawx2, 5500, x, GetNowCount() - this->Stime);
			DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
			break;
		case FBDF_DANCER_STATE_DANCING_LONG:
			drawx2 = lins_scale(0, drawx2, 750, x, GetNowCount() - this->Stime);
			DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
			break;
		case FBDF_DANCER_STATE_DANCING_1:
		case FBDF_DANCER_STATE_DANCING_2:
		case FBDF_DANCER_STATE_DANCING_3:
		case FBDF_DANCER_STATE_DANCING_4:
			drawx2 = lins_scale(this->mtime, drawx2, this->mtime + JUDGE_WIDTH, x, GetNowCount() - this->Stime);
			DrawBox(x, y, drawx2, drawy2, COLOR_WHITE, TRUE);
			break;
		}
		return;
	}

	/**
	 * @brief デバッグ用、モーション時間を描く
	 * @param[in] x 描画横位置
	 * @param[in] y 描画縦位置
	 * @return なし
	 */
	void DrawDebugDanceMotionTime(int x, int y) const {
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

#endif /* デバッグ描画 */

#if 1 /* ダンスモーション抽選系 */

	/**
	 * @brief srcがしたい下条件に合うかどうかを返す
	 * @param[in] src モーションデータ
	 * @param[in] next_len ブロック数
	 * @param[in] motion モーション指定
	 * @return bool 合ってたらtrue、違ったらfalse
	 */
	bool IsMatchMotion(const FBDF_Play_motion_st &src, int next_len, FBDF_note_motion_assign_et motion) const {
		if (next_len <= 0) { return false; } /* ダンスモーションじゃないので終わり */

		switch (next_len) {
		case 1:
			if (!src.len.n1) { return false; }
			break;
		case 2:
			if (!src.len.n2) { return false; }
			break;
		case 3:
			if (!src.len.n3) { return false; }
			break;
		case 4:
			if (!src.len.n4) { return false; }
			break;
		default: /* 5以上を想定 */
			if (!src.len.n8) { return false; }
			break;
		}

		switch (motion) {
		case FBDF_NOTE_MOTION_ASSIGN_UP:
			if (!src.type.up) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_DOWN:
			if (!src.type.down) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_LEFT:
			if (!src.type.left) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_RIGHT:
			if (!src.type.right) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_FRONT:
			if (!src.type.front) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_BACK:
			if (!src.type.back) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_JUMP:
			if (!src.type.jump) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_CLAP:
			if (!src.type.clap) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_TURN:
			if (!src.type.turn) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_1:
			if (!src.type.n1) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_2:
			if (!src.type.n2) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_3:
			if (!src.type.n3) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_4:
			if (!src.type.n4) { return false; }
			break;
		case FBDF_NOTE_MOTION_ASSIGN_VPOSE:
			if (!src.type.vpose) { return false; }
			break;
		default: /* 無指定と見なして処理 */
			if (src.extra) { return false; }
			break;
		}

		return true;
	}
	
	/**
	 * @brief 条件に合うダンスモーションを検索してsearched_motionに入れる。
	 * @param[in] next_len ブロック数
	 * @param[in] motion モーション指定
	 * @return size_t 見つけた数
	 */
	size_t SearchMotion(int next_len, FBDF_note_motion_assign_et motion) {
		this->searched_motion.clear();
		for (size_t i = 0; i < motion_data.size(); i++) {
			if (IsMatchMotion(motion_data[i], next_len, motion)) { searched_motion.push_back(i); }
		}
		return this->searched_motion.size();
	}

	/**
	 * @brief nextの中から条件に合うダンスモーションを検索してsearched_motionに入れる。
	 * @param[in] next_len ブロック数
	 * @param[in] motion モーション指定
	 * @param[in] next_list ネクストリスト
	 * @return size_t 見つけた数
	 */
	size_t SearchMotionWithNext(int next_len, FBDF_note_motion_assign_et motion, std::vector<size_t> next_list) {
		this->searched_motion.clear();
		if (next_list.size() == 0) { return 0; }
		for (size_t i = 0; i < next_list.size(); i++) {
			if (next_list[i] < motion_data.size()) {
				if (IsMatchMotion(motion_data[next_list[i]], next_len, motion)) {
					searched_motion.push_back(next_list[i]);
				}
			}
		}
		return this->searched_motion.size();
	}

	/**
	 * @brief searched_motionの中から抽選を行う
	 * @param なし
	 * @return size_t 抽選結果
	 */
	size_t GetMotionRandom(void) const {
		if (this->searched_motion.size() < 1) { return 0; } /* searched_motionが空っぽなのでとりあえず0を返しておく(1個でも画像データがあればそれを表示できる) */
		return searched_motion.at(GetRand(this->searched_motion.size() - 1));
	}

	/**
	 * @brief ダンスモーションを更新する。
	 * @param[in] next_len 次のモーションのブロック数
	 * @param[in] motion モーション指定
	 * @return なし
	 */
	void SetDanceMotionNo(int next_len, FBDF_note_motion_assign_et motion) {
		if (this->len <= 0) { return; } /* idle or miss なのでスキップ */
		if ((this->len == 1) &&
			(this->btn != 1) &&
			(this->motion_data[this->Nmotion_picNo].len.n1 &&
			motion == FBDF_NOTE_MOTION_ASSIGN_NONE)
		) {
			return;
		} /* len1モーションの時は、モーション指定or1ボタンでのみ更新 */

		if ((this->len == 2) &&
			(this->btn != 1) &&
			(this->btn != 2) &&
			(this->motion_data[this->Nmotion_picNo].len.n2 &&
			motion == FBDF_NOTE_MOTION_ASSIGN_NONE)
		) {
			return;
		} /* len2モーションの時は、モーション指定or1/2ボタンでのみ更新 */

		if (GetRand(99) + 1 < 30) { /* 30%にヒット */
			SearchMotion(next_len, motion);
			this->Nmotion_picNo = this->GetMotionRandom();
		}
		else if (SearchMotionWithNext(next_len, motion, this->motion_data[this->Nmotion_picNo].next) == 0) {
			/* nextの中に条件に合うモーションがない */
			SearchMotion(next_len, motion);
			this->Nmotion_picNo = this->GetMotionRandom();
		}
		else {
			/* 検索は終わってるのでさっさと抽選する */
			this->Nmotion_picNo = this->GetMotionRandom();
		}

#if FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
		MV1DetachAnim(this->n3Dmodel_handle, this->n3Dmotion_dance_ath);
		/* this->Nmotion_picNoを+2しているのは、0,1を別の用途で使用していて、nextに登録している番号はそれを考慮していないため */
		this->n3Dmotion_dance_ath = MV1AttachAnim(this->n3Dmodel_handle, this->Nmotion_picNo + 2);
#endif /* 3Dモデル */
	}

#endif /* ダンスモーション抽選系 */

#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
	/**
	 * @brief ダンサーを描く
	 * @param[in] x 描画横位置
	 * @param[in] y 描画縦位置
	 * @return なし
	 */
	void DrawDancerGraph(int x, int y) const {
		if (this->len == 0) { /* 待機モーション */
			DrawGraph(x, y, this->idle_pic.handle(this->GetMotionAnimNo()), TRUE);
		}
		else if (this->len < 0) { /* ミスモーション */
			DrawGraph(x, y, this->miss_pic.handle(this->GetMotionAnimNo()), TRUE);
		}
		else { /* ダンスモーション */
			const FBDF_Play_motion_st &motion_p = this->motion_data.at(Nmotion_picNo);
			DrawGraph(x, y, motion_p.pic.handle(this->GetMotionAnimNo()), TRUE);
		}
	}
#endif /* 画像 */

public:
	/**
	 * @brief ダンサー描画
	 * @param[in] x 描画横位置
	 * @param[in] y 描画縦位置
	 * @return なし
	 */
	void DrawDance(int x, int y) const {
#if 0 /* デバッグ */
		DrawString(x, y - 20, "ここでダンサーが踊る", COLOR_WHITE);
		this->DrawDebugDanceString(x, y);
		this->DrawDebugDanceWaitTime(x, y + 22);
		this->DrawDebugDanceMotionTime(x, y + 42);
#endif
#if FBDF_DANCER_MAT_TYPE == 0 /* 画像 */
		this->DrawDancerGraph(x, y);
#elif FBDF_DANCER_MAT_TYPE == 1 /* 3Dモデル */
		MV1DrawModel(this->n3Dmodel_handle);
#endif /* 3Dモデル */
	}

#if 1 /* update系 */
private: /* update系 */
	void UpdateAnimTime(void) {
		size_t motion_frameNo = this->GetMotionAnimNo();
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_IDLE:
			MV1SetAttachAnimTime(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,  motion_frameNo);
			break;
		case FBDF_DANCER_STATE_MISS:
		case FBDF_DANCER_STATE_AFK:
			MV1SetAttachAnimTime(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,  motion_frameNo);
			break;
		case FBDF_DANCER_STATE_DANCING_1:
		case FBDF_DANCER_STATE_DANCING_2:
		case FBDF_DANCER_STATE_DANCING_3:
		case FBDF_DANCER_STATE_DANCING_4:
			MV1SetAttachAnimTime(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, motion_frameNo);
			break;
		case FBDF_DANCER_STATE_DANCING_LONG:
			MV1SetAttachAnimTime(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, motion_frameNo);
			MV1SetAttachAnimTime(this->n3Dmodel_handle, this->n3Dmotion_idle_ath, this->GetIdleMotionAnimNo()); /* idleも計算する必要あり */
			break;
		}
	}

	void UpdateAttachAnimMat(void) {
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_IDLE:
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,  1);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,  0);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, 0);
			break;
		case FBDF_DANCER_STATE_MISS:
		case FBDF_DANCER_STATE_AFK:
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,  0);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,  1);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, 0);
			break;
		case FBDF_DANCER_STATE_DANCING_1:
		case FBDF_DANCER_STATE_DANCING_2:
		case FBDF_DANCER_STATE_DANCING_3:
		case FBDF_DANCER_STATE_DANCING_4:
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,  0);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,  0);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, 1);
			break;
		case FBDF_DANCER_STATE_DANCING_LONG:
		{
			float idle_per = lins_scale(500, 0.0, 750, 1.0, GetNowCount() - this->Stime);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,      idle_per);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,             0);
			MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, 1 - idle_per);
		}
			break;
		}
	}

	/* ステートの更新 */
	void UpdateState(void) {
		FBDF_dancer_state_et resv_state = FBDF_DANCER_STATE_IDLE;
		if (this->len == 0) { /* 待機モーション */
			resv_state = FBDF_DANCER_STATE_IDLE;
		}
		else if (this->len < 0) { /* ミスモーション */
			if (5000 <= GetNowCount() - this->Stime) {
				resv_state = FBDF_DANCER_STATE_AFK;
			}
			else {
				resv_state = FBDF_DANCER_STATE_MISS;
			}
		}
		else if (this->len == 1) { /* ダンスモーション */
			resv_state = FBDF_DANCER_STATE_DANCING_1;
		}
		else if (this->len == 2) {
			resv_state = FBDF_DANCER_STATE_DANCING_2;
		}
		else if (this->len == 3) {
			resv_state = FBDF_DANCER_STATE_DANCING_3;
		}
		else if (this->len == 4) {
			resv_state = FBDF_DANCER_STATE_DANCING_4;
		}
		else if (5 <= this->len) {
			resv_state = FBDF_DANCER_STATE_DANCING_LONG;
		}
		this->Nstate = resv_state;
		UpdateAttachAnimMat();
	}

public: /* update系 */
	/**
	 * @brief 内部情報の更新、最低でも描画前に呼んで。
	 * @param なし
	 * @return なし
	 */
	void Update(void) {
		/* missは自動解消されない */
		if (0 <= this->len) {
			/* long->idle */
			if (4 < this->len) {
				if (750 + this->Stime <= GetNowCount()) {
					this->len = 0;
				}
			}
			else {
				/* motion->idle */
				if (this->mtime + JUDGE_WIDTH + this->Stime <= GetNowCount()) {
					this->len = 0;
				}
			}
		}
		this->UpdateState();
		this->UpdateAnimTime();
		this->SetShapeAll();
		return;
	}
#endif /* update系 */

#if 1 /* set系 */
	/**
	 * @brief ミスモーションにする
	 * @param なし 
	 * @return なし
	 */
	void SetMissState(void) {
		if (0 <= this->len) {
			this->len = -1;
			this->Stime = GetNowCount();
		}
	}
	
	/**
	 * @brief ステートを変える
	 * @param[in] a_btn 押されたボタン
	 * @param[in] a_len 次のノートまでのブロック数、0未満にするとmissモーション、0にするとidleモーションにできる。
	 * @param[in] a_mtime モーションの時間
	 * @param[in] motion モーション指定
	 * @return なし
	 * @details 裏技、a_lenを0未満にするとmissモーション、0にするとidleモーションにできる。
	 * その時はa_btnとa_mtimeとallowを無視できる。適当に0とか入れといて。
	 */
	void SetState(FBDF_Play_note_btn_et a_btn, int a_len, uint a_mtime, FBDF_note_motion_assign_et motion) {
		if (a_len < 0) { /* a_lenに0未満指定はミスモーションとして扱うので、専用関数呼んで終わり */
			this->SetMissState();
			return;
		}

		this->btn = a_btn;
		this->len = a_len;
		this->mtime = a_mtime;
		this->Stime = GetNowCount();
		if (1 <= a_len) { this->SetDanceMotionNo(this->len, motion); }
	}

	/**
	 * @brief BPMを変える
	 * @param[in] val BPM。0.1未満を指定することはできない。
	 * @return なし
	 */
	void SetBpm(double val) {
		this->bpm = max(0.1, val);
	}
#endif /* set系 */
};

class FBDF_score_bar_c {
private:
	FBDF_score_bar_st score_bar;
	double score_ave = 100.0;

	int Stime = 0;
	int Etime = 60000;

	FBDF_score_bar_st graph[FBDF_RESULT_SCORE_GRAPH_COUNT]; /* リザルト用 */
	int graphNo = 0; /* 次に入れる場所 */

	/**
	 * @brief 現在のスコアバーの状態をスコア推移データに追加する
	 * @param なし
	 * @return なし
	 */
	void set_graph(void) {
		if (this->graphNo >= FBDF_RESULT_SCORE_GRAPH_COUNT) { return; }
		this->graph[this->graphNo] = this->score_bar;
		this->graphNo++;
	}

public:
	/**
	 * @brief スコアバーを更新する
	 * @param[in] score スコア
	 * @param[in] noteN 今のノート番号
	 * @return なし
	 */
	void update_score(const FBDF_score_st *score, uint noteN) {
		uint hit_notes = score->crit + score->hit + score->drop;
		uint remain_notes = noteN - hit_notes;
		this->score_bar.bar_70 = 100 * (score->point + (70.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_90 = 100 * (score->point + (90.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_96 = 100 * (score->point + (96.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_98 = 100 * (score->point + (98.5 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_99 = 100 * (score->point + (99.1 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_ave = DIV_AVOID_ZERO(100 * score->point, hit_notes * CRIT_SCORE, 100.00);
	}

	/**
	 * @brief スコアバーの推移データを更新する
	 * @param[in] Ntime 今の時間。相対時間。[ms]
	 * @return なし
	 */
	void update_graph(int Ntime) {
		if (this->graphNo >= FBDF_RESULT_SCORE_GRAPH_COUNT) { return; }
		if (((Etime * this->graphNo + Stime * (FBDF_RESULT_SCORE_GRAPH_COUNT - this->graphNo - 1)) / (FBDF_RESULT_SCORE_GRAPH_COUNT - 1)) < Ntime)
		{
			this->set_graph();
		}
	}

	/**
	 * @brief 時間をセットする。スコアバーの推移データに必要
	 * @param[in] Stime 開始時間。相対時間。[ms]
	 * @param[in] Etime 終了時間。相対時間。[ms]
	 * @return なし
	 */
	void set_time(int Stime, int Etime) {
		this->Stime = Stime;
		this->Etime = Etime;
	}

	/**
	 * @brief 残りのスコアバーの推移を強制的に埋める。強制終了用。
	 * @param なし
	 * @return なし
	 */
	void fill_graph_force(void) {
		for (int i = this->graphNo; i < FBDF_RESULT_SCORE_GRAPH_COUNT; i++) {
			this->set_graph();
		}
	}

	/**
	 * @brief スコアバーの推移を取得する
	 * @param[out] dest 格納先
	 * @return なし
	 */
	void get_graph(FBDF_score_bar_st *dest) const {
		for (int i = 0; i < FBDF_RESULT_SCORE_GRAPH_COUNT; i++) {
			dest[i] = this->graph[i];
		}
	}

	/**
	 * @brief スコアバーの描画
	 * @param[in]  left 描画左位置
	 * @param[in]    up 描画上位置
	 * @param[in] right 描画右位置
	 * @param[in]  down 描画下位置
	 * @return なし
	 */
	void draw_bar(int x1, int y1, int x2, int y2) const {
		FBDF_DrawScoreBarHori(this->score_bar, x1, y1, x2, y2);
	}

	/**
	 * @brief 精度スコアを取得する
	 * @param なし
	 * @return 精度スコア
	 */
	double GetScore_ave(void) const { return this->score_ave; }
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
	/**
	 * @brief ギャップバーの描画
	 * @param[in]  left 描画左位置
	 * @param[in]    up 描画上位置
	 * @param[in] right 描画右位置
	 * @param[in]  down 描画下位置
	 * @return なし
	 */
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

	/**
	 * @brief データを追加する
	 * @param[in] val 追加するデータ
	 * @return なし
	 */
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

	/**
	 * @brief 全データの平均を取得する。this->max_countに関わらず、今までのデータすべてが対象。データがないときは0を返す。
	 * @param なし
	 * @return 全データの平均
	 */
	double GetAve(void) const { return DIV_AVOID_ZERO(this->sum, this->count, 0); }
};

class FBDF_play_notes_draw_c {
private:
	bool is_missing = false;
	DxTime_t wave_time = 0;
	FBDF_Play_note_pic_st note_pic;

	int GetNotesAlpha(void) const {
		if (is_missing) { return 0xFF; }
		return lins_scale(1000, 0xFF, 3000, 0, GetNowCount() - wave_time);
	}

public:
	/**
	 * @brief ノーツの描画
	 * @param[in] left  描画左位置
	 * @param[in] right 描画右位置
	 * @param[in] down  描画下位置
	 * @param[in] wave_time ミス復帰時間[ms]
	 * @param[in] map マップデーター
	 * @return なし
	 * @details 描画上位置は0固定
	 */
	void DrawNotes(int left, int right, int down, const FBDF_map_t &map) const {
		int BaseLeft  = left;
		int BaseRight = right;
		int DrawYpos  = 0;
		int DrawLeft  = BaseLeft;
		int DrawRight = BaseRight;
		DxPic_t Npic = DXLIB_PIC_HAND_DEFAULT;

		for (int in = map.note.nowNo(); in < map.note.size(); in++) {
			if (map.note[in].time == 0) { break; }

			/* 描画横位置 */
			switch (game_option.play_style) {
        	case FBDF_PLAYSTYLE_ASSIST_PLUS:
				switch (map.note[in].btn) {
				case 1:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 0);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 1);
					break;
				case 2:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 1);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 2);
					break;
				case 3:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 2);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 3);
					break;
				case 4:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 3);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 4);
					break;
				}
				break;
        	case FBDF_PLAYSTYLE_ASSIST:
				switch (map.note[in].btn) {
				case 1:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 0);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 1);
					break;
				case 2:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 1);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 2);
					break;
				case 3:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 2);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 3);
					break;
				case 4:
					DrawLeft  = lins_scale(0, BaseLeft, 4, BaseRight, 3);
					DrawRight = lins_scale(0, BaseLeft, 4, BaseRight, 4);
					break;
				}
				if (!this->is_missing) {
					DrawLeft  = lins_scale(1000, DrawLeft,  3000, BaseLeft,  GetNowCount() - this->wave_time);
					DrawRight = lins_scale(1000, DrawRight, 3000, BaseRight, GetNowCount() - this->wave_time);
				}
				break;
        	case FBDF_PLAYSTYLE_NORMAL:
        	case FBDF_PLAYSTYLE_BLANC:
        	case FBDF_PLAYSTYLE_BLANC_PLUS:
				DrawLeft  = BaseLeft;
				DrawRight = BaseRight;
				break;
			}

			/* 描画色 */
			switch (map.note[in].btn) {
			case 1:
				Npic = this->note_pic.one.handle();
				break;
			case 2:
				Npic = this->note_pic.two.handle();
				break;
			case 3:
				Npic = this->note_pic.three.handle();
				break;
			case 4:
				Npic = this->note_pic.four.handle();
				break;
			}

			/* 描画縦位置 */ {
				int time_gap = map.note[in].time - map.Ntime;
				DrawYpos = down - NOTE_HEIGHT -
					(int)((time_gap - 16 + game_option.note_offset_draw) *
					game_option.lane_speed) / 50;
			}
			if (NOTE_HEIGHT + DrawYpos < 0) { break; } /* 画面外break */

			/* 描画処理 */
			switch (game_option.play_style) {
        	case FBDF_PLAYSTYLE_ASSIST_PLUS:
        	case FBDF_PLAYSTYLE_ASSIST:
        	case FBDF_PLAYSTYLE_NORMAL:
				DrawExtendGraph(DrawLeft, NOTE_HEIGHT + DrawYpos, DrawRight, DrawYpos, Npic, TRUE);
				break;
        	case FBDF_PLAYSTYLE_BLANC:
				DrawExtendGraph(DrawLeft, NOTE_HEIGHT + DrawYpos, DrawRight, DrawYpos, this->note_pic.white.handle(), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->GetNotesAlpha());
				DrawExtendGraph(DrawLeft, NOTE_HEIGHT + DrawYpos, DrawRight, DrawYpos, Npic, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				break;
        	case FBDF_PLAYSTYLE_BLANC_PLUS:
				DrawExtendGraph(DrawLeft, NOTE_HEIGHT + DrawYpos, DrawRight, DrawYpos, this->note_pic.white.handle(), TRUE);
				break;
			}
		}
		return;
	}

	void SetMiss(void) {
		is_missing = true;
	}

	void SetRecover(void) {
		if (!is_missing) { return; }
		is_missing = false;
		wave_time = GetNowCount();
	}
};

#endif /* class */

/* プレイ画面に関するクラスをまとめたもの */
typedef struct FBDF_play_class_set_s {
	FBDF_judge_c judge_class;
	FBDF_dancer_c dancer_class;
	FBDF_score_bar_c score_bar_class;
	FBDF_gap_bar_c gap_bar_class;
	FBDF_play_notes_draw_c notes_draw_class;
} FBDF_play_class_set_t;

#if 1 /* Draw系 */

/**
 * @brief 押しキーランプの描画
 * @param[in] pkey キー入力情報
 * @return なし
 */
static void FBDF_PlayDrawLamp(const FBDF_push_key_st *pkey) {
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

#if 1 /* ノーツ判定系 */

/**
 * @brief キー押し検出後の1ノーツ判定
 * @param[out] buf 判定イベント格納先
 * @param[out] key_detect キー検出。falseに書き換えられることがある
 * @param[out] map マップデータ
 * @return なし
 */
static void FBDF_Play_OneNoteJudgeAfterKeyDetect(FBDF_judge_event_st &buf, bool &key_detect, const FBDF_map_t *map) {
	if (key_detect) {
		key_detect = false;
		buf.gap = map->note.nowData().time - map->Ntime;
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

	switch (game_option.play_style) {
	case FBDF_PLAYSTYLE_ASSIST_PLUS:
		buf.score = buf.score * 90 / 100;
		break;
	case FBDF_PLAYSTYLE_ASSIST:
		buf.score = buf.score * 95 / 100;
		break;
	case FBDF_PLAYSTYLE_NORMAL:
		buf.score = buf.score * 99 / 100;
		break;
	case FBDF_PLAYSTYLE_BLANC:
		buf.score = buf.score * 995 / 1000;
		break;
	case FBDF_PLAYSTYLE_BLANC_PLUS:
		/* buf.score *= 1.00; */
		break;
	}
	return;
}

/**
 * @brief ノートイベントを処理する
 * @param[in] judge_event 予約されたノートイベント。関数を抜けた後は空っぽになる。
 * @param[out] play_class プレイクラス
 * @param[out] score スコアデータ
 * @param[in] noteN ノーツ数
 * @param[in] se 効果音データ
 */
static void FBDF_Play_NoteJudgeEventAntion(
	std::queue<FBDF_judge_event_st> &judge_event, FBDF_play_class_set_t *play_class,
	FBDF_score_st *score, size_t noteN, const FBDT_hit_snd_t *se
) {
	bool note_judged = !judge_event.empty();
	FBDF_judge_event_st buf;

	FBDF_judge_c           *judge_class      = &play_class->judge_class;
	FBDF_dancer_c          *dancer_class     = &play_class->dancer_class;
	FBDF_score_bar_c       *score_bar_class  = &play_class->score_bar_class;
	FBDF_gap_bar_c         *gap_bar_class    = &play_class->gap_bar_class;
	FBDF_play_notes_draw_c *notes_draw_class = &play_class->notes_draw_class;

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
			case FBDF_PLAY_NOTE_BTN_1:
				PlaySoundMem(se->SE1Data.handle(), DX_PLAYTYPE_BACK);
				break;
			case FBDF_PLAY_NOTE_BTN_2:
			case FBDF_PLAY_NOTE_BTN_3:
			case FBDF_PLAY_NOTE_BTN_4:
				PlaySoundMem(se->SE2Data.handle(), DX_PLAYTYPE_BACK);
				break;
			}
		}

		/* コンボ計算 */
		if (buf.mat != JUDGE_MISS) { score->chain++; }

		/* スコア計算 */
		score->point += buf.score;
		if (FBDF_PLAYSTYLE_NORMAL <= game_option.play_style) { score->chain_point += score->chain; }

		/* キャラモーション変更 */
		if (buf.mat == JUDGE_MISS) {
			dancer_class->SetMissState();
		}
		else {
			dancer_class->SetState(buf.tip, buf.len, buf.mtime, buf.motion);
		}

		/* gap追加 */
		if (buf.mat != JUDGE_MISS) {
			gap_bar_class->SetVal(buf.gap);
		}

		/* ノーツ描画に影響するパラメータ */
		if (buf.mat != JUDGE_MISS) {
			notes_draw_class->SetRecover();
		}
		else {
			notes_draw_class->SetMiss();
		}
	}

	if (note_judged) {
		score_bar_class->update_score(score, noteN);
	}

	return;
}

/**
 * @brief ノーツの判定
 * @param[out] play_class プレイクラス
 * @param[out] score スコア
 * @param[out] map マップデータ
 * @param[in] pkey キー入力情報
 * @param[in] se 効果音情報
 * @return なし
 */
static void FBDF_PlayNoteJudge(
	FBDF_play_class_set_t *play_class, FBDF_score_st *score, FBDF_map_t *map,
	const FBDF_push_key_st *pkey, const FBDT_hit_snd_t *se)
{
	std::queue<FBDF_judge_event_st>judge_event;
	FBDF_judge_event_st buf;

	bool key_detect_d = (pkey->D == 1);
	bool key_detect_f = (pkey->F == 1);
	bool key_detect_j = (pkey->J == 1);
	bool key_detect_k = (pkey->K == 1);

	if (1 <= pkey->alltap) {
		while (map->note.nowData().time != 0 &&
			map->note.nowData().time - JUDGE_WIDTH < map->Ntime &&
			(key_detect_d || key_detect_f || key_detect_j || key_detect_k))
		{
			buf.tip = map->note.nowData().btn;
			buf.len = map->note.nowData().len;
			buf.mtime = map->note.nowData().mtime;
			buf.motion = map->note.nowData().motion;
			switch (map->note.nowData().btn) {
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
			if (buf.mat != JUDGE_NONE) { map->note.stepNo(); }
		}
	}

	if (map->note.nowData().time != 0 &&
		map->note.nowData().time + JUDGE_WIDTH < map->Ntime)
	{
		buf.mat = JUDGE_MISS;
		judge_event.push(buf);
		map->note.stepNo();
	}

	FBDF_Play_NoteJudgeEventAntion(judge_event, play_class, score, map->note.size(), se);
	return;
}

/**
 * @brief 残っているすべてのノーツをdropにする。強制終了した時用
 * @param[out] play_class プレイクラス
 * @param[out] score スコア
 * @param[out] map マップデータ
 * @return なし
 */
static void FBDF_PlayNoteTrash(FBDF_play_class_set_t *play_class, FBDF_score_st *score, FBDF_map_t *map) {
	size_t remain_notes = 0;
	FBDF_judge_c     *judge_class     = &play_class->judge_class;
	FBDF_dancer_c    *dancer_class    = &play_class->dancer_class;
	FBDF_score_bar_c *score_bar_class = &play_class->score_bar_class;

	FBDF_judge_event_st buf;

	while (map->note.nowData().time != 0) {
		remain_notes++;
		map->note.stepNo();
	}

	if (0 < remain_notes) {
		judge_class->SetJudge(JUDGE_MISS);
		score->drop += remain_notes;
		dancer_class->SetMissState();
		score_bar_class->update_score(score, map->note.size());
	}

	return;
}

#endif /* ノーツ判定系 */

/**
 * @brief リザルト用のデータを作成
 * @param[out] result_data 格納先
 * @param[in] nex_music セレクト画面から渡されたデータ
 * @param[in] map マップデータ
 * @param[in] score スコア
 * @param[in] play_class プレイクラス
 * @return なし
 */
static void FBDF_Play_MakeResultData(FBDF_result_data_t *result_data, const FBDF_play_choose_music_st *nex_music,
	const FBDF_map_t &map, const FBDF_score_st &score, const FBDF_play_class_set_t &play_class
) {
	result_data->name        = nex_music->folder_name;
	result_data->artist      = map.artist;
	result_data->folder_name = nex_music->folder_name;
	result_data->level       = 0;
	result_data->score       = score.point + score.chain_point;
	result_data->acc         = play_class.score_bar_class.GetScore_ave();
	result_data->crit        = score.crit;
	result_data->hit         = score.hit;
	result_data->save        = score.save;
	result_data->drop        = score.drop;
	result_data->gap_ave     = play_class.gap_bar_class.GetAve();
	play_class.score_bar_class.get_graph(result_data->score_graph);
	result_data->dif_type    = nex_music->dif_type;
	return;
}

/**
 * @brief 譜面を読み込む
 * @param[in] folder_name フォルダ名
 * @param[in] map_file_name 譜面のファイル名
 * @return bool true=成功, false=失敗
 */
static bool FBDF_Play_MapLoad(FBDF_map_t &map, const TCHAR *folder_name, const TCHAR *map_file_name) {
	FBDF_mapenc_error_et ret;
	std::string path = "music/";
	path += folder_name;
	path += '/';
	path += map_file_name;
	ret = MapLoadOne(map, path.c_str());
	if (ret != FBDF_MAPENC_ERROR_NONE) {
		/* エラーメッセージか何かを残したい */
		if (ret == FBDF_MAPENC_ERROR_FILE) {
			return false;
		}
		else {
			/* 気にせずスルー */
		}
	}
	map.note.resetNo();
	map.Stime = GetNowCount();
	return true;
}

/**
 * @brief 曲の音声ファイルを読み込む
 * @param[in] folder_name フォルダ名
 * @param[in] music_file_name 曲のファイル名
 * @return なし
 */
static void FBDF_Play_Loadmusic(dxcur_snd_c &dest, const TCHAR *folder_name, const TCHAR *music_file_name) {
	std::string path = "music/";
	path += folder_name;
	path += '/';
	path += music_file_name;
	dest.reload(path.c_str());
	return;
}

/**
 * @brief キー入力関係
 * @param[out] pkey キー入力情報
 * @param[out] play_class プレイクラス、クローズが押された時の処理用
 * @param[out] score スコア、クローズが押された時の処理用
 * @param[out] map マップデータ、クローズが押された時とオートプレイ時のキー判定用
 * @param[in] auto_fg オートフラグ、trueならオートプレイ用の処理になる
 * @param[out] cutin カットインクラス、クローズが押された時の判定と処理用
 * @return なし
 */
static void FBDF_Play_KeyCheck(
	FBDF_push_key_st &pkey, FBDF_play_class_set_t &play_class,
	FBDF_score_st &score, FBDF_map_t &map, bool auto_fg, FBDF_cutin_c &cutin
) {
	if (!cutin.IsClosing() && (CheckHitKey(KEY_INPUT_ESCAPE) == 1)) {
		FBDF_PlayNoteTrash(&play_class, &score, &map);
		play_class.score_bar_class.fill_graph_force();
		cutin.SetIo(CUT_FRAG_IN);
	}

	if (auto_fg) {
		pkey.D = (IS_BETWEEN(1, pkey.D, 5)) ? (pkey.D + 1) : (0);
		pkey.F = (IS_BETWEEN(1, pkey.F, 5)) ? (pkey.F + 1) : (0);
		pkey.J = (IS_BETWEEN(1, pkey.J, 5)) ? (pkey.J + 1) : (0);
		pkey.K = (IS_BETWEEN(1, pkey.K, 5)) ? (pkey.K + 1) : (0);

		if (map.note.nowData().time <= 8 + map.Ntime) {
			switch (map.note.nowData().btn) {
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

static void FBDF_Play_DrawScore(int x, int y, FBDF_score_st &score) {
	uint all_point = score.point + score.chain_point;
	int drawX = x;
	if (all_point < 10) {
		drawX = x - 28;
	}
	else if (all_point < 100) {
		drawX = x - 28 * 2;
	}
	else if (all_point < 1000) {
		drawX = x - 28 * 3;
	}
	else if (all_point < 10000) {
		drawX = x - 28 * 4;
	}
	else if (all_point < 100000) {
		drawX = x - 28 * 5;
	}
	else if (all_point < 1000000) {
		drawX = x - 28 * 6;
	}
	else if (all_point < 10000000) {
		drawX = x - 28 * 7;
	}
	DrawFormatStringToHandle(drawX, y, COLOR_WHITE, FBDF_font_DSEG7Modern, _T("%7d"), all_point);
}

/**
 * @brief プレイ画面のベース
 * @param[out] result_data リザルト画面に渡すデータ
 * @param[in] nex_music セレクト画面から渡されたデータ
 * @return view_num_t 次の画面
 */
view_num_t FBDF_PlayView(FBDF_result_data_t *result_data, const FBDF_play_choose_music_st *nex_music) {
	FBDF_map_t map;
	FBDF_score_st score;
	FBDF_push_key_st pkey;

	FBDF_play_class_set_t play_class;
	FBDF_cutin_c cutin;

	dxcur_pic_c backPic(_T("pic/play/PlayBack.png"));
	dxcur_pic_c lanePic(_T("pic/play/PlayLane.png"));

	dxcur_snd_c musicData;
	FBDT_hit_snd_t se;

	DxTime_t FinishTime = 0;

	/* 譜面読み込み系 */
	if (FBDF_Play_MapLoad(map, nex_music->folder_name.c_str(), nex_music->map_file_name.c_str()) == false) { return VIEW_SELECT; }
	map.note.resetNo();
	play_class.score_bar_class.set_time(map.offset, map.Etime);
	play_class.dancer_class.SetBpm(map.bpm);

	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	FBDF_Play_Loadmusic(musicData, nex_music->folder_name.c_str(), map.music_file);
	PlaySoundMem(musicData.handle(), DX_PLAYTYPE_BACK);

	/* 曲再生後にやるべき初期化 */
	cutin.SetIo(CUT_FRAG_OUT);
	map.Stime = GetNowCount();

	/* ゲーム実行ループ */
	while (1) {
		if (cutin.IsEndAnim()) { break; }

		map.Ntime = GetNowCount() - map.Stime; /* 時間更新 */

		FBDF_Play_KeyCheck(pkey, play_class, score, map, game_option.auto_en, cutin);

		/* ノーツ全処理判定 */
		if ((FinishTime == 0) && (map.note.size() == map.note.nowNo() + 1)) { FinishTime = map.Ntime; }

		/* 譜面終了判定 */
		if (!cutin.IsClosing() && (FinishTime != 0) && ((FinishTime + 2000) <= map.Ntime) && (CheckSoundMem(musicData.handle()))) {
			cutin.SetIo(CUT_FRAG_IN);
		}

		FBDF_PlayNoteJudge(&play_class, &score, &map, &pkey, &se);

		/* update系 */
		play_class.dancer_class.Update();
		play_class.score_bar_class.update_graph(map.Ntime);
		cutin.update();

		ClearDrawScreen(); /* 作画エリアここから */ {
			DrawGraph(0, 0, backPic.handle(), TRUE); /* 背景描画 */

			/* ダンサー周り描画 */
			play_class.dancer_class.DrawDance(500, 300);
			FBDF_Play_DrawScore(WINDOW_SIZE_X - 20, 20, score);

			/* スコアバー周り描画 */
			play_class.score_bar_class.draw_bar(167, 600, 928, 650);
			FBDF_PlayDrawLamp(&pkey);

			/* プレイエリア周り描画 */
			DrawGraph(0, 0, lanePic.handle(), TRUE);
			DrawFormatString(166, 663, COLOR_WHITE, _T("%s"), nex_music->folder_name.c_str());
			play_class.notes_draw_class.DrawNotes(42, 42 + 110, 570, map);
			play_class.gap_bar_class.DrawBar(40, 576, 155, 691);
			if (game_option.judge_draw_en) { play_class.judge_class.DrawJudge(270, 530); }

			cutin.DrawCut();

		} ScreenFlip(); /* 作画エリアここまで */

		if (GetWindowUserCloseFlag(TRUE)) { // 閉じるボタンが押された
			StopSoundMem(musicData.handle());
			return VIEW_EXIT;
		}
		WaitTimer(10); // ループウェイト
	}

	StopSoundMem(musicData.handle());

	if (game_option.auto_en) { return VIEW_SELECT; } /* オートプレイなら選択画面直行 */

	FBDF_Play_MakeResultData(result_data, nex_music, map, score, play_class);

	return VIEW_RESULT;
}
