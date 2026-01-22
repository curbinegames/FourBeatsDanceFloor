
#include <vector>

#include <DxLib.h>

#include <dxcur.h>
#include <keycur.h>

#include <main.h>
#include <fbdf_cutin.h>

#define FBDF_TITLE_PART_LIFETIME  4000
#define FBDF_TITLE_PART_MAX_COUNT 500

/**
 * ライフタイムは4s
 */
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

	void dequeue(void) {
		for (int i = 0; i < particle.size(); i++) {
			if (FBDF_TITLE_PART_LIFETIME + particle[i].STime < GetNowCount() + this->ATime) {
				particle.erase(particle.begin() + i);
				i--;
			}
		}
	}

	void inqueue(void) {
		for (int i = particle.size(); i < max_count; i++) {
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
	particle_system_c(const TCHAR *path) {
		this->pic.reload(path);
	}

	void SetSize(double low, double high) {
		this->base_Ssize_low  = low;
		this->base_Ssize_high = high;
	}

	void SetMaxCount(int val) {
		this->max_count = val;
	}

	void SetSimTime(double val) {
		this->simTime = val;
	}

	void init(void) {
		this->BTime = GetNowCount();
		this->Next_Gene_time = GetNowCount() + DIV_AVOID_ZERO(FBDF_TITLE_PART_LIFETIME, (double)max_count, 0);
		for (int i = 0; i < max_count; i++) {
			FBDF_title_particle_mat_t buf;
			buf.STime  =  GetNowCount() - i * DIV_AVOID_ZERO(FBDF_TITLE_PART_LIFETIME, (double)max_count, 0);
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

	void update(void) {
		this->dequeue();
		this->inqueue();
		for (int i = 0; i < particle.size(); i++) {
			particle[i].Nrot += particle[i].divrot;
		}
		this->ATime += (this->simTime - 1) * (GetNowCount() - this->BTime);
		this->BTime = GetNowCount();
	}

	void draw(void) const {
		for (int i = 0; i < particle.size(); i++) {
			int    DXpos = pals(      FBDF_TITLE_PART_LIFETIME, particle[i].EXpos, 0,            particle[i].SXpos, GetNowCount() - particle[i].STime + this->ATime);
			int    DYpos = lins(      FBDF_TITLE_PART_LIFETIME, particle[i].EYpos, 0,            particle[i].SYpos, GetNowCount() - particle[i].STime + this->ATime);
			double Dsize = lins_scale(FBDF_TITLE_PART_LIFETIME, 0, FBDF_TITLE_PART_LIFETIME / 2, particle[i].Ssize, GetNowCount() - particle[i].STime + this->ATime);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA,
				min(abs(sin((GetNowCount() - particle[i].STime + this->ATime) / 1000.0 + particle[i].STime)) * 400, 255)
			);
			DrawDeformationPic(DXpos, DYpos, Dsize, Dsize, particle[i].Nrot, this->pic.handle());
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
	}
};

static int CalMusicMovieDiv(int STime, DxPic_t handle) {
	const int BPM = 150;
	int music_pos = (GetNowCount() - STime) % (60000 / BPM);
	int movie_pos = TellMovieToGraph(handle) % (60000 / BPM);
	int ret = movie_pos - music_pos;
	if ((30000 / BPM) < ret) {
		ret = movie_pos - (music_pos + (60000 / BPM));
	}
	if (ret < -(30000 / BPM)) {
		ret = (movie_pos + (60000 / BPM)) - music_pos;
	}
	return ret;
}

/* 返り値: 次のシーンの番号 */
view_num_t FirstTitleView() {
	int keybox[1] = { KEY_INPUT_RETURN };
	int hitkey = 0;
	int STime = 0;
	const int BPM = 150;

	int seek_count = 0;
	dxcur_pic_c title_pic(_T("pic/title.png"));
	dxcur_pic_c ring_pic(_T("pic/white_ring.png"));
	dxcur_pic_c movie(_T("pic/titleMovie.mp4"));
	dxcur_snd_c intro_bgm(_T("SE/Midsummer Philosophy/intro.mp3"));
	dxcur_snd_c loop_bgm(_T("SE/Midsummer Philosophy/loop.mp3"));

	particle_system_c particle_pent(_T("pic/titlePent.png"));
	particle_system_c particle_dot(_T("pic/titleDot.png"));

	fbdf_cutin_c cutin;
	cutin.SetWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);

	intro_bgm.PlaySound();

	while (CheckSoundMem(intro_bgm.handle()) == 1) {
		;
	}

	loop_bgm.PlaySound(true);
	STime = GetNowCount();

	particle_pent.SetSize(0.03, 0.12);
	particle_pent.SetMaxCount(500);
	particle_pent.SetSimTime(2);
	particle_dot.SetSize(0.01, 0.15);
	particle_dot.SetMaxCount(500);
	particle_dot.SetSimTime(2);

	particle_pent.init();
	particle_dot.init();

	while (1) {
		if (cutin.IsEndAnim()) { return VIEW_SELECT; }

		if (!cutin.IsClosing()) {
			switch (keycur(keybox, 1)) {
			case KEY_INPUT_RETURN:
				cutin.SetIo(CUT_FRAG_IN);
				break;
			default:
				break;
			}
		}

		if (GetMovieStateToGraph(movie.handle()) == 0) {
			SeekMovieToGraph(movie.handle(), 0);
			PlayMovieToGraph(movie.handle());
		}
		else {
			int div = CalMusicMovieDiv(STime, movie.handle());
			if (div <= -(15000 / BPM)) {
				SeekMovieToGraph(movie.handle(), TellMovieToGraph(movie.handle()) - div + 50);
				seek_count++;
			}
			else if ((15000 / BPM) <= div) {
				SeekMovieToGraph(movie.handle(), TellMovieToGraph(movie.handle()) - div - 50);
				seek_count++;
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

		DrawGraph(0, 0, title_pic.handle(), TRUE);

		cutin.DrawCut();

		ScreenFlip(); // 作画エリアここまで
		if (GetWindowUserCloseFlag(TRUE)) { break; } // 閉じるボタンが押された
		WaitTimer(1000 / 30); // ループウェイト
	}
	return VIEW_EXIT;
}
