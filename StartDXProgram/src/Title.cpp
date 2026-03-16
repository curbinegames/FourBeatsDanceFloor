
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

/**
 * @brief タイトル画面のベース
 * @param なし
 * @return view_num_t 次の画面
 */
view_num_t FBDF_TitleView(void) {
	/* TODO: 前半と後半に分ける */
	int keybox[1] = { KEY_INPUT_RETURN };
	int STime = 0;
	const int BPM = 150;

	dxcur_pic_c title_pic(_T("pic/title.png"));
	dxcur_pic_c ring_pic(_T("pic/white_ring.png"));
	dxcur_snd_c intro_bgm(_T("SE/Midsummer Philosophy/intro.mp3"));
	dxcur_snd_c loop_bgm(_T("SE/Midsummer Philosophy/loop.mp3"));
	dxcur_key_c key;

	particle_system_c particle_pent(_T("pic/titlePent.png"));
	particle_system_c particle_dot(_T("pic/titleDot.png"));

	FBDF_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	intro_bgm.PlaySound();

	while (CheckSoundMem(intro_bgm.handle()) == 1) {
		if (GetWindowUserCloseFlag()) { return VIEW_EXIT; } // 閉じるボタンが押された
		WaitTimer(1000 / 30); // ループウェイト
	}

	loop_bgm.PlaySound(true); /* TODO: 順序もっと後では? */
	STime = GetNowCount();

	particle_pent.SetSize(
		lins(0, 0, 960, 0.03, WINDOW_SIZE_X), lins(0, 0, 960, 0.12, WINDOW_SIZE_X)
	);
	particle_pent.SetMaxCount(500);
	particle_pent.SetSimTime(2);
	particle_dot.SetSize(
		lins(0, 0, 960, 0.01, WINDOW_SIZE_X), lins(0, 0, 960, 0.15, WINDOW_SIZE_X)
	);
	particle_dot.SetMaxCount(500);
	particle_dot.SetSimTime(2);

	particle_pent.init();
	particle_dot.init();

	while (!GetWindowUserCloseFlag() && !cutin.IsEndAnim()) {
		key.update();
		if (!cutin.IsClosing() && (key.GetKeyState(KEY_INPUT_RETURN) == 1)) { cutin.SetIo(CUT_FRAG_IN); }

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
