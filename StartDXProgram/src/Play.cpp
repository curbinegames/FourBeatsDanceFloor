
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

#define SAVE_SCORE_WIDTH 140

#define NOTE_COLOR_1 0xFF7FD5FD
#define NOTE_COLOR_2 0xFF00E600
#define NOTE_COLOR_3 0xFFFF62FB
#define NOTE_COLOR_4 0xFFFFFF00

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

class FBDF_push_key_c {
public:
	int D = 0;
	int F = 0;
	int J = 0;
	int K = 0;
	int alltap = 0;

private:
	void update_key_auto(const FBDF_map_t &map) {
		this->D = (IS_BETWEEN(1, this->D, 5)) ? (this->D + 1) : (0);
		this->F = (IS_BETWEEN(1, this->F, 5)) ? (this->F + 1) : (0);
		this->J = (IS_BETWEEN(1, this->J, 5)) ? (this->J + 1) : (0);
		this->K = (IS_BETWEEN(1, this->K, 5)) ? (this->K + 1) : (0);

		if (map.note.nowData().time <= 8 + map.Ntime) {
			switch (map.note.nowData().btn) {
			case FBDF_PLAY_NOTE_BTN_1:
				this->D = 1;
				this->F = 0;
				this->J = 0;
				this->K = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_2:
				this->D = 0;
				this->F = 1;
				this->J = 0;
				this->K = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_3:
				this->D = 0;
				this->F = 0;
				this->J = 1;
				this->K = 0;
				break;
			case FBDF_PLAY_NOTE_BTN_4:
				this->D = 0;
				this->F = 0;
				this->J = 0;
				this->K = 1;
				break;
			}
		}
	}

	void update_key_manual(void) {
		this->D = (CheckHitKey(KEY_INPUT_D) == 1) ? (this->D + 1) : (0);
		this->F = (CheckHitKey(KEY_INPUT_F) == 1) ? (this->F + 1) : (0);
		this->J = (CheckHitKey(KEY_INPUT_J) == 1) ? (this->J + 1) : (0);
		this->K = (CheckHitKey(KEY_INPUT_K) == 1) ? (this->K + 1) : (0);
	}

	void update_alltap(void) {
		this->alltap = 0;
		this->alltap += (this->D == 1);
		this->alltap += (this->F == 1);
		this->alltap += (this->J == 1);
		this->alltap += (this->K == 1);
	}

public:
	void update(const FBDF_map_t &map, bool auto_fg) {
		if (auto_fg) { this->update_key_auto(map); }
		else { this->update_key_manual(); }
		this->update_alltap();
	}
};

typedef struct FBDF_score_s {
	uint crit  = 0;
	uint  hit  = 0;
	uint save  = 0;
	uint drop  = 0;
	uint chain = 0;
	uint base_point = 0; /* 難易度補正をかける前の得点。理論値 = 184 * ノーツ数 */
	uint scale_point = 0; /* base_pointに難易度補正をかけた後の得点。スコアバーで使用する */
	uint chain_point = 0; /* チェインによる得点。理論値 = 1~ノーツ数までの和 */
	uint all_point = 0; /* 最終的な得点 */
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
	bool is_fast = false;
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
		dxcur_pic_c fast = dxcur_pic_c(_T("pic/play/JudgeFast.png"));
		dxcur_pic_c slow = dxcur_pic_c(_T("pic/play/JudgeSlow.png"));
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
	* @brief 判定の描画
	* @param[in] x 描画横位置
	* @param[in] y 描画縦位置
	* @return なし
	*/
	void DrawFastSlow(int x, int y) const {
		if (this->Jmat == JUDGE_CRIT) { return; }
		if (this->Jmat == JUDGE_MISS) { return; }
		const double zoom = 0.5;
		if (this->Jtime + 750 < GetNowCount()) { return; }
		DrawDeformationPic(x, y, zoom, zoom, 0,
			(this->is_fast) ? (this->pic.fast.handle()) : (this->pic.slow.handle())
		);
	}

	/**
	 * @brief 判定をセットする
	 * @param[in] mat セットする判定
	 * @param[in] gap 判定の差
	 * @return なし
	 */
	void SetJudge(FBDF_judge_mat_et mat, int gap) {
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
		this->is_fast = (0 < gap);
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

	int      len    = 0; /* -1:miss, 0:idle, 1~4:tip, 5~:long */
	DxTime_t mtime  = 0; /* モーション長さ */
	int      Stime  = 0; /* モーションスタート絶対時間 */
	int      offset = 0; /* 待機ステップ開始時間 */
	size_t   Nmotion_picNo = 0; /* 今のダンスモーション番号 */
	double   bpm = 120;
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

	int n3Dshape_blick_hdl = -1;
	int n3Dshape_smile_hdl = -1;
	int n3Dshape_surps_hdl = -1;
	int n3Dshape_a_hdl = -1;
	int n3Dshape_i_hdl = -1;
	int n3Dshape_u_hdl = -1;
	int n3Dshape_e_hdl = -1;
	int n3Dshape_o_hdl = -1;

	std::vector<size_t> searched_motion;
	std::vector<FBDF_Play_motion_st> motion_data;

public: /* コンストラクタ系 */
	FBDF_dancer_c(void) : FBDF_dancer_c(FBDF_DANCER_UNIOW) {} /* ユニオとして初期化 */

	FBDF_dancer_c(size_t n) {
		float model_size = lins(0, 0, 720, 2, WINDOW_SIZE_Y);
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
			lins(0, 0,  720, 500,  WINDOW_SIZE_Y)
		));
		MV1SetScale(this->n3Dmodel_handle, VGet(model_size, model_size, model_size));
		this->n3Dmotion_idle_ath = MV1AttachAnim(this->n3Dmodel_handle, 0);
		this->n3Dmotion_miss_ath = MV1AttachAnim(this->n3Dmodel_handle, 1);
		this->n3Dshape_blick_hdl = MV1SearchShape(this->n3Dmodel_handle, _T("まばたき"));
		this->n3Dshape_smile_hdl = MV1SearchShape(this->n3Dmodel_handle, _T("笑い"));
		this->n3Dshape_surps_hdl = MV1SearchShape(this->n3Dmodel_handle, _T("点目"));
		this->n3Dshape_a_hdl     = MV1SearchShape(this->n3Dmodel_handle, _T("あ"));
		this->n3Dshape_i_hdl     = MV1SearchShape(this->n3Dmodel_handle, _T("い"));
		this->n3Dshape_u_hdl     = MV1SearchShape(this->n3Dmodel_handle, _T("う"));
		this->n3Dshape_e_hdl     = MV1SearchShape(this->n3Dmodel_handle, _T("え"));
		this->n3Dshape_o_hdl     = MV1SearchShape(this->n3Dmodel_handle, _T("お"));
		MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_idle_ath,  1);
		MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_miss_ath,  0);
		MV1SetAttachAnimBlendRate(this->n3Dmodel_handle, this->n3Dmotion_dance_ath, 0);
#if (FBDF_LOG_LEVEL_DEF <= 1)
		{
			int anim_num = MV1GetAnimNum(this->n3Dmodel_handle);
			std::string log_str = "";
			for (int inum = 0; inum < anim_num; inum++) {
				log_str  = "検出したアニメ[";
				log_str += ('0' + inum);
				log_str += "]: ";
				log_str += MV1GetAnimName(this->n3Dmodel_handle, inum);
				FBDF_LOG_INFO(log_str.c_str());
			}
		}
		{
			int shape_num = MV1GetShapeNum(this->n3Dmodel_handle);
			std::string log_str = "検出したシェイプキーの数: ";
			log_str += std::to_string(shape_num);
			FBDF_LOG_INFO(log_str.c_str());
		}
#endif
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
	/**
	 * @brief 待機モーションのアニメ番号を取得する
	 * @param なし
	 * @return アニメ番号
	 */
	size_t GetIdleMotionAnimNo(void) const {
		double loop_time = 4 * 60000 / this->bpm; /* 1ループの時間、this->bpmは0以外を保証 */
		int base_time = GetNowCount() - this->Stime - this->offset; /* オフセットからの時間 */
		int now_block = (int)(base_time / loop_time); /* ループ回数、loop_timeは0以外を保証 */
		int in_time = base_time - now_block * loop_time; /* ループ内の時間 */
		if (in_time < 0) { in_time += loop_time; } /* マイナス補正 */
		return (int)(lins(0, 0, loop_time, motion_len * 2, in_time)) % (motion_len * 2);
	}

	/**
	 * @brief 今のモーションのアニメ番号を取得する
	 * @param なし
	 * @return アニメ番号
	 */
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
			retval = lins_scale(0, 0, 500, this->motion_len / 2, GetNowCount() - this->Stime);
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
		DrawBox(200,  20, lins_scale(0, 200, 1, 300, MV1GetShapeApplyRate(this->n3Dmodel_handle, this->n3Dshape_a_hdl)),  35, COLOR_WHITE, TRUE);
		DrawBox(200,  40, lins_scale(0, 200, 1, 300, MV1GetShapeApplyRate(this->n3Dmodel_handle, this->n3Dshape_i_hdl)),  55, COLOR_WHITE, TRUE);
		DrawBox(200,  60, lins_scale(0, 200, 1, 300, MV1GetShapeApplyRate(this->n3Dmodel_handle, this->n3Dshape_u_hdl)),  75, COLOR_WHITE, TRUE);
		DrawBox(200,  80, lins_scale(0, 200, 1, 300, MV1GetShapeApplyRate(this->n3Dmodel_handle, this->n3Dshape_e_hdl)),  95, COLOR_WHITE, TRUE);
		DrawBox(200, 100, lins_scale(0, 200, 1, 300, MV1GetShapeApplyRate(this->n3Dmodel_handle, this->n3Dshape_o_hdl)), 115, COLOR_WHITE, TRUE);
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

	/**
	 * @brief 目のシェイプキーを更新する
	 * @param なし
	 * @return なし
	 */
	void UpdateEyesShape(void) const {
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_blick_hdl, 0.0);
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_smile_hdl, 0.0);
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_surps_hdl, 0.0);
		switch (this->Nstate) {
		case FBDF_DANCER_STATE_MISS:
		case FBDF_DANCER_STATE_AFK:
			/* FIXME: ユニオ専用の設定になってる */
			if (GetNowCount() - this->Stime < 1000 * 20 / 60) {
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_surps_hdl, 1.0);
			}
			else if (GetNowCount() - this->Stime < 5000) {
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_blick_hdl, 0.5);
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_smile_hdl, 0.5);
			}
			else {
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_blick_hdl, 0.5);
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_smile_hdl, lins_scale(5000, 0.5, 5500, 0.0, GetNowCount() - this->Stime));
			}
			break;
		default:
			MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_blick_hdl, 1 - abs(lins_scale(0, 0.0, 200, 2.0, GetNowCount() % 6000) - 1));
			break;
		}
	}

	/* 口のシェイプキーの更新 */
	void UpdateMouthShape(const tvec<FBDF_lyrics_mat_et> &lyrics, int Ntime) const {
		double now_blend = lins_scale(-100, 1.0, 0, 0.0, Ntime - lyrics.offsetDataTime(1));
		double nex_blend = lins_scale(-100, 0.0, 0, 1.0, Ntime - lyrics.offsetDataTime(1));
		
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_a_hdl, 0.0);
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_i_hdl, 0.0);
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_u_hdl, 0.0);
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_e_hdl, 0.0);
		MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_o_hdl, 0.0);
		if ((this->Nstate == FBDF_DANCER_STATE_MISS) ||
		    (this->Nstate == FBDF_DANCER_STATE_AFK)) {
			return;
		}

		switch (lyrics.nowData()) {
		case FBDF_LYRICS_MAT_A:
			MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_a_hdl, now_blend);
			break;
		case FBDF_LYRICS_MAT_I:
			MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_i_hdl, now_blend);
			break;
		case FBDF_LYRICS_MAT_U:
			MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_u_hdl, now_blend);
			break;
		case FBDF_LYRICS_MAT_E:
			MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_e_hdl, now_blend);
			break;
		case FBDF_LYRICS_MAT_O:
			MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_o_hdl, now_blend);
			break;
		case FBDF_LYRICS_MAT_N:
			/* 口閉じなので何もしない */
			break;
		case FBDF_LYRICS_MAT_NONE:
		case FBDF_LYRICS_MAT_FREE:
		default:
			/* 指定なし、キャラの気分で勝手に動く。 */
			/* TODO: ↑の実装をする */
			break;
		}

		if (lyrics.offsetDataTime(1) <= Ntime + 100) {
			switch (lyrics.offsetData(1)) {
			case FBDF_LYRICS_MAT_A:
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_a_hdl, nex_blend);
				break;
			case FBDF_LYRICS_MAT_I:
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_i_hdl, nex_blend);
				break;
			case FBDF_LYRICS_MAT_U:
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_u_hdl, nex_blend);
				break;
			case FBDF_LYRICS_MAT_E:
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_e_hdl, nex_blend);
				break;
			case FBDF_LYRICS_MAT_O:
				MV1SetShapeRate(this->n3Dmodel_handle, this->n3Dshape_o_hdl, nex_blend);
				break;
			case FBDF_LYRICS_MAT_N:
			case FBDF_LYRICS_MAT_NONE:
			case FBDF_LYRICS_MAT_FREE:
			default:
				/* 何もしない */
				break;
			}
		}
	}

public: /* update系 */
	/**
	 * @brief 内部情報の更新、最低でも描画前に呼んで。
	 * @param[out] lyrics 歌詞/口パクデータ
	 * @return なし
	 */
	void Update(const tvec<FBDF_lyrics_mat_et> &lyrics, int Ntime) {
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
		this->UpdateEyesShape();
		this->UpdateMouthShape(lyrics, Ntime);
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
	void update_score(const FBDF_score_st &score, uint noteN) {
		uint hit_notes = score.crit + score.hit + score.save + score.drop;
		uint remain_notes = noteN - hit_notes;
		this->score_bar.bar_70 = 100 * (score.scale_point + (70.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_90 = 100 * (score.scale_point + (90.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_96 = 100 * (score.scale_point + (96.0 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_98 = 100 * (score.scale_point + (98.5 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_bar.bar_99 = 100 * (score.scale_point + (99.1 / 100.0) * remain_notes * CRIT_SCORE) / (double)(noteN * CRIT_SCORE);
		this->score_ave = DIV_AVOID_ZERO(100 * score.scale_point, hit_notes * CRIT_SCORE, 100.00);
	}

	/**
	 * @brief スコアバーの推移データを更新する
	 * @param[in] Ntime 今の時間。相対時間。[ms]
	 * @return なし
	 */
	void update_graph(int Ntime) {
		if (this->graphNo >= FBDF_RESULT_SCORE_GRAPH_COUNT) { return; }
		DxTime_t point_time = lins_scale(0, this->Stime, (FBDF_RESULT_SCORE_GRAPH_COUNT - 1), this->Etime, this->graphNo);
		if (point_time < Ntime) { this->set_graph(); }
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
	void draw_bar(int left, int up, int right, int down) const {
		FBDF_DrawScoreBarHori(this->score_bar, left, up, right, down);
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
	const size_t max_count = 50;
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
	void DrawBar(int left, int up, int right, int down) const {
		int Yborder = (up + down) / 2;
		DrawLine(left, Yborder, right, Yborder, COLOR_RED);
		for (int in = 0; in < this->queue.size(); in++) {
			int Drawx1 = lins_scale(        0, left, this->max_count - 1            , right, in + this->max_count - this->queue.size());
			int Drawx2 = lins_scale(this->len, left, this->max_count - 1 + this->len, right, in + this->max_count - this->queue.size());
			int Drawy  = lins_scale(   height,   up,        -height,  down, this->queue[in]);
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
		if (this->max_count <= this->queue.size()) {
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
	const uint note_height = lins(0, 0, 720, 15, WINDOW_SIZE_Y);
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
				DrawYpos = down - this->note_height -
					(int)((time_gap - 16 + game_option.note_offset_draw) *
					game_option.lane_speed) / 50;
			}
			if (this->note_height + DrawYpos < 0) { break; } /* 画面外break */

			/* 描画処理 */
			switch (game_option.play_style) {
        	case FBDF_PLAYSTYLE_ASSIST_PLUS:
        	case FBDF_PLAYSTYLE_ASSIST:
        	case FBDF_PLAYSTYLE_NORMAL:
				DrawExtendGraph(DrawLeft, this->note_height + DrawYpos, DrawRight, DrawYpos, Npic, TRUE);
				break;
        	case FBDF_PLAYSTYLE_BLANC:
				DrawExtendGraph(DrawLeft, this->note_height + DrawYpos, DrawRight, DrawYpos, this->note_pic.white.handle(), TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->GetNotesAlpha());
				DrawExtendGraph(DrawLeft, this->note_height + DrawYpos, DrawRight, DrawYpos, Npic, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				break;
        	case FBDF_PLAYSTYLE_BLANC_PLUS:
				DrawExtendGraph(DrawLeft, this->note_height + DrawYpos, DrawRight, DrawYpos, this->note_pic.white.handle(), TRUE);
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

class FBDF_play_chain_draw_c {
private:
	uint chain = 0;
	DxTime_t Stime = 0;
	dxcur_pic_c strpic = dxcur_pic_c(_T("pic/play/chainstr.png"));
	dxcur_divpic_c numpic = dxcur_divpic_c(_T("pic/play/chainnum.png"), 10, 5, 2);

	                        /* 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 */
	const int strsize[10] = { 80,38,65,55,65,60,70,75,65,72 };
	const int picsizeX    =   80;
	const int picsizeY    =   88;

	/* x座標は勝手に進むので注意 */
	void DrawNumOnce(int &x, int y, uint num, double size) const {
		num = betweens(0, num, 9);
		DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
			this->numpic.handle(num), TRUE);
		x += this->strsize[num] * size;
	}

	/* x座標は勝手に進むので注意 */
	void DrawNumOnceStr(int &x, int y, char num, double size) const {
		num = betweens('0', num, '9');
		DrawExtendGraph(x, y, x + this->picsizeX * size, y + this->picsizeY * size,
			this->numpic.handle(num - '0'), TRUE);
		x += this->strsize[num - '0'] * size;
	}

	uint GetPicSize(uint num, double size) const {
		uint ret = 0;
		while (num != 0) {
			ret += this->strsize[num % 10];
			num /= 10;
		}
		return (uint)(ret * size);
	}

	void DrawNum(int x, int y, uint num) const {
		char buf[5];
		double size_p = lins_scale(4, 0.4, 200, 0.8, num) + lins_scale(0, 0.1, 100, 0, GetNowCount() - Stime);
		int DrawX = x;
		int DrawY = y - this->picsizeY * size_p;
		strnums(buf, num, 5);
		for (size_t i = 0; buf[i] != '\0'; i++) {
			this->DrawNumOnceStr(DrawX, DrawY, buf[i], size_p);
		}
	}

public:
	void DrawChain(int x, int y) const {
		if (this->Stime + 750 < GetNowCount()) { return; }
		if (this->chain < 4) { return; }

		int DrawX = x;
		this->DrawNum(x, y, this->chain);
		DrawX = x + this->GetPicSize(this->chain, lins_scale(4, 0.4, 200, 0.8, this->chain) + lins_scale(0, 0.1, 100, 0, GetNowCount() - Stime));
		{
			DrawExtendGraph(DrawX, y - 36, DrawX + 120, y, this->strpic.handle(), TRUE);
		}
	}

	void SetChain(uint chain) {
		this->Stime = GetNowCount();
		this->chain = chain;
	}
};

#endif /* class */

/* プレイ画面に関するクラスをまとめたもの */
typedef struct FBDF_play_class_set_s {
	FBDF_judge_c judge_class;
	FBDF_dancer_c dancer_class{ game_option.chara };
	FBDF_score_bar_c score_bar_class;
	FBDF_gap_bar_c gap_bar_class;
	FBDF_play_notes_draw_c notes_draw_class;
	FBDF_play_chain_draw_c chain_draw_class;
} FBDF_play_class_set_t;

#if 1 /* ノーツ判定系 */

/**
 * @brief キー押し検出後の1ノーツ判定
 * @param[out] buf 判定イベント格納先
 * @param[out] key_detect キー検出。falseに書き換えられることがある
 * @param[out] map マップデータ
 * @return なし
 */
static void FBDF_Play_OneNoteJudgeAfterKeyDetect(FBDF_judge_event_st &buf, bool &key_detect, const FBDF_map_t &map) {
	if (key_detect) {
		key_detect = false;
		buf.gap = map.note.nowData().time - map.Ntime;
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

/**
 * @brief ノートイベントを処理する
 * @param[in] judge_event 予約されたノートイベント。関数を抜けた後は空っぽになる。
 * @param[out] play_class プレイクラス
 * @param[out] score スコアデータ
 * @param[in] noteN ノーツ数
 * @param[in] se 効果音データ
 */
static void FBDF_Play_NoteJudgeEventAntion(
	std::queue<FBDF_judge_event_st> &judge_event, FBDF_play_class_set_t &play_class,
	FBDF_score_st &score, size_t noteN, const FBDT_hit_snd_t &se
) {
	bool note_judged = !judge_event.empty();
	FBDF_judge_event_st buf;

	FBDF_judge_c           &judge_class      = play_class.judge_class;
	FBDF_dancer_c          &dancer_class     = play_class.dancer_class;
	FBDF_score_bar_c       &score_bar_class  = play_class.score_bar_class;
	FBDF_gap_bar_c         &gap_bar_class    = play_class.gap_bar_class;
	FBDF_play_notes_draw_c &notes_draw_class = play_class.notes_draw_class;
	FBDF_play_chain_draw_c &chain_draw_class = play_class.chain_draw_class;

	while (!judge_event.empty()) {
		buf = judge_event.front();
		judge_event.pop();

		if (buf.mat == JUDGE_NONE) { continue; }

		judge_class.SetJudge(buf.mat, buf.gap);

		/* 判定数追加 */
		switch (buf.mat) {
		case JUDGE_CRIT:
			score.crit++;
			break;
		case JUDGE_HIT:
			score.hit++;
			break;
		case JUDGE_SAVE:
			score.save++;
			break;
		case JUDGE_MISS:
			score.drop++;
			break;
		}

		/* 効果音再生 */
		if (buf.mat != JUDGE_MISS) {
			switch (buf.tip) {
			case FBDF_PLAY_NOTE_BTN_1:
				PlaySoundMem(se.SE1Data.handle(), DX_PLAYTYPE_BACK);
				break;
			case FBDF_PLAY_NOTE_BTN_2:
			case FBDF_PLAY_NOTE_BTN_3:
			case FBDF_PLAY_NOTE_BTN_4:
				PlaySoundMem(se.SE2Data.handle(), DX_PLAYTYPE_BACK);
				break;
			}
		}

		/* チェイン計算 */
		if (buf.mat == JUDGE_MISS) { score.chain = 0; }
		else { score.chain++; }
		chain_draw_class.SetChain(score.chain);

		/* スコア計算 */
		score.base_point += buf.score;
		score.scale_point = score.base_point; /* TODO: basepointの方にプレイスタイル補正をかけていた名残り。消す。 */
		switch (game_option.play_style) {
		case FBDF_PLAYSTYLE_ASSIST_PLUS:
			break;
		case FBDF_PLAYSTYLE_ASSIST:
			score.chain_point += score.chain * 0.45;
			break;
		case FBDF_PLAYSTYLE_NORMAL:
			score.chain_point += score.chain;
			break;
		case FBDF_PLAYSTYLE_BLANC:
			score.chain_point += score.chain * 1.28;
			break;
		case FBDF_PLAYSTYLE_BLANC_PLUS:
			score.chain_point += score.chain * 1.84;
			break;
		}
		score.all_point = score.scale_point + score.chain_point;

		/* キャラモーション変更 */
		if (buf.mat == JUDGE_MISS) {
			dancer_class.SetMissState();
		}
		else {
			dancer_class.SetState(buf.tip, buf.len, buf.mtime, buf.motion);
		}

		/* gap追加 */
		if (buf.mat != JUDGE_MISS) {
			gap_bar_class.SetVal(buf.gap);
		}

		/* ノーツ描画に影響するパラメータ */
		if (buf.mat != JUDGE_MISS) {
			notes_draw_class.SetRecover();
		}
		else {
			notes_draw_class.SetMiss();
		}
	}

	if (note_judged) {
		score_bar_class.update_score(score, noteN);
	}

	return;
}

class FBDF_Play_tutorial_c {
private:
	const int BaseTime = 10;
	const int bpm = 135;
	dxcur_pic_c pic[12] = {
		dxcur_pic_c(_T("music/tutorial/001.png")),
		dxcur_pic_c(_T("music/tutorial/002.png")),
		dxcur_pic_c(_T("music/tutorial/003.png")),
		dxcur_pic_c(_T("music/tutorial/004.png")),
		dxcur_pic_c(_T("music/tutorial/005.png")),
		dxcur_pic_c(_T("music/tutorial/006.png")),
		dxcur_pic_c(_T("music/tutorial/007.png")),
		dxcur_pic_c(_T("music/tutorial/008.png")),
		dxcur_pic_c(_T("music/tutorial/009.png")),
		dxcur_pic_c(_T("music/tutorial/010.png")),
		dxcur_pic_c(_T("music/tutorial/011.png")),
		dxcur_pic_c(_T("music/tutorial/012.png"))
	};

	int GetAlpha(int Ntime) const {
		int beat = 100 * this->bpm * (double)(Ntime - this->BaseTime) / 60000; /* x100 */
		if (beat <= 3100) {
			return lins_scale(300, 0, 400, 255, beat);
		}
		beat = (beat - 3100) % 3200;
		if (beat <= 100) {
			return lins_scale(0, 255, 100, 0, beat);
		}
		if (beat <= 200) {
			return lins_scale(100, 0, 200, 255, beat);
		}
		return 255;
	}

	int GetSection(int Ntime) const {
		int beat = 100 * this->bpm * (double)(Ntime - this->BaseTime) / 60000; /* x100 */
		for (size_t i = 0; i < 20; i++) {
			if (beat < 3200 * (i + 1)) { return i; }
		}
		return 20;
	}

public:
	void draw(int Ntime) const {
		const int drawX = 220;
		const int drawY = 100;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, this->GetAlpha(Ntime));
		switch (this->GetSection(Ntime)) {
		case 0: /* イントロ */
			DrawGraph(drawX, drawY, this->pic[0].handle(), TRUE);
			break;
		case 1: /* キー */
			DrawGraph(drawX, drawY, this->pic[1].handle(), TRUE);
			break;
		case 2: /* ノーツ */
			DrawGraph(drawX, drawY, this->pic[2].handle(), TRUE);
			break;
		case 3: /* 青ノーツ */
			DrawGraph(drawX, drawY, this->pic[3].handle(), TRUE);
			break;
		case 5: /* 緑ノーツ */
			DrawGraph(drawX, drawY, this->pic[4].handle(), TRUE);
			break;
		case 7: /* 赤ノーツ */
			DrawGraph(drawX, drawY, this->pic[5].handle(), TRUE);
			break;
		case 9: /* 黄ノーツ */
			DrawGraph(drawX, drawY, this->pic[6].handle(), TRUE);
			break;
		case 11: /* カンタン */
			DrawGraph(drawX, drawY, this->pic[7].handle(), TRUE);
			break;
		case 12: /* ゲージ */
			DrawGraph(drawX, drawY, this->pic[8].handle(), TRUE);
			break;
		case 13: /* 実践 */
			DrawGraph(drawX, drawY, this->pic[9].handle(), TRUE);
			break;
		case 16: /* 終わりに */
			DrawGraph(drawX, drawY, this->pic[10].handle(), TRUE);
			break;
		case 17: /* エンド */
			DrawGraph(drawX, drawY, this->pic[11].handle(), TRUE);
			break;
		default:
			break;
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
};

/**
 * @brief ノーツの判定
 * @param[out] play_class プレイクラス
 * @param[out] score スコア
 * @param[out] map マップデータ
 * @param[in] pkey キー入力情報
 * @param[in] se 効果音情報
 * @return なし
 */
static void FBDF_Play_NoteJudge(
	FBDF_play_class_set_t &play_class, FBDF_score_st &score, FBDF_map_t &map,
	const FBDF_push_key_c &pkey, const FBDT_hit_snd_t &se)
{
	std::queue<FBDF_judge_event_st>judge_event;
	FBDF_judge_event_st buf;

	bool key_detect_d = (pkey.D == 1);
	bool key_detect_f = (pkey.F == 1);
	bool key_detect_j = (pkey.J == 1);
	bool key_detect_k = (pkey.K == 1);

	if (1 <= pkey.alltap) {
		while (map.note.nowData().time != 0 &&
			map.note.nowData().time - JUDGE_WIDTH < map.Ntime &&
			(key_detect_d || key_detect_f || key_detect_j || key_detect_k))
		{
			buf.tip = map.note.nowData().btn;
			buf.len = map.note.nowData().len;
			buf.mtime = map.note.nowData().mtime;
			buf.motion = map.note.nowData().motion;
			switch (map.note.nowData().btn) {
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
			if (buf.mat != JUDGE_NONE) { map.note.stepNo(); }
		}
	}

	if (map.note.nowData().time != 0 &&
		map.note.nowData().time + JUDGE_WIDTH < map.Ntime)
	{
		buf.mat = JUDGE_MISS;
		judge_event.push(buf);
		map.note.stepNo();
	}

	FBDF_Play_NoteJudgeEventAntion(judge_event, play_class, score, map.note.size() - 1, se);
	return;
}

/**
 * @brief 残っているすべてのノーツをdropにする。強制終了した時用
 * @param[out] play_class プレイクラス
 * @param[out] score スコア
 * @param[out] map マップデータ
 * @return なし
 */
static void FBDF_Play_NoteTrash(FBDF_play_class_set_t &play_class, FBDF_score_st &score, FBDF_map_t &map) {
	size_t remain_notes = 0;
	FBDF_judge_c           &judge_class      = play_class.judge_class;
	FBDF_dancer_c          &dancer_class     = play_class.dancer_class;
	FBDF_score_bar_c       &score_bar_class  = play_class.score_bar_class;
	FBDF_play_chain_draw_c &chain_draw_class = play_class.chain_draw_class;

	FBDF_judge_event_st buf;

	while (map.note.nowData().time != 0) {
		remain_notes++;
		map.note.stepNo();
	}

	if (0 < remain_notes) {
		judge_class.SetJudge(JUDGE_MISS, 0);
		score.drop += remain_notes;
		dancer_class.SetMissState();
		score_bar_class.update_score(score, map.note.size());
		chain_draw_class.SetChain(0);
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
static void FBDF_Play_MakeResultData(FBDF_result_data_t &result_data, const FBDF_play_choose_music_st &nex_music,
	const FBDF_map_t &map, const FBDF_score_st &score, const FBDF_play_class_set_t &play_class
) {
	result_data.music_name  = nex_music.folder_name;
	result_data.artist_name = map.artist_name;
	result_data.folder_name = nex_music.folder_name;
	result_data.level       = map.user_level;
	result_data.score       = score.all_point;
	result_data.acc         = play_class.score_bar_class.GetScore_ave();
	result_data.crit        = score.crit;
	result_data.hit         = score.hit;
	result_data.save        = score.save;
	result_data.drop        = score.drop;
	result_data.gap_ave     = play_class.gap_bar_class.GetAve();
	play_class.score_bar_class.get_graph(result_data.score_graph);
	result_data.dif_type    = nex_music.dif_type;
	return;
}

/**
 * @brief 譜面を読み込む
 * @param[out] map 格納場所
 * @param[in] folder_name フォルダ名
 * @param[in] dif 難易度
 * @return bool true=成功, false=失敗
 */
static bool FBDF_Play_MapLoad(FBDF_map_t &map, const TCHAR *folder_name, FBDF_dif_type_ec dif) {
	FBDF_mapenc_error_et ret;
	ret = FBDF_MapLoadOne(map, folder_name, dif);
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
 * @brief 途中終了判定
 * @param[out] play_class プレイクラス、クローズが押された時の処理用
 * @param[out] score スコア、クローズが押された時の処理用
 * @param[out] map マップデータ、クローズが押された時とオートプレイ時のキー判定用
 * @param[out] cutin カットインクラス、クローズが押された時の判定と処理用
 * @return なし
 */
static void FBDF_Play_CheckStop(
	FBDF_play_class_set_t &play_class,
	FBDF_score_st &score, FBDF_map_t &map, FBDF_cutin_c &cutin
) {
	if (!cutin.IsClosing() && (CheckHitKey(KEY_INPUT_ESCAPE) == 1)) {
		FBDF_Play_NoteTrash(play_class, score, map);
		play_class.score_bar_class.fill_graph_force();
		cutin.SetIo(CUT_FRAG_IN);
	}
	return;
}

static void FBDF_Play_AllUpdate(
	FBDF_play_class_set_t &play_class, FBDF_score_st &score, FBDF_map_t &map,
	FBDF_push_key_c &pkey, DxTime_t &FinishTime, FBDF_cutin_c &cutin,
	DxSnd_t music_hand, FBDT_hit_snd_t &se
) {
	/* 時間更新 */
	map.Ntime = GetNowCount() - map.Stime;
	map.lyrics.stepNoTime(map.Ntime);

	FBDF_Play_CheckStop(play_class, score, map, cutin);
	pkey.update(map, game_option.auto_en);

	/* ノーツ全処理判定 */
	if ((FinishTime == 0) && (map.note.size() == map.note.nowNo() + 1)) { FinishTime = map.Ntime; }

	/* 譜面終了判定 */
	if (
		!cutin.IsClosing() && (FinishTime != 0) &&
		((FinishTime + 2000) <= map.Ntime) && (CheckSoundMem(music_hand) != 1)
	) {
		cutin.SetIo(CUT_FRAG_IN);
	}

	FBDF_Play_NoteJudge(play_class, score, map, pkey, se);

	/* update系 */
	play_class.dancer_class.Update(map.lyrics, map.Ntime);
	play_class.score_bar_class.update_graph(map.Ntime);
	cutin.update();
}

#if 1 /* Draw系 */

/**
 * @brief 押しキーランプの描画
 * @param[in] pkey キー入力情報
 * @return なし
 */
static void FBDF_PlayDrawLamp(const FBDF_push_key_c &pkey) {
	/* TODO: 画像にしたい。 */
	static const int baseX = lins(0, 0, 960, 165, WINDOW_SIZE_X);
	static const int baseY = lins(0, 0, 720, 575, WINDOW_SIZE_Y);
	static const int sizeX = lins(0, 0, 960,  60, WINDOW_SIZE_X);
	static const int sizeY = lins(0, 0, 720,  15, WINDOW_SIZE_Y);
	static const int   gap =  10;
	if (1 <= pkey.D) {
		DrawBox(baseX                      , baseY, baseX +     sizeX          , baseY + sizeY, NOTE_COLOR_1, TRUE);
	}
	if (1 <= pkey.F) {
		DrawBox(baseX +     sizeX +     gap, baseY, baseX + 2 * sizeX +     gap, baseY + sizeY, NOTE_COLOR_2, TRUE);
	}
	if (1 <= pkey.J) {
		DrawBox(baseX + 2 * sizeX + 2 * gap, baseY, baseX + 3 * sizeX + 2 * gap, baseY + sizeY, NOTE_COLOR_3, TRUE);
	}
	if (1 <= pkey.K) {
		DrawBox(baseX + 3 * sizeX + 3 * gap, baseY, baseX + 4 * sizeX + 3 * gap, baseY + sizeY, NOTE_COLOR_4, TRUE);
	}
	return;
}

static void FBDF_Play_DrawJudgeArea(const FBDF_play_class_set_t &play_class) {
	uint Yoffset = 0;
	if (game_option.judge_draw_en) {
		play_class.judge_class.DrawJudge(
			lins(0, 120, 960, 270, WINDOW_SIZE_X), lins(0, 0, 720, 530, WINDOW_SIZE_Y)
		);
		Yoffset += 70;
	}
	if (game_option.fast_slow_en ) {
		play_class.judge_class.DrawFastSlow(
			lins(0, 50, 960, 230, WINDOW_SIZE_X), lins(0, 0, 720, 545, WINDOW_SIZE_Y) - Yoffset
		);
		Yoffset += 40;
	}
	if (game_option.chain_draw_en) {
		play_class.chain_draw_class.DrawChain(
			lins(0, 0, 960, 180, WINDOW_SIZE_X), lins(0, 0, 720, 560, WINDOW_SIZE_Y) - Yoffset
		);
	}
}

/**
 * @brief プレイ画面に関するものを全て描画する。
 */
static void FBDF_Play_AllDraw(
	const FBDF_play_class_set_t &play_class, const FBDF_score_st &score,
	const FBDF_map_t &map, const FBDF_push_key_c &pkey,
	const dxcur_pic_c &backPic, const dxcur_pic_c &lanePic, const FBDF_cascadia_pic_c &numPic,
	const FBDF_Play_tutorial_c *tutorial)
{
	DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, backPic.handle(), TRUE); /* 背景描画 */

	/* ダンサー周り描画 */
	play_class.dancer_class.DrawDance(500, 300);
	numPic.DrawNumRight(WINDOW_SIZE_X - 15, 15, 0.6, score.all_point);

	/* スコアバー周り描画 */
	play_class.score_bar_class.draw_bar(
		lins(0, 0, 960, 167, WINDOW_SIZE_X),
		lins(0, 0, 720, 600, WINDOW_SIZE_Y),
		WINDOW_SIZE_X - 10,
		lins(0, 0, 720, 650, WINDOW_SIZE_Y)
	);
	FBDF_PlayDrawLamp(pkey);

	/* プレイエリア周り描画 */
	DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, lanePic.handle(), TRUE);
	DrawFormatString(
		lins(0, 0, 960, 166, WINDOW_SIZE_X), lins(0, 0, 720, 653, WINDOW_SIZE_Y) + 10,
		COLOR_WHITE, _T("%s"), map.music_name.c_str()
	);
	play_class.notes_draw_class.DrawNotes(
		lins(0, 0, 960, 42      , WINDOW_SIZE_X),
		lins(0, 0, 960, 42 + 110, WINDOW_SIZE_X),
		lins(0, 0, 720, 570     , WINDOW_SIZE_Y),
		map
	);
	play_class.gap_bar_class.DrawBar(
		lins(0, 0, 960,  40, WINDOW_SIZE_X),
		lins(0, 0, 720, 576, WINDOW_SIZE_Y),
		lins(0, 0, 960, 155, WINDOW_SIZE_X),
		lins(0, 0, 720, 691, WINDOW_SIZE_Y)
	);
	FBDF_Play_DrawJudgeArea(play_class);

	if (tutorial != nullptr) { tutorial->draw(map.Ntime); }
}

#endif /* Draw系 */

/**
 * @brief プレイ画面のベース
 * @param[out] result_data リザルト画面に渡すデータ
 * @param[in] nex_music セレクト画面から渡されたデータ
 * @return view_num_t 次の画面
 */
view_num_t FBDF_PlayView(FBDF_result_data_t &result_data, const FBDF_play_choose_music_st &nex_music) {
	FBDF_map_t map;
	FBDF_score_st score;
	FBDF_push_key_c pkey;

	FBDF_play_class_set_t play_class;
	FBDF_cascadia_pic_c numPic;
	FBDF_cutin_c cutin;

	dxcur_pic_c backPic(_T("pic/play/PlayBack.png"));
	dxcur_pic_c lanePic(_T("pic/play/PlayLane.png"));

	dxcur_snd_c musicData;
	FBDT_hit_snd_t se;

	DxTime_t FinishTime = 0;

	FBDF_Play_tutorial_c *tutorial = nullptr;
	FBDF_game_option_st option_buf;

	/* 譜面読み込み系 */
	if (FBDF_Play_MapLoad(map, nex_music.folder_name.c_str(), nex_music.dif_type) == false) { return VIEW_SELECT; }
	map.note.resetNo();
	play_class.score_bar_class.set_time(map.offset, map.Etime);
	play_class.dancer_class.SetBpm(map.bpm);
	
	FBDF_Mapenc_LyricsEnc(map.lyrics, nex_music.folder_name.c_str());

	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	if (map.music_name == "チュートリアル") {
		option_buf = game_option;
		game_option.play_style = FBDF_PLAYSTYLE_NORMAL;
		game_option.lane_speed = 20;
		tutorial = new FBDF_Play_tutorial_c;
	}

	FBDF_Play_Loadmusic(musicData, nex_music.folder_name.c_str(), map.music_file_name.c_str());
	PlaySoundMem(musicData.handle(), DX_PLAYTYPE_BACK);

	/* 曲再生後にやるべき初期化 */
	cutin.SetIo(CUT_FRAG_OUT);
	map.Stime = GetNowCount();

	/* ゲーム実行ループ */
	while (!cutin.IsEndAnim() && !GetWindowUserCloseFlag()) {
		FBDF_Play_AllUpdate(play_class, score, map, pkey, FinishTime, cutin, musicData.handle(), se);
		ClearDrawScreen(); /* 作画エリアここから */
		FBDF_Play_AllDraw(play_class, score, map, pkey, backPic, lanePic, numPic, tutorial);
		cutin.DrawCut();
		ScreenFlip(); /* 作画エリアここまで */
		WaitTimer(10); // ループウェイト
	}

	StopSoundMem(musicData.handle());

	if (GetWindowUserCloseFlag()) { return VIEW_EXIT; } /* 閉じるボタンが押された */
	if (game_option.auto_en) { return VIEW_SELECT; } /* オートプレイなら選択画面直行 */

	FBDF_Play_MakeResultData(result_data, nex_music, map, score, play_class);

	if (tutorial != nullptr) {
		game_option = option_buf;
		delete tutorial;
	}

	return VIEW_RESULT;
}
