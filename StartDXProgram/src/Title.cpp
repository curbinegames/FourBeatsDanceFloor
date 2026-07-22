
#include <vector>

#include <DxLib.h>

#include <dxcur.h>

#include <main.h>
#include <fbdf_cutin.h>

#define FBDF_TITLE_PART_LIFETIME  4000
#define FBDF_TITLE_PART_MAX_COUNT 500

typedef struct FBDF_title_particle_mat_s {
	DxTime_t STime = 0; /* スタート時間 */

	/* 現在値はlinsで取る */
	int SXpos = 0; /* Xのスタート位置 */
	int EXpos = 0; /* Xのエンド位置 */

	/* 現在値はpalsで取る */
	int SYpos = 0; /* Yのスタート位置 */
	int EYpos = 0; /* Yのエンド位置 */

	/* 現在値はlins_scaleで取る */
	double Ssize = 0; /* スタートの大きさ */
	/* エンド値は0 */

	/* 単位はdeg */
	double Nrot = 0; /* 現在角度 */
	double divrot = 0; /* 回転量 */
} FBDF_title_particle_mat_t;

typedef struct FBDF_title_particle_backlight_s {
	/* 位置は中央固定 */
	double size   = 0.8; /* 単位は倍率 */
	double rot    = 0.0; /* 単位はdeg */
	double divrot = 0.0; /* 秒間角度移動量 */
} FBDF_title_particle_backlight_t;

/**
 * 下から出るタイプのパーティクル
 */
class particle_system_c {
private:
	std::vector<FBDF_title_particle_mat_t> particle;
	dxcur_pic_c pic;

	double BTime = 0; /* 前回のupdate時間 */
	double ATime = 0; /* 追加時間 */

	double Next_Gene_time = 0;

	double base_Ssize_high = 0.10;
	double base_Ssize_low  = 0.03;
	int max_count = 500;
	double simTime = 1.0;

	/**
	 * @brief 時間切れのパーティクルを削除する
	 * @param なし
	 * @return なし
	 */
	void dequeue(void) {
		particle.erase(
			std::remove_if(particle.begin(), particle.end(),
				[&](const FBDF_title_particle_mat_t& p) {
					return FBDF_TITLE_PART_LIFETIME + p.STime < GetNowCount() + this->ATime;
				}),
			particle.end()
		);
	}

	/**
	 * @brief パーティクルを生成する
	 * @param なし
	 * @return なし
	 */
	void inqueue(void) {
		for (size_t i = particle.size(); i < max_count; i++) {
			FBDF_title_particle_mat_t buf;
			buf.STime  =  this->Next_Gene_time;
			buf.SXpos  =  GetRand(WINDOW_SIZE_X);
			buf.EXpos  =  buf.SXpos + GetRand(300) - 150;
			buf.SYpos  =  WINDOW_SIZE_Y + GetRand(75);
			buf.EYpos  =  GetRand(WINDOW_SIZE_Y / 2);
			buf.Ssize  =  lins(0, base_Ssize_low, 10000, base_Ssize_high, GetRand(10000));
			buf.Nrot   =  GetRand(360);
			buf.divrot = (GetRand(100) - 50) / 10.0;
			particle.push_back(buf);
			this->Next_Gene_time += DIV_AVOID_ZERO(FBDF_TITLE_PART_LIFETIME, (double)max_count, 0);
		}
	}

public:
	/**
	 * @brief コンストラクタ
	 * @param[in] path 画像ファイルのパス
	 * @return なし
	 */
	particle_system_c(const TCHAR *path) {
		this->pic.reload(path);
	}

	/**
	 * @brief パーティクルの大きさを変える
	 * @param[in] low 最小の大きさ
	 * @param[in] high 最大の大きさ
	 * @return なし
	 */
	void SetSize(double low, double high) {
		this->base_Ssize_low  = low;
		this->base_Ssize_high = high;
	}

	/**
	 * @brief パーティクルの最大数を変える
	 * @param[in] val 最大数
	 * @return なし
	 */
	void SetMaxCount(int val) {
		this->max_count = val;
	}

	/**
	* @brief 現在のパーティクルの最大数を取得する
	* @return int
	*/
	int GetMaxCount() const {
		return this->max_count;
	}

	/**
	 * @brief 全体の時間の進み具合を変える
	 * @param[in] val 進む速さ
	 * @return なし
	 */
	void SetSimTime(double val) {
		this->simTime = val;
	}

	/**
	 * @brief パーティクル情報を初期化する (名前startのほうが良い?)
	 * @param なし
	 * @return なし
	 */
	void init(void) {
		DxTime_t Ntime = GetNowCount();
		this->BTime = Ntime;
		this->Next_Gene_time = Ntime + DIV_AVOID_ZERO(FBDF_TITLE_PART_LIFETIME, (double)max_count, 0);
		this->particle.clear();
		this->particle.reserve(max_count);
		for (size_t i = 0; i < max_count; i++) {
			FBDF_title_particle_mat_t buf;
			buf.STime  =  Ntime - i * DIV_AVOID_ZERO(FBDF_TITLE_PART_LIFETIME, (double)max_count, 0);
			buf.SXpos  =  GetRand(WINDOW_SIZE_X);
			buf.EXpos  =  buf.SXpos + GetRand(300) - 150;
			buf.SYpos  =  WINDOW_SIZE_Y + GetRand(75);
			buf.EYpos  =  GetRand(WINDOW_SIZE_Y / 2);
			buf.Ssize  =  lins(0, base_Ssize_low, 10000, base_Ssize_high, GetRand(10000));
			buf.Nrot   =  GetRand(360);
			buf.divrot = (GetRand(100) - 50) / 10.0;
			particle.push_back(buf);
		}
	}

	/**
	 * @brief パーティクル情報を更新する
	 * @param なし
	 * @return なし
	 */
	void update(void) {
		this->dequeue();
		this->inqueue();
		for (size_t i = 0; i < particle.size(); i++) {
			particle[i].Nrot += particle[i].divrot;
		}
		this->ATime += (this->simTime - 1) * (GetNowCount() - this->BTime);
		this->BTime = GetNowCount();
	}

	/**
	 * @brief パーティクルを描く
	 * @param なし
	 * @return なし
	 */
	void draw(void) const {
		DxTime_t Ntime = GetNowCount();
		for (size_t i = 0; i < particle.size(); i++) {
			int    DXpos = pals(      FBDF_TITLE_PART_LIFETIME, particle[i].EXpos, 0,            particle[i].SXpos, Ntime - particle[i].STime + this->ATime);
			int    DYpos = lins(      FBDF_TITLE_PART_LIFETIME, particle[i].EYpos, 0,            particle[i].SYpos, Ntime - particle[i].STime + this->ATime);
			double Dsize = lins_scale(FBDF_TITLE_PART_LIFETIME, 0, FBDF_TITLE_PART_LIFETIME / 2, particle[i].Ssize, Ntime - particle[i].STime + this->ATime);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,
				min(abs(sin((Ntime - particle[i].STime + this->ATime) / 1000.0 + particle[i].STime)) * 400, 255)
			);
			DrawDeformationPic(DXpos, DYpos, Dsize, Dsize, particle[i].Nrot, this->pic.handle());
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
	}
};

bool FBDF_TitleViewP1(const dxcur_snd_c &intro_bgm) {
	int STime = 0;
	int NTime = 0;
	int FTime = 0;
	int NDanser = GetRand(14);
	dxcur_pic_c back_light_pic(_T("pic/title/backlight.png"));
	dxcur_pic_c danser_pic[15] = {
		_T("pic/title/101.png"),
		_T("pic/title/102.png"),
		_T("pic/title/103.png"),
		_T("pic/title/104.png"),
		_T("pic/title/105.png"),
		_T("pic/title/106.png"),
		_T("pic/title/107.png"),
		_T("pic/title/201.png"),
		_T("pic/title/202.png"),
		_T("pic/title/203.png"),
		_T("pic/title/204.png"),
		_T("pic/title/205.png"),
		_T("pic/title/206.png"),
		_T("pic/title/207.png"),
		_T("pic/title/208.png")
	};
	std::vector<FBDF_title_particle_backlight_t> particle_backlight(100);

	for (int i = 0; i < particle_backlight.size(); i++) {
		particle_backlight[i].size   = lins(0, 0.2, 1500, 0.7, GetRand(1500));
		particle_backlight[i].rot    = GetRand(359);
		particle_backlight[i].divrot = GetRand(180) - 90.0;
	}

	STime = GetNowCount();
	while (CheckSoundMem(intro_bgm.handle()) == 1) {
		if (GetWindowUserCloseFlag()) { return false; } // 閉じるボタンが押された
		NTime = GetNowCount() - STime;
		if (FTime < NTime && NTime < 1600) {
			NDanser = GetRand(14);
			FTime += 100;
		}
		ClearDrawScreen(); // 作画エリアここから
		// バックライトの描画
		for (int i = 0; i < particle_backlight.size(); i++) {
			// SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
			double DrawSX = particle_backlight[i].size * 0.7;
			double DrawSY = particle_backlight[i].size;
			double DrawR  = particle_backlight[i].rot + particle_backlight[i].divrot * (NTime / 1000.0);
			DrawSX *= pals_scale(300, 0, 1500, 1, NTime);
			DrawSY *= pals_scale(300, 0, 1500, 1, NTime);
			if (1500 <= NTime) {
				DrawSX = pals_scale(1500, DrawSX, 1950, 0.0, NTime);
				if (DrawR < 180) {
					DrawR = pals_scale(1500, DrawR, 1950, 90, NTime);
				} else {
					DrawR = pals_scale(1500, DrawR, 1950, 270, NTime);
				}
			}
			DrawRotaGraph3(
				WINDOW_SIZE_X / 2, WINDOW_SIZE_Y / 2,
				100, 747, DrawSX, DrawSY,
				DrawR * 3.14159 / 180.0,
				back_light_pic.handle(), TRUE
			);
			// SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		/* ダンサーの描画 */ {
			double DrawSX = pals_scale(300, 0.0, 1500, 0.9, NTime);
			double DrawSY = DrawSX;
			if (1500 <= NTime) {
				DrawSX = pals_scale(1500, DrawSX, 1950, 2.2, NTime);
				DrawSY = pals_scale(1500, DrawSY, 1950, 0.0, NTime);
			}
			DrawDeformationPic(
				WINDOW_SIZE_X / 2, WINDOW_SIZE_Y / 2,
				DrawSX, DrawSY, 0.0,
				danser_pic[NDanser].handle()
			);
		}
		ScreenFlip(); // 作画エリアここまで
		WaitTimer(1000 / 30); // ループウェイト
	}
	return true;
}

/**
 * @brief タイトル画面のベース
 * @param なし
 * @return view_num_t 次の画面
 */
view_num_t FBDF_TitleView(void) {
	/* TODO: 前半と後半に分ける */
	int keybox[1] = { KEY_INPUT_RETURN };
	int STime = 0;
	int NTime = 0;
	const int BPM = 150;

	dxcur_pic_c title_pic(_T("pic/title.png"));
	dxcur_pic_c ring_pic(_T("pic/white_ring.png"));
	dxcur_snd_c intro_bgm(_T("SE/Midsummer Philosophy/intro.mp3"));
	dxcur_snd_c loop_bgm(_T("SE/Midsummer Philosophy/loop.mp3"));
	dxcur_key_c key;

	particle_system_c particle_pent(_T("pic/titlePent_small.png"));
	particle_system_c particle_dot(_T("pic/titleDot_small.png"));

	FBDF_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	intro_bgm.PlaySound();

	if (FBDF_TitleViewP1(intro_bgm) != true) { return VIEW_EXIT; }

	loop_bgm.PlaySound(true); /* TODO: 順序もっと後では? */

	particle_pent.SetSize(
		lins(0, 0, 960, 0.25, WINDOW_SIZE_X), lins(0, 0, 960, 1.00, WINDOW_SIZE_X)
	);
	particle_pent.SetMaxCount(500);
	particle_pent.SetSimTime(2);
	particle_dot.SetSize(
		lins(0, 0, 960, 0.10, WINDOW_SIZE_X), lins(0, 0, 960, 1.00, WINDOW_SIZE_X)
	);
	particle_dot.SetMaxCount(500);
	particle_dot.SetSimTime(2);

	particle_pent.init();
	particle_dot.init();

	STime = GetNowCount();
	NTime = STime;
	std::vector<int> gap_time(30, NTime);
	while (!GetWindowUserCloseFlag() && !cutin.IsEndAnim()) {
		key.update();
		if (!cutin.IsClosing() && (key.GetKeyState(KEY_INPUT_RETURN) == 1)) { cutin.SetIo(CUT_FRAG_IN); }

		/* FPS計測 */
		NTime = GetNowCount();
		gap_time.push_back(NTime);
		if (gap_time.back() > 1000 + gap_time[0] && !gap_time.empty()) {
			gap_time.pop_back();
		}

		if (gap_time.size() < 30) {
			if (50 <= particle_pent.GetMaxCount()) {
				particle_pent.SetMaxCount(particle_pent.GetMaxCount() - 10);
			}
			if (50 <= particle_dot.GetMaxCount()) {
				particle_dot.SetMaxCount(particle_dot.GetMaxCount() - 10);
			}
		}

		if (50 < gap_time.size()) {
			if (particle_pent.GetMaxCount() < 500) {
				particle_pent.SetMaxCount(particle_pent.GetMaxCount() + 1);
			}
			if (particle_dot.GetMaxCount() < 500) {
				particle_dot.SetMaxCount(particle_dot.GetMaxCount() + 1);
			}
		}

		particle_pent.SetSimTime(lins(0, 4, 60000 / (double)BPM, 1, (GetNowCount() - STime) % (60000 / BPM)));
		particle_dot.SetSimTime( lins(0, 4, 60000 / (double)BPM, 1, (GetNowCount() - STime) % (60000 / BPM)));

		particle_pent.update();
		particle_dot.update();

		cutin.update();

		ClearDrawScreen(); // 作画エリアここから

		/* particle */
		particle_pent.draw();
		particle_dot.draw();

		/* ring */
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, lins(0, 127, 60000 / (double)BPM, 0, (GetNowCount() - STime) % (60000 / BPM)));
		DrawExtendGraph(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y, ring_pic.handle(), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		DrawExtendGraph(5, 5, WINDOW_SIZE_X - 5, 5 + (WINDOW_SIZE_X - 10) * 367 / 967, title_pic.handle(), TRUE);

		cutin.DrawCut();

		ScreenFlip(); // 作画エリアここまで
		WaitTimer(1000 / 30); // ループウェイト
	}
	if (GetWindowUserCloseFlag()) { return VIEW_EXIT; } // 閉じるボタンが押された
	return VIEW_SELECT;
}
